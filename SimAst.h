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
    class Connection;

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
        uint inList : 1;
        uint ownstype : 1;
        uint owned : 1;
        // 6

        // Declaration
        uint visi : 2;
        uint mode : 2;
        uint isVirtual : 1;
        uint id : 16;
        // 21

        // Statement
        uint re : 1;               // reactivate
        uint prior: 1;
        // 2

        // Type
        uint ownsexpr : 1;
        // 1

        // all 30

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
            Integer, ShortInteger, Real, LongReal, Boolean, Character, Label, Text,
            MaxBasicType,
            Ref, Pointer, Array, Procedure, Switch
        };
        static const char* name[];

        Kind kind;

        bool isArithmetic() const;
        bool isInteger() const { return kind == Integer || kind == ShortInteger; }
        bool isReal() const { return kind == Real || kind == LongReal; }
        bool isRef() const { return kind == Ref || kind == Text; }

        Type(Kind k = Undefined);
        ~Type();

        void setExpr(Expression* e);
        Expression* getExpr() const { return expr; }
    private:
        // For Array: 'expr' is the head of a linked list of bound pairs (Expression nodes)
        // For String/Text: 'expr' is the length
        Expression* expr; // Dimension or Length, owned TODO: many Type might point to the same expr
    };

    class Declaration : public Node
    {
    public:
        enum Kind {
            Invalid, Module, Program, Class, Procedure, Block, Variable, Array, Switch,
            Parameter, VirtualSpec, External, LabelDecl, Builtin, Import, StandardClass
        };
        enum ParamMode { ModeDefault, ModeValue, ModeName };
        enum Visi { NA, Hidden, Protected };

        Kind kind;
        QByteArray name;
        
        Declaration* link;   // Members/Locals
        Declaration* next;   // Next in scope
        Declaration* outer;  // Parent scope
        Statement*   body;   // class, procedure

        union {
            // Class / Procedure
            Declaration* prefix; // Superclass, not owned

            // Module
            QString* data; // Source path, Class prefix name, External ext name, owned

            // Switch
            Expression* list; // Chain of label expressions

            // Variable / Parameter / Const
            Expression* init; // Initialization expression, owned
        };


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
            // Helper for Loop Elements
            StepUntil, // lhs=start, rhs=step, condition=until
            WhileLoop, // lhs=start, condition=cond
            TypeRef,   // val = type name
            MAX
        };

        Kind kind;
        QVariant val;
        Expression* lhs;
        Expression* rhs;
        Expression* next; // For lists (Args, Switch list, For list, Array bounds)
        Expression* condition; // For IfExpr, StepUntil, WhileLoop

        Expression(Kind k = Invalid, const RowCol& rc = RowCol());
        ~Expression();

        static void append(Expression* list, Expression* elem);
    };

    struct ActivateData
    {
        Expression* obj;
        Expression* at;
        Expression* delay;
        Expression* priorObj;
        ActivateData():obj(0),at(0),delay(0),priorObj(0){}
        ~ActivateData() {
            if(obj) delete obj;
            if(at) delete at;
            if(delay) delete delay;
            if(priorObj) delete priorObj;
        }
    };

    struct BlockPrefix
    {
        Expression* prefix;    // Class prefix (for Block)
        Expression* args;      // Arguments for prefix (linked list of Expressions)
        BlockPrefix():prefix(0),args(0){}
        ~BlockPrefix() {
            if( prefix ) delete prefix;
            if( args ) delete args;
        }
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
        Statement* next;
        Statement* body;  // If, While, For, Compound, Block, Inspect(otherwise)

        union {
            // Compound / Block
            struct {
                Declaration* scope;     // scope local declarations
                BlockPrefix* prefix;    // optional prefix
            };

            // If / While
            struct {
                Expression* cond;
                Statement* elseStmt;   // For If
            };

            // For
            struct {
                Expression* var;       // Control variable
                Expression* list;      // Linked list of 'For' elements (Expression nodes)
            };

            // Inspect
            struct {
                Expression* obj;       // Inspect object
                Connection* conn;      // Chain of WHEN clauses
            };

            // Activate, Reactivate
            ActivateData* activate;

            // Assign / Call / Detach / Resume / Goto
            struct {
                 Expression* lhs;      // Target / Callable
                 Expression* rhs;      // Value / Ref / Args
            };

        };

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
    
    class AstModel
    {
    public:
        AstModel(SimulaVersion v = Sim86);
        ~AstModel();

        void openScope(Declaration* scope);
        Declaration* closeScope();
        Declaration* addDecl(const QByteArray& name, Declaration::Kind k);
        Declaration* currentScope() const;
        Declaration* getTopScope() const;
        Type* getType(Type::Kind k) const;
        
    private:
        Type* newType(Type::Kind k);
        SimulaVersion version;
        QList<Declaration*> scopes;
        Declaration* globalScope;
        Type* basicTypes[Type::MaxBasicType];
        
        void initBuiltins();
    };
}

Q_DECLARE_METATYPE(Sim::Declaration*)
Q_DECLARE_METATYPE(Sim::Expression*)

#endif // SIMAST_H
