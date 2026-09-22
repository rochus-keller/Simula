/*
* Copyright 2026 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the Simula67 parser library.
*
* The following is the license that applies to this copy of the
* library. For a license to use the library under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "SimImporter.h"
#include "SimLexer.h"
#include "SimParser3.h"
#include "SimValidator2.h"
#include <QDir>
#include <QFileInfo>
using namespace Sim;

namespace {
class Lex : public Scanner
{
public:
    Lexer lex;
    Token next() { return lex.nextToken(); }
    Token peek(int offset) { return lex.peekToken(offset); }
    QString source() const { return lex.sourcePath(); }
};
}

SourceImporter::SourceImporter(AstModel* mdl):mdl(mdl)
{
}

SourceImporter::~SourceImporter()
{
    foreach( Declaration* m, modules )
        Declaration::deleteAll(m);
}

void SourceImporter::addSearchPath(const QString& dir)
{
    if( !dir.isEmpty() && !searchPaths.contains(dir) )
        searchPaths.append(dir);
}

QString SourceImporter::findSource(Atom moduleName) const
{
    // the mapping of an external identifier to a source module here is a file of the same name (case insens)
    const QString name = QString::fromUtf8(moduleName).toLower();
    foreach( const QString& dir, searchPaths )
    {
        QDir d(dir);
        foreach( const QString& f, d.entryList(QStringList() << "*.sim" << "*.SIM", QDir::Files) )
        {
            const QFileInfo info(f);
            if( info.completeBaseName().toLower() == name )
                return d.absoluteFilePath(f);
        }
    }
    return QString();
}

Declaration* SourceImporter::parseModule(const QString& path)
{
    Lex lex;
    if( !lex.lex.setStream(path) )
        return 0;
    lex.lex.setIgnoreComments(true);
    lex.lex.setPackComments(true);
    Parser3 p(&lex, mdl);
    p.RunParser();
    Declaration* module = p.takeResult();
    foreach( const Parser3::Error& e, p.errors )
        errors.append(Error(e.msg, e.pos, e.path));
    if( module == 0 )
        return 0;
    modules.append(module);
    if( !p.errors.isEmpty() )
        module->hasErrors = true;
    return module;
}

Declaration* SourceImporter::loadModule(const Import& imp)
{
    if( cache.contains(imp.moduleName) )
        return cache.value(imp.moduleName);
    cache.insert(imp.moduleName, 0); // breaks a cycle of external declarations

    const QString path = findSource(imp.moduleName);
    if( path.isEmpty() )
        return 0;

    Declaration* module = parseModule(path);
    if( module == 0 )
        return 0;
    cache.insert(imp.moduleName, module);

    if( !module->hasErrors )
    {
        // the module is validated in its own context with its own source position
        Validator2 v(mdl, this);
        v.validate(module);
        foreach( const Validator2::Error& e, v.errors )
            errors.append(Error(e.msg, e.pos, e.path));
    }

    loaded.append(module);

    return module;
}
