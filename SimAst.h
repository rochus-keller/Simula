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

class QTextStream;

namespace Sim
{
    enum SimulaVersion { Sim70, Sim75, Sim86 };

    class Declaration;
    class Type;
    class Statement;
    class Expression;
    class Connection;

    typedef const char* Atom;

    struct Builtin
    {
        enum Kind {
            // Text Handling
            BLANKS, COPY, CHAR, ISOCHAR, RANK, ISORANK, DIGIT,
            LETTER, LOWTEN, DECIMALMARK, UPCASE, LOWCASE,
            // Math Functions (Standard ALGOL 60 + Extensions)
            ABS, SIGN, ENTIER, SQRT, SIN, COS, TAN, COTAN,
            ARCSIN, ARCCOS, ARCTAN, ARCTAN2,
            SINH, COSH, TANH,
            LOG10, LN, EXP, MOD, REM,
            MAX, MIN,
            // Input / Output (Basic)
            ININT, INREAL, INCHAR, INTEXT,
            OUTINT, OUTREAL, OUTCHAR, OUTTEXT, OUTIMAGE,
            SYSIN, SYSOUT, OUTFIX,
            // Control & System
            ERROR, TIME, RANDOM, SOURCELINE, ELAPSED,
            // Scheduling
            DETACH, RESUME, CALL,
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
        uint validated : 1;
        // 7

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

        // all 31

        RowCol pos;

        Type* type() const { return _ty; }
        void setType(Type*);
        
        Node(Meta m);
        virtual ~Node();

        static void reportLeftovers();
    private:
        Type* _ty;
    };

    class Type : public Node
    {
    public:
        enum Kind {
            Undefined, NoType, None, Notext,
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
            Parameter, VirtualSpec, ExternalProc, ExternalClass, LabelDecl, Builtin, Import, StandardClass
        };
        enum ParamMode { ModeDefault, ModeValue, ModeName };
        enum Visi { NA, Hidden, Protected };

        Kind kind;
        QByteArray name;
        Atom sym;
        
        Declaration* link;   // Members/Locals
        Declaration* next;   // Next in scope
        Declaration* outer;  // Parent scope
        Statement*   body;   // class, procedure
        Atom   nameRef;     // Class prefix name, External ext name
        union {
            // Class / Procedure
            Declaration* prefix; // Superclass, not owned

            // Switch
            Expression* list; // Chain of label expressions

            // Module
            QString* path;    // Source path

            // Variable / Parameter / Const
            Expression* init; // Initialization expression, owned
        };


        Declaration(Kind k = Invalid);
        ~Declaration();
        
        Declaration* find(const char* id, bool recursive = true) const;

        void appendMember(Declaration* d);
        static void deleteAll(Declaration* d);
    };

    class Expression : public Node
    {
    public:
        enum Kind {
            Invalid,
            Plus, Minus, Mul, Div, IntDiv, Exp,
            And, Or, Not, Imp, Eqv, AndThen, OrElse,
            Eq, Neq, Lt, Leq, Gt, Geq, RefEq, RefNeq, Is, In,
            Identifier, DeclRef, Dot, Subscript,
            New, This, Qua,
            StringConst, CharConst, UnsignedConst, RealConst, BoolConst, Notext, None,
            IfExpr, Call, AssignVal, AssignRef,
            // Helper
            StepUntil, // lhs=start, rhs=step, condition=until
            WhileLoop, // lhs=start, condition=cond
            TypeRef,   // a = type name
            MAX
        };

        Kind kind;
        union {
            quint64 u;
            double r;
            Atom a; // string, name
            Declaration* d;
        };
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
        Statement* body;  // If, While, For, Compound, Block, Inspect do

        union {
            // Compound / Block
            struct {
                Declaration* scope;     // scope local declarations, not owned
                Expression* prefix;    // Class prefix (for Block)
                Expression* args;      // Arguments for prefix (linked list of Expressions)
            };

            // If / While
            struct {
                Expression* cond;       // owned
                Statement* elseStmt;   //  owned
            };

            // For
            struct {
                Expression* var;       // Control variable, owned
                Expression* list;      // Linked list of 'For' elements (Expression nodes), owned
            };

            // Inspect
            struct {
                Expression* obj;       // Inspect object, owned
                Connection* conn;      // Chain of WHEN clauses, owned
                Statement* otherwise;  // otherwise_clause, owned
            };

            // Activate, Reactivate
            ActivateData* activate;     // owned

            // Assign / Call / Detach / Resume / Goto
            struct {
                 Expression* lhs;      // Target / Callable, owned
                 Expression* rhs;      // Value / Ref / Args, owned
            };

        };

        Statement(Kind k = Invalid, const RowCol& p = RowCol());
        ~Statement();
        
        Declaration* getScope() const;

        void append(Statement* s);
        static void deleteAll(Statement* s);
    };

    class Connection : public Node {
    public:
        Atom className;
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
        Declaration* addDecl(const char *id, const QByteArray& name, Declaration::Kind k);
        Declaration* getTopScope() const;
        Type* getType(Type::Kind k) const;
        Declaration* getGlobals() const { return globalScope; }
        Declaration* getBasicIo() const;
        Declaration* getSimSet() const;
        Declaration* getSimulation() const;

        static Declaration* resolveInClass(Declaration* cls, Atom name);
        static Declaration* findInScope(Declaration* scope, const char* sym);

        static void dump(QTextStream&, Declaration*);
    private:
        Declaration* currentScope() const;
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
