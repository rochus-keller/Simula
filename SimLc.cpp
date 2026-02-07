/*
* Copyright 2020 Rochus Keller <mailto:me@rochus-keller.ch>
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

#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QFileInfo>
#include <QDir>
#include <QElapsedTimer>
#include <QThread>
#include "SimParser3.h"
#include "SimAst.h"
#include "SimValidator2.h"
#include "SimLexer.h"

static QStringList collectFiles( const QDir& dir )
{
    QStringList res;
    QStringList files = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );

    foreach( const QString& f, files )
        res += collectFiles( QDir( dir.absoluteFilePath(f) ) );

    files = dir.entryList( QStringList() << QString("*.sim")
                                           << QString("*.si") << QString("*.s67"),
                                           QDir::Files, QDir::Name );
    foreach( const QString& f, files )
    {
        res.append( dir.absoluteFilePath(f) );
    }
    return res;
}


class Lex : public Sim::Scanner {
public:
    Sim::Lexer lex;
    virtual Sim::Token next() { return lex.nextToken(); }
    virtual Sim::Token peek(int offset) { return lex.peekToken(offset); }
    virtual QString source() const { return lex.sourcePath(); }
};


static void run( const QStringList& files, bool dump )
{
    Sim::AstModel mdl;
    {
        Lex lex;
        lex.lex.setStream(":/runtime/builtins.sim");
        lex.lex.setIgnoreComments(true);
        lex.lex.setPackComments(true);
        Sim::Parser3 p(&lex, &mdl);
        Sim::Declaration* module = p.RunParser();
        if( !p.errors.isEmpty() )
        {
            foreach( const Sim::Parser3::Error& e, p.errors )
                qCritical() << e.path << e.pos.d_row << e.pos.d_col << e.msg;
        }
        Sim::Declaration* decls = module->link;
        module->link = 0;
        Sim::Declaration* d = decls;
        while( d )
        {
            d->outer = mdl.getGlobals();
            d = d->next;
        }
        mdl.getGlobals()->appendMember(decls);
    }
    foreach( const QString& path, files )
    {
        qDebug() << "processing" << path;

        Lex lex;
        lex.lex.setStream(path);
        lex.lex.setIgnoreComments(true);
        lex.lex.setPackComments(true);
        Sim::Parser3 p(&lex, &mdl);
        Sim::Declaration* module = p.RunParser();
        if( !p.errors.isEmpty() )
        {
            foreach( const Sim::Parser3::Error& e, p.errors )
                qCritical() << e.path << e.pos.d_row << e.pos.d_col << e.msg;
        }
        else
        {
            if( dump )
            {
                QTextStream out(stdout);
                Sim::AstModel::dump(out, module);
            }
#if 1
            Sim::Validator2 va(&mdl);
            va.validate(module);
            if( !va.errors.isEmpty() )
            {
                foreach( const Sim::Validator2::Error& e, va.errors )
                    qCritical() << e.path << e.pos.d_row << e.pos.d_col << e.msg;
            }
#endif
        }

    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setOrganizationName("me@rochus-keller.ch");
    a.setOrganizationDomain("https://github.com/rochus-keller/Simula");
    a.setApplicationName("SimLc");
    a.setApplicationVersion("2020-10-26");

    QTextStream out(stdout);
    out << "SimLc version: " << a.applicationVersion() <<
                 " author: me@rochus-keller.ch  license: GPL" << endl;

    QStringList dirOrFilePaths;
    QString outPath;
    bool dump = false;
    QString ns;
    QString mod;
    const QStringList args = QCoreApplication::arguments();
    for( int i = 1; i < args.size(); i++ ) // arg 0 enthaelt Anwendungspfad
    {
        if(  args[i] == "-h" || args.size() == 1 )
        {
            out << "usage: SimLc [options] sources" << endl;
            out << "  reads Simula 67 sources (files or directories) and translates them to corresponding Lua code." << endl;
            out << "options:" << endl;
            out << "  -dst      dump syntax trees to files" << endl;
            out << "  -o=path   path where to save generated files (default like first source)" << endl;
            out << "  -ns=name  namespace for the generated files (default empty)" << endl;
            out << "  -mod=name directory of the generated files (default empty)" << endl;
            out << "  -h        display this information" << endl;
            return 0;
        }else if( args[i] == "-dst" )
            dump = true;
        else if( args[i].startsWith("-o=") )
            outPath = args[i].mid(3);
        else if( args[i].startsWith("-ns=") )
            ns = args[i].mid(4);
        else if( args[i].startsWith("-mod=") )
            mod = args[i].mid(5);
        else if( !args[ i ].startsWith( '-' ) )
        {
            dirOrFilePaths += args[ i ];
        }else
        {
            qCritical() << "error: invalid command line option " << args[i] << endl;
            return -1;
        }
    }
    if( dirOrFilePaths.isEmpty() )
    {
        qWarning() << "no file or directory to process; quitting (use -h option for help)" << endl;
        return -1;
    }

    QStringList files;
    foreach( const QString& path, dirOrFilePaths )
    {
        QFileInfo info(path);
        if( outPath.isEmpty() )
            outPath = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
        if( info.isDir() )
            files += collectFiles( info.absoluteFilePath() );
        else
            files << path;
    }

    run(files, dump);
    Sim::Node::reportLeftovers();

    return 0;
}
