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
#include "SimLexer.h"
#include <limits>
#include <QTextStream>
#include <QtDebug>
using namespace Sim;

const char* Builtin::name[] = {
    // Text Handling
    "BLANKS", "COPY", "CHAR", "ISOCHAR", "RANK", "ISORANK", "DIGIT",
    "LETTER", "LOWTEN", "DECIMALMARK", "UPCASE", "LOWCASE",
    // Math Functions (Standard ALGOL 60 + Extensions)
    "ABS", "SIGN", "ENTIER", "SQRT", "SIN", "COS", "TAN", "COTAN",
    "ARCSIN", "ARCCOS", "ARCTAN", "ARCTAN2",
    "SINH", "COSH", "TANH",
    "LOG10", "LN", "EXP", "MOD", "REM",
    "MAX", "MIN",
    // Input / Output (Basic)
    "ININT", "INREAL", "INCHAR", "INTEXT",
    "OUTINT", "OUTREAL", "OUTCHAR", "OUTTEXT", "OUTIMAGE",
    "SYSIN", "SYSOUT", "OUTFIX",
    // Control & System
    "ERROR", "TIME", "RANDOM", "SOURCELINE", "ELAPSED",
    // Scheduling
    "DETACH", "RESUME", "CALL"
};

const char* Type::name[] = {
    "Undefined", "NoType", "NONE", "NOTEXT",
    "Integer", "ShortInteger", "Real", "LongReal", "Boolean", "Character", "Label", "Text",
    "",
    "Ref", "Pointer", "Array", "Procedure", "Switch"
};

// #define SIM_TRACK_LEFTOVERS
#ifdef SIM_TRACK_LEFTOVERS
static QList<Node*> nodes;
#endif

Node::Node(Meta m) : meta(m), isExternal(0), ownstype(0), owned(0), _ty(0), mode(0),
    visi(0), id(0), isVirtual(0), re(0), prior(0), ownsexpr(0),validated(0)
{
#ifdef SIM_TRACK_LEFTOVERS
    nodes << this;
#endif
}

Node::~Node() {
    if (_ty && ownstype)
       delete _ty;
#ifdef SIM_TRACK_LEFTOVERS
    nodes.removeAll(this);
#endif
}

void Node::reportLeftovers()
{
#ifdef SIM_TRACK_LEFTOVERS
    qDebug() << "*** nodes not deleted:" << nodes.size();
    foreach( Node* n, nodes )
    {
        switch(n->meta)
        {
        case T: {
            Type* t = (Type*)n;
            qDebug() << "type" << Type::name[t->kind];
        } break;
        case D: {
            Declaration* d = (Declaration*)n;
            qDebug() << "declaration" << d->kind << d->name;
        } break;
        case E: {
            Expression* e = (Expression*)n;
            qDebug() << "expression" << e->kind;
        } break;
        case S: {
            Statement* s = (Statement*)n;
            qDebug() << "statement" << s->kind;
        } break;
        case C: {
            Connection* c = (Connection*)n;
            qDebug() << "connection";
        } break;
        }
    }
#endif
}

void Node::setType(Type* t) {
    if (_ty == t)
        return;
    if (_ty && ownstype) {
        delete _ty;
        ownstype = false;
    }
    _ty = t;
    if (t && !t->owned) {
        ownstype = true;
        t->owned = true;
    }
}

Type::Type(Kind k) : Node(T), kind(k), expr(0)
{

}

Type::~Type() {
    if (expr && ownsexpr)
        delete expr;
}

void Type::setExpr(Expression *e)
{
    if (expr == e)
        return;
    if (expr && ownsexpr) {
        delete expr;
        ownsexpr = false;
    }
    expr = e;
    if (e && !e->owned) {
        ownsexpr = true;
        e->owned = true;
    }
}
bool Type::isArithmetic() const { return kind >= Integer && kind <= LongReal; }

Declaration::Declaration(Kind k) : Node(D), kind(k), link(0), next(0), outer(0), body(0), prefix(0),sym(0), nameRef(0)
{

}

Declaration::~Declaration() {
    if (link)
        deleteAll(link); // Recursive delete of members
    if (body)
        Statement::deleteAll(body);     // Delete statement tree

    // Union cleanup based on Kind
    switch (kind) {
    case Switch:
        if (list) delete list;
        break;
    case Module:
        if( path) delete path;
        break;
    case Variable:
    case Parameter:
        if (init) delete init;
        break;
    default:
        break;
    }
}

Declaration *Declaration::find(const char *id, bool recursive) const
{
    Declaration* d = link;
    while( d )
    {
        if( d->sym == id)
            return d;
        d = d->next;
    }
    if( recursive && outer )
        return outer->find(id);
    return 0;
}

void Declaration::appendMember(Declaration* d) {
    if (!link) link = d;
    else {
        Declaration* cur = link;
        while (cur->next) cur = cur->next;
        cur->next = d;
    }
}

void Declaration::deleteAll(Declaration* d) {
    while (d) {
        Declaration* next = d->next;
        delete d;
        d = next;
    }
}

Expression::Expression(Kind k, const RowCol& rc) : Node(E), kind(k), lhs(0), rhs(0), next(0), condition(0), u(0) { pos = rc; }
Expression::~Expression() {
    if (lhs) delete lhs;
    if (rhs) delete rhs;
    if (next) delete next;
    if (condition) delete condition;
}
void Expression::append(Expression* list, Expression* elem) {
    while (list->next) list = list->next;
    list->next = elem;
}

Statement::Statement(Kind k, const RowCol& p) : Node(S), kind(k), body(0), next(0),
    prefix(0), args(0), scope(0) { pos = p; }

Statement::~Statement() {
    if (body)
        deleteAll(body); // Recursively delete child statements (Block/Compound/Then)

    // Note: 'next' is NOT deleted here recursively to avoid stack overflow on long blocks,
    // handled by deleteAll or manual iteration in parent. 

    switch (kind) {
    case Compound:
    case Block:
        if (prefix) delete prefix;
        if (args) delete args;
        // no: if (scope) delete scope;
        break;
    case If:
    case While:
        if (cond) delete cond;
        if (elseStmt)
            deleteAll(elseStmt);
        break;
    case For:
        if (var) delete var;
        if (list) delete list; // Deletes the chain
        break;
    case Inspect:
        if (obj) delete obj;
        if (conn) delete conn;
        if (otherwise)
            deleteAll(otherwise);
        break;
    case Activate:
        if (activate) delete activate;
        break;
    case Assign:
    case Call:
    case Detach:
    case Resume:
    case Goto:
    case Inner:
        if (lhs) delete lhs;
        if (rhs) delete rhs;
        break;
    default:
        break;
    }
}

Declaration *Statement::getScope() const
{
    if( kind == Compound || kind == Block )
        return scope;
    else
        return 0;
}

void Statement::append(Statement* s) {
    Statement* last = this;
    while(last->next) last = last->next;
    last->next = s;
}

void Statement::deleteAll(Statement* s) {
    while (s) {
        Statement* next = s->next;
        // Detach next to prevent double delete if destructor were recursive
        s->next = 0; 
        delete s;
        s = next;
    }
}

Connection::Connection() : Node(Node::C), classDecl(0), body(0), next(0), className(0) {}
Connection::~Connection() {
    if(body)
        Statement::deleteAll(body);
    if(next)
        delete next;
}

AstModel::AstModel(SimulaVersion v) : version(v), globalScope(0) {


    globalScope = new Declaration(Declaration::Invalid);
    openScope(globalScope);
    
    // Init basic types
    for (int i = 0; i < Type::MaxBasicType; ++i)
        basicTypes[i] = 0;
    
    basicTypes[Type::Integer] = newType(Type::Integer);
    basicTypes[Type::ShortInteger] = newType(Type::ShortInteger);
    basicTypes[Type::Real] = newType(Type::Real);
    basicTypes[Type::LongReal] = newType(Type::LongReal);
    basicTypes[Type::Boolean] = newType(Type::Boolean);
    basicTypes[Type::Character] = newType(Type::Character);
    basicTypes[Type::Text] = newType(Type::Text);
    basicTypes[Type::NoType] = newType(Type::NoType);
    basicTypes[Type::Label] = newType(Type::Label);

    initBuiltins();
}

AstModel::~AstModel() {
    Declaration::deleteAll(globalScope);
    for (int i=0; i<Type::MaxBasicType; ++i)
        if(basicTypes[i])
            delete basicTypes[i];
}

void AstModel::openScope(Declaration* scope) {
    scopes.push_back(scope);
}

Declaration* AstModel::closeScope() {
    if (scopes.isEmpty()) return 0;
    Declaration* s = scopes.takeLast();
    return s;
}

Declaration* AstModel::addDecl(const char* id, const QByteArray &name, Declaration::Kind k) {
    Declaration* d = new Declaration(k);
    d->name = name;
    d->sym = id;
    if (!scopes.isEmpty()) {
        d->outer = scopes.last();
        scopes.last()->appendMember(d);
    }
    return d;
}

Declaration* AstModel::currentScope() const {
    return scopes.isEmpty() ? 0 : scopes.last();
}

Declaration* AstModel::getTopScope() const { return globalScope; }

Type* AstModel::getType(Type::Kind k) const {
    if (k < Type::MaxBasicType) return basicTypes[k];
    return 0;
}

Declaration *AstModel::getBasicIo() const
{
    return findInScope(getEnv(), Lexer::toId("basicio"));
}

Declaration *AstModel::getSimSet() const
{
    return findInScope(getEnv(), Lexer::toId("simset"));
}

Declaration *AstModel::getSimulation() const
{
    return findInScope(getEnv(), Lexer::toId("simulation"));
}

Declaration* AstModel::resolveInClass(Declaration* cls, Atom name)
{
    if (!cls)
        return 0;

    // Search in class and its prefix chain
    Declaration* cur = cls;
    while (cur) {
        Declaration* d = findInScope(cur->body->scope, name);
        if (d)
            return d;
        cur = cur->prefix;
    }

    return 0;
}

Declaration* AstModel::findInScope(Declaration* scope, const char *sym, bool includeBodyscope)
{
    if (!scope)
        return 0;

    Declaration* d = scope->link;
    while (d) {
        if (d->sym == sym)
            return d;
        d = d->next;
    }

    if( includeBodyscope && scope->kind == Declaration::Class && scope->body && scope->body->scope )
    {
        d = scope->body->scope->link;
        while (d) {
            if (d->sym == sym)
                return d;
            d = d->next;
        }
    }

    return 0;
}
Type* AstModel::newType(Type::Kind k) {
    Type* t = new Type(k);
    t->owned = true;

    const QByteArray name = Type::name[k];
    Declaration* d = addDecl(Lexer::toId(name.toLower()), name, Declaration::StandardClass);
    d->validated = true;
    d->setType(t);

    return t;
}

void AstModel::initBuiltins() {
    for (int i = 0; i < Builtin::Max; ++i) {
        const QByteArray name = Builtin::name[i];
        Declaration* d = addDecl(Lexer::toId(name.toLower()), name, Declaration::Builtin);
        d->id = i;
    }

    Declaration* textobj = addDecl(Lexer::toId("textobj"), "TEXTOBJ", Declaration::StandardClass);
    // TODO: CONSTANT, START, LENGTH, MAIN, POS, SETPOS, MORE, GETCHAR, PUTCHAR

}

class AstDumper
{
public:
    AstDumper(QTextStream& out) : out(out), indent(0) {}

    void dump(Declaration* d)
    {
        if (!d) return;
        dumpDecl(d);
    }

private:
    QTextStream& out;
    int indent;

    void writeIndent()
    {
        for (int i = 0; i < indent; ++i)
            out << "  ";
    }

    const char* declKindName(Declaration::Kind k)
    {
        switch (k) {
            case Declaration::Invalid: return "Invalid";
            case Declaration::Module: return "Module";
            case Declaration::Program: return "Program";
            case Declaration::Class: return "Class";
            case Declaration::Procedure: return "Procedure";
            case Declaration::Block: return "Block";
            case Declaration::Variable: return "Variable";
            case Declaration::Array: return "Array";
            case Declaration::Switch: return "Switch";
            case Declaration::Parameter: return "Parameter";
            case Declaration::VirtualSpec: return "VirtualSpec";
            case Declaration::ExternalProc: return "ExternalProc";
            case Declaration::ExternalClass: return "ExternalClass";
            case Declaration::LabelDecl: return "LabelDecl";
            case Declaration::Builtin: return "Builtin";
            case Declaration::Import: return "Import";
            case Declaration::StandardClass: return "StandardClass";
            default: return "Unknown";
        }
    }

    const char* exprKindName(Expression::Kind k)
    {
        switch (k) {
            case Expression::Invalid: return "Invalid";
            case Expression::Plus: return "Plus";
            case Expression::Minus: return "Minus";
            case Expression::Mul: return "Mul";
            case Expression::Div: return "Div";
            case Expression::IntDiv: return "IntDiv";
            case Expression::Exp: return "Exp";
            case Expression::And: return "And";
            case Expression::Or: return "Or";
            case Expression::Not: return "Not";
            case Expression::Imp: return "Imp";
            case Expression::Eqv: return "Eqv";
            case Expression::Eq: return "Eq";
            case Expression::Neq: return "Neq";
            case Expression::Lt: return "Lt";
            case Expression::Leq: return "Leq";
            case Expression::Gt: return "Gt";
            case Expression::Geq: return "Geq";
            case Expression::RefEq: return "RefEq";
            case Expression::RefNeq: return "RefNeq";
            case Expression::Identifier: return "Identifier";
            case Expression::DeclRef: return "DeclRef";
            case Expression::Dot: return "Dot";
            case Expression::Subscript: return "Subscript";
            case Expression::New: return "New";
            case Expression::This: return "This";
            case Expression::Qua: return "Qua";
            case Expression::StringConst: return "StringConst";
            case Expression::CharConst: return "CharConst";
            case Expression::UnsignedConst: return "UnsignedConst";
            case Expression::RealConst: return "RealConst";
            case Expression::BoolConst: return "BoolConst";
            case Expression::Notext: return "Notext";
            case Expression::None: return "None";
            case Expression::IfExpr: return "IfExpr";
            case Expression::Call: return "Call";
            case Expression::AssignVal: return "AssignVal";
            case Expression::AssignRef: return "AssignRef";
            case Expression::StepUntil: return "StepUntil";
            case Expression::WhileLoop: return "WhileLoop";
            case Expression::TypeRef: return "TypeRef";
            default: return "Unknown";
        }
    }

    const char* stmtKindName(Statement::Kind k)
    {
        switch (k) {
            case Statement::Invalid: return "Invalid";
            case Statement::Compound: return "Compound";
            case Statement::Block: return "Block";
            case Statement::Assign: return "Assign";
            case Statement::Call: return "Call";
            case Statement::If: return "If";
            case Statement::While: return "While";
            case Statement::For: return "For";
            case Statement::Inspect: return "Inspect";
            case Statement::Goto: return "Goto";
            case Statement::Activate: return "Activate";
            case Statement::Detach: return "Detach";
            case Statement::Resume: return "Resume";
            case Statement::Inner: return "Inner";
            case Statement::Dummy: return "Dummy";
            case Statement::End: return "End";
            default: return "Unknown";
        }
    }

    const char* typeKindName(Type::Kind k)
    {
        if (k < Type::MaxBasicType)
            return Type::name[k];
        switch (k) {
            case Type::Ref: return "Ref";
            case Type::Pointer: return "Pointer";
            case Type::Array: return "Array";
            case Type::Procedure: return "Procedure";
            case Type::Switch: return "Switch";
            default: return "Unknown";
        }
    }

    void dumpDecl(Declaration* d)
    {
        while (d) {
            writeIndent();
            out << declKindName(d->kind);
            if (!d->name.isEmpty())
                out << " \"" << d->name << "\"";
            if (d->sym)
                out << " sym=" << d->sym;
            out << " [" << d->pos.d_row << ":" << d->pos.d_col << "]";

            if (d->mode == Declaration::ModeValue)
                out << " value";
            else if (d->mode == Declaration::ModeName)
                out << " name";

            if (d->visi == Declaration::Hidden)
                out << " hidden";
            else if (d->visi == Declaration::Protected)
                out << " protected";

            if (d->isVirtual)
                out << " virtual";

            if (d->nameRef)
                out << " nameRef=" << d->nameRef;

            // Union fields based on kind
            switch (d->kind) {
            case Declaration::Class:
            case Declaration::Procedure:
                if (d->prefix)
                    out << " prefix=" << d->prefix->name.constData();
                break;
            case Declaration::Module:
                if (d->path)
                    out << " path=\"" << *d->path << "\"";
                break;
            default:
                break;
            }

            out << "\n";

            if (d->type()) {
                ++indent;
                dumpType(d->type());
                --indent;
            }

            // Dump switch list for Switch declarations
            if (d->kind == Declaration::Switch && d->list) {
                ++indent;
                writeIndent();
                out << "switch_list:\n";
                ++indent;
                dumpExprList(d->list);
                --indent;
                --indent;
            }

            // Dump init expression for Variable/Parameter
            if ((d->kind == Declaration::Variable || d->kind == Declaration::Parameter) && d->init) {
                ++indent;
                writeIndent();
                out << "init:\n";
                ++indent;
                dumpExpr(d->init);
                --indent;
                --indent;
            }

            if (d->link) {
                ++indent;
                writeIndent();
                out << "members:\n";
                ++indent;
                dumpDecl(d->link);
                --indent;
                --indent;
            }

            if (d->body) {
                ++indent;
                writeIndent();
                out << "body:\n";
                ++indent;
                dumpStmt(d->body);
                --indent;
                --indent;
            }

            d = d->next;
        }
    }

    void dumpType(Type* t)
    {
        if (!t) return;
        writeIndent();
        out << "type: " << typeKindName(t->kind);
        out << "\n";

        if (t->getExpr()) {
            ++indent;
            writeIndent();
            out << "expr:\n";
            ++indent;
            dumpExprList(t->getExpr());
            --indent;
            --indent;
        }
    }

    void dumpExprList(Expression* e)
    {
        while (e) {
            dumpExpr(e);
            e = e->next;
        }
    }

    void dumpExpr(Expression* e)
    {
        if (!e) return;

        writeIndent();
        out << exprKindName(e->kind);
        out << " [" << e->pos.d_row << ":" << e->pos.d_col << "]";

        // Print value based on expression kind
        switch (e->kind) {
        case Expression::Identifier:
        case Expression::StringConst:
        case Expression::TypeRef:
            if (e->a)
                out << " \"" << e->a << "\"";
            break;
        case Expression::CharConst:
            out << " '" << (char)e->u << "'";
            break;
        case Expression::UnsignedConst:
            out << " " << e->u;
            break;
        case Expression::RealConst:
            out << " " << e->r;
            break;
        case Expression::BoolConst:
            out << " " << (e->u ? "true" : "false");
            break;
        case Expression::DeclRef:
            if (e->d)
                out << " -> " << e->d->name.constData();
            break;
        case Expression::This:
            if (e->a)
                out << " " << e->a;
            break;
        default:
            break;
        }
        out << "\n";

        if (e->condition) {
            ++indent;
            writeIndent();
            out << "condition:\n";
            ++indent;
            dumpExpr(e->condition);
            --indent;
            --indent;
        }

        if (e->lhs) {
            ++indent;
            writeIndent();
            out << "lhs:\n";
            ++indent;
            dumpExpr(e->lhs);
            --indent;
            --indent;
        }

        if (e->rhs) {
            ++indent;
            writeIndent();
            out << "rhs:\n";
            ++indent;
            // For Call expressions, rhs is a list of arguments
            if (e->kind == Expression::Call || e->kind == Expression::Subscript ||
                e->kind == Expression::New) {
                dumpExprList(e->rhs);
            } else {
                dumpExpr(e->rhs);
            }
            --indent;
            --indent;
        }
    }

    void dumpStmt(Statement* s)
    {
        while (s) {
            writeIndent();
            out << stmtKindName(s->kind);
            out << " [" << s->pos.d_row << ":" << s->pos.d_col << "]";

            if (s->kind == Statement::Activate && s->re)
                out << " reactivate";
            if (s->prior)
                out << " prior";

            out << "\n";

            // Dump based on statement kind (union fields)
            switch (s->kind) {
            case Statement::Compound:
            case Statement::Block:
                if (s->scope) {
                    ++indent;
                    writeIndent();
                    out << "scope: " << s->scope->name.constData() << "\n";
                    if (s->scope->link) {
                        writeIndent();
                        out << "locals:\n";
                        ++indent;
                        dumpDecl(s->scope->link);
                        --indent;
                    }
                    --indent;
                }
                if (s->prefix) {
                    ++indent;
                    writeIndent();
                    out << "prefix:\n";
                    ++indent;
                    dumpExpr(s->prefix);
                    --indent;
                    --indent;
                }
                if (s->args) {
                    ++indent;
                    writeIndent();
                    out << "args:\n";
                    ++indent;
                    dumpExprList(s->args);
                    --indent;
                    --indent;
                }
                if (s->body) {
                    ++indent;
                    writeIndent();
                    out << "body:\n";
                    ++indent;
                    dumpStmt(s->body);
                    --indent;
                    --indent;
                }
                break;

            case Statement::If:
            case Statement::While:
                if (s->cond) {
                    ++indent;
                    writeIndent();
                    out << "cond:\n";
                    ++indent;
                    dumpExpr(s->cond);
                    --indent;
                    --indent;
                }
                if (s->body) {
                    ++indent;
                    writeIndent();
                    out << "then:\n";
                    ++indent;
                    dumpStmt(s->body);
                    --indent;
                    --indent;
                }
                if (s->elseStmt) {
                    ++indent;
                    writeIndent();
                    out << "else:\n";
                    ++indent;
                    dumpStmt(s->elseStmt);
                    --indent;
                    --indent;
                }
                break;

            case Statement::For:
                if (s->var) {
                    ++indent;
                    writeIndent();
                    out << "var:\n";
                    ++indent;
                    dumpExpr(s->var);
                    --indent;
                    --indent;
                }
                if (s->list) {
                    ++indent;
                    writeIndent();
                    out << "for_list:\n";
                    ++indent;
                    dumpExprList(s->list);
                    --indent;
                    --indent;
                }
                if (s->body) {
                    ++indent;
                    writeIndent();
                    out << "do:\n";
                    ++indent;
                    dumpStmt(s->body);
                    --indent;
                    --indent;
                }
                break;

            case Statement::Inspect:
                if (s->obj) {
                    ++indent;
                    writeIndent();
                    out << "obj:\n";
                    ++indent;
                    dumpExpr(s->obj);
                    --indent;
                    --indent;
                }
                if (s->conn) {
                    ++indent;
                    writeIndent();
                    out << "when_clauses:\n";
                    ++indent;
                    dumpConnection(s->conn);
                    --indent;
                    --indent;
                }
                if (s->otherwise) {
                    ++indent;
                    writeIndent();
                    out << "otherwise:\n";
                    ++indent;
                    dumpStmt(s->otherwise);
                    --indent;
                    --indent;
                }
                if (s->body) {
                    ++indent;
                    writeIndent();
                    out << "do:\n";
                    ++indent;
                    dumpStmt(s->body);
                    --indent;
                    --indent;
                }
                break;

            case Statement::Activate:
                if (s->activate) {
                    ++indent;
                    if (s->activate->obj) {
                        writeIndent();
                        out << "obj:\n";
                        ++indent;
                        dumpExpr(s->activate->obj);
                        --indent;
                    }
                    if (s->activate->at) {
                        writeIndent();
                        out << "at:\n";
                        ++indent;
                        dumpExpr(s->activate->at);
                        --indent;
                    }
                    if (s->activate->delay) {
                        writeIndent();
                        out << "delay:\n";
                        ++indent;
                        dumpExpr(s->activate->delay);
                        --indent;
                    }
                    if (s->activate->priorObj) {
                        writeIndent();
                        out << "priorObj:\n";
                        ++indent;
                        dumpExpr(s->activate->priorObj);
                        --indent;
                    }
                    --indent;
                }
                break;

            case Statement::Assign:
            case Statement::Call:
            case Statement::Detach:
            case Statement::Resume:
            case Statement::Goto:
                if (s->lhs) {
                    ++indent;
                    writeIndent();
                    out << "lhs:\n";
                    ++indent;
                    dumpExpr(s->lhs);
                    --indent;
                    --indent;
                }
                if (s->rhs) {
                    ++indent;
                    writeIndent();
                    out << "rhs:\n";
                    ++indent;
                    if (s->kind == Statement::Call) {
                        dumpExprList(s->rhs);
                    } else {
                        dumpExpr(s->rhs);
                    }
                    --indent;
                    --indent;
                }
                break;

            default:
                break;
            }

            s = s->next;
        }
    }

    void dumpConnection(Connection* c)
    {
        while (c) {
            writeIndent();
            out << "WHEN";
            if (c->className)
                out << " " << c->className;
            if (c->classDecl)
                out << " -> " << c->classDecl->name.constData();
            out << " [" << c->pos.d_row << ":" << c->pos.d_col << "]\n";

            if (c->body) {
                ++indent;
                writeIndent();
                out << "do:\n";
                ++indent;
                dumpStmt(c->body);
                --indent;
                --indent;
            }

            c = c->next;
        }
    }
};

void AstModel::dump(QTextStream & out, Declaration * d)
{
    AstDumper dumper(out);
    dumper.dump(d);
}

Declaration *AstModel::getEnv() const
{
    return findInScope(globalScope, Lexer::toId("environment"));
}

