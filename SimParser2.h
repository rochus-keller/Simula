#ifndef __SIM_PARSER2__
#define __SIM_PARSER2__

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

namespace Sim {

	class Scanner {
	public:
		virtual Token next() = 0;
		virtual Token peek(int offset) = 0;
	};

	class Parser2 {
	public:
		Parser2(Scanner* s):scanner(s) {}
		void RunParser();
		struct Error {
		    QString msg;
		    int row, col;
		    QString path;
		    Error( const QString& m, int r, int c, const QString& p):msg(m),row(r),col(c),path(p){}
		};
		QList<Error> errors;
	protected:
		void module();
		void module_body_();
		void external_head();
		void program();
		void while_statement();
		void block();
		void block_prefix();
		void actual_parameter_part();
		void main_block();
		void compound_tail();
		void declaration();
		void class_declaration();
		void prefix();
		void main_part();
		void protection_part();
		void protection_specification();
		void class_body();
		void virtual_part();
		void virtual_spec();
		void procedure_declaration();
		void procedure_heading();
		void procedure_identifier();
		void mode_part();
		void value_part();
		void name_part();
		void formal_parameter_part();
		void formal_parameter_list();
		void formal_parameter();
		void parameter_delimiter();
		void procedure_body();
		void statement();
		void unconditional_statement();
		void Common_Base_statement();
		void Common_Base_conditional_statement();
		void for_statement();
		void for_clause();
		void for_right_part();
		void value_for_list();
		void object_for_list();
		void value_for_list_element();
		void object_for_list_element();
		void go_to_statement();
		void unlabelled_basic_statement();
		void when_clause();
		void otherwise_clause();
		void connection_part();
		void connection_statement();
		void activator();
		void activation_clause();
		void simple_timing_clause();
		void timing_clause();
		void scheduling_clause();
		void activation_statement();
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
		void upper_bound();
		void lower_bound();
		void array_declaration();
		void type();
		void value_type();
		void reference_type();
		void object_reference();
		void qualification();
		void label();
		void if_clause();
		void local_object();
		void object_generator();
		void actual_parameter_list();
		void actual_parameter();
		void expression();
		void quaternary_();
		void tertiary_();
		void equivalence_();
		void equiv_sym_();
		void implication();
		void impl_sym_();
		void simple_expression_();
		void adding_operator();
		void or_sym_();
		void term();
		void multiplying_operator();
		void and_sym_();
		void factor();
		void power_sym_();
		void secondary();
		void not_sym_();
		void primary();
		void primary_nlr_();
		void relation_();
		void qualified_();
		void selector_();
		void relational_operator();
		void logical_value();
		void unsigned_number();
		void class_identifier();
		void identifier_list();
		void subscript_list();
		void subscript_expression();
		void attribute_identifier();
		void string_();
	protected:
		Token cur;
		Token la;
		Scanner* scanner;
		void next();
		Token peek(int off);
		void invalid(const char* what);
		bool expect(int tt, bool pkw, const char* where);
	};
}
#endif // include
