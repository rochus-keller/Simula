#/*
#* Copyright 2020 Rochus Keller <mailto:me@rochus-keller.ch>
#*
#* This file is part of the Simula67 parser library.
#*
#* The following is the license that applies to this copy of the
#* library. For a license to use the library under conditions
#* other than those described here, please email to me@rochus-keller.ch.
#*
#* GNU General Public License Usage
#* This file may be used under the terms of the GNU General Public
#* License (GPL) versions 2.0 or 3.0 as published by the Free Software
#* Foundation and appearing in the file LICENSE.GPL included in
#* the packaging of this file. Please review the following information
#* to ensure GNU General Public Licensing requirements will be met:
#* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
#* http://www.gnu.org/copyleft/gpl.html.
#*/

HEADERS += \
    $$PWD/SimErrors.h \
    $$PWD/SimFileCache.h \
    $$PWD/SimLexer.h \
    $$PWD/SimParser.h \
    $$PWD/SimParser2.h \
    $$PWD/SimSynTree.h \
    $$PWD/SimToken.h \
    $$PWD/SimTokenType.h

SOURCES += \
    $$PWD/SimErrors.cpp \
    $$PWD/SimFileCache.cpp \
    $$PWD/SimLexer.cpp \
    $$PWD/SimParser.cpp \
    $$PWD/SimParser2.cpp \
    $$PWD/SimSynTree.cpp \
    $$PWD/SimToken.cpp \
    $$PWD/SimTokenType.cpp
