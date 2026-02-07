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

// Adopted from the Luon IDE

#include "SimProject.h"
#include "SimParser3.h"
#include "SimLexer.h"
#include "SimValidator2.h"
#include <QBuffer>
#include <QDir>
#include <QtDebug>
#include <QSettings>
#include <QCoreApplication>
#include <qdatetime.h>
using namespace Sim;

struct HitTest
{
    quint32 line, col;
    QList<Declaration*> scopes;
    Declaration* scopeHit;

    HitTest():scopeHit(0){}

    QVariant findHit( Declaration* module, int row, int col )
    {
        this->col = row;
        this->line = line;
        try
        {
            visitDecl(module);
        }catch( Expression* e )
        {
            Q_ASSERT( !scopes.isEmpty() );
            scopeHit = scopes.back();
            return QVariant::fromValue(e);
        }catch( Declaration* d )
        {
            Q_ASSERT( !scopes.isEmpty() );
            scopeHit = scopes.back();
            return QVariant::fromValue(d);
        }catch(...)
        {

        }
        return 0;
    }

    void test( Declaration* d )
    {
        if( line == d->pos.d_row && col >= d->pos.d_col && col <= d->pos.d_col + d->name.size() )
            throw d;
    }

    void test(Expression* e)
    {
        if( e == 0 )
            return;
        if( e->pos.d_row > line )
            return;
        Declaration* n = 0;
        if( e->kind == Expression::DeclRef )
            n = e->d;
        if( n == 0 )
            return;
        if( line == e->pos.d_row && col >= e->pos.d_col && col <= e->pos.d_col + n->name.size() )
            throw e;
    }

    void visitDecl(Declaration* d)
    {
        test(d);
    }
};

class Lex : public Sim::Scanner {
public:
    Sim::Lexer lex;
    virtual Sim::Token next() { return lex.nextToken(); }
    virtual Sim::Token peek(int offset) { return lex.peekToken(offset); }
    virtual QString source() const { return lex.sourcePath(); }
};

Project::Project(QObject *parent) : QObject(parent),d_dirty(false)
{
    d_suffixes << ".sim";
}

Project::~Project()
{
    clearModules();
}

void Project::clear()
{
    clearModules();
    d_filePath.clear();
    d_files.clear();
}

void Project::createNew()
{
    clear();
    d_filePath.clear();
    d_dirty = false;
    emit sigModified(d_dirty);
    emit sigRenamed();
}

bool Project::initializeFromDir(const QDir& dir, bool recursive)
{
    clear();
    d_dirty = false;

    QStringList files = findFiles(dir, recursive);
    foreach( const QString& filePath, files )
        addFile(filePath);
    emit sigRenamed();
    return true;
}

void Project::setSuffixes(const QStringList& s)
{
    d_suffixes = s;
    touch();
}

void Project::setMain(const Project::ModProc& mp)
{
    d_main = mp;
    touch();
}

QString Project::renderMain() const
{
    QString res = d_main.first;
    if( !d_main.second.isEmpty() )
        res += "." + d_main.second;
    return res;
}

bool Project::addFile(const QString& filePath)
{
    if( d_files.contains(filePath) )
        return false;
    FileRef ref( new File() );
    ref->d_filePath = filePath;
    ref->d_name = QFileInfo(filePath).baseName().toUtf8();
    d_files.insert(filePath,ref);
    touch();
    return true;
}

bool Project::removeFile(const QString& filePath)
{
    FileHash::iterator i = d_files.find(filePath);
    if( i == d_files.end() )
        return false;
    d_files.erase(i);
    touch();
    return true;
}

Symbol* Project::findSymbolBySourcePos(const QString& file, quint32 line, quint16 col, Declaration** scopePtr) const
{
    File* f = findFile(file);
    if( f == 0 || f->d_mod == 0 )
        return 0;

    return findSymbolByModule(f->d_mod,line,col, scopePtr);
}

Symbol* Project::findSymbolByModule(Declaration* m, quint32 line, quint16 col, Declaration** scopePtr) const
{
    Q_ASSERT(m && m->kind == Declaration::Module);
    const ModuleSlot* module = findModule(m);
    if( module == 0 || module->xref.syms == 0 )
        return 0;
    Symbol* s = module->xref.syms;
    do
    {
        if( line == s->pos.d_row && col >= s->pos.d_col && col <= s->pos.d_col + s->len )
            return s;
        s = s->next;
    }while( s && s != module->xref.syms );
    return 0;
}

Project::File* Project::findFile(const QString& file) const
{
    File* f = d_files.value(file).data();
    return f;
}

Project::UsageByMod Project::getUsage(Declaration* n) const
{
    UsageByMod res;
    for( int i = 0; i < modules.size(); i++ )
    {
        const SymList& syms = modules[i].xref.uses.value(n);
        if( !syms.isEmpty() )
            res << qMakePair(modules[i].decl, syms);
    }

    return res;
}

Symbol*Project::getSymbolsOfModule(Declaration* module) const
{
    for( int i = 0; i < modules.size(); i++ )
    {
        if( modules[i].decl == module )
            return modules[i].xref.syms;
    }
    return 0;
}

DeclList Project::getSubs(Declaration* d) const
{
    return subs.value(d);
}

QString Project::getWorkingDir(bool resolved) const
{
    if( d_workingDir.isEmpty() )
    {
        if( !d_filePath.isEmpty() )
            return QFileInfo(d_filePath).dir().path();
        else
            return QCoreApplication::applicationDirPath();
    }
    else if( !resolved )
        return d_workingDir;
    // else
    QString wd = d_workingDir;
    wd.replace("%PRODIR%", QFileInfo(d_filePath).dir().path() );
    QString path;
#ifdef Q_OS_MAC
    QDir cur = QCoreApplication::applicationDirPath();
    if( cur.path().endsWith("/Contents/MacOS") )
    {
        // we're in a bundle
        cur.cdUp();
        cur.cdUp();
        cur.cdUp();
    }
    path = cur.path();
#else
    path = QCoreApplication::applicationDirPath();
#endif
    wd.replace("%APPDIR%", path );
    return wd;
}

void Project::setWorkingDir(const QString& wd)
{
    d_workingDir = wd;
    touch();
}

QString Project::getBuildDir(bool resolved) const
{
    if( d_buildDir.isEmpty() )
    {
        if( !d_filePath.isEmpty() )
            return QFileInfo(d_filePath).dir().absoluteFilePath("build");
        else
            return QCoreApplication::applicationDirPath() + "/build";
    }
    else if( !resolved )
        return d_buildDir;
    // else
    QString bd = d_buildDir;
    bd.replace("%PRODIR%", QFileInfo(d_filePath).dir().path() );
    QString path;
#ifdef Q_OS_MAC
    QDir cur = QCoreApplication::applicationDirPath();
    if( cur.path().endsWith("/Contents/MacOS") )
    {
        // we're in a bundle
        cur.cdUp();
        cur.cdUp();
        cur.cdUp();
    }
    path = cur.path();
#else
    path = QCoreApplication::applicationDirPath();
#endif
    bd.replace("%APPDIR%", path );
    return bd;
}

void Project::setBuildDir(const QString& bd)
{
    d_buildDir = bd;
    touch();
}

void Project::setOptions(const QByteArrayList& o)
{
    d_options = o;
    touch();
}

void Project::setArguments(const QStringList& args)
{
    d_arguments = args;
    touch();
}

bool Project::printTreeShaken(const QString& module, const QString& fileName)
{
#if 0
    // TODO
    FileRef f = d_files.value(module);
    if( f->d_mod.isNull() )
        return false;
    Declaration* m = d_mdl->treeShaken(f->d_mod.data());
    QFile out(fileName);
    if( !out.open(QIODevice::WriteOnly) )
        return false;


    ObxModuleDump dump;
    dump.out.setDevice( &out );
    m->accept(&dump);
#endif
    return true;
}

static inline QByteArray escapeDot(QByteArray name)
{
    return "\"" + name + "\"";
}

#if 0
static void removeRedundantImports(Declaration* cur, QSet<Declaration*>& imports )
{
    foreach( Import* i, cur->d_imports )
    {
        imports.remove( i->d_mod.data() );
        removeRedundantImports( i->d_mod.data(), imports );
    }
}

static QList<Declaration*> removeRedundantImports(Declaration* m)
{
    QSet<Declaration*> imports;
    foreach( Import* i, m->d_imports )
    {
        if( !i->d_mod->isFullyInstantiated() || i->d_mod->d_synthetic )
            continue;
        imports << i->d_mod.data();
    }
    foreach( Import* i, m->d_imports )
        removeRedundantImports(i->d_mod.data(), imports);
    return imports.toList();
}
#endif

bool Project::printImportDependencies(const QString& fileName, bool pruned)
{
    QFile f(fileName);
    if( !f.open(QIODevice::WriteOnly) )
        return false;
    QTextStream s(&f);

    s << "digraph \"Import Dependency Tree\" {" << endl;
    if( !pruned )
        s << "    graph [splines=ortho]" << endl;
    s << "    node [shape=box]" << endl;

#if 0
    // TODO
    QList<Declaration*> mods = getModulesToGenerate(true);
    foreach( Declaration* m, mods )
    {
        if( !m->isFullyInstantiated() || m->d_synthetic )
            continue;
        QSet<QByteArray> names;
        if( pruned )
        {
            QList<Declaration*> imports = removeRedundantImports(m);
            foreach( Declaration* i, imports )
                names << escapeDot(i->getFullName());
        }else
        {
            foreach( Import* i, m->d_imports )
            {
                if( i->d_mod.isNull() || i->d_mod->d_synthetic )
                    continue;
                names << escapeDot(i->d_mod->getFullName());
            }
        }
        const QByteArray name = escapeDot(m->getFullName());
        // s << "    " << name << " [shape=box];" << endl;
        s << "    " << name << " -> {";
        bool first = true;
        foreach( const QByteArray& to, names )
        {
            if( !first )
                s << ", ";
            first = false;
            s << to;
        }
        s << "};" << endl;
    }
#endif

    s << "}";
    return true;
}

void Project::addError(const QString& file, const RowCol& pos, const QString& msg)
{
    errors << Error(msg, pos, file);
}

QStringList Project::findFiles(const QDir& dir, bool recursive)
{
    QStringList res;
    QStringList files;

    if( recursive )
    {
        files = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );

        foreach( const QString& f, files )
            res += findFiles( QDir( dir.absoluteFilePath(f) ), recursive );
    }

    QStringList suff = d_suffixes;
    for(int i = 0; i < suff.size(); i++ )
        suff[i] = "*" + suff[i];
    files = dir.entryList( suff, QDir::Files, QDir::Name );
    foreach( const QString& f, files )
    {
        res.append( dir.absoluteFilePath(f) );
    }
    return res;
}

void Project::touch()
{
    if( !d_dirty )
    {
        d_dirty = true;
        emit sigModified(d_dirty);
    }
}

Declaration *Project::parse(const QString &path)
{
    Lex lex;
    lex.lex.setStream(path);
#if 0
    // TODO
    if( !file->d_cache.isEmpty() )
    {
        buf.setData(file->d_cache);
        buf.open(QIODevice::ReadOnly);
        lex.lex.setStream(&buf, file->d_filePath);
    }else
        lex.lex.setStream(file->d_filePath);
#endif
    lex.lex.setIgnoreComments(true);
    lex.lex.setPackComments(true);
    Sim::Parser3 p(&lex, &mdl);
    p.RunParser();
    if( !p.errors.isEmpty() )
    {
        foreach( const Sim::Parser3::Error& e, p.errors )
            errors << Error(e.msg, e.pos, e.path);
        return 0;
    }else
        return p.takeResult();
}

bool Project::validate(Declaration * module)
{
    Sim::Validator2 va(&mdl, this, true);
    va.validate(module);
    if( !va.errors.isEmpty() )
    {
        foreach( const Sim::Validator2::Error& e, va.errors )
            errors << Error(e.msg, e.pos, e.path);
        return false;
    }// else

    ModuleSlot* slot = const_cast<ModuleSlot*>(findModule(module));
    if( slot )
    {
        slot->xref = va.takeXref();

        QHash<Declaration*,DeclList>::const_iterator i;
        for( i = slot->xref.subs.begin(); i != slot->xref.subs.end(); ++i )
            subs[i.key()] += i.value();
    }
    if( !dependencyOrder.contains(module) )
        dependencyOrder << module;

    return true;
}

Project::File* Project::toFile(const QString& path)
{
    return d_files.value(path).data();
}

void Project::clearModules()
{
    errors.clear();
    dependencyOrder.clear();
    Modules::const_iterator i;
    for( i = modules.begin(); i != modules.end(); ++i )
    {
        Symbol::deleteAll((*i).xref.syms);
        Declaration::deleteAll((*i).decl);
    }
    modules.clear();
    subs.clear();
    mdl.clear();
    FileHash::const_iterator j;
    for( j = d_files.begin(); j != d_files.end(); ++j )
        j.value()->d_mod = 0;

    Declaration* module = parse(":/runtime/builtins.sim");
    if( module )
    {
        Sim::Validator2 va(&mdl);
        va.validate(module);
        if( !va.errors.isEmpty() )
        {
            foreach( const Sim::Validator2::Error& e, va.errors )
                errors << Error(e.msg, e.pos, e.path);
            Declaration::deleteAll(module);
            return;
        }// else

        Sim::Declaration* decls = module->link;
        module->link = 0;
        Sim::Declaration* d = decls;
        while( d )
        {
            d->outer = mdl.getGlobals();
            d = d->next;
        }
        mdl.getGlobals()->appendMember(decls);
        Declaration::deleteAll(module);
    }
}

const Project::ModuleSlot*Project::findModule(Declaration* m) const
{
    for( int i = 0; i < modules.size(); i++ )
    {
        if( modules[i].decl == m )
            return &modules[i];
    }
    return 0;
}

Declaration *Project::loadExternal(const char *id)
{
    return 0; // TODO
}

bool Project::parse()
{
    clearModules();

    int all = 0, ok = 0;


    // first parse everything to memory
    FileHash::const_iterator i;
    for( i = d_files.begin(); i != d_files.end(); ++i )
    {
        Declaration* module = parse(i.value()->d_filePath);
        all++;
        if( module )
        {
            modules.append(ModuleSlot(i.value()->d_filePath, module));
            i.value()->d_mod = module;
        }
    }

    // then validate and connect everything
    for( i = d_files.begin(); i != d_files.end(); ++i )
    {
        Declaration* module = i.value()->d_mod;
        if( !module->validated )
            validate(module);
    }

    emit sigReparsed();
    return all == ok;
}

bool Project::save()
{
    if( d_filePath.isEmpty() )
        return false;

    QDir dir = QFileInfo(d_filePath).dir();

    QSettings out(d_filePath,QSettings::IniFormat);
    if( !out.isWritable() )
        return false;

    out.setValue("Suffixes", d_suffixes );
    out.setValue("MainModule", d_main.first );
    out.setValue("MainProc", d_main.second );
    out.setValue("WorkingDir", d_workingDir );
    out.setValue("BuildDir", d_buildDir );
    out.setValue("Options", d_options.join(' ') );
    out.setValue("Arguments", d_arguments );

    out.beginWriteArray("Modules", d_files.size() );
    FileHash::const_iterator i;
    int n = 0;
    for( i = d_files.begin(); i != d_files.end(); ++i )
    {
        const QString absPath = i.value()->d_filePath;
        const QString relPath = dir.relativeFilePath( absPath );
        out.setArrayIndex(n++);
        out.setValue("AbsPath", absPath );
        out.setValue("RelPath", relPath );
    }
    out.endArray();

    d_dirty = false;
    emit sigModified(d_dirty);
    return true;
}

bool Project::loadFrom(const QString& filePath)
{
    clear();

    d_filePath = filePath;

    QDir dir = QFileInfo(d_filePath).dir();

    QSettings in(d_filePath,QSettings::IniFormat);

    d_suffixes = in.value("Suffixes").toStringList();
    d_main.first = in.value("MainModule").toByteArray();
    d_main.second = in.value("MainProc").toByteArray();
    d_workingDir = in.value("WorkingDir").toString();
    d_buildDir = in.value("BuildDir").toString();
    d_options = in.value("Options").toByteArray().split(' ');
    d_arguments = in.value("Arguments").toStringList();

    int count = in.beginReadArray("Modules");
    for( int i = 0; i < count; i++ )
    {
        in.setArrayIndex(i);
        QString absPath = in.value("AbsPath").toString();
        const QString relPath = in.value("RelPath").toString();
        if( QFileInfo(absPath).exists() )
            addFile(absPath);
        else
        {
            absPath = dir.absoluteFilePath(relPath);
            QFileInfo info(absPath);
            if( info.exists() && info.isFile() )
                addFile(absPath);
            else
                qCritical() << "Could not open module" << relPath;
        }
    }
    in.endArray();

    d_dirty = false;
    emit sigModified(d_dirty);
    emit sigRenamed();
    return true;
}

bool Project::saveTo(const QString& filePath)
{
    d_filePath = filePath;
    const bool res = save();
    emit sigRenamed();
    return res;
}

