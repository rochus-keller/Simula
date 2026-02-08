#ifndef __SIM_PARSER3__
#define __SIM_PARSER3__

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
#include <Simula/SimAst.h>
#include <QList>

namespace Sim {

    class Scanner {
    public:
        virtual Token next() = 0;
        virtual Token peek(int offset) = 0;
        virtual QString source() const = 0;
    };

    class Parser3 {
    public:
        Parser3(Scanner* s, AstModel* m);
        ~Parser3();
        
        Declaration* RunParser();
        Declaration* takeResult();
        
        struct Error {
            QString msg;
            RowCol pos;
            QString path;
            Error(const QString& m, const RowCol& rc, const QString& p)
                : msg(m), pos(rc), path(p) {}
        };
        QList<Error> errors;
        
    protected:
        Declaration* module();
        void module_body_();
        void external_head();
        void program();
        Statement* while_statement();
        Statement* block();
        void block_prefix(Token &prefixName, QList<Expression*>& args);
        void actual_parameter_part(QList<Expression*>& args);
        Statement* main_block(const Token& prefixName, const QList<Expression*>& args);
        Statement* compound_tail();
        void declaration();
        void class_declaration();
        Token prefix();
        Declaration *main_part();
        TokenList protection_part();
        TokenList protection_specification();
        Statement* class_body();
        DeclList virtual_part();
        DeclList virtual_spec();
        void procedure_declaration();
        Declaration *procedure_heading();
        void mode_part(Declaration* procDecl);
        void value_part(Declaration* procDecl);
        void name_part(Declaration* procDecl);
        void formal_parameter_part(Declaration* procDecl);
        void formal_parameter_list(Declaration* procDecl);
        void formal_parameter(Declaration* procDecl);
        Statement* procedure_body();
        Statement* statement();
        Statement* unconditional_statement();
        Statement* Common_Base_statement();
        Statement* Common_Base_conditional_statement();
        Statement* for_statement();
        void for_clause(Expression*& varExpr, bool& isRefAssign, QList<Expression*>& forList);
        void for_right_part(bool& isRefAssign, QList<Expression*>& forList);
        void value_for_list(QList<Expression*>& forList);
        void object_for_list(QList<Expression*>& forList);
        Expression* value_for_list_element();
        Expression* object_for_list_element();
        Statement* go_to_statement();
        Statement* unlabelled_basic_statement();
        Connection* when_clause();
        Statement* otherwise_clause();
        Statement* connection_part();
        Statement* connection_statement();
        bool activator();
        Statement* activation_statement();
        void simple_timing_clause(Statement* stmt);
        void timing_clause(Statement* stmt);
        void scheduling_clause(Statement* stmt);
        Type* specifier(bool& isArray, bool& isProcedure);
        void specification_part(Declaration* parent);
        void procedure_specification();
        Declaration* external_item();
        QList<Declaration*> external_list();
        void external_declaration();
        Token external_identifier();
        Declaration* switch_declaration();
        void switch_list(Declaration* switchDecl);
        void type_list_element(Type* t);
        void type_list(Type* t);
        void type_declaration();
        void array_list(Type* elemType);
        void array_segment(Type* elemType);
        void bound_pair_list(QList<Expression*>& bounds);
        void bound_pair(QList<Expression*>& bounds);
        Declaration* array_declaration();
        Type* type();
        Type* value_type();
        Type* reference_type();
        Type* object_reference();
        Token qualification();
        Token label();
        Expression* if_clause();
        Expression* local_object();
        Expression* object_generator();
        void actual_parameter_list(QList<Expression*>& args);
        Expression* actual_parameter();
        Expression* expression();
        Expression* quaternary_();
        Expression* tertiary_();
        Expression* equivalence_();
        Expression::Kind equiv_sym_();
        Expression* implication();
        Expression::Kind impl_sym_();
        Expression* simple_arithmetic_expression();
        Expression* boolean_term();
        Expression::Kind adding_operator();
        Expression::Kind or_sym_();
        Expression* arithmetic_term();
        Expression* boolean_factor();
        Expression::Kind multiplying_operator();
        Expression::Kind and_sym_();
        Expression* factor();
        Expression::Kind power_sym_();
        Expression* secondary();
        Expression* not_expression();
        Expression::Kind not_sym_();
        Expression* primary();
        Expression* primary_nlr_(Expression* lhs);
        Expression* relation_();
        Expression* qualified_(Expression* lhs);
        Expression* selector_(Expression* lhs);
        Expression::Kind relational_operator();
        Expression* logical_value();
        Expression* unsigned_number();
        Token class_identifier();
        TokenList identifier_list();
        void subscript_list(QList<Expression*>& subs);
        Expression* subscript_expression();
        Token attribute_identifier();
        Expression* string_();
        
    protected:
        Token cur;
        Token la;
        Scanner* scanner;
        AstModel* mdl;
        Declaration* thisMod;
        
        void next();
        Token peek(int off);
        void error(const Token& t, const QString& msg);
        void error(const RowCol& pos, const QString& msg);
        void invalid(const char* what);
        bool expect(int tt, bool pkw, const char* where);
        void fixParamTypes(Declaration*);
        void appendName(Declaration* d, const Token& id);
        
        RowCol toRowCol(const Token& t) const;
        bool versionCheck(SimulaVersion minVersion, const char* feature = 0);
        bool versionMax(SimulaVersion maxVersion, const char* feature = 0);
    };
}
#endif // include
