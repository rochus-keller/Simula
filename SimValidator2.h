#ifndef __SIM_VALIDATOR2__
#define __SIM_VALIDATOR2__

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
#include <QList>
#include <QHash>

namespace Sim {

    struct Symbol {
        enum Kind { Decl, Use, Module, Subclass };
        Declaration* decl;
        RowCol pos;
        quint16 len;
        quint8 kind;
        Symbol* next;
        
        Symbol() : decl(0), len(0), kind(Use), next(0) {}
        static void deleteAll(Symbol* s) {
            while (s) {
                Symbol* n = s->next;
                delete s;
                s = n;
            }
        }
    };
    
    struct Xref {
        Symbol* syms;
        QHash<Declaration*, QList<Symbol*> > uses;
        QHash<Declaration*, QList<Declaration*> > subs;
        
        Xref() : syms(0) {}
    };

    class Validator2 {
    public:
        Validator2(AstModel* mdl, bool haveXref = false);
        ~Validator2();
        
        bool validate(Declaration* module);
        Xref takeXref();
        
        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error(const QString& m, const RowCol& rc, const QString& p)
                : msg(m), pos(rc), path(p) {}
        };
        mutable QList<Error> errors;
        
    protected:
        void Module(Declaration* module);
        void Decl(Declaration* d);
        void DeclSeq(Declaration* d);
        void ClassDecl(Declaration* d);
        void ProcDecl(Declaration* d);
        void VarDecl(Declaration* d);
        void ArrayDecl(Declaration* d);
        void SwitchDecl(Declaration* d);
        void ParamDecl(Declaration* d);
        void ExternalDecl(Declaration* d);
        void BlockDecl(Declaration* d);
        void LabelDecl(Declaration* d);
        
        void Type_(Type* t);
        
        void Body(Statement* s);
        void StatSeq(Statement* s);
        Statement* Stat(Statement* s);
        void BlockStat(Statement* s);
        void CompoundStat(Statement* s);
        void IfStat(Statement* s);
        void WhileStat(Statement* s);
        void ForStat(Statement* s);
        void InspectStat(Statement* s);
        void GotoStat(Statement* s);
        void AssignStat(Statement* s);
        void CallStat(Statement* s);
        void ActivateStat(Statement* s);
        void DetachStat(Statement* s);
        void ResumeStat(Statement* s);
        
        bool Expr(Expression* e);
        bool ConstExpr(Expression* e);
        bool BinaryOp(Expression* e);
        bool UnaryOp(Expression* e);
        bool Identifier(Expression* e);
        bool DeclRefExpr(Expression* e);
        bool DotExpr(Expression* e);
        bool SubscriptExpr(Expression* e);
        bool CallExpr(Expression* e);
        bool NewExpr(Expression* e);
        bool ThisExpr(Expression* e);
        bool QuaExpr(Expression* e);
        bool IfExpr(Expression* e);
        bool Literal(Expression* e);
        
        bool assigCompat(Type* lhs, Type* rhs, bool isRefAssign);
        bool assigCompat(Type* lhs, Expression* rhs, bool isRefAssign);
        bool typeCompat(Type* t1, Type* t2);
        bool isSubclassOf(Declaration* sub, Declaration* super);
        Type* resultType(Expression::Kind op, Type* lhs, Type* rhs);
        Type* deref(Type* t);
        
        Declaration* resolve(Atom sym);
        
        void checkBuiltinCall(Declaration* builtin, Expression* args, const RowCol& pos);
        
    protected:
        void invalid(const char* what, const RowCol& pos);
        bool error(const RowCol& pos, const QString& msg) const;
        void markDecl(Declaration* d);
        Symbol* markRef(Declaration* d, const RowCol& pos);
        Symbol* markUnref(int len, const RowCol& pos);
        
    private:
        Declaration* module;
        QString sourcePath;
        AstModel* mdl;
        QList<Declaration*> scopeStack;
        Symbol* first;
        Symbol* last;
        QHash<Declaration*, QList<Symbol*> > xref;
        QHash<Declaration*, QList<Declaration*> > subs;
    };
}

#endif // __SIM_VALIDATOR2__
