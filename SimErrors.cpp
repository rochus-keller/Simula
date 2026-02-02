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

#include "SimErrors.h"
#include "SimSynTree.h"
#include <QtDebug>
#include <QFileInfo>
using namespace Sim;

Errors::Errors(QObject *parent) :
    QObject(parent),
    d_numOfErrs(0),d_numOfSyntaxErrs(0),d_numOfWrns(0),d_showWarnings(true),
    d_reportToConsole(false),d_record(false)
{

}

void Errors::error(Errors::Source s, const SynTree* st, const QString& msg)
{
    Q_ASSERT( st != 0 );
    error( s, st->d_tok.d_sourcePath, st->d_tok.d_lineNr, st->d_tok.d_colNr, msg );
}

void Errors::error(Errors::Source s, const QString& file, int line, int col, const QString& msg)
{
    Entry e;
    e.d_col = col;
    e.d_line = line;
    e.d_msg = msg;
    e.d_source = s;
    e.d_file = file;
    e.d_kind = Error;
    if( d_record )
    {
        d_entries.append(e);
    }
    if( d_reportToConsole )
        log(e,true);
    if( !d_reportToConsole )
    {

        d_numOfErrs++;
        if( s == Syntax )
            d_numOfSyntaxErrs++;
    }
}

void Errors::warning(Errors::Source s, const SynTree* st, const QString& msg)
{
    Q_ASSERT( st != 0 );
    warning( s, st->d_tok.d_sourcePath, st->d_tok.d_lineNr, st->d_tok.d_colNr, msg );
}

void Errors::warning(Errors::Source s, const QString& file, int line, int col, const QString& msg)
{
    if( d_showWarnings )
    {
        Entry e;
        e.d_col = col;
        e.d_line = line;
        e.d_msg = msg;
        e.d_source = s;
        e.d_file = file;
        e.d_kind = Warning;
        if( d_record )
        {
            d_entries.append(e);
        }
        if( d_reportToConsole  )
            log(e,false);
    }
    d_numOfWrns++;
}

bool Errors::showWarnings() const
{
    const bool res = d_showWarnings;
    return res;
}

void Errors::setShowWarnings(bool on)
{
    d_showWarnings = on;
}

bool Errors::reportToConsole() const
{
    const bool res = d_reportToConsole;
    return res;
}

void Errors::setReportToConsole(bool on)
{
    d_reportToConsole = on;
}

bool Errors::record() const
{
    const bool res = d_record;
    return res;
}

void Errors::setRecord(bool on)
{
    d_record = on;
}

quint32 Errors::getErrCount() const
{
    const quint32 res = d_numOfErrs;
    return res;
}

quint32 Errors::getWrnCount() const
{
    const quint32 res = d_numOfWrns;
    return res;
}

void Errors::clear()
{
    d_numOfErrs = 0;
    d_numOfWrns = 0;
    d_numOfSyntaxErrs = 0;
    d_entries.clear();
}

const char* Errors::sourceName(int s)
{
    switch(s)
    {
    case Lexer:
        return "Lexer";
    case Syntax:
        return "Syntax";
    case Semantics:
        return "Semantics";
    default:
        return "";
    }
}

void Errors::log(const Errors::Entry& e, bool isErr)
{
    if( isErr )
        qCritical() << d_root.relativeFilePath(e.d_file) << ":" << e.d_line << ":" << e.d_col << ": error:" <<
                       e.d_msg.toUtf8().data();
    else
        qWarning() << d_root.relativeFilePath(e.d_file) << ":" << e.d_line << ":" << e.d_col << ": warning:" <<
                      e.d_msg.toUtf8().data();

}
