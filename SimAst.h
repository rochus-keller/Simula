#ifndef SIMAST_H
#define SIMAST_H

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

#include <QByteArray>
#include <QList>
#include <QVariant>
#include "SimRowCol.h"

namespace Sim
{
    enum SimulaVersion { Sim70, Sim75, Sim86 };

    class Declaration;
    class Type;
    class Statement;
    class Expression;

    struct Builtin
    {
        enum Kind {
            ABS, SIGN, ENTIER, MOD, REM, SQRT, SIN, COS, ARCTAN, LN, EXP,
            LOWTEN, COPY, TEXT_START, TEXT_POS, TEXT_LENGTH, TEXT_SUB, TEXT_STRIP,
            ISO_CHAR, ISO_RANK, RANK, CHAR_FUNC, DETACH, RESUME, CALL, TIME, SOURCELINE,
            Max
        };
        static const char* name[];
    };

    class Node
    {
    public:
        enum Meta { T, D, E, S, C }; // Type, Declaration, Expression, Statement, Connection
        uint meta : 3;
        uint validated : 1;
        uint inList : 1;
        uint hasErrors : 1;
        uint ownstype : 1;
        uint allocated : 1;
        uint owned : 1;

        RowCol pos;

        Type* type() const { return _ty; }
        void setType(Type*);
        
        Node(Meta m);
        virtual ~Node();
    private:
        Type* _ty;
    };

    class Type : public Node
    {
    public:
        enum Kind {
            Undefined, NoType,
            Integer, ShortInteger, Real, LongReal, Boolean, Character,
            Text, Ref, Pointer, Array, Procedure, Label, Switch,
            MaxBasicType
        };
        static const char* name[];

        Kind kind;
        Expression* expr; // Dimension or Length
        Declaration* decl; // For Ref/Proc/Array
        QList<Declaration*> subs; // Record fields or Params

        bool isArithmetic() const;
        bool isInteger() const { return kind == Integer || kind == ShortInteger; }
        bool isReal() const { return kind == Real || kind == LongReal; }
        bool isRef() const { return kind == Ref || kind == Text; }

        Type(Kind k = Undefined);
        ~Type();
    };

    class Declaration : public Node
    {
    public:
        enum Kind {
            Invalid, Scope, Module, Class, Procedure, Variable, Array, Switch,
            Parameter, VirtualSpec, External, LabelDecl, Builtin, Import, StandardClass
        };
        enum ParamMode { ModeDefault, ModeValue, ModeName };
        enum Visi { NA, Hidden, Protected };

        Kind kind;
        QByteArray name;
        
        Declaration* link;   // Members/Locals
        Declaration* next;   // Next in scope
        Declaration* outer;  // Parent scope
        Declaration* prefix; // Superclass
        
        Statement* body;
        
        uint visi : 2;
        uint mode : 2;
        uint id : 16;
        uint isVirtual : 1;

        QVariant data;

        Declaration(Kind k = Invalid);
        ~Declaration();
        
        Declaration* find(const QByteArray& name, bool recursive = true) const;

        void appendMember(Declaration* d);
        static void deleteAll(Declaration* d);
    };

    class Expression : public Node
    {
    public:
        enum Kind {
            Invalid,
            Plus, Minus, Mul, Div, IntDiv, Exp,
            And, Or, Not, Imp, Eqv,
            Eq, Neq, Lt, Leq, Gt, Geq, RefEq, RefNeq,
            Identifier, DeclRef, Dot, Subscript,
            New, This, Qua,
            TextConst, CharConst, NumConst,
            IfExpr, Call, AssignVal, AssignRef,
            MAX
        };

        Kind kind;
        QVariant val;
        Expression* lhs;
        Expression* rhs;
        Expression* next;
        Expression* condition; 

        Expression(Kind k = Invalid, const RowCol& rc = RowCol());
        ~Expression();

        static void append(Expression* list, Expression* elem);
    };

    class Statement : public Node
    {
    public:
        enum Kind {
            Invalid, Compound, Block, Assign, Call,
            If, While, For, Inspect, Goto,
            Activate, Detach, Resume, Inner, Dummy, End
        };

        Kind kind;
        Expression* expr; // Cond, LHS, Inspect Object
        Statement* body;  // Then, Do, Compound list
        Statement* elseStmt; 
        
        // Inspect connections or For-loops iterators (stored as special statements or expressions)
        Statement* connections; 
        
        // Activate
        bool isReactivate;
        Expression* atExpr;
        Expression* delayExpr;
        Expression* beforeAfterExpr;
        bool prior;

        Statement* next;

        Statement(Kind k = Invalid, const RowCol& p = RowCol());
        ~Statement();
        
        void append(Statement* s);
        static void deleteAll(Statement* s);
    };

    class Connection : public Node {
    public:
        QByteArray className;
        Declaration* classDecl; 
        Statement* body;
        Connection* next;
        
        Connection();
        ~Connection();
    };

    struct ModuleData {
        QString sourcePath;
        QByteArray fullName;
    };
    
    class AstModel
    {
    public:
        AstModel(SimulaVersion v = Sim86);
        ~AstModel();

        void openScope(Declaration* scope);
        Declaration* closeScope(bool takeMembers = false);
        Declaration* addDecl(const QByteArray& name, Declaration::Kind k);
        Declaration* currentScope() const;
        Declaration* getTopScope() const;
        Type* getType(Type::Kind k) const;
        Type* newType(Type::Kind k);
        
    private:
        SimulaVersion version;
        QList<Declaration*> scopes;
        Declaration* globalScope;
        Type* basicTypes[Type::MaxBasicType];
        
        void initBuiltins();
    };
}

Q_DECLARE_METATYPE(Sim::Declaration*)
Q_DECLARE_METATYPE(Sim::Expression*)
Q_DECLARE_METATYPE(Sim::ModuleData)

#endif // SIMAST_H
