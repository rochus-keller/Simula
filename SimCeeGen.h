#ifndef SIMTRANSPILER_H
#define SIMTRANSPILER_H

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

#include "SimAst.h"
#include <QTextStream>
#include <QSet>
#include <QMap>

namespace Sim
{
    class CeeGen
    {
    public:
        CeeGen();
        ~CeeGen();

        // Main entry point: transpile a validated module to C99
        bool transpile(Declaration* module, const QString& outputPath);

        // Get generated C code as string (for testing)
        QString getGeneratedCode() const { return generatedCode; }

        // Error reporting
        struct Error {
            QString msg;
            RowCol pos;
            Error(const QString& m = QString(), const RowCol& p = RowCol())
                : msg(m), pos(p) {}
        };
        QList<Error> errors;

    private:
        // Code generation state
        QString generatedCode;
        QString headerCode;
        QString forwardDecls;
        QString structDefs;
        QString vtableDefs;
        QString funcDefs;
        QString mainCode;
        
        int indentLevel;
        int tempVarCounter;
        int labelCounter;
        
        Declaration* currentModule;
        Declaration* currentClass;
        Declaration* currentProc;
        
        QSet<Declaration*> emittedClasses;
        QSet<Declaration*> emittedProcs;
        QMap<Declaration*, QString> classIdMap;
        QMap<Declaration*, QString> mangledNames;
        
        // Helper methods
        void error(const RowCol& pos, const QString& msg);
        QString indent() const;
        void increaseIndent() { indentLevel++; }
        void decreaseIndent() { if (indentLevel > 0) indentLevel--; }
        
        QString newTempVar(const QString& prefix = "tmp");
        QString newLabel(const QString& prefix = "L");
        
        // Name mangling
        QString mangleName(Declaration* d);
        QString mangleClassName(Declaration* cls);
        QString mangleProcName(Declaration* proc);
        QString mangleVarName(Declaration* var);
        QString mangleTypeName(Type* t);
        
        // Type mapping
        QString mapType(Type* t);
        QString mapBasicType(Type::Kind k);
        bool isBuiltinDecl(Declaration* d);
        bool isBuiltinProc(Declaration* d);
        QString getBuiltinProcName(Declaration* d);
        
        // Code generation - Declarations
        void emitModule(Declaration* mod);
        void emitDeclarations(Declaration* d);
        void emitDeclaration(Declaration* d);
        void emitClass(Declaration* cls);
        void emitClassStruct(Declaration* cls);
        void emitClassVtable(Declaration* cls);
        void emitClassConstructor(Declaration* cls);
        void emitClassBody(Declaration* cls);
        void emitProcedure(Declaration* proc);
        void emitVariable(Declaration* var);
        void emitArray(Declaration* arr);
        void emitSwitch(Declaration* sw);
        void emitParameter(Declaration* param);
        void emitBlock(Declaration* blk);
        
        // Code generation - Statements
        void emitStatement(Statement* s, QTextStream& out);
        void emitStatementSeq(Statement* s, QTextStream& out);
        void emitCompound(Statement* s, QTextStream& out);
        void emitBlockStmt(Statement* s, QTextStream& out);
        void emitAssign(Statement* s, QTextStream& out);
        void emitCallStmt(Statement* s, QTextStream& out);
        void emitIf(Statement* s, QTextStream& out);
        void emitWhile(Statement* s, QTextStream& out);
        void emitFor(Statement* s, QTextStream& out);
        void emitInspect(Statement* s, QTextStream& out);
        void emitGoto(Statement* s, QTextStream& out);
        void emitLabel(Statement* s, QTextStream& out);
        void emitInner(Statement* s, QTextStream& out);
        void emitActivate(Statement* s, QTextStream& out);
        void emitDetach(Statement* s, QTextStream& out);
        void emitResume(Statement* s, QTextStream& out);
        
        // Code generation - Expressions
        QString emitExpr(Expression* e, QTextStream& out);
        QString emitBinaryOp(Expression* e, QTextStream& out);
        QString emitUnaryOp(Expression* e, QTextStream& out);
        QString emitIdentifier(Expression* e, QTextStream& out);
        QString emitDeclRef(Expression* e, QTextStream& out);
        QString emitDot(Expression* e, QTextStream& out);
        QString emitSubscript(Expression* e, QTextStream& out);
        QString emitCall(Expression* e, QTextStream& out);
        QString emitNew(Expression* e, QTextStream& out);
        QString emitThis(Expression* e, QTextStream& out);
        QString emitQua(Expression* e, QTextStream& out);
        QString emitIfExpr(Expression* e, QTextStream& out);
        QString emitLiteral(Expression* e, QTextStream& out);
        QString emitAssignExpr(Expression* e, QTextStream& out);
        
        // Builtin procedure calls
        QString emitBuiltinCall(Declaration* proc, Expression* args, QTextStream& out);
        
        // Class hierarchy helpers
        void collectClassHierarchy(Declaration* cls, QList<Declaration*>& hierarchy);
        Declaration* findVirtualMatch(Declaration* cls, Declaration* vspec);
        bool isSubclassOf(Declaration* sub, Declaration* super);
        int getClassId(Declaration* cls);
        
        // Array helpers
        QString emitArrayBounds(Type* arrType, QTextStream& out);
        QString emitArrayAccess(Expression* arr, Expression* indices, QTextStream& out);
        
        // Text helpers
        QString emitTextOp(Expression* e, QTextStream& out);
        
        // Utility
        void collectForwardDecls(Declaration* d);
        void emitIncludes(QTextStream& out);
        void emitRuntimeDecls(QTextStream& out);
    };
}

#endif // SIMTRANSPILER_H
