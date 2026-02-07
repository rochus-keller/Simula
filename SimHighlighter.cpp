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

#include "SimHighlighter.h"
#include "SimLexer.h"
#include <QBuffer>
using namespace Sim;

static const char* s_reserved[] = {
    // from 1984 Common Base Language
    "FILE", "infile", "outfile", "directfile", "printfile",
    "SIMSET", "Linkage", "link", "head", "SIMULATION", "process",
    "histo", "terminate_program",
    "draw", "randint", "uniform", "normal", "negexp", "Poisson", "Erlang", "discrete", "linear", "histd",

    "mod", "rem", "abs", "sign", "entier", "addepsilon", "subepsilon",
    "copy", "blanks", "char", "isochar", "rank", "isorank", "digit", "letter", "lowten", "decimalmark", "upcase", "lowcase",
    "call", "resume",
    "sqrt", "sin", "cos", "tan", "cotan", "arcsin", "arccos", "arctan", "arctan2", "sinh", "cosh", "tanh", "ln", "log10", "exp",
    "max", "min",
    "error",
    "upperbound", "lowerbound",
    "datetime", "cputime", "clocktime",
    // "sysin", "sysout", "file", "imagefile", "bytefile", "infile", "outfile", "directfile", "printfile", "inbytefile", "outbytefile", "directbytefile",
    "endfile", "open", "close",
    "outtext", "outchar", "outimage", "outrecord", "breakoutimage", "inimage", "inrecord", "inchar", "intext",
    "Detach",

    // from IBM/360 Simula Users Guide (1975 edition)
    "ABS", "ARCCOS", "ARCSIN", "ARCTAN", "COS", "COSH", "EXP", "LN", "SIN", "SINH", "SQRT", "TAN", "TANH",
    "ENTIER", "MOD", "SIGN",
    "DIGIT", "LETTER", "CHAR", "RANK",
    "COPY", "BLANKS",
    "GETCHAR", "GETFRAC", "GETINT", "GETREAL", "LENGTH", "MAIN", "MORE", "POS", "PUTCHAR", "PUTFIX",
    "PUTFRAC", "PUTINT", "PUTREAL", "SETPOS", "STRIP", "SUB",
    "CLOSE", "OPEN", "OUTCHAR", "OUTFIX", "OUTFRAC", "OUTIMAGE", "OUTINT", "OUTREAL", "OUTTEXT",
    "ENDFILE", "INCHAR", "INFRAC", "INIMAGE", "ININT", "INREAL", "INTEXT",
    "CALL", "DETACH", "RESUME",

    0

};
Highlighter::Highlighter(QTextDocument* parent) :
    QSyntaxHighlighter(parent),d_enableExt(true)
{
    for( int i = 0; i < C_Max; i++ )
    {
        d_format[i].setFontWeight(QFont::Normal);
        d_format[i].setForeground(Qt::black);
        d_format[i].setBackground(Qt::transparent);
    }
    d_format[C_Num].setForeground(QColor(0, 153, 153));
    d_format[C_Str].setForeground(QColor(208, 16, 64));
    d_format[C_Cmt].setForeground(QColor(153, 153, 136));
    d_format[C_Kw].setForeground(QColor(68, 85, 136));
    d_format[C_Kw].setFontWeight(QFont::Bold);
    d_format[C_Op].setForeground(QColor(153, 0, 0));
    d_format[C_Op].setFontWeight(QFont::Bold);
    d_format[C_Type].setForeground(QColor(153, 0, 115));
    d_format[C_Type].setFontWeight(QFont::Bold);
    d_format[C_Pp].setForeground(QColor(0, 134, 179));
    d_format[C_Pp].setFontWeight(QFont::Bold);

    d_format[C_Section].setForeground(QColor(0, 128, 0));
    d_format[C_Section].setBackground(QColor(230, 255, 230));

    d_builtins = createBuiltins(true);
}

void Highlighter::setEnableExt(bool b)
{
    const bool old = d_enableExt;
    d_enableExt = b;
    if( old != b )
    {
        d_builtins = createBuiltins(d_enableExt);
        rehighlight();
    }
}

QTextCharFormat Highlighter::formatForCategory(int c) const
{
    return d_format[c];
}

QSet<QByteArray> Highlighter::createBuiltins(bool withLowercase)
{
    QSet<QByteArray> res;
    int i = 0;
    while( s_reserved[i] )
    {
        res << QByteArray(s_reserved[i]).toUpper();
        if( false ) // withLowercase )
            res << QByteArray(s_reserved[i]).toLower();
        i++;
    }
    return res;
}

void Highlighter::highlightBlock(const QString& text)
{
    static const QRegExp commentEnd("\\b(END|ELSE|WHEN|OTHERWISE)\\b|;", Qt::CaseInsensitive);

    const int previousBlockState_ = previousBlockState();
    int lexerState = 0, initialBraceDepth = 0;
    if (previousBlockState_ != -1) {
        lexerState = previousBlockState_ & 0xff;
        initialBraceDepth = previousBlockState_ >> 8;
    }

    int braceDepth = initialBraceDepth;

    if( text.startsWith('%') )
    {
        setFormat( 0, text.size(), formatForCategory(C_Cmt) );
        return;
    }

    int start = 0;
    if( lexerState > 0 )
    {
        // wir sind in einem Multi Line Comment
        // suche das Ende
        QTextCharFormat f = formatForCategory(C_Cmt);
        f.setProperty( TokenProp, int(Tok_Comment) );
        int pos = lexerState == 1 ? text.indexOf(";") : text.indexOf(commentEnd);
        if( pos == -1 )
        {
            // the whole block ist part of the comment
            setFormat( start, text.size(), f );
            setCurrentBlockState( (braceDepth << 8) | lexerState);
            return;
        }else
        {
            // End of Comment found; semi is not part of comment
            setFormat( start, pos , f );
            lexerState = 0;
            braceDepth--;
            start = pos;
        }
    }


    Sim::Lexer lex;
    lex.setIgnoreComments(false);
    lex.setPackComments(false);

    QList<Token> tokens =  lex.tokens(text.mid(start));
    for( int i = 0; i < tokens.size(); ++i )
    {
        Token &t = tokens[i];
        t.d_colNr += start;

        QTextCharFormat f;
        if( t.d_type == Tok_Comment )
        {
            f = formatForCategory(C_Cmt);
            if( i == tokens.size() - 1 )
            {
                // enable multi line comment
                // if there is another token following the comment then it cannot be multi line
                braceDepth++;
                if( i > 0 && tokens[i-1].d_type == Tok_COMMENT )
                    lexerState = 1;
                else
                    lexerState = 2; // after end comment
            }
        }/*else if( t.d_type == Tok_Semi && lexerState > 0 )
        {
            // not needed
            braceDepth--;
            f = formatForCategory(C_Op);
            lexerState = 0;
        }*/else if( t.d_type == Tok_string || t.d_type == Tok_character )
            f = formatForCategory(C_Str);
        else if( t.d_type == Tok_decimal_number || t.d_type == Tok_unsigned_integer )
            f = formatForCategory(C_Num);
        else if( tokenTypeIsLiteral(t.d_type) )
        {
            f = formatForCategory(C_Op);
        }else if( tokenTypeIsKeyword(t.d_type) )
        {
            f = formatForCategory(C_Kw);
        }else if( t.d_type == Tok_identifier )
        {
            if( i < tokens.size() - 1 && tokens[i+1].d_type == Tok_Colon )
                f = formatForCategory(C_Section);
            else if( d_builtins.contains(t.d_val.toUpper()) )
                f = formatForCategory(C_Type);
            else
                f = formatForCategory(C_Ident);
        }

        if( f.isValid() )
        {
            setFormat( t.d_colNr-1, t.d_len, f );
        }
    }

    setCurrentBlockState((braceDepth << 8) | lexerState );
}



