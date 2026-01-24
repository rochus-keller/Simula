#ifndef SIMPARSER3_H
#define SIMPARSER3_H

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

#include <Simula/SimToken.h>
#include <QList>
#include <QVector>
#include <QString>
#include <Simula/SimAst.h>

namespace Sim
{
    class Lexer;
    class Errors;

    class Parser3
    {
    public:
        struct ParseError
        {
            QString message;
            QString rule;              // nonterminal / function name (if provided)
            Token got;                 // token at which we reported the error
            QVector<TokenType> expected;

            ParseError() = default;
            ParseError(const QString& msg, const Token& g,
                       const QVector<TokenType>& exp = {},
                       const QString& r = QString())
                : message(msg), rule(r), got(g), expected(exp) {}
        };

        explicit Parser3(Lexer* lexer, AstModel* model);

        void setErrorSink(Errors* sink) { d_errSink = sink; }
        const QList<ParseError>& errors() const { return d_errors; }

        // Main entry point (syntax-first; AST wiring comes in the next steps).
        Declaration* parseModule();

    private:
        Token la(int k = 1) const;             // look-ahead (1-based)
        Token take();                          // consume one token
        bool accept(TokenType t);              // if la(1) matches, consume and return true
        bool expect(TokenType t,
                    const char* what = nullptr,
                    const char* rule = nullptr);

        void addError(const QString& msg,
                      const QVector<TokenType>& expected = {},
                      const char* rule = nullptr,
                      const Token& at = Token());

        // Basic panic-mode recovery (minimal for now).
        void synchronize();

        Expression* newExpr(Expression::Kind k);
        Expression* newExpr(Expression::Kind k, Expression* lhs, Expression* rhs);
        Expression* newExprVal(Expression::Kind k, const Token& t); // for literals
        Statement* newStmt(Statement::Kind k);
        Declaration* newDecl(const QByteArray& name, Declaration::Kind k);

        void module();
        void module_body_();
        void external_head();
        void program();
        Declaration* block();
        void block_prefix();
        Statement* main_block();
        Statement* compound_tail();
        Declaration* declaration();
        Declaration* class_declaration();
        void prefix();
        void main_part();
        void protection_part();
        void protection_specification();
        Statement* class_body();
        void virtual_part();
        void virtual_spec();
        Declaration* procedure_declaration();
        void procedure_heading(Declaration*);
        void procedure_identifier();
        void mode_part();
        void value_part();
        void name_part();
        void formal_parameter_part();
        void formal_parameter_list();
        void formal_parameter();
        void parameter_delimiter();
        Statement* procedure_body();
        void for_clause(Statement*);
        void for_right_part(Statement*);
        void value_for_list(Statement*);
        void object_for_list(Statement*);
        void value_for_list_element(Statement*);
        void object_for_list_element(Statement*);
        void when_clause(Statement*);
        Statement* otherwise_clause();
        void connection_part(Statement*);
        Statement* connection_statement();
        Token activator();
        void activation_clause(Statement*);
        void simple_timing_clause(Statement*);
        void timing_clause(Statement*);
        void scheduling_clause(Statement*);
        Statement* activation_statement();
        void specifier();
        void specification_part();
        void procedure_specification();
        void external_item();
        void external_list();
        void external_declaration();
        void external_identifier();
        void switch_declaration();
        void switch_identifier();
        void switch_list();
        void simple_variable();
        void variable_identifier();
        void type_list_element();
        void type_list();
        void type_declaration();
        void array_list();
        void array_segment();
        void bound_pair_list();
        void bound_pair();
        Expression* upper_bound();
        Expression* lower_bound();
        void array_declaration();
        void type();
        void value_type();
        void reference_type();
        void object_reference();
        void qualification();
        void label();
        void primary_nlr_();
        void relation_();
        void qualified_();
        void selector_();
        Token class_identifier();
        void identifier_list();

        Statement* statement();
        Statement* while_statement();
        Statement* Common_Base_statement();
        Statement* Common_Base_conditional_statement();
        Statement* unconditional_statement();
        Statement* unlabelled_basic_statement();
        Statement* go_to_statement();
        Statement* for_statement();

        Expression* expression();
        Expression* if_clause();
        Expression* quaternary_();
        Expression* tertiary_();
        Expression* equivalence_();
        Expression* implication();
        Expression* simple_expression_();
        Expression* term();
        Expression* factor();
        Expression* secondary();
        Expression* primary();
        // primary_nlr_ is now integrated into primary() as a loop

        // Helpers that return operator Kind or Invalid
        Expression::Kind adding_operator();
        Expression::Kind multiplying_operator();
        Expression::Kind relational_operator();
        Expression::Kind equiv_sym_();
        Expression::Kind impl_sym_();
        Expression::Kind or_sym_();
        Expression::Kind and_sym_();
        Expression::Kind power_sym_();
        Expression::Kind not_sym_();

        // Small lexical primaries
        Expression* unsigned_number();
        Expression* logical_value();
        Expression* string_();
        Expression* attribute_identifier();
        Expression* local_object();
        Expression* object_generator();

        // Parameter lists
        void actual_parameter_part(Expression*& lhs); // modifies lhs (call/index)
        QList<Expression*> actual_parameter_list();
        Expression* actual_parameter();

        QList<Expression*> subscript_list();
        Expression* subscript_expression();

        void qualified_(Expression*& lhs);
        void selector_(Expression*& lhs);
        void relation_(Expression*& lhs);

        static inline int TT(const Parser3* p, int k = 1) { return p->la(k).d_type; }

        // Matches the lookahead idea from main_block in the EBNF:
        // declarations start with a keyword-type, or (identifier CLASS).
        static inline bool declStartAt(const Parser3* p, int off)
        {
            const int t1 = p->la(off).d_type;
            const int t2 = p->la(off + 1).d_type;
            return isKw(t1) || (t1 == Tok_identifier && t2 == Tok_CLASS);
        }

        static inline bool isKw(int t)
        {
            switch (t) {
            case Tok_ARRAY:
            case Tok_BOOLEAN:
            case Tok_CHARACTER:
            case Tok_CLASS:
            case Tok_EXTERNAL:
            case Tok_INTEGER:
            case Tok_LONG:
            case Tok_PROCEDURE:
            case Tok_REAL:
            case Tok_REF:
            case Tok_SHORT:
            case Tok_SWITCH:
            case Tok_TEXT:
                return true;
            default:
                return false;
            }
        }

        static inline bool isTypeStart(int t)
            {
                switch (t) {
                case Tok_SHORT:
                case Tok_LONG:
                case Tok_INTEGER:
                case Tok_REAL:
                case Tok_BOOLEAN:
                case Tok_CHARACTER:
                case Tok_TEXT:
                case Tok_REF:
                    return true;
                default:
                    return false;
                }
            }

        // Heuristics copied from Parser2's declaration-dispatch style.
        static inline bool looksLikeProcedureDecl(const Parser3* p)
        {
            const int t1 = TT(p,1), t2 = TT(p,2), t3 = TT(p,3), t5 = TT(p,5);
            return (t1 == Tok_PROCEDURE || t2 == Tok_PROCEDURE ||
                    ((t1 == Tok_SHORT || t1 == Tok_LONG) && t3 == Tok_PROCEDURE) ||
                    (t1 == Tok_REF && t5 == Tok_PROCEDURE));
        }

        static inline bool looksLikeArrayDecl(const Parser3* p)
        {
            const int t1 = TT(p,1), t2 = TT(p,2), t3 = TT(p,3), t5 = TT(p,5);
            return (t1 == Tok_ARRAY || t2 == Tok_ARRAY ||
                    ((t1 == Tok_SHORT || t1 == Tok_LONG) && t3 == Tok_ARRAY) ||
                    (t1 == Tok_REF && t5 == Tok_ARRAY));
        }

        static inline bool looksLikeClassDecl(const Parser3* p)
        {
            return (TT(p,1) == Tok_CLASS || TT(p,2) == Tok_CLASS);
        }

        static inline bool isSpecifierStart(int t)
        {
            switch (t) {
            case Tok_ARRAY:
            case Tok_BOOLEAN:
            case Tok_CHARACTER:
            case Tok_INTEGER:
            case Tok_LABEL:
            case Tok_LONG:
            case Tok_PROCEDURE:
            case Tok_REAL:
            case Tok_REF:
            case Tok_SHORT:
            case Tok_SWITCH:
            case Tok_TEXT:
                return true;
            default:
                return false;
            }
        }

        static inline bool isProtectionStart(int t)
        {
            return t == Tok_HIDDEN || t == Tok_PROTECTED;
        }

        static inline bool isPrimaryStart(int t)
         {
             switch (t) {
             case Tok_unsigned_integer:
             case Tok_decimal_number:
             case Tok_TRUE:
             case Tok_FALSE:
             case Tok_character:
             case Tok_string:
             case Tok_NOTEXT:
             case Tok_NONE:
             case Tok_THIS:
             case Tok_NEW:
             case Tok_Lpar:
             case Tok_identifier:
                 return true;
             default:
                 return false;
             }
         }
    private:
        Lexer* d_lexer;
        Errors* d_errSink;
        QList<ParseError> d_errors;
        AstModel* d_ast;

        Declaration* d_root; // will become the module AST root later
    };
}

#endif // SIMPARSER3_H
