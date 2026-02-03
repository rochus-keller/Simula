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
#include <limits>
using namespace Sim;

const char* Builtin::name[] = {
    "ABS", "SIGN", "ENTIER", "MOD", "REM", "SQRT", "SIN", "COS", "ARCTAN", "LN", "EXP",
    "LOWTEN", "COPY", "TEXT_START", "TEXT_POS", "TEXT_LENGTH", "TEXT_SUB", "TEXT_STRIP",
    "ISO_CHAR", "ISO_RANK", "RANK", "CHAR_FUNC", "DETACH", "RESUME", "CALL", "TIME", "SOURCELINE"
};

const char* Type::name[] = {
    "Undefined", "NoType",
    "Integer", "ShortInteger", "Real", "LongReal", "Boolean", "Character", "Label", "Text",
    "",
    "Ref", "Pointer", "Array", "Procedure", "Switch"
};

Node::Node(Meta m) : meta(m), inList(0), ownstype(0), owned(0), _ty(0), mode(0),
    visi(0), id(0), isVirtual(0), re(0), prior(0), ownsexpr(0){}

Node::~Node() {
    if (_ty && ownstype)
       delete _ty;
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

Type::Type(Kind k) : Node(T), kind(k), expr(0) {}
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

Declaration::Declaration(Kind k) : Node(D), kind(k), link(0), next(0), outer(0), body(0), prefix(0)
{

}

Declaration::~Declaration() {
    if (link) deleteAll(link); // Recursive delete of members
    if (body)
        Statement::deleteAll(body);     // Delete statement tree

    // Union cleanup based on Kind
    switch (kind) {
    case Module:
    case Class:
    case ExternalProc:
    case ExternalClass:
        if (data) delete data;
        break;
    case Switch:
        if (list) delete list;
        break;
    case Variable:
    case Parameter:
        if (init) delete init;
        break;
    default:
        break;
    }
}

Declaration *Declaration::find(const QByteArray &name, bool recursive) const
{
    Declaration* d = link;
    while( d )
    {
        if( d->name.constData() == name.constData() )
            return d;
        d = d->next;
    }
    if( recursive && outer )
        return outer->find(name);
    return 0;
}

void Declaration::appendMember(Declaration* d) {
    if (!link) link = d;
    else {
        Declaration* cur = link;
        while (cur->next) cur = cur->next;
        cur->next = d;
    }
    d->inList = true;
}
void Declaration::deleteAll(Declaration* d) {
    while (d) {
        Declaration* next = d->next;
        delete d;
        d = next;
    }
}

Expression::Expression(Kind k, const RowCol& rc) : Node(E), kind(k), lhs(0), rhs(0), next(0), condition(0) { pos = rc; }
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
    // However, for simplicity here assuming moderate depth or manual management:
    // Ideally, a container deletes the chain iteratively.

    switch (kind) {
    case Compound:
    case Block:
        if (prefix) delete prefix;
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
        if (otherwise) delete otherwise;
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

Connection::Connection() : Node(Node::C), classDecl(0), body(0), next(0) {}
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

Declaration* AstModel::addDecl(const QByteArray& name, Declaration::Kind k) {
    Declaration* d = new Declaration(k);
    d->name = name;
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

Type* AstModel::newType(Type::Kind k) {
    Type* t = new Type(k);
    t->owned = true;
    return t;
}

void AstModel::initBuiltins() {
    for (int i = 0; i < Builtin::Max; ++i) {
        Declaration* d = addDecl(Builtin::name[i], Declaration::Builtin);
        d->id = i;
    }
}
