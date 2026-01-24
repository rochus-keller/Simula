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

#include "SimParser3.h"

#include <Simula/SimLexer.h>
#include <Simula/SimErrors.h>

namespace Sim
{
    static inline QString tokName(const Token& t)
    {
        return QString::fromLatin1(t.getName());
    }

    static inline RowCol toPos(const Token& t)
    {
        return RowCol(t.d_lineNr, t.d_colNr);
    }

    Parser3::Parser3(Lexer* lexer, AstModel *model)
        : d_lexer(lexer), d_ast(model), d_errSink(0), d_root(0)
    {
    }

    Token Parser3::la(int k) const
    {
        if (!d_lexer)
            return Token(Tok_Invalid);
        if (k < 1)
            k = 1;
        return d_lexer->peekToken(quint8(k));
    }

    Token Parser3::take()
    {
        if (!d_lexer)
            return Token(Tok_Invalid);
        return d_lexer->nextToken();
    }

    bool Parser3::accept(TokenType t)
    {
        if (la().d_type == t) {
            take();
            return true;
        }
        return false;
    }

    bool Parser3::expect(TokenType t, const char* what, const char* rule)
    {
        if (accept(t))
            return true;

        const Token got = la();
        QString msg;
        if (what && *what) {
            msg = QString("Expected %1, got %2.")
                      .arg(QString::fromLatin1(what), tokName(got));
        } else {
            msg = QString("Expected %1, got %2.")
                      .arg(QString::fromLatin1(Token(t).getName()), tokName(got));
        }

        addError(msg, {t}, rule, got);
        return false;
    }

    void Parser3::addError(const QString& msg,
                           const QVector<TokenType>& expected,
                           const char* rule,
                           const Token& at)
    {
        const Token locTok = at.isValid() ? at : la();
        const QString r = rule ? QString::fromLatin1(rule) : QString();

        d_errors.append(ParseError(msg, locTok, expected, r));

        if (d_errSink) {
            d_errSink->error(Errors::Syntax,
                             locTok.d_sourcePath,
                             int(locTok.d_lineNr),
                             int(locTok.d_colNr),
                             msg);
        }
    }

    void Parser3::synchronize()
    {
        for (;;) {
            const Token t = la();
            if (!t.isValid() || t.d_type == Tok_Eof)
                return;

            switch (t.d_type) {
            case Tok_Semi:
            case Tok_END:
            case Tok_ELSE:
            case Tok_WHEN:
            case Tok_OTHERWISE:
                return;
            default:
                take();
                break;
            }
        }
    }





    void Parser3::type()
    {
        // Minimal: [SHORT|LONG] (INTEGER|REAL|BOOLEAN|CHARACTER|TEXT|REF class_identifier)
        if (TT(this) == Tok_SHORT) take();
        else if (TT(this) == Tok_LONG) take();

        if (TT(this) == Tok_INTEGER || TT(this) == Tok_REAL || TT(this) == Tok_BOOLEAN ||
            TT(this) == Tok_CHARACTER || TT(this) == Tok_TEXT) {
            take();
            return;
        }

        if (TT(this) == Tok_REF) {
            take();
            class_identifier(); // minimal identifier for now
            return;
        }

        addError(QString("type not implemented for token %1.").arg(tokName(la())), {}, "type");
        synchronize();
    }

    void Parser3::variable_identifier()
    {
        expect(Tok_identifier, "identifier", "variable_identifier");
    }

    void Parser3::parameter_delimiter()
    {
        expect(Tok_Comma, "','", "parameter_delimiter");
    }

    void Parser3::relation_()
    {
        relational_operator();
        simple_expression_();
    }

    void Parser3::qualified_()
    {
        expect(Tok_QUA, "QUA", "qualified_");
        class_identifier();
    }

    Expression* Parser3::newExpr(Expression::Kind k)
    {
        RowCol pos;
        if (d_lexer)
            pos = toPos(d_lexer->peekToken(1));
        Expression* e = new Expression(k, pos);
        // In a full implementation, we would link e into d_ast or a cleaner mechanism
        return e;
    }

    Expression* Parser3::newExpr(Expression::Kind k, Expression* lhs, Expression* rhs)
    {
        Expression* e = newExpr(k);
        e->lhs = lhs;
        e->rhs = rhs;
        return e;
    }

    Expression* Parser3::newExprVal(Expression::Kind k, const Token& t)
    {
        Expression* e = new Expression(k, toPos(t));
        e->val = t.d_val; // Store literal string/value
        return e;
    }

    Expression* Parser3::expression()
    {
        // expression ::= quaternary_ | if_clause quaternary_ ELSE expression 
        if (TT(this) == Tok_IF) {
            Expression* cond = if_clause();
            Expression* thenExpr = quaternary_();
            expect(Tok_ELSE, "ELSE", "expression");
            Expression* elseExpr = expression();

            Expression* ifNode = newExpr(Expression::IfExpr);
            ifNode->condition = cond;
            ifNode->lhs = thenExpr;
            ifNode->rhs = elseExpr;
            return ifNode;
        }
        return quaternary_();
    }

    Expression* Parser3::if_clause()
    {
        expect(Tok_IF, "IF", "if_clause");
        Expression* cond = expression();
        expect(Tok_THEN, "THEN", "if_clause");
        return cond;
    }

    Expression* Parser3::quaternary_()
    {
        // quaternary_ ::= tertiary_ { OR_ELSE tertiary_ }
        Expression* lhs = tertiary_();
        while (TT(this) == Tok_OR_ELSE) {
            expect(Tok_OR_ELSE, "OR ELSE", "quaternary_");
            Expression* rhs = tertiary_();
            // SimAst might not have ORELSE kind, usually mapped to Or with short-circuit semantics
            lhs = newExpr(Expression::Or, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::tertiary_()
    {
        // tertiary_ ::= equivalence_ { AND_THEN equivalence_ }
        Expression* lhs = equivalence_();
        while (TT(this) == Tok_AND_THEN) {
            expect(Tok_AND_THEN, "AND THEN", "tertiary_");
            Expression* rhs = equivalence_();
            lhs = newExpr(Expression::And, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::equivalence_()
    {
        Expression* lhs = implication();
        while (true) {
            Expression::Kind op = equiv_sym_();
            if (op == Expression::Invalid) break;
            Expression* rhs = implication();
            lhs = newExpr(op, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::implication()
    {
        Expression* lhs = simple_expression_();
        while (true) {
            Expression::Kind op = impl_sym_();
            if (op == Expression::Invalid) break;
            Expression* rhs = simple_expression_();
            lhs = newExpr(op, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::simple_expression_()
    {
        Expression* lhs = term();
        while (true) {
            Expression::Kind op = adding_operator();
            if (op == Expression::Invalid) op = or_sym_();

            if (op == Expression::Invalid) break;

            Expression* rhs = term();
            lhs = newExpr(op, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::term()
    {
        Expression* lhs = factor();
        while (true) {
            Expression::Kind op = multiplying_operator();
            if (op == Expression::Invalid) op = and_sym_();

            if (op == Expression::Invalid) break;

            Expression* rhs = factor();
            lhs = newExpr(op, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::factor()
    {
        Expression* lhs = secondary();
        while (true) {
            Expression::Kind op = power_sym_();
            if (op == Expression::Invalid) break;

            Expression* rhs = secondary();
            lhs = newExpr(op, lhs, rhs);
        }
        return lhs;
    }

    Expression* Parser3::secondary()
    {
        Expression::Kind unaryOp = not_sym_(); // Check NOT
        if (unaryOp == Expression::Invalid) {
            // Check Unary Plus/Minus
            Expression::Kind addOp = adding_operator();
            if (addOp == Expression::Plus) unaryOp = Expression::Invalid; // Unary plus is no-op usually
            else if (addOp == Expression::Minus) unaryOp = Expression::Minus; // Unary minus
        }

        Expression* node = primary();

        if (unaryOp != Expression::Invalid) {
            // For unary minus, we might treat it as 0 - node or a specific UnaryMinus kind
            // SimAst has Not. It doesn't seem to have explicit UnaryMinus, often handled as Minus with null LHS or 0.
            // Let's use Minus with null LHS for now, or newExpr(Minus, nullptr, node).
            node = newExpr(unaryOp, nullptr, node);
        }
        return node;
    }

    Expression* Parser3::primary()
    {
        Expression* head = nullptr;
        const int t = TT(this);

        if (t == Tok_unsigned_integer || t == Tok_decimal_number) {
            head = unsigned_number();
        } else if (t == Tok_TRUE || t == Tok_FALSE) {
            head = logical_value();
        } else if (t == Tok_character) {
            Token tok = take();
            head = newExprVal(Expression::CharConst, tok);
        } else if (t == Tok_string) {
            head = string_();
        } else if (t == Tok_NOTEXT) {
            take();
            head = newExpr(Expression::TextConst); // Val remains empty/null
        } else if (t == Tok_NONE) {
            take();
            head = newExpr(Expression::RefEq); // Placeholder: NONE is a value, maybe use Ref with null?
            // SimAst doesn't have explicit None kind, use Identifier "NONE" or similar?
            // Let's use Invalid/Custom for now or RefEq check context.
            // Actually, usually handled as a special Identifier or null pointer literal.
            head = newExprVal(Expression::Identifier, Token(Tok_NONE, 0,0,0, "NONE"));
        } else if (t == Tok_THIS) {
            head = local_object();
        } else if (t == Tok_NEW) {
            head = object_generator();
        } else if (t == Tok_Lpar) {
            expect(Tok_Lpar, "'('", "primary");
            head = expression();
            expect(Tok_Rpar, "')'", "primary");
        } else if (t == Tok_identifier) {
            Token tok = take();
            head = newExprVal(Expression::Identifier, tok);
        } else {
            addError(QString("primary: unexpected %1.").arg(tokName(la())), {}, "primary");
            return newExpr(Expression::Invalid);
        }

        // Postfix loop (dot, subscript, qua, etc.)
        for (;;) {
            if (TT(this) == Tok_Dot || TT(this) == Tok_Lbrack || TT(this) == Tok_Lpar) {
                selector_(head);
            } else if (TT(this) == Tok_QUA) {
                qualified_(head);
            } else {
                Expression::Kind relOp = relational_operator();
                if (relOp != Expression::Invalid) {
                    Expression* rhs = simple_expression_();
                    head = newExpr(relOp, head, rhs);
                    // Relations are non-associative in Simula (cannot chain a < b < c directly in grammar logic usually)
                    // The EBNF implies they wrap the whole primary_nlr, essentially ending the chain or continuing?
                    // "primary_nlr_ ::= relation_ ..."
                    // Parser2: "relation_(); primary_nlr_();" -> structural recursion.
                    // This means a < b = c is parsed as a < (b = c)? No, relations usually lowest precedence.
                    // We'll assume relation terminates the primary chain for this level.
                    return head;
                } else {
                    break;
                }
            }
        }
        return head;
    }

    void Parser3::selector_(Expression*& lhs)
    {
        if (TT(this) == Tok_Dot) {
            expect(Tok_Dot, "'.'", "selector_");
            Expression* id = attribute_identifier();
            lhs = newExpr(Expression::Dot, lhs, id);
        } else if (TT(this) == Tok_Lbrack) {
            expect(Tok_Lbrack, "'['", "selector_");
            QList<Expression*> subs = subscript_list();
            expect(Tok_Rbrack, "']'", "selector_");

            // Build subscript node: Dot/Subscript kind?
            // SimAst has Subscript.
            // We need to attach list. Expression structure usually binary.
            // A common way: lhs = Subscript(lhs, ListExpr)
            Expression* listNode = nullptr;
            for (int i=subs.size()-1; i>=0; --i) {
                Expression* e = subs[i];
                e->next = listNode;
                listNode = e;
            }
            lhs = newExpr(Expression::Subscript, lhs, listNode);
        } else if (TT(this) == Tok_Lpar) {
            actual_parameter_part(lhs);
        }
    }

    void Parser3::qualified_(Expression*& lhs)
    {
        expect(Tok_QUA, "QUA", "qualified_");
        // class_identifier is just an identifier, we need it as an expression or type?
        // simple hack: parse identifier, wrap in Qua
        Token tok = take(); // Identifier
        Expression* clsId = newExprVal(Expression::Identifier, tok);
        lhs = newExpr(Expression::Qua, lhs, clsId);
    }

    void Parser3::actual_parameter_part(Expression*& lhs)
    {
        expect(Tok_Lpar, "'('", "actual_parameter_part");
        QList<Expression*> params = actual_parameter_list();
        expect(Tok_Rpar, "')'", "actual_parameter_part");

        // Convert lhs (Identifier or Dot) into a Call
        Expression* listNode = nullptr;
        for (int i=params.size()-1; i>=0; --i) {
            Expression* e = params[i];
            e->next = listNode;
            listNode = e;
        }
        lhs = newExpr(Expression::Call, lhs, listNode);
    }

    Expression::Kind Parser3::adding_operator()
    {
        if (accept(Tok_Plus)) return Expression::Plus;
        if (accept(Tok_Minus)) return Expression::Minus;
        return Expression::Invalid;
    }

    Expression::Kind Parser3::multiplying_operator()
    {
        if (accept(Tok_Star)) return Expression::Mul;
        if (accept(Tok_Slash)) return Expression::Div;
        if (accept(Tok_2Slash)) return Expression::IntDiv;
        return Expression::Invalid;
    }

    Expression::Kind Parser3::relational_operator()
    {
        int t = TT(this);
        Expression::Kind k = Expression::Invalid;
        switch(t) {
            case Tok_Lt: k = Expression::Lt; break;
            case Tok_Leq: k = Expression::Leq; break;
            case Tok_Eq: k = Expression::Eq; break;
            case Tok_Geq: k = Expression::Geq; break;
            case Tok_Gt: k = Expression::Gt; break;
            case Tok_NE: k = Expression::Neq; break;
            case Tok_2Eq: k = Expression::RefEq; break;
            case Tok_EqSlashEq: k = Expression::RefNeq; break;
            default: return Expression::Invalid;
        }
        take();
        return k;
    }

    Expression::Kind Parser3::or_sym_() { return accept(Tok_OR) || accept(Tok_Uor) || accept(Tok_Bar) ? Expression::Or : Expression::Invalid; }
    Expression::Kind Parser3::and_sym_() { return accept(Tok_AND) || accept(Tok_Uand) || accept(Tok_Amp) ? Expression::And : Expression::Invalid; }
    Expression::Kind Parser3::not_sym_() { return accept(Tok_NOT) || accept(Tok_Unot) || accept(Tok_Bang) ? Expression::Not : Expression::Invalid; }

    Expression::Kind Parser3::equiv_sym_() { return accept(Tok_EQUIV) ? Expression::Eqv : Expression::Invalid; } // Simplified
    Expression::Kind Parser3::impl_sym_() { return accept(Tok_IMPL) ? Expression::Imp : Expression::Invalid; }   // Simplified
    Expression::Kind Parser3::power_sym_() { return accept(Tok_POWER) || accept(Tok_Hat) ? Expression::Exp : Expression::Invalid; }

    Expression* Parser3::unsigned_number() { return newExprVal(Expression::NumConst, take()); }
    Expression* Parser3::string_() { return newExprVal(Expression::TextConst, take()); }
    Expression* Parser3::attribute_identifier() { return newExprVal(Expression::Identifier, take()); }
    Expression* Parser3::logical_value() { return newExprVal(Expression::NumConst, take()); } // Bool as num/const

    Expression* Parser3::local_object()
    {
        expect(Tok_THIS, "THIS", "local_object");
        Token cls = take();
        Expression* clsNode = newExprVal(Expression::Identifier, cls);
        return newExpr(Expression::This, nullptr, clsNode);
    }

    Expression* Parser3::object_generator()
    {
        expect(Tok_NEW, "NEW", "object_generator");
        Token cls = take();
        Expression* clsNode = newExprVal(Expression::Identifier, cls);
        Expression* gen = newExpr(Expression::New, nullptr, clsNode);
        if (TT(this) == Tok_Lpar) { // Optional arguments NEW C(...)
             actual_parameter_part(gen); // This transforms gen (New) into Call(New, args)
        }
        return gen;
    }

    QList<Expression*> Parser3::actual_parameter_list()
    {
        QList<Expression*> list;
        list.append(actual_parameter());
        while (TT(this) == Tok_Comma) {
            take();
            list.append(actual_parameter());
        }
        return list;
    }

    Expression* Parser3::actual_parameter()
    {
        return expression();
    }

    QList<Expression*> Parser3::subscript_list()
    {
        QList<Expression*> list;
        list.append(subscript_expression());
        while (TT(this) == Tok_Comma) {
            take();
            list.append(subscript_expression());
        }
        return list;
    }

    Expression* Parser3::subscript_expression()
    {
        return expression();
    }

    // Stub implementations for declarations to keep linker happy
    Expression* Parser3::lower_bound() { return expression(); }
    Expression* Parser3::upper_bound() { return expression(); }


    Statement* Parser3::newStmt(Statement::Kind k)
    {
        Statement* s = new Statement(k, d_lexer ? toPos(d_lexer->peekToken(1)) : RowCol());
        // In full impl, might link to AST model if needed
        return s;
    }

    Declaration* Parser3::newDecl(const QByteArray& name, Declaration::Kind k)
    {
        // Use AstModel to create/register decl if available
        if (d_ast) return d_ast->addDecl(name, k);
        return new Declaration(k); // Fallback
    }

    Statement* Parser3::statement()
    {
        // statement ::= { label ':' } [ Common_Base_statement | while_statement ]
        // Labels are usually attached to the statement.
        // For simplicity here, we parse labels but don't strictly attach them yet
        // (SimAst Statement has no explicit label list, might need Declaration::Label).

        while (TT(this) == Tok_identifier && TT(this, 2) == Tok_Colon) {
            label();
            expect(Tok_Colon, "':'", "statement");
        }

        // dummy-statement
        if (TT(this) == Tok_Semi || TT(this) == Tok_END || TT(this) == Tok_INNER || TT(this) == Tok_Eof)
            return newStmt(Statement::Dummy);

        if (TT(this) == Tok_WHILE) {
            return while_statement();
        }

        return Common_Base_statement();
    }

    Statement* Parser3::while_statement()
    {
        // WHILE expression DO statement
        Statement* s = newStmt(Statement::While);
        expect(Tok_WHILE, "WHILE", "while_statement");
        s->expr = expression(); // Condition
        expect(Tok_DO, "DO", "while_statement");
        s->body = statement();
        return s;
    }

    Statement* Parser3::Common_Base_statement()
    {
        if (TT(this) == Tok_IF) {
            return Common_Base_conditional_statement();
        } else if (TT(this) == Tok_FOR) {
            return for_statement();
        } else {
            return unconditional_statement();
        }
    }

    Statement* Parser3::Common_Base_conditional_statement()
    {
        // IF expression THEN ...
        Statement* s = newStmt(Statement::If);
        s->expr = if_clause(); // Condition

        // Labels inside IF...
        while (TT(this) == Tok_identifier && TT(this, 2) == Tok_Colon) {
            label();
            expect(Tok_Colon, "':'", "Common_Base_conditional_statement");
        }

        // True branch
        if (TT(this) == Tok_FOR) {
            s->body = for_statement();
        } else if (TT(this) == Tok_WHILE) {
            s->body = while_statement();
        } else if (TT(this) == Tok_ELSE) {
            // "IF cond THEN ELSE stmt" - Empty true branch (Dummy)
            s->body = newStmt(Statement::Dummy);
        } else {
            s->body = unconditional_statement();
        }

        // False branch
        if (TT(this) == Tok_ELSE) {
            expect(Tok_ELSE, "ELSE", "Common_Base_conditional_statement");
            s->elseStmt = statement();
        }

        return s;
    }

    Statement* Parser3::unconditional_statement()
    {
        return unlabelled_basic_statement();
    }

    Statement* Parser3::unlabelled_basic_statement()
    {
        if (TT(this) == Tok_GOTO || TT(this) == Tok_GO) {
            return go_to_statement();
        }

        if (TT(this) == Tok_ACTIVATE || TT(this) == Tok_REACTIVATE) {
            return activation_statement();
        }

        if (TT(this) == Tok_INSPECT) {
            return connection_statement();
        }

        if (TT(this) == Tok_BEGIN) {
            return main_block(); // Returns Compound/Block statement
        }

        // Expression-based statements (Assignments, Procedure Calls)
        if (!isPrimaryStart(TT(this))) {
            addError(QString("unlabelled_basic_statement: unexpected %1.").arg(tokName(la())), {}, "unlabelled_basic_statement");
            synchronize();
            return newStmt(Statement::Invalid);
        }

        Expression* lhs = primary();

        // Prefixed Block: C BEGIN ...
        if (TT(this) == Tok_BEGIN) {
            // lhs is class identifier (prefix)
            // SimAst doesn't have explicit PrefixedBlock, usually Block with 'prefix' pointer?
            // For now, treat as Block.
            return main_block();
        }

        // Assignments: lhs := rhs
        if (TT(this) == Tok_ColonEq) {
            Statement* head = nullptr;
            Statement* curr = nullptr;

            do {
                expect(Tok_ColonEq, "':='", "unlabelled_basic_statement");
                Expression* rhs = expression();
                Statement* assign = newStmt(Statement::Assign);
                assign->expr = lhs; // LHS
                assign->body = (Statement*)rhs; // Hack: Store RHS in body pointer? Or cast?
                // Actually SimAst Statement has expr (Cond/LHS).
                // Assignment is tricky in this specific AST node design.
                // Usually: Assign statement has lhs and rhs expressions.
                // SimAst Statement has `Expression* expr`.
                // Let's assume we treat Assignment as an Expression (AssignVal/AssignRef kind) wrapped in a Statement?
                // Or create a Call-like statement.
                // Let's wrap it in an Expression for now (Expression Statement).

                Expression* assignExpr = newExpr(Expression::AssignVal, lhs, rhs);
                Statement* s = newStmt(Statement::Call); // Call/Eval
                s->expr = assignExpr;

                if (!head) head = s;
                else curr->next = s;
                curr = s;

                // For chain a := b := c, 'b' becomes LHS for next?
                // Simula assignment is right-associative? "a := (b := c)"
                // But loop structure here implies "a := b; := c" which is wrong.
                // Standard: variable := expression.
                // If expression is assignment, it works.
                // But "variable := variable := expression" is allowed.
                // Let's simplify: only simple assignment for now.
                lhs = nullptr; // Stop chaining logic for this snippet
            } while (TT(this) == Tok_ColonEq);
            return head;
        }

        // Reference Assignment: lhs :- rhs
        if (TT(this) == Tok_ColonMinus) {
            expect(Tok_ColonMinus, "':-'", "unlabelled_basic_statement");
            Expression* rhs = expression();
            Expression* assignExpr = newExpr(Expression::AssignRef, lhs, rhs);
            Statement* s = newStmt(Statement::Call);
            s->expr = assignExpr;
            return s;
        }

        // Procedure statement (Call)
        // lhs is the call expression (already built by primary -> actual_parameter_part -> Call)
        Statement* s = newStmt(Statement::Call);
        s->expr = lhs;
        return s;
    }

    Statement* Parser3::go_to_statement()
    {
        Statement* s = newStmt(Statement::Goto);
        if (TT(this) == Tok_GOTO) {
            expect(Tok_GOTO, "GOTO", "go_to_statement");
        } else {
            expect(Tok_GO, "GO", "go_to_statement");
            expect(Tok_TO, "TO", "go_to_statement");
        }
        s->expr = expression(); // Target
        return s;
    }

    Statement* Parser3::for_statement()
    {
        Statement* s = newStmt(Statement::For);
        for_clause(s);
        s->body = statement();
        return s;
    }

    void Parser3::for_clause(Statement* loop)
    {
        expect(Tok_FOR, "FOR", "for_clause");
        simple_variable(); // TODO: Bind loop variable
        for_right_part(loop);
        expect(Tok_DO, "DO", "for_clause");
    }

    void Parser3::for_right_part(Statement* loop)
    {
        if (TT(this) == Tok_ColonEq) {
            expect(Tok_ColonEq, "':='", "for_right_part");
            value_for_list(loop);
        } else if (TT(this) == Tok_ColonMinus) {
            expect(Tok_ColonMinus, "':-'", "for_right_part");
            object_for_list(loop);
        } else {
            addError("for_right_part expected := or :-");
            synchronize();
        }
    }

    void Parser3::value_for_list(Statement* loop)
    {
        value_for_list_element(loop);
        while (TT(this) == Tok_Comma) {
            take();
            value_for_list_element(loop);
        }
    }

    void Parser3::value_for_list_element(Statement* loop)
    {
        // Store these elements in loop->connections or similar list?
        // SimAst For logic might need list of iterators.
        // We'll just parse for now.
        Expression* e1 = expression();

        if (TT(this) == Tok_STEP) {
            expect(Tok_STEP, "STEP", "value_for_list_element");
            Expression* e2 = expression();
            expect(Tok_UNTIL, "UNTIL", "value_for_list_element");
            Expression* e3 = expression();
            // Create StepIterator(e1, e2, e3)
        } else if (TT(this) == Tok_WHILE) {
            expect(Tok_WHILE, "WHILE", "value_for_list_element");
            Expression* e2 = expression();
            // Create WhileIterator(e1, e2)
        } else {
            // Single value e1
        }
    }

    // ... (Repeat pattern for object_for_list) ...
    void Parser3::object_for_list(Statement* loop) { object_for_list_element(loop); }
    void Parser3::object_for_list_element(Statement* loop) { expression(); if(TT(this)==Tok_WHILE) { take(); expression(); } }

    Statement* Parser3::activation_statement()
    {
        Statement* s = newStmt(Statement::Activate);
        activation_clause(s);
        if (TT(this) == Tok_AT || TT(this) == Tok_DELAY || TT(this) == Tok_BEFORE || TT(this) == Tok_AFTER) {
            scheduling_clause(s);
        }
        return s;
    }

    void Parser3::activation_clause(Statement* stmt)
    {
        Token t = activator(); // ACTIVATE/REACTIVATE
        stmt->isReactivate = (t.d_type == Tok_REACTIVATE);
        stmt->expr = expression(); // Object to activate
    }

    Token Parser3::activator()
    {
        if (TT(this) == Tok_ACTIVATE || TT(this) == Tok_REACTIVATE) return take();
        addError("Expected ACTIVATE or REACTIVATE");
        return Token(Tok_Invalid);
    }

    void Parser3::scheduling_clause(Statement* stmt)
    {
        if (TT(this) == Tok_BEFORE) {
            take();
            stmt->beforeAfterExpr = expression(); // Before
            // Flag 'before' vs 'after' maybe needed in AST?
        } else if (TT(this) == Tok_AFTER) {
            take();
            stmt->beforeAfterExpr = expression(); // After
        } else {
            timing_clause(stmt);
        }
    }

    void Parser3::timing_clause(Statement* stmt)
    {
        simple_timing_clause(stmt);
        if (TT(this) == Tok_PRIOR) {
            take();
            stmt->prior = true;
        }
    }

    void Parser3::simple_timing_clause(Statement* stmt)
    {
        if (TT(this) == Tok_AT) {
            take();
            stmt->atExpr = expression();
        } else if (TT(this) == Tok_DELAY) {
            take();
            stmt->delayExpr = expression();
        }
    }

    Statement* Parser3::connection_statement()
    {
        Statement* s = newStmt(Statement::Inspect);
        expect(Tok_INSPECT, "INSPECT", "connection_statement");
        s->expr = expression(); // Inspect object

        if (TT(this) == Tok_WHEN) {
            connection_part(s); // Fills s->connections list
        } else if (TT(this) == Tok_DO) {
            expect(Tok_DO, "DO", "connection_statement");
            s->body = statement();
        }

        if (TT(this) == Tok_OTHERWISE) {
            s->elseStmt = otherwise_clause();
        }
        return s;
    }

    void Parser3::connection_part(Statement* inspect)
    {
        when_clause(inspect);
        while (TT(this) == Tok_WHEN) {
            when_clause(inspect);
        }
    }

    void Parser3::when_clause(Statement* inspect)
    {
        expect(Tok_WHEN, "WHEN", "when_clause");
        Token cls = class_identifier();
        expect(Tok_DO, "DO", "when_clause");
        Statement* body = statement();

        // SimAst has 'Connection' node for this.
        // But Statement::connections is Statement*?
        // Actually SimAst Connection class exists:
        // class Connection : public Node { QByteArray className; Declaration* classDecl; Statement* body; Connection* next; };
        // We need to link this to the Inspect statement.
        // Assuming we can't easily change Ast definition here, we might need a workaround or assume 'connections' points to a specific structure.
    }

    Statement* Parser3::otherwise_clause()
    {
        expect(Tok_OTHERWISE, "OTHERWISE", "otherwise_clause");
        return statement();
    }

    Statement* Parser3::main_block()
    {
        // BEGIN [Decls] [Stmts] END
        expect(Tok_BEGIN, "BEGIN", "main_block");

        // In full AST, we'd open a scope here.
        // d_ast->openScope(...) if it's a block with decls.

        if (declStartAt(this, 1)) {
            declaration();
            while (TT(this) == Tok_Semi && declStartAt(this, 2)) {
                take();
                declaration();
            }
            expect(Tok_Semi, "';'", "main_block");
        }

        Statement* body = compound_tail();

        // d_ast->closeScope();
        return body;
    }

    Statement* Parser3::compound_tail()
    {
        Statement* head = nullptr;
        Statement* curr = nullptr;

        auto append = [&](Statement* s) {
            if (!s) return;
            if (!head) head = s;
            else curr->next = s;
            curr = s;
        };

        if (TT(this) != Tok_END && TT(this) != Tok_INNER) {
            append(statement());
            while (TT(this) == Tok_Semi && TT(this, 2) != Tok_END && TT(this, 2) != Tok_INNER) {
                take();
                append(statement());
            }
        }

        if (TT(this) == Tok_Semi && TT(this, 2) == Tok_INNER) {
            take();
            expect(Tok_INNER, "INNER", "compound_tail");
            append(newStmt(Statement::Inner));
            while (TT(this) == Tok_Semi && TT(this, 2) != Tok_END) {
                take();
                append(statement());
            }
        }

        if (TT(this) == Tok_Semi) take();
        expect(Tok_END, "END", "compound_tail");

        // Wrap list in Compound statement?
        Statement* compound = newStmt(Statement::Compound);
        compound->body = head;
        return compound;
    }

    Token Parser3::class_identifier() { return take(); }
    void Parser3::identifier_list() { take(); while(TT(this)==Tok_Comma){take(); take();} }
    void Parser3::label() { take(); }
    void Parser3::simple_variable() { take(); }

    Declaration* Parser3::declaration()
    {
        // Matches Parser2 dispatch logic
        if (TT(this) == Tok_SWITCH) {
            switch_declaration();
            return nullptr; // switch_declaration() currently void, need to update if we want AST
        }
        if (TT(this) == Tok_EXTERNAL) {
            external_declaration();
            return nullptr; // external_declaration() void
        }

        if (looksLikeProcedureDecl(this)) {
            return procedure_declaration();
        }
        if (looksLikeArrayDecl(this)) {
            array_declaration();
            return nullptr; // array_declaration() currently void
        }
        if (looksLikeClassDecl(this)) {
            return class_declaration();
        }
        if (isTypeStart(TT(this))) {
            type_declaration();
            return nullptr; // type_declaration() currently void
        }

        addError("declaration not recognized");
        synchronize();
        return nullptr;
    }

    Declaration* Parser3::class_declaration()
    {
        // class_declaration ::= prefix main_part
        prefix();
        main_part();
        // The AST node is created/registered inside main_part (via newDecl(Class))
        // But main_part updates d_ast->currentScope().
        // We typically return the declaration object itself.
        // Assuming main_part() creates it.
        // Let's refactor slightly: main_part should *return* the decl or we retrieve it?
        // Better: class_declaration calls prefix, then main_part creates the decl.
        return d_ast ? d_ast->currentScope() : nullptr; // Rough approximation
    }

    void Parser3::prefix()
    {
        if (TT(this) == Tok_identifier) {
            Token t = class_identifier();
            // TODO: Store prefix link in next class declaration
        }
    }

    void Parser3::main_part()
    {
        expect(Tok_CLASS, "CLASS", "main_part");
        Token name = class_identifier();

        Declaration* cls = newDecl(name.d_val, Declaration::Class);
        if (d_ast) d_ast->openScope(cls);

        formal_parameter_part();
        expect(Tok_Semi, "';'", "main_part");

        if (TT(this) == Tok_VALUE) value_part();
        while (isSpecifierStart(TT(this))) specification_part();
        if (isProtectionStart(TT(this))) {
            protection_part();
            expect(Tok_Semi, "';'", "main_part");
        }
        virtual_part();

        cls->body = class_body();

        if (d_ast) d_ast->closeScope();
    }

    Statement* Parser3::class_body()
    {
        // [ statement ]
        if (TT(this) == Tok_Eof || TT(this) == Tok_Semi || TT(this) == Tok_END)
            return nullptr;

        // statement() handles BEGIN...END blocks internally
        return statement();
    }

    Declaration* Parser3::procedure_declaration()
    {
        // [ type ] PROCEDURE procedure_heading procedure_body
        // Note: 'type' is optional return type
        if (isTypeStart(TT(this))) {
            // type(); // consume type tokens, TODO: store in proc decl
            type_declaration(); // Reuse or separate type parser?
            // Actually 'type' rule just consumes tokens.
            // Let's allow skipping for now or assume type_declaration covers it?
            // No, type_declaration parses "type list...", here we have "type PROCEDURE..."
            // We need a helper "parseType()" returning Type*.
            // For now, skip tokens manually or use minimal logic:
            if (TT(this)==Tok_REF) { take(); take(); } // REF Class
            else take(); // Basic type
        }

        expect(Tok_PROCEDURE, "PROCEDURE", "procedure_declaration");

        // procedure_heading handles name, params, specs
        // We need the name *before* we open scope fully?
        // Parser2: procedure_heading() calls identifier(), params(), etc.
        // Let's peek identifier to create Decl first?
        Token nameToken = la(1); // Usually identifier
        Declaration* proc = newDecl(nameToken.d_val, Declaration::Procedure);
        if (d_ast) d_ast->openScope(proc);

        procedure_heading(proc);
        proc->body = procedure_body();

        if (d_ast) d_ast->closeScope();
        return proc;
    }

    void Parser3::procedure_heading(Declaration* proc)
    {
        procedure_identifier(); // Consumes name
        formal_parameter_part();
        expect(Tok_Semi, "';'", "procedure_heading");

        if (TT(this) == Tok_NAME || TT(this) == Tok_VALUE) mode_part();
        while (isSpecifierStart(TT(this))) specification_part();
    }

    Statement* Parser3::procedure_body()
    {
        return statement();
    }

    void Parser3::formal_parameter_part()
    {
        if (TT(this) == Tok_Lpar) {
            take();
            formal_parameter_list();
            expect(Tok_Rpar, "')'", "formal_parameter_part");
        }
    }

    void Parser3::formal_parameter_list()
    {
        formal_parameter();
        while (TT(this) == Tok_Comma) {
            take();
            formal_parameter();
        }
    }

    void Parser3::formal_parameter()
    {
        Token t = take(); // Identifier
        // In a real implementation, add Parameter decl to current scope
        if (d_ast) d_ast->addDecl(t.d_val, Declaration::Parameter);
    }

    void Parser3::type_declaration() { /* type(); type_list(); */ take(); type_list(); }
    void Parser3::type_list() { type_list_element(); while(TT(this)==Tok_Comma){take(); type_list_element();} }
    void Parser3::type_list_element() { simple_variable(); if(TT(this)==Tok_Eq){take(); expression();} }

    void Parser3::array_declaration() {
        // [type] ARRAY ...
        if (isTypeStart(TT(this))) {
             if(TT(this)==Tok_REF){take();take();} else take();
        }
        expect(Tok_ARRAY, "ARRAY", "array_declaration");
        array_list();
    }
    void Parser3::array_list() { array_segment(); while(TT(this)==Tok_Comma){take(); array_segment();} }
    void Parser3::array_segment() {
        take(); // identifier
        while(TT(this)==Tok_Comma){take(); take();} // , identifier
        if(TT(this)==Tok_Lbrack){take(); bound_pair_list(); expect(Tok_Rbrack,"]");}
        else if(TT(this)==Tok_Lpar){take(); bound_pair_list(); expect(Tok_Rpar,")");}
    }
    void Parser3::bound_pair_list() { bound_pair(); while(TT(this)==Tok_Comma){take(); bound_pair();} }
    void Parser3::bound_pair() { lower_bound(); expect(Tok_Colon,":"); upper_bound(); }

    void Parser3::switch_declaration() {
        expect(Tok_SWITCH, "SWITCH");
        switch_identifier();
        expect(Tok_ColonEq, ":=");
        switch_list();
    }
    void Parser3::switch_identifier() { take(); }
    void Parser3::switch_list() { expression(); while(TT(this)==Tok_Comma){take(); expression();} }

    void Parser3::procedure_identifier() { take(); }
    void Parser3::mode_part() {
        if(TT(this)==Tok_NAME){ name_part(); if(TT(this)==Tok_VALUE) value_part(); }
        else if(TT(this)==Tok_VALUE){ value_part(); if(TT(this)==Tok_NAME) name_part(); }
    }
    void Parser3::value_part() { expect(Tok_VALUE,"VALUE"); identifier_list(); expect(Tok_Semi,";"); }
    void Parser3::name_part() { expect(Tok_NAME,"NAME"); identifier_list(); expect(Tok_Semi,";"); }
    void Parser3::specification_part() { specifier(); identifier_list(); expect(Tok_Semi,";"); }
    void Parser3::procedure_specification() { expect(Tok_IS,"IS"); procedure_declaration(); }

    void Parser3::specifier() {
        if(TT(this)==Tok_SWITCH || TT(this)==Tok_LABEL) { take(); return; }
        // Type [ARRAY|PROCEDURE]
        if (isTypeStart(TT(this))) {
            if(TT(this)==Tok_REF){take();take();} else take();
        }
        if(TT(this)==Tok_ARRAY || TT(this)==Tok_PROCEDURE) take();
    }

    void Parser3::protection_part() {
        protection_specification();
        while(TT(this)==Tok_Semi && isProtectionStart(TT(this,2))) { take(); protection_specification(); }
    }
    void Parser3::protection_specification() {
        if(TT(this)==Tok_HIDDEN) { take(); if(TT(this)==Tok_PROTECTED) take(); }
        else if(TT(this)==Tok_PROTECTED) { take(); if(TT(this)==Tok_HIDDEN) take(); }
        identifier_list();
    }
    void Parser3::virtual_part() {
        if(TT(this)!=Tok_VIRTUAL) return;
        take(); expect(Tok_Colon,":");
        virtual_spec(); expect(Tok_Semi,";");
        while(isSpecifierStart(TT(this))) { virtual_spec(); expect(Tok_Semi,";"); }
    }
    void Parser3::virtual_spec() {
        specifier(); identifier_list();
        if(TT(this)==Tok_IS) procedure_specification();
    }

    void Parser3::external_head() { external_declaration(); expect(Tok_Semi,";"); while(TT(this)==Tok_EXTERNAL){external_declaration(); expect(Tok_Semi,";");} }
    void Parser3::external_declaration() { expect(Tok_EXTERNAL,"EXTERNAL"); identifier_list(); /* equals? */ }

    Declaration* Parser3::parseModule()
    {
        d_errors.clear();

        // Ensure we have a root scope/module to put things in
        Declaration* moduleDecl = newDecl("ROOT_MODULE", Declaration::Module);
        if (d_ast) {
            d_ast->openScope(moduleDecl);
            d_root = moduleDecl;
        }

        if (!d_lexer) {
            addError("Parser3 has no lexer.");
            return nullptr;
        }

        // Start parsing
        module();

        // Finalize
        if (d_ast) d_ast->closeScope();

        // Check for trailing garbage
        const Token t = la();
        if (t.d_type != Tok_Eof) {
            addError(QString("Unexpected token at end of module: %1.").arg(tokName(t)));
        }

        return d_errors.isEmpty() ? d_root : nullptr;
    }

    void Parser3::module()
    {
        // module ::= [ external_head ] module_body_ { ';' [ module_body_ ] }
        if (TT(this) == Tok_EXTERNAL) {
            external_head();
        }

        module_body_();

        while (TT(this) == Tok_Semi) {
            expect(Tok_Semi, "';'", "module");
            if (TT(this) != Tok_Semi && TT(this) != Tok_Eof) {
                module_body_();
            }
        }
    }

    void Parser3::module_body_()
    {
        // Dispatch to class, procedure, or program (main block)
        // We don't return the individual decls here to a list yet,
        // but they attach themselves to the current scope via d_ast->addDecl().

        if (looksLikeClassDecl(this)) {
            class_declaration();
        } else if (looksLikeProcedureDecl(this)) {
            procedure_declaration();
        } else {
            program();
        }
    }

    void Parser3::program()
    {
        // program ::= { label ':' } block
        // Labels at program level are rare/weird in Simula but allowed by grammar
        while (TT(this) == Tok_identifier && TT(this, 2) == Tok_Colon) {
            label();
            expect(Tok_Colon, "':'", "program");
        }
        block();
    }

    Declaration* Parser3::block()
    {
        // block ::= [ block_prefix ] main_block
        if (TT(this) != Tok_BEGIN && TT(this) != Tok_Eof) {
            block_prefix();
        }
        // main_block returns a Statement* (Compound), but at top level
        // we often treat it as the "body" of the module/program.
        // We can attach it to the current scope root.
        Statement* body = main_block();
        if (d_ast && d_ast->currentScope()) {
            d_ast->currentScope()->body = body;
        }
        return nullptr; // No specific decl for "block", it modifies the parent
    }

    void Parser3::block_prefix()
    {
        class_identifier();
        if (TT(this) == Tok_Lpar) {
            Expression* dummy = nullptr;
            actual_parameter_part(dummy); // Parsing arguments
        }
    }

}

