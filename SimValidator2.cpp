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

#include "SimValidator2.h"
#include "SimLexer.h"
#include <QtDebug>
using namespace Sim;

static inline bool isClassDecl(Declaration* d)
{
    return d->kind == Declaration::Class || d->kind == Declaration::StandardClass ||
            d->kind == Declaration::ExternalClass;
}

Validator2::Validator2(AstModel* mdl, Importer * i, bool haveXref)
    : module(0), mdl(mdl), first(0), last(0), imp(i), paramOf(0), qualCheck(false)
{
    Q_ASSERT(mdl);
    if( haveXref )
        first = last = new Symbol();
}

Validator2::~Validator2()
{
    if( first )
        Symbol::deleteAll(first);
}

bool Validator2::validate(Declaration* mod)
{
    Q_ASSERT(mod);

    if( mod->validated )
        return true;

    errors.clear();

    if( first ) {
        first->decl = mod;
        first->kind = Symbol::Module;
        first->pos = mod->pos;
        first->len = mod->name.size();
    }

    markDecl(mod);

    if( mod->kind == Declaration::Module ) {
        sourcePath = *mod->path;
    }

    this->module = mod;

    Declaration* env = mdl->getEnv();
    if( env )
        scopeStack.push_back(env);
    Declaration* basicio = mdl->getBasicIo();
    if( basicio )
        scopeStack.push_back(basicio);
    // the program acts as if embraced by "inspect sysin do inspect sysout do" 
    Declaration* sysio = mdl->getSysIo();
    if( sysio )
        scopeStack.push_back(sysio);

    try {
        if( env )
            Decl(env);
        Module(mod);
    } catch( ... ) {
    }

    if( sysio )
        scopeStack.pop_back();
    if( basicio )
        scopeStack.pop_back();
    if( env )
        scopeStack.pop_back();

    if( first )
        last->next = first; // close the circle

    mod->validated = true;
    mod->hasErrors = !errors.isEmpty();

    return errors.isEmpty();
}

Xref Validator2::takeXref()
{
    Xref res;
    res.syms = first;
    res.uses = xref;
    res.subs = subs;
    first = last = 0;
    xref.clear();
    subs.clear();
    return res;
}

void Validator2::invalid(const char* what, const RowCol& pos)
{
    errors << Error(QString("invalid %1").arg(what), pos, sourcePath);
}

bool Validator2::error(const RowCol& pos, const QString& msg) const
{
    errors << Error(msg, pos, sourcePath);
    return false;
}

void Validator2::markDecl(Declaration* d)
{
    if( !first || !d )
        return;
    Symbol* s = new Symbol();
    s->decl = d;
    s->kind = Symbol::Decl;
    s->pos = d->pos;
    s->len = d->name.size();
    last->next = s;
    xref[d].append(s);
    last = s;
}

Symbol* Validator2::markRef(Declaration* d, const RowCol& pos)
{
    if( !first || !d )
        return 0;
    Symbol* s = new Symbol();
    s->decl = d;
    s->kind = Symbol::Use;
    s->pos = pos;
    s->len = d->name.size();
    last->next = s;
    last = s;
    xref[d].append(s);
    return s;
}

Symbol* Validator2::markUnref(int len, const RowCol& pos)
{
    if( !first )
        return 0;
    Symbol* s = new Symbol();
    s->decl = 0;
    s->kind = Symbol::Use;
    s->pos = pos;
    s->len = len;
    last->next = s;
    last = s;
    return s;
}

void Validator2::Module(Declaration* mod)
{
    scopeStack.push_back(mod);
    checkDuplicates(mod);
    DeclSeq(mod->link);
    if( mod->body )
        Body(mod->body);
    scopeStack.pop_back();
}

void Validator2::Decl(Declaration *d)
{
    if( !d->validated ) {
        d->validated = true;
        markDecl(d);

        // a declaration referenced before it was reached is validated in its own context
        const QList<Scope> saved = scopeStack;
        Declaration* savedParamOf = paramOf;
        const bool savedQualCheck = qualCheck;
        paramOf = 0;
        qualCheck = false;
        for( int i = scopeStack.size() - 1; i >= 0; i-- ) {
            if( scopeStack[i].d == d->outer ) {
                scopeStack.erase(scopeStack.begin() + i + 1, scopeStack.end());
                break;
            }
        }

        switch( d->kind ) {
        case Declaration::Class:
            ClassDecl(d);
            break;
        case Declaration::Procedure:
            ProcDecl(d);
            break;
        case Declaration::Variable:
            VarDecl(d);
            break;
        case Declaration::Array:
            ArrayDecl(d);
            break;
        case Declaration::Switch:
            SwitchDecl(d);
            break;
        case Declaration::Parameter:
            ParamDecl(d);
            break;
        case Declaration::ExternalProc:
        case Declaration::ExternalClass:
            ExternalDecl(d);
            break;
        case Declaration::Block:
        case Declaration::Program:
            BlockDecl(d);
            break;
        case Declaration::LabelDecl:
            LabelDecl(d);
            break;
        case Declaration::VirtualSpec:
            // Virtual specs are validated as part of class
            break;
        case Declaration::Builtin:
        case Declaration::StandardClass:
            // These don't need validation
            break;
        default:
            break;
        }

        scopeStack = saved;
        paramOf = savedParamOf;
        qualCheck = savedQualCheck;
    }
}

void Validator2::DeclSeq(Declaration* d)
{
    while( d ) {
        if( d->kind != Declaration::Block )
            // a subordinate block is validated with its statement
	    // which knows the prefix and the connection
            Decl(d);
        d = d->next;
    }
}

void Validator2::ClassDecl(Declaration* d)
{
    // Resolve prefix
    if( !d->prefix )
    {
        if( d->nameRef && Identifier(d->nameRef) )
            d->prefix = d->nameRef->d;
    }

    // Validate prefix (superclass)
    if( d->prefix ) {
        Declaration* super = d->prefix;
        if( super->kind != Declaration::Class &&
            super->kind != Declaration::StandardClass &&
            super->kind != Declaration::ExternalClass ) {
            error(d->pos, QString("prefix '%1' is not a class").arg(super->name.constData()));
        } else {
            // Track subclass relationship
            if( first )
                subs[super].append(d);
        }
    }

    ProtectionPart(d);

    checkDuplicates(d->body ? d->body->scope : 0, d);

    // Open class scope
    scopeStack.push_back(d);

    // Validate parameters
    Declaration* param = d->link;
    while( param && param->kind == Declaration::Parameter ) {
        if( !param->validated ) {
#if 1
            Decl(param);
#else
            param->validated = true;
            markDecl(param);
            ParamDecl(param);
#endif
        }
        param = param->next;
    }

    // Validate virtual specs
    Declaration* member = d->link;
    while( member ) {
        if( member->kind == Declaration::VirtualSpec && !member->validated ) {
            member->validated = true;
            if( member->sym )
                // this is an abstract virtual declaration
                markDecl(member);
            else
            {
                // this is a virtual annotation to a later declaration in the same class
                markRef(member->forward, member->pos);
                checkVirtualMatch(member, member->forward);
            }
            DeclType(member);
        }
        member = member->next;
    }

    // Validate other members
    member = d->link;
    while( member ) {
        if( member->kind != Declaration::Parameter &&
            member->kind != Declaration::VirtualSpec &&
            !member->validated ) {

            Decl(member);
        }
        member = member->next;
    }

    markInheritedVirtuals(d->prefix, d->body ? d->body->scope : 0);

    if( d->body )
        Body(d->body);

    scopeStack.pop_back();
}

static Declaration* findVirtualSpec(Declaration* cls, Atom name)
{
    while( cls )
    {
        if( cls->hidden.contains(name) )
            // hiding a virtual disables further matching at inner levels
            return 0;
        Declaration* v = cls->link;
        while( v )
        {
            // the matching declaration carries the symbol
            if( v->kind == Declaration::VirtualSpec &&
                    ( v->sym == name || ( v->forward && v->forward->sym == name ) ) )
                return v;
            v = v->next;
        }
        cls = cls->prefix;
    }
    return 0;
}

void Validator2::markInheritedVirtuals(Declaration* prefix, Declaration* scope)
{
    if( prefix == 0 || scope == 0 )
        return;
    Declaration* m = scope->link;
    while( m )
    {
        // a declaration matching a virtual of an outer class is virtual as well
        if( m->kind == Declaration::Procedure && !m->isVirtual )
        {
            Declaration* spec = findVirtualSpec(prefix, m->sym);
            if( spec )
            {
                m->isVirtual = true;
                checkVirtualMatch(spec, m);
            }
        }
        m = m->next;
    }
}

void Validator2::ProcDecl(Declaration* d)
{
    // Validate return type
    DeclType(d);

    // the formal parameters are local to a fictitious block embracing the procedure body block
    // so the body block may redeclare a formal parameter
    checkDuplicates(d);
    if( d->body && d->body->scope )
        checkDuplicates(d->body->scope);

    // Open procedure scope
    scopeStack.push_back(d);

    // Validate parameters
    Declaration* param = d->link;
    while( param && param->kind == Declaration::Parameter ) {
        Decl(param);
        param = param->next;
    }

    Declaration* local = d->link;
    while( local ) {
        if( local->kind != Declaration::Parameter && !local->validated ) {
            Decl(local);
        }
        local = local->next;
    }

    if( d->body )
        Body(d->body);

    scopeStack.pop_back();
}

void Validator2::DeclType(Declaration* d)
{
    if( d->getType() == 0 )
        return;
    // the qualification of a parameter is resolved in the scope enclosing the procedure,
    Declaration* outer = paramOf;
    const bool qual = qualCheck;
    paramOf = d->kind == Declaration::Parameter ? d->outer : 0;
    qualCheck = true;
    Type_(d->getType());
    paramOf = outer;
    qualCheck = qual;
}

void Validator2::VarDecl(Declaration* d)
{
    DeclType(d);

    if( d->init )
        Expr(d->init);
}

void Validator2::ArrayDecl(Declaration* d)
{
    DeclType(d);
}

void Validator2::SwitchDecl(Declaration* d)
{
    Expression* e = d->list;
    while( e ) {
        Expr(e);
        // Each element should be a label designator
        e = e->next;
    }
}

void Validator2::ParamDecl(Declaration* d)
{
    DeclType(d);

    Expression* e = d->nameRef;
    while( e )
    {
        Expr(e);
        e = e->next;
    }
}

void Validator2::ExternalDecl(Declaration* d)
{
    // External declarations may not have full type info
    if( d->getType() )
        Type_(d->getType());

    Import imp_;
    imp_.moduleName = d->nameRef && d->nameRef->a ? d->nameRef->a : d->sym;
    imp_.importer = module;
    imp_.importedAt = d->pos;

    if( imp == 0 )
    {
        error(d->pos, "no importer available to resolve external declarations");
        return;
    }
    Declaration* mod = imp->loadModule(imp_);
    if( mod == 0 )
    {
        error(d->pos, QString("cannot load the source module '%1'").arg(imp_.moduleName));
        return;
    }
    if( mod->hasErrors )
    {
        error(d->pos, QString("the source module '%1' has errors").arg(imp_.moduleName));
        return;
    }
    if( !imported.contains(mod) )
        imported.append(mod);
    imp_.resolved = mod;

    // an external declaration is a substitute for a complete introduction of the
    // source module, so every declaration of the module is visible here; the
    // external identifier itself designates the one with the same name
    Declaration* ext = AstModel::findInScope(mod, d->sym, false);
    if( ext == 0 && d->sym != imp_.moduleName )
        ext = AstModel::findInScope(mod, imp_.moduleName, false);
    if( ext == 0 )
    {
        error(d->pos, QString("the source module '%1' has no declaration of '%2'").
              arg(imp_.moduleName).arg(d->name.constData()));
        return;
    }
    if( (d->kind == Declaration::ExternalClass && ext->kind != Declaration::Class) ||
            (d->kind == Declaration::ExternalProc && ext->kind != Declaration::Procedure) )
    {
        error(d->pos, "loaded external object is not compatible with the declaration");
        return;
    }
    d->ext = ext;
    d->setType(ext->getType());
}

void Validator2::ProtectionPart(Declaration* cls)
{
    foreach( const Declaration::Prot& p, cls->prot )
    {
        // a class parameter is an attribute too
        Declaration* here = AstModel::findInScope(cls, p.sym, true);
        if( p.flags & Declaration::Prot::Hide )
        {
            Declaration* d = here;
            bool isProt = here && ( here->visi == Declaration::Protected ||
                                    ( p.flags & Declaration::Prot::Protect ) );
            for( Declaration* super = cls->prefix; super; super = super->prefix )
            {
                Declaration* o = AstModel::findInScope(super, p.sym, true);
                if( o == 0 )
                    continue;
                if( d == 0 )
                    d = o;
                if( o->visi == Declaration::Protected )
                    isProt = true;
            }
            if( d == 0 )
            {
                error(p.pos, QString("no attribute '%1' to be hidden").arg(p.sym));
                continue;
            }
            if( !isProt )
            {
                error(p.pos, QString("only a protected attribute may be hidden: '%1'").arg(p.sym));
                continue;
            }
            if( here )
                here->visi = Declaration::Protected;
            cls->hidden.append(p.sym);
        }else
        {
            if( here == 0 )
            {
                if( findAttr(cls->prefix, p.sym, true, false) )
                    error(p.pos, QString("an attribute may only be protected at the prefix "
                                         "level where it is declared: '%1'").arg(p.sym));
                else
                    error(p.pos, QString("unknown attribute in protection part: '%1'").arg(p.sym));
                continue;
            }
            here->visi = Declaration::Protected;
        }
    }
}

void Validator2::checkDuplicates(Declaration* scope, Declaration* params)
{
    if( ( scope && dupChecked.contains(scope) ) || ( params && dupChecked.contains(params) ) )
        return;
    if( scope )
        dupChecked.insert(scope);
    if( params )
        dupChecked.insert(params);

    QHash<Atom, Declaration*> seen;
    for( int pass = 0; pass < 2; pass++ )
    {
        Declaration* d = pass == 0 ? (params ? params->link : 0) : (scope ? scope->link : 0);
        for( ; d; d = d->next )
        {
            if( d->sym == 0 || d->name.isEmpty() || d->kind == Declaration::VirtualSpec ||
                    d->kind == Declaration::Block )
                continue; // a virtual specification is not a declaration of its own,
                          // and a nested block does not declare an identifier
            Declaration* prev = seen.value(d->sym);
            if( prev )
                error(d->pos, QString("'%1' has already been declared in this block "
                                      "(line %2)").arg(d->name.constData()).arg(prev->pos.d_row));
            else
                seen.insert(d->sym, d);
        }
    }
}

Declaration* Validator2::external(Declaration* d)
{
    if( d == 0 || ( d->kind != Declaration::ExternalClass &&
                    d->kind != Declaration::ExternalProc ) )
        return d;
    Decl(d); // loads the source module if this is the first occurrence
    return d->ext && d->ext != d ? d->ext : d;
}

void Validator2::BlockDecl(Declaration* d)
{
    Declaration* prefix = 0;
    if( d->nameRef && d->nameRef->kind == Expression::Identifier )
        prefix = resolveClass(d->nameRef);
    else if( d->nameRef && d->nameRef->kind == Expression::DeclRef )
        prefix = d->nameRef->d;
    if( prefix )
        scopeStack.push_back(Scope(prefix, PrefixScope));
    scopeStack.push_back(d);
    checkDuplicates(d);
    DeclSeq(d->link);
    markInheritedVirtuals(prefix, d);
    if( d->body )
        Body(d->body);
    scopeStack.pop_back();
    if( prefix )
        scopeStack.pop_back();
}

void Validator2::LabelDecl(Declaration* d)
{
    // Labels don't need type validation
    d->setType(mdl->getType(Type::Label));
}

void Validator2::Type_(Type* t)
{
    if( !t || t->validated )
        return;
    t->validated = true;

    switch( t->kind ) {
    case Type::Array:
        // Validate bound expressions
        if( t->getExpr() ) {
            Expression* bound = t->getExpr();
            while( bound ) {
                Expr(bound);
                bound = bound->next;
            }
        }
        Type_(t->getType()); 
        break;
    case Type::Ref:
        if( t->getExpr() ) {
            Expression* name = t->getExpr();
            if( name->kind == Expression::Identifier )
                resolveClass(name);
            else
                Expr(name);
        }
        break;
    case Type::Procedure:
        // Procedure type - parameters validated separately
        break;
    default:
        // Basic types don't need validation
        break;
    }
}

void Validator2::Body(Statement* s)
{
    StatSeq(s);
}

void Validator2::StatSeq(Statement* s)
{
    while( s ) {
        Declaration* scope = s->getScope();
        if( scope )
            scopeStack.push_back(scope);
        s = Stat(s);
        if( scope )
            scopeStack.pop_back();
    }
}

Statement* Validator2::Stat(Statement* s)
{
    if( !s )
        return 0;

    switch( s->kind ) {
    case Statement::Block:
        BlockStat(s);
        break;
    case Statement::Compound:
        CompoundStat(s);
        break;
    case Statement::If:
        IfStat(s);
        break;
    case Statement::While:
        WhileStat(s);
        break;
    case Statement::For:
        ForStat(s);
        break;
    case Statement::Inspect:
        InspectStat(s);
        break;
    case Statement::Goto:
        GotoStat(s);
        break;
    case Statement::Assign:
        AssignStat(s);
        break;
    case Statement::Call:
        CallStat(s);
        break;
    case Statement::Activate:
        ActivateStat(s);
        break;
    case Statement::Detach:
        DetachStat(s);
        break;
    case Statement::Resume:
        ResumeStat(s);
        break;
    case Statement::Inner:
        // Inner statement - no validation needed
        break;
    case Statement::Dummy:
    case Statement::End:
        // No validation needed
        break;
    case Statement::Label:
        // NOP
        break;
    default:
        invalid("statement", s->pos);
        break;
    }

    return s->next;
}

void Validator2::BlockStat(Statement* s)
{
    if( s->args ) {
        Expression* arg = s->args;
        while( arg ) {
            Expr(arg);
            arg = arg->next;
        }
    }

    // the attributes of the prefix class are visible in the block, but are
    // shadowed by its own declarations
    Declaration* prefix = 0;
    if( s->prefix && s->prefix->kind == Expression::Identifier )
        prefix = resolveClass(s->prefix);
    else if( s->prefix )
        Expr(s->prefix);
    if( s->scope && s->scope->nameRef &&
            s->scope->nameRef->kind == Expression::Identifier )
        resolveClass(s->scope->nameRef);
    if( prefix )
        scopeStack.push_back(Scope(prefix, PrefixScope));

    if( s->scope ) {
        s->scope->validated = true;
        scopeStack.push_back(s->scope);
        checkDuplicates(s->scope);
        DeclSeq(s->scope->link);
        markInheritedVirtuals(prefix, s->scope);
    }

    if( s->body )
        StatSeq(s->body);

    if( s->scope )
        scopeStack.pop_back();
    if( prefix )
        scopeStack.pop_back();
}

void Validator2::CompoundStat(Statement* s)
{
    if( s->scope ) {
        scopeStack.push_back(s->scope);
        checkDuplicates(s->scope);
        DeclSeq(s->scope->link);
    }

    if( s->body )
        StatSeq(s->body);

    if( s->scope )
        scopeStack.pop_back();
}

void Validator2::IfStat(Statement* s)
{
    if( s->cond ) {
        Expr(s->cond);
        Type* ct = s->cond->getType();
        if( ct && ct->kind != Type::Boolean )
            error(s->cond->pos, "if condition must be boolean");
    }

    // then branch
    if( s->body )
        StatSeq(s->body);

    if( s->elseStmt )
        StatSeq(s->elseStmt);
}

void Validator2::WhileStat(Statement* s)
{
    if( s->cond ) {
        Expr(s->cond);
        Type* ct = s->cond->getType();
        if( ct && ct->kind != Type::Boolean )
            error(s->cond->pos, "while condition must be boolean");
    }

    if( s->body )
        StatSeq(s->body);
}

void Validator2::ForStat(Statement* s)
{
    // control variable
    if( s->var ) {
        Expr(s->var);
    }

    Expression* elem = s->list;
    while( elem ) {
        if( elem->kind == Expression::StepUntil ) {
            // step-until element: lhs=start, rhs=step, condition=until
            if( elem->lhs ) Expr(elem->lhs);
            if( elem->rhs ) Expr(elem->rhs);
            if( elem->condition ) Expr(elem->condition);
        } else if( elem->kind == Expression::WhileLoop ) {
            // while element: lhs=start, condition=cond
            if( elem->lhs ) Expr(elem->lhs);
            if( elem->condition ) Expr(elem->condition);
        } else {
            // Simple expression
            Expr(elem);
        }
        elem = elem->next;
    }

    // Validate body
    if( s->body )
        StatSeq(s->body);
}

void Validator2::InspectStat(Statement* s)
{
    // inspect object
    Declaration* cls = 0;
    if( s->obj ) {
        Expr(s->obj);
        Type* ot = s->obj->getType();
        if( ot && ot->kind != Type::Ref && ot->kind != Type::Text )
            error(s->obj->pos, "inspect object must be a reference");
        else if( ot && ot->kind == Type::Ref )
            cls = ot->getRefType();
    }

    // when clauses
    Connection* conn = s->conn;
    while( conn ) {
        // Resolve class name
        if( conn->className != 0 ) {
            Declaration* cls = resolve(conn->className, true);
            if( cls ) {
                conn->classDecl = cls;
                markRef(cls, conn->pos);
            } else {
                error(conn->pos, QString("class '%1' not found").arg(conn->className));
            }
        }

        if( conn->classDecl )
            scopeStack.push_back(Scope(conn->classDecl, ConnectedScope));
        // when body
        if( conn->body )
            StatSeq(conn->body);

        if( conn->classDecl )
            scopeStack.pop_back();

        conn = conn->next;
    }

    if( s->otherwise )
        StatSeq(s->otherwise);

    if( s->body )
    {
        if( cls )
            scopeStack.push_back(Scope(cls, ConnectedScope));
        StatSeq(s->body);
        if( cls )
            scopeStack.pop_back();
    }
}

void Validator2::GotoStat(Statement* s)
{
    // Validate label designator
    if( s->lhs ) {
        Expr(s->lhs);
        // Should resolve to a label
    }
}

void Validator2::AssignStat(Statement* s)
{
    if( s->lhs )
        Expr(s->lhs);

    if( s->rhs )
        Expr(s->rhs);

    if( s->lhs && s->rhs && s->lhs->getType() && s->rhs->getType() ) {
        bool isRefAssign = (s->lhs->kind == Expression::AssignRef);
        if( !assigCompat(s->lhs->getType(), s->rhs, isRefAssign) ) {
            if( isRefAssign )
                error(s->pos, "rhs is not reference assignment compatible with lhs");
            else
                error(s->pos, "rhs is not assignment compatible with lhs");
        }
    }
}

void Validator2::CallStat(Statement* s)
{
    // callable expression
    if( s->lhs )
        Expr(s->lhs);

    // arguments
    // TODO typecheck to param types
    if( s->rhs ) {
        Expression* arg = s->rhs;
        while( arg ) {
            Expr(arg);
            arg = arg->next;
        }
    }
}

void Validator2::ActivateStat(Statement* s)
{
    if( !s->activate )
        return;

    ActivateData* act = s->activate;

    // object expression
    if( act->obj )
        Expr(act->obj);

    // timing expressions
    if( act->at )
        Expr(act->at);
    if( act->delay )
        Expr(act->delay);
    if( act->priorObj )
        Expr(act->priorObj);
}

void Validator2::DetachStat(Statement* s)
{
    // detach may have an expression
    if( s->lhs )
        Expr(s->lhs);
}

void Validator2::ResumeStat(Statement* s)
{
    // resume takes an object expression
    if( s->lhs )
        Expr(s->lhs);
}

bool Validator2::Expr(Expression* e)
{
    if( !e )
        return false;
    if( e->validated )
        return e->getType() != 0;
    e->validated = true;

    bool ok = false;

    switch( e->kind ) {
    case Expression::Add:
    case Expression::Sub:
    case Expression::Mul:
    case Expression::Div:
    case Expression::IntDiv:
    case Expression::Exp:
    case Expression::Concat:
    case Expression::And:
    case Expression::AndThen:
    case Expression::Or:
    case Expression::OrElse:
    case Expression::Imp:
    case Expression::Eqv:
    case Expression::Eq:
    case Expression::Is:
    case Expression::In:
    case Expression::Neq:
    case Expression::Lt:
    case Expression::Leq:
    case Expression::Gt:
    case Expression::Geq:
    case Expression::RefEq:
    case Expression::RefNeq:
        ok = BinaryOp(e);
        break;
    case Expression::Not:
    case Expression::Neg:
        ok = UnaryOp(e);
        break;
    case Expression::Identifier:
        ok = Identifier(e);
        break;
    case Expression::DeclRef:
        ok = DeclRefExpr(e);
        break;
    case Expression::Dot:
        ok = DotExpr(e);
        break;
    case Expression::Subscript:
        ok = SubscriptExpr(e);
        break;
    case Expression::Call:
        ok = CallExpr(e);
        break;
    case Expression::New:
        ok = NewExpr(e);
        break;
    case Expression::This:
        ok = ThisExpr(e);
        break;
    case Expression::Qua:
        ok = QuaExpr(e);
        break;
    case Expression::IfExpr:
        ok = IfExpr(e);
        break;
    case Expression::StringConst:
    case Expression::CharConst:
    case Expression::UnsignedConst:
    case Expression::RealConst:
    case Expression::BoolConst:
    case Expression::Notext:
    case Expression::None:
        ok = Literal(e);
        break;
    case Expression::AssignVal:
    case Expression::AssignRef:
        // These are handled as statements
        if( e->lhs )
            Expr(e->lhs);
        if( e->rhs )
            Expr(e->rhs);
        ok = true;
        break;
    case Expression::StepUntil:
    case Expression::WhileLoop:
        // These are for-loop elements, handled in ForStat
        ok = true;
        break;
    case Expression::TypeRef:
        // Type reference in expressions
        ok = true;
        break;
    default:
        invalid("expression", e->pos);
        break;
    }

    return ok;
}

bool Validator2::ConstExpr(Expression* e)
{
    return Expr(e);
}

bool Validator2::BinaryOp(Expression* e)
{
    Q_ASSERT(e->lhs && e->rhs);
    if( !e->lhs || !e->rhs )
        return false;

    Expr(e->lhs);
    if( ( e->kind == Expression::Is || e->kind == Expression::In ) &&
            e->rhs->kind == Expression::Identifier )
    {
        if( resolveClass(e->rhs) ) {
            Type* refType = new Type(Type::Ref);
            Expression* ref = new Expression(Expression::DeclRef);
            ref->pos = e->rhs->pos;
            ref->d = e->rhs->d;
            refType->setExpr(ref);
            e->rhs->setType(refType);
        }
        e->rhs->validated = true;
    }else
        Expr(e->rhs);

    Type* lt = e->lhs->getType();
    Type* rt = e->rhs->getType();

    if( !lt || !rt )
        return false;

    Type* res = resultType((Expression::Kind)e->kind, lt, rt);
    if( res ) {
        e->setType(res);
        return true;
    }else
        resultType((Expression::Kind)e->kind, lt, rt);

    error(e->pos, "incompatible operand types for binary operator");
    return false;
}

bool Validator2::UnaryOp(Expression* e)
{
    Q_ASSERT(e->rhs);
    if( !e->rhs )
        return false;

    Expr(e->rhs);
    Type* t = e->rhs->getType();

    if( !t )
        return false;

    if( e->kind == Expression::Not ) {
        if( t->kind != Type::Boolean ) {
            error(e->pos, "NOT operator requires boolean operand");
            return false;
        }
        e->setType(mdl->getType(Type::Boolean));
    } else if( e->kind == Expression::Neg ) {
        if( !t->isArithmetic() ) {
            error(e->pos, "unary +/- requires arithmetic operand");
            return false;
        }
        e->setType(t);
    }else
        Q_ASSERT(false);

    return true;
}

bool Validator2::Identifier(Expression* e)
{
    Declaration* d = resolve(e->a);

    if( !d ) {
        // d = resolve(e->a); // TEST
        error(e->pos, QString("declaration for '%1' not found").arg(e->a));
        markUnref(strlen(e->a), e->pos);
        return false;
    }

    Decl(d); // make sure variables declared later are validated

    markRef(d, e->pos);

    e->kind = Expression::DeclRef;
    e->d = d;

    if( d->getType() )
        e->setType(deref(d->getType()));

    return true;
}

bool Validator2::DeclRefExpr(Expression* e)
{
    Declaration* d = e->d;
    if( !d )
        return false;

    if( d->getType() )
        e->setType(deref(d->getType()));

    return true;
}

bool Validator2::DotExpr(Expression* e)
{
    Q_ASSERT(e->lhs);
    if( !e->lhs )
        return false;

    Expr(e->lhs);
    Type* lt = e->lhs->getType();

    if( !lt )
        return false;

    Type_(lt);

    // the class/object type
    Declaration* cls = 0;
    if( lt->kind == Type::Ref )
        cls = lt->getRefType();
    else if( lt->kind == Type::Text )
        cls = mdl->getPrimitiveText();

    // member name from rhs
    if( e->rhs && e->rhs->kind == Expression::Identifier ) {
        Atom memberName = e->rhs->a;

        // try to resolve in the class
        if( cls ) {
            Declaration* member = findAttr(cls, memberName, true, true);
            if( member ) {
                markRef(member, e->rhs->pos);
                e->rhs->kind = Expression::DeclRef;
                e->rhs->d = member;
                if( member->getType() )
                    e->setType(deref(member->getType()));
                return true;
            }
            // detach is an attribute of every object and thus not declared as an attribute of the class
            Declaration* det = AstModel::findInScope(mdl->getBasicIo(), memberName);
            if( det && memberName == Lexer::toId("detach") ) {
                markRef(det, e->rhs->pos);
                e->rhs->kind = Expression::DeclRef;
                e->rhs->d = det;
                return true;
            }
            error(e->pos,"member not found in class");
        }else
            error(e->pos,"cannot apply the dot operator to this type");
        // no error
        markUnref(strlen(memberName), e->rhs->pos);
    }else if( e->rhs && e->rhs->kind == Expression::DeclRef )
    {
        Declaration* member = e->rhs->d;
        markRef(member, e->rhs->pos);
        if( member->getType() )
            e->setType(deref(member->getType()));
        return true;
    }else
        error(e->pos,"invalid member declaration");


    return true;
}

bool Validator2::SubscriptExpr(Expression* e)
{
    Q_ASSERT(e->lhs);
    if( !e->lhs )
        return false;

    Expr(e->lhs);
    Type* lt = e->lhs->getType();

    // Validate subscripts
    if( e->rhs ) {
        Expression* sub = e->rhs;
        while( sub ) {
            Expr(sub);
            Type* st = sub->getType();
            if( st && !st->isArithmetic() ) {
                error(sub->pos, "array subscript must be arithmetic");
            }
            sub = sub->next;
        }
    }

    if( lt && lt->kind == Type::Array )
        e->setType(lt->getType());
    else if( lt && lt->kind == Type::Switch )
        e->setType(mdl->getType(Type::Label));

    return true;
}

bool Validator2::CallExpr(Expression* e)
{
    Q_ASSERT(e->lhs);
    if( !e->lhs )
        return false;

    Expr(e->lhs);

    Type* lt = e->lhs->getType();
    const bool subscripted = lt && ( lt->kind == Type::Array || lt->kind == Type::Switch );
    if( subscripted )
        // a subscripted variable and a call cannot be told apart by the parser
        e->kind = Expression::Subscript;

    // arguments
    if( e->rhs ) {
        Expression* arg = e->rhs;
        while( arg ) {
            Expr(arg);
            arg = arg->next;
        }
    }

    if( subscripted ) {
        Expression* sub = e->rhs;
        while( sub ) {
            Type* st = sub->getType();
            if( st && !st->isArithmetic() )
                error(sub->pos, "array subscript must be arithmetic");
            sub = sub->next;
        }
        e->setType(lt->kind == Type::Array ? lt->getType() : mdl->getType(Type::Label));
    }else if( e->lhs->kind == Expression::DeclRef ) {
        Declaration* d = e->lhs->d;
        if( d ) {
            if( d->kind == Declaration::Builtin )
                checkBuiltinCall(d, e->rhs, e->pos);
            else if( d->kind == Declaration::Procedure || isClassDecl(d) )
                checkActuals(d, e->rhs, e->pos);
            e->setType(callResultType(d->getType(), e->rhs, e->pos));
            stdResultType(d, e);
        }
    }else {
        if( e->lhs->kind == Expression::New ) {
            // the parameters of an object generator are the ones of its class (4.7)
            Type* t = e->lhs->getType();
            Expression* cls = t && t->kind == Type::Ref ? t->getExpr() : 0;
            if( cls && cls->d && isClassDecl(cls->d) )
                checkActuals(cls->d, e->rhs, e->pos);
        }
        e->setType(callResultType(e->lhs->getType(), e->rhs, e->pos));
    }

    return true;
}

bool Validator2::NewExpr(Expression* e)
{
    //  class_identifier(args)
    Q_ASSERT(e->lhs && e->lhs->kind == Expression::Identifier);
    if( resolveClass(e->lhs) )
    {
        Declaration* cls = e->lhs->d;
        Type* refType = new Type(Type::Ref);
        refType->setExpr(e->lhs);
        e->lhs = 0;
        e->setType(refType);
    }

    return true;
}

bool Validator2::ThisExpr(Expression* e)
{
    Atom className = e->a;

    if( className != 0 ) {
        Declaration* cls = resolve(className, true);
        if( cls ) {
            markRef(cls, e->pos);
            Type* refType = new Type(Type::Ref);
            Expression* ref = new Expression(Expression::DeclRef);
            ref->pos = e->pos;
            ref->d = cls;
            refType->setExpr(ref);
            e->setType(refType);
        } else {
            error(e->pos, QString("class '%1' not found").arg(className));
        }
    }

    return true;
}

bool Validator2::QuaExpr(Expression* e)
{
    if( e->lhs )
        Expr(e->lhs);

    if( e->rhs && e->rhs->kind == Expression::Identifier ) {
        Atom className = e->rhs->a;
        Declaration* cls = resolve(className, true);
        if( cls ) {
            markRef(cls, e->rhs->pos);
            e->rhs->kind = Expression::DeclRef;
            e->rhs->d = cls;

            Type* refType = new Type(Type::Ref);
            Expression* ref = new Expression(Expression::DeclRef);
            ref->pos = e->pos;
            ref->d = cls;
            refType->setExpr(ref);
            e->setType(refType);
        } else {
            error(e->rhs->pos, QString("class '%1' not found").arg(className));
        }
    }

    return true;
}

bool Validator2::IfExpr(Expression* e)
{
    if( e->condition ) {
        Expr(e->condition);
        Type* ct = e->condition->getType();
        if( ct && ct->kind != Type::Boolean )
            error(e->condition->pos, "if expression condition must be boolean");
    }

    if( e->lhs )
        Expr(e->lhs);
    if( e->rhs )
        Expr(e->rhs);

    // result type is the common type of then/else branches
    if( e->lhs && e->lhs->getType() )
        e->setType(e->lhs->getType());

    return true;
}

bool Validator2::Literal(Expression* e)
{
    switch( e->kind ) {
    case Expression::UnsignedConst:
        e->setType(mdl->getType(Type::Integer));
        break;
    case Expression::RealConst:
        e->setType(mdl->getType(e->lng ? Type::LongReal : Type::Real));
        break;
    case Expression::CharConst:
        e->setType(mdl->getType(Type::Character));
        break;
    case Expression::StringConst:
        e->setType(mdl->getType(Type::Text));
        break;
    case Expression::BoolConst:
        e->setType(mdl->getType(Type::Boolean));
        break;
    case Expression::Notext:
        e->setType(mdl->getType(Type::Notext));
        break;
    case Expression::None:
        e->setType(mdl->getType(Type::None));
        break;
    default:
        break;
    }
    return true;
}

Type* Validator2::callResultType(Type* t, Expression* args, const RowCol& pos)
{
    if( t == 0 || t->kind != Type::Procedure )
        return t;
    if( !t->subs.isEmpty() )
        checkArgs(t->subs, args, pos);
    return t->getType();
}

void Validator2::checkArgs(const DeclList& params, Expression* args, const RowCol& pos)
{
    int n = 0;
    while( args )
    {
        if( n < params.size() && params[n]->mode == Declaration::ModeValue &&
                args->getType() && params[n]->getType() &&
                !assigCompat(params[n]->getType(), args, args->kind == Expression::AssignRef) )
            error(args->pos, QString("actual parameter is not compatible with formal parameter '%1'").
                  arg(params[n]->name.constData()));
        n++;
        args = args->next;
    }
    if( n != params.size() )
        error(pos, QString("expecting %1 actual parameters, got %2").arg(params.size()).arg(n));
}

enum ParamKind { AnyPar, ArithPar, BoolPar, TextPar, RefPar, OtherPar };


static ParamKind paramKind(Type* t)
{
    if( t == 0 )
        return AnyPar;
    switch( t->kind ) {
    case Type::Integer:
    case Type::ShortInteger:
    case Type::Real:
    case Type::LongReal:
    case Type::Character:
        // let the standard environment do conversions
        return ArithPar;
    case Type::Boolean:
        return BoolPar;
    case Type::Text:
    case Type::Notext:
        return TextPar;
    case Type::Ref:
    case Type::None:
        return RefPar;
    case Type::Array:
    case Type::Switch:
    case Type::Label:
    case Type::Procedure:
    case Type::NoType:
        return OtherPar;
    default:
        return AnyPar;
    }
}

static bool hasCall(Expression* e); 

static bool hasCallList(Expression* e)
{
    for( ; e; e = e->next )
        if( hasCall(e) )
            return true;
    return false;
}

static bool hasCall(Expression* e)
{
    if( e == 0 )
        return false;
    switch( e->kind ) {
    case Expression::Call:
    case Expression::New:
        return true;
    case Expression::DeclRef:
        // a function designator without parameters is just the identifier
        if( e->d && (e->d->kind == Declaration::Procedure || e->d->kind == Declaration::Builtin ||
                     e->d->kind == Declaration::VirtualSpec || e->d->kind == Declaration::ExternalProc) )
            return true;
        break;
    default:
        break;
    }
    return hasCallList(e->lhs) || hasCallList(e->rhs) || hasCallList(e->condition);
}

static Declaration* designated(Expression* e)
{
    // the declaration a variable designator refers to
    while( e && (e->kind == Expression::Subscript || e->kind == Expression::Dot ||
                 e->kind == Expression::Call ||
                 e->kind == Expression::AssignVal || e->kind == Expression::AssignRef) )
        e = e->kind == Expression::Dot ? e->rhs : e->lhs;
    return e && e->kind == Expression::DeclRef ? e->d : 0;
}

static bool assignsTo(Statement* s, Declaration* formal)
{
    for( ; s; s = s->next )
    {
        switch( s->kind ) {
        case Statement::Assign:
            // the left part list is a chain of assignment expressions (4.1)
            for( Expression* a = s->rhs; a && (a->kind == Expression::AssignVal ||
                                               a->kind == Expression::AssignRef); a = a->rhs )
                if( designated(a->lhs) == formal )
                    return true;
            break;
        case Statement::If:
        case Statement::While:
            if( assignsTo(s->elseStmt, formal) )
                return true;
            break;
        case Statement::For:
            if( designated(s->var) == formal )
                return true;
            break;
        case Statement::Inspect:
            for( Connection* c = s->conn; c; c = c->next )
                if( assignsTo(c->body, formal) )
                    return true;
            if( assignsTo(s->otherwise, formal) )
                return true;
            break;
        default:
            break;
        }
        if( s->kind != Statement::Inspect && assignsTo(s->body, formal) )
            return true;
    }
    return false;
}

void Validator2::checkNameActual(Declaration* formal, Expression* arg)
{
    // the left part of an assignment is evaluated before the right part 
    if( formal->mode != Declaration::ModeName || !hasCall(arg) )
        return;
    Declaration* proc = formal->outer;
    if( proc == 0 || !assignsTo(proc->body, formal) )
        return;
    qWarning() << qPrintable(QString(
	"%1:%2:%3: warning: the actual parameter for the NAME formal "
        "'%4' has a side effect and is assigned to; the evaluation order is not standard compliant!")
	     .arg(sourcePath).arg(arg->pos.d_row).arg(arg->pos.d_col)
	     .arg(formal->name.constData()));
}

void Validator2::checkActuals(Declaration* proc, Expression* args, const RowCol& pos)
{
    Q_UNUSED(pos);
    DeclList chain;
    for( Declaration* c = proc; c; c = isClassDecl(c) ? c->prefix : 0 )
        chain.prepend(c); // the parameters of the prefix levels precede the ones of the class (4.7)
    DeclList formals;
    for( int i = 0; i < chain.size(); i++ )
        for( Declaration* d = chain[i]->link; d; d = d->next )
            if( d->kind == Declaration::Parameter )
                formals << d;
    int n = 0;
    while( args && n < formals.size() )
    {
        const ParamKind f = paramKind(formals[n]->getType());
        const ParamKind a = paramKind(args->getType());
        if( f != AnyPar && f != OtherPar && a != AnyPar && a != OtherPar && f != a )
            error(args->pos, QString("the actual parameter is not compatible with the formal parameter '%1'").
                  arg(formals[n]->name.constData()));
        checkNameActual(formals[n], args);
        args = args->next;
        n++;
    }
}

void Validator2::checkVirtualMatch(Declaration* spec, Declaration* impl)
{
    Type* st = spec->getType();
    if( st == 0 || st->kind != Type::Procedure )
        // only a procedure specification (5.5.3) constrains the matching declaration
        return;

    if( impl->kind != Declaration::Procedure )
    {
        error(impl->pos, "a specified virtual procedure can only be matched by a procedure");
        return;
    }

    Type* res = impl->getType();
    if( res && res->kind == Type::NoType )
        res = 0;
    if( ( st->getType() == 0 ) != ( res == 0 ) ||
            ( res && !typeCompat(st->getType(), res) ) )
        error(impl->pos, "the result type doesn't match the virtual procedure specification");

    Declaration* param = impl->link;
    int n = 0;
    while( param && param->kind == Declaration::Parameter )
    {
        if( n < st->subs.size() )
        {
            Declaration* formal = st->subs[n];
            if( formal->mode != param->mode || formal->kind != param->kind ||
                    !typeCompat(formal->getType(), param->getType()) )
                error(param->pos, QString("parameter '%1' doesn't match the virtual procedure specification").
                      arg(param->name.constData()));
        }
        n++;
        param = param->next;
    }
    if( n != st->subs.size() )
        error(impl->pos, "the number of parameters doesn't match the virtual procedure specification");
}

bool Validator2::assigCompat(Type* lhs, Type* rhs, bool isRefAssign)
{
    if( !lhs || !rhs )
        return false;

    if( isRefAssign ) {
        // Reference assignment: both must be reference types
        if( !lhs->isRef() || !rhs->isRef() )
            return false;
        // For now, accept if both are references
        return true;
    }

    if( lhs->kind == rhs->kind )
        return true;

    // arithmetic widening
    if( lhs->isArithmetic() && rhs->isArithmetic() ) {
        // Integer can be assigned to Real
        if( lhs->isReal() && rhs->isInteger() )
            return true;
        // Same arithmetic type
        if( lhs->kind == rhs->kind )
            return true;
        // LongReal accepts Real
        if( lhs->kind == Type::LongReal && rhs->kind == Type::Real )
            return true;
        // ShortInteger to Integer
        if( lhs->kind == Type::Integer && rhs->kind == Type::ShortInteger )
            return true;
    }

    if( lhs->kind == Type::Text && rhs->kind == Type::Text )
        return true;

    return false;
}

bool Validator2::assigCompat(Type* lhs, Expression* rhs, bool isRefAssign)
{
    if( !rhs )
        return false;
    return assigCompat(lhs, rhs->getType(), isRefAssign);
}

bool Validator2::typeCompat(Type* t1, Type* t2)
{
    if( !t1 || !t2 )
        return false;
    if( t1->kind == t2->kind )
        return true;
    if( t1->isArithmetic() && t2->isArithmetic() )
        return true;
    return false;
}

bool Validator2::isSubclassOf(Declaration* sub, Declaration* super)
{
    if( !sub || !super )
        return false;

    Declaration* cur = sub;
    while( cur ) {
        if( cur == super )
            return true;
        cur = cur->prefix;
    }
    return false;
}

static inline bool isTextType(Type* t)
{
    return t && ( t->kind == Type::Text || t->kind == Type::Notext );
}

void Validator2::stdResultType(Declaration* proc, Expression* e)
{
    // abs delivers the type of its parameter, max and min the type resulting from the two parameters
    if( proc == 0 || e == 0 || e->rhs == 0 )
        return;
    if( AstModel::findInScope(mdl->getEnv(), proc->sym) != proc )
        return; // not an attribute of the class environment
    const bool isAbs = proc->sym == Lexer::toId("abs");
    const bool isExtremum = proc->sym == Lexer::toId("max") ||
                            proc->sym == Lexer::toId("min");
    if( !isAbs && !isExtremum )
        return;
    Type* a = deref(e->rhs->getType());
    if( a == 0 || !a->isArithmetic() )
        return;
    if( isAbs ) {
        e->setType(a);
        return;
    }
    Type* b = e->rhs->next ? deref(e->rhs->next->getType()) : 0;
    if( b == 0 || !b->isArithmetic() )
        return;
    if( a->kind == Type::LongReal || b->kind == Type::LongReal )
        e->setType(mdl->getType(Type::LongReal));
    else if( a->kind == Type::Real || b->kind == Type::Real )
        e->setType(mdl->getType(Type::Real));
    else
        e->setType(mdl->getType(Type::Integer));
}

Type* Validator2::resultType(Expression::Kind op, Type* lhs, Type* rhs)
{
    if( !lhs || !rhs )
        return 0;

    switch( op ) {
    case Expression::Add:
    case Expression::Sub:
    case Expression::Mul:
    case Expression::Div:
    case Expression::Exp:
        if( lhs->isArithmetic() && rhs->isArithmetic() ) {
            // 3.5.1: the wider type, but never short integer, and / is a real division
            if( lhs->kind == Type::LongReal || rhs->kind == Type::LongReal )
                return mdl->getType(Type::LongReal);
            if( lhs->kind == Type::Real || rhs->kind == Type::Real
                    || op == Expression::Div )
                return mdl->getType(Type::Real);
            return mdl->getType(Type::Integer);
        }
        break;

    case Expression::Concat:
        if( isTextType(lhs) && isTextType(rhs) )
            return mdl->getType(Type::Text);
        break;

    case Expression::IntDiv:
        if( lhs->isInteger() && rhs->isInteger() )
            return mdl->getType(Type::Integer);
        break;

    case Expression::And:
    case Expression::AndThen:
    case Expression::Or:
    case Expression::OrElse:
    case Expression::Imp:
    case Expression::Eqv:
        if( lhs->kind == Type::Boolean && rhs->kind == Type::Boolean )
            return mdl->getType(Type::Boolean);
        break;

    case Expression::Eq:
    case Expression::Neq:
    case Expression::Lt:
    case Expression::Leq:
    case Expression::Gt:
    case Expression::Geq:
    case Expression::Is:
    case Expression::In:
        if( typeCompat(lhs, rhs) )
            return mdl->getType(Type::Boolean);
        break;

    case Expression::RefEq:
    case Expression::RefNeq:
        if( lhs->isRef() && rhs->isRef() )
            return mdl->getType(Type::Boolean);
        break;

    default:
        break;
    }

    return 0;
}

Type* Validator2::deref(Type* t)
{
    if( t && t->kind == Type::Procedure )
        // an identifier denoting a procedure is a call of the procedure
        return t->getType();
    return t;
}


bool Validator2::withinClass(Declaration* cls) const
{
    // true if the current context is the body of cls or of a subclass
    // or a block prefixed by cls or a subclass
    for( int i = scopeStack.size() - 1; i >= 0; --i ) 
    {
        if( scopeStack[i].kind == ConnectedScope || !isClassDecl(scopeStack[i].d) )
            continue;
        Declaration* p = scopeStack[i].d;
        while( p ) {
            if( p == cls )
                return true;
            p = p->prefix;
        }
    }
    return false;
}

static bool isLabelOrSwitch(Declaration* d)
{
    if( d->kind == Declaration::LabelDecl || d->kind == Declaration::Switch )
        return true;
    if( d->kind == Declaration::VirtualSpec ) {
        Type* t = d->getType();
        return t && ( t->kind == Type::Label || t->kind == Type::Switch );
    }
    return false;
}

int Validator2::contextHides(Declaration* cls, Atom sym) const
{
    for( int i = scopeStack.size() - 1; i >= 0; --i )
    {
        if( scopeStack[i].kind == ConnectedScope || !isClassDecl(scopeStack[i].d) )
            continue;
        int hides = 0;
        Declaration* p = scopeStack[i].kind == OwnScope ?
                    scopeStack[i].d->prefix : scopeStack[i].d;
        for( ; p; p = p->prefix )
            if( p->hidden.contains(sym) )
                hides++;
        return hides;
    }
    return 0;
}

Declaration* Validator2::findAttr(Declaration* cls, Atom sym, bool inclusive, bool remote, bool classOnly)
{
    // the prefix chain is searched from the innermost level outwards
    // each hidden spec makes the innermost accessible attribute of that name invisible
    int hides = contextHides(cls, sym);
    Declaration* level = cls;
    while( level ) {
        Decl(level); // many names are resolved from classes defined after the reference
        Declaration* d = external(AstModel::findInScope(level, sym));
        if( d && remote && isLabelOrSwitch(d) )
            // label and switch attributes are not accessible by remote access (4.4, 5.5.5)
            d = 0;
        if( d && ( !classOnly || isClassDecl(d) ) ) {
            if( hides > 0 )
                hides--;
            else if( !remote || d->visi != Declaration::Protected || withinClass(level) )
                return d;
        }
        level = level->prefix;
    }
    return 0;
}

Declaration* Validator2::resolve(Atom sym, bool classOnly)
{
    // Search scope stack from innermost to outermost
    for( int i = scopeStack.size() - 1; i >= 0; --i ) {
        Declaration* scope = scopeStack[i].d;

        Declaration* d;
        if( isClassDecl(scope) )
            d = findAttr(scope, sym, scopeStack[i].kind != OwnScope,
                         scopeStack[i].kind == ConnectedScope, classOnly);
        else
            d = external(AstModel::findInScope(scope, sym));
        if( d && ( !classOnly || isClassDecl(d) ) )
            return d;
    }
    Declaration* d = external(AstModel::findInScope(mdl->getGlobals(), sym));
    if( d && ( !classOnly || isClassDecl(d) ) )
        return d;
    foreach( Declaration* mod, imported )
    {
        d = external(AstModel::findInScope(mod, sym, false));
        if( d && ( !classOnly || isClassDecl(d) ) )
            return d;
    }
    return 0;
}

Declaration* Validator2::resolveHiding(Atom sym)
{
    Declaration* res = resolve(sym, false);
    if( res == 0 || paramOf == 0 )
        return res;

    Declaration* bodyScope = paramOf->body ? paramOf->body->scope : 0;
    if( res->outer != paramOf && res->outer != bodyScope )
        return res;

    const QList<Scope> saved = scopeStack;
    for( int i = scopeStack.size() - 1; i >= 0; i-- )
        if( scopeStack[i].d == paramOf || scopeStack[i].d == bodyScope )
            scopeStack.removeAt(i);
    res = resolve(sym, false);
    scopeStack = saved;
    if( res && ( res->outer == paramOf || res->outer == bodyScope ) )
        res = 0;
    return res;
}

Declaration* Validator2::resolveClass(Expression* name)
{
    // a class identifier in a qualification is an identifier like any other, so an
    // inner declaration of the same name hides the class and makes the qualification illegal
    Declaration* cls = resolve(name->a, true);
    Declaration* hiding = qualCheck ? resolveHiding(name->a) : 0;
    if( cls && hiding && hiding != cls && !isClassDecl(hiding) )
        error(name->pos, QString("'%1' has illegal qualification: the class is hidden by "
                                 "the declaration in line %2")
              .arg(name->a).arg(hiding->pos.d_row));
    if( cls )
    {
        markRef(cls, name->pos);
        name->kind = Expression::DeclRef;
        name->d = cls;
        Decl(cls);
    }else
    {
        error(name->pos, QString("class '%1' not found").arg(name->a));
        markUnref(strlen(name->a), name->pos);
    }
    return cls;
}

void Validator2::checkBuiltinCall(Declaration* builtin, Expression* args, const RowCol& pos)
{
    if( !builtin )
        return;

    int id = builtin->id;

    // Count arguments
    int argCount = 0;
    Expression* arg = args;
    while( arg ) {
        argCount++;
        arg = arg->next;
    }

#if 0
    switch( id ) {
    case Builtin::ABS:
    case Builtin::SIGN:
    case Builtin::ENTIER:
    case Builtin::SQRT:
    case Builtin::SIN:
    case Builtin::COS:
    case Builtin::ARCTAN:
    case Builtin::LN:
    case Builtin::EXP:
        if( argCount != 1 )
            error(pos, QString("builtin '%1' expects 1 argument").arg(Builtin::name[id]));
        break;

    case Builtin::MOD:
    case Builtin::REM:
    case Builtin::LOWTEN:
        if( argCount != 2 )
            error(pos, QString("builtin '%1' expects 2 arguments").arg(Builtin::name[id]));
        break;

    case Builtin::COPY:
        if( argCount != 1 )
            error(pos, QString("builtin '%1' expects 1 argument").arg(Builtin::name[id]));
        break;

    case Builtin::DETACH:
    case Builtin::RESUME:
    case Builtin::CALL:
        // These may have 0 or 1 argument
        break;

    case Builtin::TIME:
    case Builtin::SOURCELINE:
        if( argCount != 0 )
            error(pos, QString("builtin '%1' expects no arguments").arg(Builtin::name[id]));
        break;
        // TODO

    default:
        break;
    }
#endif
}
