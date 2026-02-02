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
#include "SimErrors.h"
#include "SimParser.h"
#include "SimParser3.h"
#include "SimAst.h"
//#include "SimValidator.h"

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

static void dumpTree( Sim::SynTree* node, int level = 0)
{
    QByteArray str;
    if( node->d_tok.d_type == Sim::Tok_Invalid )
        level--;
    else if( node->d_tok.d_type < Sim::SynTree::R_First )
    {
        if( Sim::tokenTypeIsKeyword( node->d_tok.d_type ) )
            str = Sim::tokenTypeString(node->d_tok.d_type);
        else if( node->d_tok.d_type > Sim::TT_Specials )
            str = QByteArray("\"") + node->d_tok.d_val + QByteArray("\"");
        else
            str = QByteArray("\"") + node->d_tok.getString() + QByteArray("\"");

    }else
        str = Sim::SynTree::rToStr( node->d_tok.d_type );
    if( !str.isEmpty() )
    {
        str += QByteArray("\t") /* + QFileInfo(node->d_tok.d_sourcePath).baseName().toUtf8() +
                ":" */ + QByteArray::number(node->d_tok.d_lineNr) +
                ":" + QByteArray::number(node->d_tok.d_colNr);
        QByteArray ws;
        for( int i = 0; i < level; i++ )
            ws += "|  ";
        str = ws + str;
        qDebug() << str.data();
    }
    foreach( Sim::SynTree* sub, node->d_children )
        dumpTree( sub, level + 1 );
}

class Lex : public Sim::Scanner {
public:
    Sim::Lexer lex;
    virtual Sim::Token next() { return lex.nextToken(); }
    virtual Sim::Token peek(int offset) { return lex.peekToken(offset); }
    virtual QString source() const { return lex.sourcePath(); }
};

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

    Sim::Errors err;
    err.setReportToConsole(true);

    Sim::AstModel mdl;
    foreach( const QString& path, files )
    {
        qDebug() << "processing" << path;

#if 0
        Sim::Lexer lex;
        lex.setStream(path);
        lex.setErrors(&err);
        lex.setIgnoreComments(true);
        lex.setPackComments(true);
    #if 0
        Sim::Token t = lex.nextToken();
        while( t.isValid() )
        {
            qDebug() << t.getString() << QString::fromUtf8(t.d_val);
            t = lex.nextToken();
        }
    #else
        Sim::Parser p(&lex,&err);
        p.Parse();
        if( dump )
            dumpTree( &p.d_root );
    #endif
#else
        Lex lex;
        lex.lex.setStream(path);
        lex.lex.setErrors(&err);
        lex.lex.setIgnoreComments(true);
        lex.lex.setPackComments(true);
        Sim::Parser3 p(&lex, &mdl);
        Sim::Declaration* module = p.RunParser();
        if( !p.errors.isEmpty() )
        {
            foreach( const Sim::Parser3::Error& e, p.errors )
                qCritical() << e.path << e.pos.d_row << e.pos.d_col << e.msg;
        }
#if 0
        else
        {
            Sim::Validator va(&mdl);
            va.validate(module);
            if( !va.errors.isEmpty() )
            {
                foreach( const Sim::Validator::Error& e, va.errors )
                    qCritical() << e.path << e.pos.d_row << e.pos.d_col << e.msg;
            }
        }
#endif
#endif

    }
#if 0
    // TODO
    if( err.getErrCount() == 0 && err.getWrnCount() == 0 )
        qDebug() << "successfully completed";
    else
        qDebug() << "completed with" << err.getErrCount() << "errors and" <<
                    err.getWrnCount() << "warnings";
#endif
    return 0;
}
