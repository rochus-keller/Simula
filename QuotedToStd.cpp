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
#include <QStringList>
#include <QtDebug>

static int line = 1;

static char next( QFile& in )
{
    char ch;
    in.getChar(&ch);
    if( ch == '\n' )
        line++;
    return ch;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if( a.arguments().size() < 2 )
        return -1;
    QFile in( a.arguments()[1] );
    if( !in.open(QIODevice::ReadOnly) )
        return -1;

    QFile out( in.fileName() + ".done" );
    if( !out.open(QIODevice::WriteOnly) )
        return -1;

    while( !in.atEnd() )
    {
        char ch = next(in);
        if( ch == '"' )
        {
            char ch2 = next(in);
            if( ch2 == '"' )
            {
                // in comment
                out.putChar(ch2);
                while( !in.atEnd() )
                {
                    ch = next(in);
                    if( ch == '"' )
                    {
                        // potential end of string
                        ch2 = next(in);
                        if( ch2 == '"' )
                        {
                            // confirmed end of string
                            out.putChar(ch2);
                            break;
                        }else
                        {
                            out.putChar('\'');
                            out.putChar(ch2);
                            qWarning() << "embedded \" in string line" << line;
                        }
                    }else
                        out.putChar(ch);
                }
            }else
            {
                // in keyword
                out.putChar(ch2);
                while( !in.atEnd() )
                {
                    ch = next(in);
                    if( ch == '"' )
                    {
                        // end of keyword
                        in.peek(&ch,1);
                        if( ch == '"' )
                            out.putChar(' ');
                        break;
                    }else
                        out.putChar(ch);
                }
            }
        }else if( ch == '$' )
            out.putChar('\'');
        else
            out.putChar(ch);
    }

    return 0;
}
