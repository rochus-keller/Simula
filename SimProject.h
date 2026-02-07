#ifndef OBXPROJECT_H
#define OBXPROJECT_H

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

#include <QObject>
#include <QStringList>
#include <QExplicitlySharedDataPointer>
#include <Simula/SimAst.h>

class QDir;
class QBuffer;

namespace Sim
{
    class Project : public QObject, public Loader
    {
#ifndef QT_NO_QOBJECT
        Q_OBJECT
#endif
    public:
        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error( const QString& m, const RowCol& pos, const QString& p):msg(m),pos(pos),path(p){}
        };

        struct File : public QSharedData
        {
            QString d_filePath;
            QByteArray d_name;
            QByteArray d_cache;
            Declaration* d_mod;
            bool d_isLib;
            File():d_isLib(false),d_mod(0){}
        };
        typedef QExplicitlySharedDataPointer<File> FileRef;


        typedef QHash<QString,FileRef> FileHash; // FilePath -> File
        typedef QList<FileRef> FileList;
        typedef QPair<QByteArray,QByteArray> ModProc; // module.procedure or just module

        explicit Project(QObject *parent = 0);
        ~Project();

        void clear();

        void createNew();
        bool initializeFromDir( const QDir&, bool recursive = false );
        bool loadFrom( const QString& filePath );
        bool save();
        bool saveTo(const QString& filePath );
        void setSuffixes( const QStringList& ); // Form: ".suffix"
        const QStringList& getSuffixes() const { return d_suffixes; }
        const QString& getProjectPath() const { return d_filePath; }
        bool isDirty() const { return d_dirty; }

        void setMain( const ModProc& );
        const ModProc& getMain() const { return d_main; }
        QString renderMain() const;
        QString getWorkingDir(bool resolved = false) const;
        void setWorkingDir( const QString& );
        QString getBuildDir(bool resolved = false) const;
        void setBuildDir( const QString& );
        QByteArrayList getOptions() const { return d_options; }
        void setOptions( const QByteArrayList& );
        QStringList getArguments() const { return d_arguments; }
        void setArguments( const QStringList& );

        bool addFile(const QString& filePath);
        bool removeFile( const QString& filePath );

        bool parse();

        const FileHash& getFiles() const { return d_files; }
        File* findFile( const QString& file ) const;

        Symbol* findSymbolBySourcePos(const QString& file, quint32 line, quint16 col, Declaration** = 0 ) const;
        Symbol* findSymbolByModule(Declaration*, quint32 line, quint16 col, Declaration** scopePtr = 0) const;
        typedef QList<QPair<Declaration*, SymList> > UsageByMod;
        UsageByMod getUsage( Declaration* ) const;
        Symbol* getSymbolsOfModule(Declaration*) const;
        DeclList getSubs(Declaration*) const;
        DeclList getDependencyOrder() const { return dependencyOrder; }

        bool printTreeShaken( const QString& module, const QString& fileName );
        bool printImportDependencies(const QString& fileName , bool pruned);

        const QList<Error>& getErrors() const {return errors; }
        void addError(const QString& file, const RowCol& pos, const QString& msg);
    signals:
        void sigModified(bool);
        void sigRenamed();
        void sigReparsed();
    protected:
        QStringList findFiles(const QDir& , bool recursive = false);
        void touch();
        Declaration* parse(const QString& imp);
        bool validate(Declaration*);

        struct ModuleSlot
        {
            QString file;
            Declaration* decl;
            Xref xref;
            ModuleSlot():decl(0) {}
            ModuleSlot( const QString& f, Declaration* d):file(f),decl(d){}
        };
        File* toFile(const QString& path);
        void clearModules();
        const ModuleSlot* findModule(Declaration*) const;
        Declaration* loadExternal(const char* id);
    private:
        AstModel mdl;
        typedef QList<ModuleSlot> Modules;
        Modules modules;
        DeclList dependencyOrder;
        QHash<Declaration*, DeclList> subs;
        QList<Error> errors;
        FileHash d_files;
        QString d_filePath; // path where the project file was loaded from or saved to
        QStringList d_suffixes;
        QByteArrayList d_options;
        QStringList d_arguments;
        QString d_workingDir, d_buildDir;
        ModProc d_main;
        bool d_dirty;
    };
}

Q_DECLARE_METATYPE(Sim::Project::File*)

#endif // OBXPROJECT_H
