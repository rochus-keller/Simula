#ifndef SIMLEXER_H
#define SIMLEXER_H

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

#include <QObject>
#include <Simula/SimToken.h>
#include <QHash>

class QIODevice;

namespace Sim
{
    class Errors;

    class Lexer : public QObject
    {
    public:
        explicit Lexer(QObject *parent = 0);

        void setStream( QIODevice*, const QString& sourcePath );
        bool setStream(const QString& sourcePath);
        void setIgnoreComments( bool b ) { d_ignoreComments = b; }
        void setPackComments( bool b ) { d_packComments = b; }

        const QString& sourcePath() const { return d_sourcePath; }

        Token nextToken();
        Token peekToken(quint8 lookAhead = 1);
        TokenList tokens( const QString& code );
        TokenList tokens( const QByteArray& code, const QString& path = QString() );
        static const char *toId( const QByteArray& );
        static bool isValidIdent( const QByteArray& str );
    protected:
        Token nextTokenImp();
        int skipWhiteSpace();
        void nextLine();
        QChar lookAhead(int off = 1) const;
        Token token(TokenType tt, int len = 1, const QByteArray &val = QByteArray());
        Token identifier();
        Token number();
        Token comment();
        Token comment2();
        Token string();
        Token character();
        int exponential_part(int off);
        int decimal_fraction(int off);
    private:
        QIODevice* d_in;
        quint32 d_lineNr;
        quint16 d_colNr;
        QString d_sourcePath;
        QString d_line;
        TokenList d_buffer;
        static QHash<QByteArray,QByteArray> d_symbols;
        Token d_lastToken;
        bool d_ignoreComments;  // don't deliver comment tokens
        bool d_packComments;    // Only deliver one Tok_Comment for (*...*) instead of Tok_Latt and Tok_Ratt
    };
}

#endif // SIMLEXER_H
