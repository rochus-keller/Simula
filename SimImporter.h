#ifndef __SIM_IMPORTER__
#define __SIM_IMPORTER__

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

#include <Simula/SimAst.h>
#include <QHash>
#include <QStringList>

namespace Sim {

    class SourceImporter : public Importer
    {
    public:
        SourceImporter(AstModel* mdl);
        ~SourceImporter();

        void addSearchPath(const QString& dir);

        Declaration* loadModule( const Import& imp );

        const DeclList& getModules() const { return loaded; }

        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error(const QString& m, const RowCol& rc, const QString& p)
                : msg(m), pos(rc), path(p) {}
        };
        QList<Error> errors;

    protected:
        virtual QString findSource(Atom moduleName) const;
        Declaration* parseModule(const QString& path);

        AstModel* mdl;
        QStringList searchPaths;
        QHash<Atom, Declaration*> cache; // external identifier -> module
        DeclList modules; // the parsed module declarations, owned
        DeclList loaded; // the modules in the order their validation completed
    };
}

#endif // __SIM_IMPORTER__
