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

// Originally generated with EBNF studio from the Simula67.ebnf

#include "SimParser2.h"
using namespace Sim;

static inline bool FIRST_module(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_BEGIN:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_CLASS:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_EXTERNAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_identifier:
		return true;
	default: return false;
	}
}

static inline bool FIRST_module_body_(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_BEGIN:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_CLASS:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_identifier:
		return true;
	default: return false;
	}
}

static inline bool FIRST_external_head(int tt) {
	return tt == Tok_EXTERNAL;
}

static inline bool FIRST_program(int tt) {
	return tt == Tok_BEGIN || tt == Tok_identifier;
}

static inline bool FIRST_while_statement(int tt) {
	return tt == Tok_WHILE;
}

static inline bool FIRST_block(int tt) {
	return tt == Tok_BEGIN || tt == Tok_identifier;
}

static inline bool FIRST_block_prefix(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_actual_parameter_part(int tt) {
	return tt == Tok_Lpar;
}

static inline bool FIRST_main_block(int tt) {
	return tt == Tok_BEGIN;
}

static inline bool FIRST_compound_tail(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_FOR:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_IF:
	case Tok_WHILE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_END:
	case Tok_FALSE:
	case Tok_Semi:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_declaration(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_ARRAY:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_SWITCH:
	case Tok_CLASS:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_EXTERNAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_identifier:
		return true;
	default: return false;
	}
}

static inline bool FIRST_class_declaration(int tt) {
	return tt == Tok_CLASS || tt == Tok_identifier;
}

static inline bool FIRST_prefix(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_main_part(int tt) {
	return tt == Tok_CLASS;
}

static inline bool FIRST_protection_part(int tt) {
	return tt == Tok_PROTECTED || tt == Tok_HIDDEN;
}

static inline bool FIRST_protection_specification(int tt) {
	return tt == Tok_PROTECTED || tt == Tok_HIDDEN;
}

static inline bool FIRST_class_body(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_FOR:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_IF:
	case Tok_WHILE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_virtual_part(int tt) {
	return tt == Tok_VIRTUAL;
}

static inline bool FIRST_virtual_spec(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_ARRAY:
	case Tok_TEXT:
	case Tok_SWITCH:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_LABEL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_procedure_declaration(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
	case Tok_PROCEDURE:
		return true;
	default: return false;
	}
}

static inline bool FIRST_procedure_heading(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_procedure_identifier(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_mode_part(int tt) {
	return tt == Tok_NAME || tt == Tok_VALUE;
}

static inline bool FIRST_value_part(int tt) {
	return tt == Tok_VALUE;
}

static inline bool FIRST_name_part(int tt) {
	return tt == Tok_NAME;
}

static inline bool FIRST_formal_parameter_part(int tt) {
	return tt == Tok_Lpar;
}

static inline bool FIRST_formal_parameter_list(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_formal_parameter(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_parameter_delimiter(int tt) {
	return tt == Tok_Comma;
}

static inline bool FIRST_procedure_body(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_FOR:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_IF:
	case Tok_WHILE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_statement(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_FOR:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_IF:
	case Tok_WHILE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_unconditional_statement(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_Common_Base_statement(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_FOR:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_IF:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_Common_Base_conditional_statement(int tt) {
	return tt == Tok_IF;
}

static inline bool FIRST_for_statement(int tt) {
	return tt == Tok_FOR;
}

static inline bool FIRST_for_clause(int tt) {
	return tt == Tok_FOR;
}

static inline bool FIRST_for_right_part(int tt) {
	return tt == Tok_ColonEq || tt == Tok_ColonMinus;
}

static inline bool FIRST_value_for_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_object_for_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_value_for_list_element(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_object_for_list_element(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_go_to_statement(int tt) {
	return tt == Tok_GOTO || tt == Tok_GO;
}

static inline bool FIRST_unlabelled_basic_statement(int tt) {
	switch(tt){
	case Tok_BEGIN:
	case Tok_NEW:
	case Tok_GOTO:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_GO:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_REACTIVATE:
	case Tok_ACTIVATE:
	case Tok_character:
	case Tok_INSPECT:
	case Tok_FALSE:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_when_clause(int tt) {
	return tt == Tok_WHEN;
}

static inline bool FIRST_otherwise_clause(int tt) {
	return tt == Tok_OTHERWISE;
}

static inline bool FIRST_connection_part(int tt) {
	return tt == Tok_WHEN;
}

static inline bool FIRST_connection_statement(int tt) {
	return tt == Tok_INSPECT;
}

static inline bool FIRST_activator(int tt) {
	return tt == Tok_REACTIVATE || tt == Tok_ACTIVATE;
}

static inline bool FIRST_activation_clause(int tt) {
	return tt == Tok_REACTIVATE || tt == Tok_ACTIVATE;
}

static inline bool FIRST_simple_timing_clause(int tt) {
	return tt == Tok_AT || tt == Tok_DELAY;
}

static inline bool FIRST_timing_clause(int tt) {
	return tt == Tok_AT || tt == Tok_DELAY;
}

static inline bool FIRST_scheduling_clause(int tt) {
	return tt == Tok_AT || tt == Tok_BEFORE || tt == Tok_DELAY || tt == Tok_AFTER;
}

static inline bool FIRST_activation_statement(int tt) {
	return tt == Tok_REACTIVATE || tt == Tok_ACTIVATE;
}

static inline bool FIRST_specifier(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_ARRAY:
	case Tok_TEXT:
	case Tok_SWITCH:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_LABEL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_specification_part(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_ARRAY:
	case Tok_TEXT:
	case Tok_SWITCH:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
	case Tok_PROCEDURE:
	case Tok_LABEL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_procedure_specification(int tt) {
	return tt == Tok_IS;
}

static inline bool FIRST_external_item(int tt) {
	return tt == Tok_identifier || tt == Tok_string;
}

static inline bool FIRST_external_list(int tt) {
	return tt == Tok_identifier || tt == Tok_string;
}

static inline bool FIRST_external_declaration(int tt) {
	return tt == Tok_EXTERNAL;
}

static inline bool FIRST_external_identifier(int tt) {
	return tt == Tok_identifier || tt == Tok_string;
}

static inline bool FIRST_switch_declaration(int tt) {
	return tt == Tok_SWITCH;
}

static inline bool FIRST_switch_identifier(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_switch_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_simple_variable(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_variable_identifier(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_type_list_element(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_type_list(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_type_declaration(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
		return true;
	default: return false;
	}
}

static inline bool FIRST_array_list(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_array_segment(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_bound_pair_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_bound_pair(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_upper_bound(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_lower_bound(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_array_declaration(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_ARRAY:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
		return true;
	default: return false;
	}
}

static inline bool FIRST_type(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_TEXT:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
	case Tok_REF:
		return true;
	default: return false;
	}
}

static inline bool FIRST_value_type(int tt) {
	switch(tt){
	case Tok_SHORT:
	case Tok_INTEGER:
	case Tok_CHARACTER:
	case Tok_BOOLEAN:
	case Tok_LONG:
	case Tok_REAL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_reference_type(int tt) {
	return tt == Tok_TEXT || tt == Tok_REF;
}

static inline bool FIRST_object_reference(int tt) {
	return tt == Tok_REF;
}

static inline bool FIRST_qualification(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_label(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_if_clause(int tt) {
	return tt == Tok_IF;
}

static inline bool FIRST_local_object(int tt) {
	return tt == Tok_THIS;
}

static inline bool FIRST_object_generator(int tt) {
	return tt == Tok_NEW;
}

static inline bool FIRST_actual_parameter_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_actual_parameter(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_expression(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_quaternary_(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_decimal_number:
	case Tok_THIS:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_tertiary_(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_THIS:
	case Tok_decimal_number:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_equivalence_(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_decimal_number:
	case Tok_THIS:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_equiv_sym_(int tt) {
	return tt == Tok_EQV || tt == Tok_EQUIV || tt == Tok_Ueq || tt == Tok_2Eq;
}

static inline bool FIRST_implication(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_THIS:
	case Tok_decimal_number:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_impl_sym_(int tt) {
	return tt == Tok_IMPL || tt == Tok_Uimpl || tt == Tok_IMP || tt == Tok_MinusGt;
}

static inline bool FIRST_simple_expression_(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_decimal_number:
	case Tok_THIS:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_adding_operator(int tt) {
	return tt == Tok_Minus || tt == Tok_Plus;
}

static inline bool FIRST_or_sym_(int tt) {
	return tt == Tok_OR || tt == Tok_Uor || tt == Tok_Bar;
}

static inline bool FIRST_term(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_THIS:
	case Tok_decimal_number:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_multiplying_operator(int tt) {
	switch(tt){
	case Tok_Percent:
	case Tok_Star:
	case Tok_Umul:
	case Tok_Slash:
	case Tok_2Slash:
	case Tok_Udiv:
		return true;
	default: return false;
	}
}

static inline bool FIRST_and_sym_(int tt) {
	return tt == Tok_Uand || tt == Tok_Amp || tt == Tok_AND;
}

static inline bool FIRST_factor(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_decimal_number:
	case Tok_THIS:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_power_sym_(int tt) {
	return tt == Tok_2Star || tt == Tok_Uexp || tt == Tok_Hat || tt == Tok_POWER;
}

static inline bool FIRST_secondary(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_NOT:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Bang:
	case Tok_TRUE:
	case Tok_THIS:
	case Tok_decimal_number:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_Minus:
	case Tok_NEW:
	case Tok_string:
	case Tok_Plus:
		return true;
	default: return false;
	}
}

static inline bool FIRST_not_sym_(int tt) {
	return tt == Tok_NOT || tt == Tok_Unot || tt == Tok_Bang;
}

static inline bool FIRST_primary(int tt) {
	switch(tt){
	case Tok_unsigned_integer:
	case Tok_FALSE:
	case Tok_TRUE:
	case Tok_decimal_number:
	case Tok_THIS:
	case Tok_NONE:
	case Tok_Lpar:
	case Tok_NOTEXT:
	case Tok_character:
	case Tok_identifier:
	case Tok_NEW:
	case Tok_string:
		return true;
	default: return false;
	}
}

static inline bool FIRST_primary_nlr_(int tt) {
	switch(tt){
	case Tok_NE:
	case Tok_BangEq:
	case Tok_LE:
	case Tok_QUA:
	case Tok_Dot:
	case Tok_Lpar:
	case Tok_Uleq:
	case Tok_NOTGREATER:
	case Tok_Geq:
	case Tok_Lt:
	case Tok_Uneq:
	case Tok_GE:
	case Tok_IS:
	case Tok_LESS:
	case Tok_Gt:
	case Tok_EQ:
	case Tok_Eq:
	case Tok_LtGt:
	case Tok_NOTEQUAL:
	case Tok_Lbrack:
	case Tok_2Eq:
	case Tok_EqSlashEq:
	case Tok_GREATER:
	case Tok_HatEq:
	case Tok_GT:
	case Tok_IN:
	case Tok_Leq:
	case Tok_LT:
	case Tok_Ugeq:
	case Tok_NOTLESS:
	case Tok_EQUAL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_relation_(int tt) {
	switch(tt){
	case Tok_NE:
	case Tok_BangEq:
	case Tok_LE:
	case Tok_Uleq:
	case Tok_NOTGREATER:
	case Tok_Geq:
	case Tok_Lt:
	case Tok_Uneq:
	case Tok_GE:
	case Tok_IS:
	case Tok_LESS:
	case Tok_Gt:
	case Tok_EQ:
	case Tok_Eq:
	case Tok_LtGt:
	case Tok_NOTEQUAL:
	case Tok_2Eq:
	case Tok_EqSlashEq:
	case Tok_GREATER:
	case Tok_HatEq:
	case Tok_GT:
	case Tok_IN:
	case Tok_Leq:
	case Tok_LT:
	case Tok_Ugeq:
	case Tok_NOTLESS:
	case Tok_EQUAL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_qualified_(int tt) {
	return tt == Tok_QUA;
}

static inline bool FIRST_selector_(int tt) {
	return tt == Tok_Lbrack || tt == Tok_Dot || tt == Tok_Lpar;
}

static inline bool FIRST_relational_operator(int tt) {
	switch(tt){
	case Tok_NE:
	case Tok_BangEq:
	case Tok_LE:
	case Tok_Uleq:
	case Tok_NOTGREATER:
	case Tok_Geq:
	case Tok_Lt:
	case Tok_Uneq:
	case Tok_GE:
	case Tok_IS:
	case Tok_LESS:
	case Tok_Gt:
	case Tok_EQ:
	case Tok_Eq:
	case Tok_LtGt:
	case Tok_NOTEQUAL:
	case Tok_2Eq:
	case Tok_EqSlashEq:
	case Tok_GREATER:
	case Tok_HatEq:
	case Tok_GT:
	case Tok_IN:
	case Tok_Leq:
	case Tok_LT:
	case Tok_Ugeq:
	case Tok_NOTLESS:
	case Tok_EQUAL:
		return true;
	default: return false;
	}
}

static inline bool FIRST_logical_value(int tt) {
	return tt == Tok_FALSE || tt == Tok_TRUE;
}

static inline bool FIRST_unsigned_number(int tt) {
	return tt == Tok_unsigned_integer || tt == Tok_decimal_number;
}

static inline bool FIRST_class_identifier(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_identifier_list(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_subscript_list(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_subscript_expression(int tt) {
	switch(tt){
	case Tok_NEW:
	case Tok_Lpar:
	case Tok_string:
	case Tok_NOTEXT:
	case Tok_Bang:
	case Tok_Plus:
	case Tok_unsigned_integer:
	case Tok_NONE:
	case Tok_TRUE:
	case Tok_identifier:
	case Tok_THIS:
	case Tok_NOT:
	case Tok_IF:
	case Tok_character:
	case Tok_Unot:
	case Tok_FALSE:
	case Tok_Minus:
	case Tok_decimal_number:
		return true;
	default: return false;
	}
}

static inline bool FIRST_attribute_identifier(int tt) {
	return tt == Tok_identifier;
}

static inline bool FIRST_string_(int tt) {
	return tt == Tok_string;
}

void Parser2::RunParser() {
	errors.clear();
	next();
	module();
}

void Parser2::next() {
	cur = la;
	la = scanner->next();
	while( la.d_type == Tok_Invalid ) {
		errors << Error(la.d_val, la.d_lineNr, la.d_colNr, la.d_sourcePath);
		la = scanner->next();
	}
}

Token Parser2::peek(int off) {
	if( off == 1 )
		return la;
	else if( off == 0 )
		return cur;
	else
		return scanner->peek(off-1);
}

void Parser2::invalid(const char* what) {
	errors << Error(QString("invalid %1").arg(what),la.d_lineNr, la.d_colNr, la.d_sourcePath);
}

bool Parser2::expect(int tt, bool pkw, const char* where) {
	if( la.d_type == tt) { next(); return true; }
	else { errors << Error(QString("'%1' expected in %2").arg(tokenTypeString(tt)).arg(where),la.d_lineNr, la.d_colNr, la.d_sourcePath); return false; }
}

static inline void dummy() {}

void Parser2::module() {
	if( FIRST_external_head(la.d_type) ) {
		external_head();
	}
	module_body_();
	while( la.d_type == Tok_Semi ) {
		expect(Tok_Semi, false, "module");
		if( FIRST_module_body_(la.d_type) ) {
			module_body_();
		}
	}
}

void Parser2::module_body_() {
	if( ( ( peek(1).d_type == Tok_CLASS || peek(1).d_type == Tok_identifier ) && ( peek(2).d_type == Tok_CLASS || peek(2).d_type == Tok_identifier ) )  ) {
		class_declaration();
	} else if( FIRST_procedure_declaration(la.d_type) ) {
		procedure_declaration();
	} else if( FIRST_program(la.d_type) ) {
		program();
	} else
		invalid("module_body_");
}

void Parser2::external_head() {
	external_declaration();
	expect(Tok_Semi, false, "external_head");
	while( FIRST_external_declaration(la.d_type) ) {
		external_declaration();
		expect(Tok_Semi, false, "external_head");
	}
}

void Parser2::program() {
	while( ( peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon )  ) {
		label();
		expect(Tok_Colon, false, "program");
	}
	block();
}

void Parser2::while_statement() {
	expect(Tok_WHILE, false, "while_statement");
	expression();
	expect(Tok_DO, false, "while_statement");
	statement();
}

void Parser2::block() {
	if( FIRST_block_prefix(la.d_type) ) {
		block_prefix();
	}
	main_block();
}

void Parser2::block_prefix() {
	class_identifier();
	if( FIRST_actual_parameter_part(la.d_type) ) {
		actual_parameter_part();
	}
}

void Parser2::actual_parameter_part() {
	expect(Tok_Lpar, false, "actual_parameter_part");
	actual_parameter_list();
	expect(Tok_Rpar, false, "actual_parameter_part");
}

void Parser2::main_block() {
	expect(Tok_BEGIN, false, "main_block");
	if( ( ( ( peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN || peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_CLASS || peek(1).d_type == Tok_EXTERNAL || peek(1).d_type == Tok_INTEGER || peek(1).d_type == Tok_LONG || peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL || peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT ) || peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_CLASS ) )  ) {
		declaration();
		while( ( peek(1).d_type == Tok_Semi && ( ( peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_BOOLEAN || peek(2).d_type == Tok_CHARACTER || peek(2).d_type == Tok_CLASS || peek(2).d_type == Tok_EXTERNAL || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_LONG || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_REF || peek(2).d_type == Tok_SHORT || peek(2).d_type == Tok_SWITCH || peek(2).d_type == Tok_TEXT ) || peek(2).d_type == Tok_identifier && peek(3).d_type == Tok_CLASS ) )  ) {
			expect(Tok_Semi, false, "main_block");
			declaration();
		}
		expect(Tok_Semi, false, "main_block");
	}
	compound_tail();
}

void Parser2::compound_tail() {
	if( FIRST_statement(la.d_type) || ( peek(1).d_type == Tok_Semi && !( peek(2).d_type == Tok_END ) && !( peek(2).d_type == Tok_INNER ) )  ) {
		statement();
		while( ( peek(1).d_type == Tok_Semi && !( peek(2).d_type == Tok_END ) && !( peek(2).d_type == Tok_INNER ) )  ) {
			expect(Tok_Semi, false, "compound_tail");
			statement();
		}
	}
	if( ( peek(1).d_type == Tok_Semi && peek(2).d_type == Tok_INNER )  ) {
		expect(Tok_Semi, false, "compound_tail");
		expect(Tok_INNER, false, "compound_tail");
		while( ( peek(1).d_type == Tok_Semi && !( peek(2).d_type == Tok_END ) )  ) {
			expect(Tok_Semi, false, "compound_tail");
			statement();
		}
	}
	if( la.d_type == Tok_Semi ) {
		expect(Tok_Semi, false, "compound_tail");
	}
	expect(Tok_END, false, "compound_tail");
}

void Parser2::declaration() {
	if( FIRST_switch_declaration(la.d_type) ) {
		switch_declaration();
	} else if( FIRST_external_declaration(la.d_type) ) {
		external_declaration();
	} else if( ( ( peek(1).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_PROCEDURE || ( peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_LONG ) && peek(3).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REF && peek(5).d_type == Tok_PROCEDURE ) )  ) {
		procedure_declaration();
	} else if( ( ( peek(1).d_type == Tok_ARRAY || peek(2).d_type == Tok_ARRAY || ( peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_LONG ) && peek(3).d_type == Tok_ARRAY || peek(1).d_type == Tok_REF && peek(5).d_type == Tok_ARRAY ) )  ) {
		array_declaration();
	} else if( ( ( peek(1).d_type == Tok_CLASS || peek(2).d_type == Tok_CLASS ) )  ) {
		class_declaration();
	} else if( FIRST_type_declaration(la.d_type) ) {
		type_declaration();
	} else
		invalid("declaration");
}

void Parser2::class_declaration() {
	prefix();
	main_part();
}

void Parser2::prefix() {
	if( FIRST_class_identifier(la.d_type) ) {
		class_identifier();
	}
}

void Parser2::main_part() {
	expect(Tok_CLASS, false, "main_part");
	class_identifier();
	formal_parameter_part();
	expect(Tok_Semi, false, "main_part");
	if( FIRST_value_part(la.d_type) ) {
		value_part();
	}
	while( ( ( peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN || peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER || peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG || peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL || peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT ) && ( peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_identifier ) )  ) {
		specification_part();
	}
	if( FIRST_protection_part(la.d_type) ) {
		protection_part();
		expect(Tok_Semi, false, "main_part");
	}
	virtual_part();
	class_body();
}

void Parser2::protection_part() {
	protection_specification();
	while( ( peek(1).d_type == Tok_Semi && ( peek(2).d_type == Tok_HIDDEN || peek(2).d_type == Tok_PROTECTED ) )  ) {
		expect(Tok_Semi, false, "protection_part");
		protection_specification();
	}
}

void Parser2::protection_specification() {
	if( la.d_type == Tok_HIDDEN ) {
		expect(Tok_HIDDEN, false, "protection_specification");
		if( la.d_type == Tok_PROTECTED ) {
			expect(Tok_PROTECTED, false, "protection_specification");
		}
		identifier_list();
	} else if( la.d_type == Tok_PROTECTED ) {
		expect(Tok_PROTECTED, false, "protection_specification");
		if( la.d_type == Tok_HIDDEN ) {
			expect(Tok_HIDDEN, false, "protection_specification");
		}
		identifier_list();
	} else
		invalid("protection_specification");
}

void Parser2::class_body() {
	if( FIRST_statement(la.d_type) ) {
		statement();
	}
}

void Parser2::virtual_part() {
	if( la.d_type == Tok_VIRTUAL ) {
		expect(Tok_VIRTUAL, false, "virtual_part");
		expect(Tok_Colon, false, "virtual_part");
		virtual_spec();
		expect(Tok_Semi, false, "virtual_part");
		while( ( ( peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN || peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER || peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG || peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL || peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT ) && ( peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_Semi || peek(2).d_type == Tok_identifier ) )  ) {
			virtual_spec();
			expect(Tok_Semi, false, "virtual_part");
		}
	}
}

void Parser2::virtual_spec() {
	specifier();
	identifier_list();
	if( FIRST_procedure_specification(la.d_type) ) {
		procedure_specification();
	}
}

void Parser2::procedure_declaration() {
	if( FIRST_type(la.d_type) ) {
		type();
	}
	expect(Tok_PROCEDURE, false, "procedure_declaration");
	procedure_heading();
	procedure_body();
}

void Parser2::procedure_heading() {
	procedure_identifier();
	formal_parameter_part();
	expect(Tok_Semi, false, "procedure_heading");
	if( FIRST_mode_part(la.d_type) ) {
		mode_part();
	}
	while( ( ( peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN || peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER || peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG || peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL || peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT ) && ( peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_identifier ) )  ) {
		specification_part();
	}
}

void Parser2::procedure_identifier() {
	expect(Tok_identifier, false, "procedure_identifier");
}

void Parser2::mode_part() {
	if( FIRST_name_part(la.d_type) ) {
		name_part();
		if( FIRST_value_part(la.d_type) ) {
			value_part();
		}
	} else if( FIRST_value_part(la.d_type) ) {
		value_part();
		if( FIRST_name_part(la.d_type) ) {
			name_part();
		}
	} else
		invalid("mode_part");
}

void Parser2::value_part() {
	expect(Tok_VALUE, false, "value_part");
	identifier_list();
	expect(Tok_Semi, false, "value_part");
}

void Parser2::name_part() {
	expect(Tok_NAME, false, "name_part");
	identifier_list();
	expect(Tok_Semi, false, "name_part");
}

void Parser2::formal_parameter_part() {
	if( la.d_type == Tok_Lpar ) {
		expect(Tok_Lpar, false, "formal_parameter_part");
		formal_parameter_list();
		expect(Tok_Rpar, false, "formal_parameter_part");
	}
}

void Parser2::formal_parameter_list() {
	formal_parameter();
	while( FIRST_parameter_delimiter(la.d_type) ) {
		parameter_delimiter();
		formal_parameter();
	}
}

void Parser2::formal_parameter() {
	expect(Tok_identifier, false, "formal_parameter");
}

void Parser2::parameter_delimiter() {
	expect(Tok_Comma, false, "parameter_delimiter");
}

void Parser2::procedure_body() {
	statement();
}

void Parser2::statement() {
	while( ( peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon )  ) {
		label();
		expect(Tok_Colon, false, "statement");
	}
	if( FIRST_Common_Base_statement(la.d_type) || FIRST_while_statement(la.d_type) ) {
		if( FIRST_Common_Base_statement(la.d_type) ) {
			Common_Base_statement();
		} else if( FIRST_while_statement(la.d_type) ) {
			while_statement();
		} else
			invalid("statement");
	}
}

void Parser2::unconditional_statement() {
	unlabelled_basic_statement();
}

void Parser2::Common_Base_statement() {
	if( FIRST_unconditional_statement(la.d_type) ) {
		unconditional_statement();
	} else if( FIRST_Common_Base_conditional_statement(la.d_type) ) {
		Common_Base_conditional_statement();
	} else if( FIRST_for_statement(la.d_type) ) {
		for_statement();
	} else
		invalid("Common_Base_statement");
}

void Parser2::Common_Base_conditional_statement() {
	if_clause();
	while( ( peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon )  ) {
		label();
		expect(Tok_Colon, false, "Common_Base_conditional_statement");
	}
	if( FIRST_unconditional_statement(la.d_type) ) {
		unconditional_statement();
		if( la.d_type == Tok_ELSE ) {
			expect(Tok_ELSE, false, "Common_Base_conditional_statement");
			statement();
		}
	} else if( la.d_type == Tok_ELSE ) {
		expect(Tok_ELSE, false, "Common_Base_conditional_statement");
		statement();
	} else if( FIRST_for_statement(la.d_type) ) {
		for_statement();
	} else if( FIRST_while_statement(la.d_type) ) {
		while_statement();
	} else
		invalid("Common_Base_conditional_statement");
}

void Parser2::for_statement() {
	for_clause();
	statement();
}

void Parser2::for_clause() {
	expect(Tok_FOR, false, "for_clause");
	simple_variable();
	for_right_part();
	expect(Tok_DO, false, "for_clause");
}

void Parser2::for_right_part() {
	if( la.d_type == Tok_ColonEq ) {
		expect(Tok_ColonEq, false, "for_right_part");
		value_for_list();
	} else if( la.d_type == Tok_ColonMinus ) {
		expect(Tok_ColonMinus, false, "for_right_part");
		object_for_list();
	} else
		invalid("for_right_part");
}

void Parser2::value_for_list() {
	value_for_list_element();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "value_for_list");
		value_for_list_element();
	}
}

void Parser2::object_for_list() {
	object_for_list_element();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "object_for_list");
		object_for_list_element();
	}
}

void Parser2::value_for_list_element() {
	expression();
	if( la.d_type == Tok_STEP || la.d_type == Tok_WHILE ) {
		if( la.d_type == Tok_STEP ) {
			expect(Tok_STEP, false, "value_for_list_element");
			expression();
			expect(Tok_UNTIL, false, "value_for_list_element");
			expression();
		} else if( la.d_type == Tok_WHILE ) {
			expect(Tok_WHILE, false, "value_for_list_element");
			expression();
		} else
			invalid("value_for_list_element");
	}
}

void Parser2::object_for_list_element() {
	expression();
	if( la.d_type == Tok_WHILE ) {
		expect(Tok_WHILE, false, "object_for_list_element");
		expression();
	}
}

void Parser2::go_to_statement() {
	if( la.d_type == Tok_GOTO ) {
		expect(Tok_GOTO, false, "go_to_statement");
	} else if( la.d_type == Tok_GO ) {
		expect(Tok_GO, false, "go_to_statement");
		expect(Tok_TO, false, "go_to_statement");
	} else
		invalid("go_to_statement");
	expression();
}

void Parser2::unlabelled_basic_statement() {
	if( FIRST_go_to_statement(la.d_type) ) {
		go_to_statement();
	} else if( FIRST_activation_statement(la.d_type) ) {
		activation_statement();
	} else if( FIRST_connection_statement(la.d_type) ) {
		connection_statement();
	} else if( FIRST_main_block(la.d_type) ) {
		main_block();
	} else if( FIRST_primary(la.d_type) ) {
		primary();
		if( ( peek(1).d_type == Tok_BEGIN && ( peek(2).d_type == Tok_ACTIVATE || peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_BEGIN || peek(2).d_type == Tok_BOOLEAN || peek(2).d_type == Tok_CHARACTER || peek(2).d_type == Tok_CLASS || peek(2).d_type == Tok_END || peek(2).d_type == Tok_EXTERNAL || peek(2).d_type == Tok_FALSE || peek(2).d_type == Tok_FOR || peek(2).d_type == Tok_GO || peek(2).d_type == Tok_GOTO || peek(2).d_type == Tok_IF || peek(2).d_type == Tok_INSPECT || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_LONG || peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_NEW || peek(2).d_type == Tok_NONE || peek(2).d_type == Tok_NOTEXT || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REACTIVATE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_REF || peek(2).d_type == Tok_SHORT || peek(2).d_type == Tok_SWITCH || peek(2).d_type == Tok_Semi || peek(2).d_type == Tok_TEXT || peek(2).d_type == Tok_THIS || peek(2).d_type == Tok_TRUE || peek(2).d_type == Tok_WHILE || peek(2).d_type == Tok_character || peek(2).d_type == Tok_decimal_number || peek(2).d_type == Tok_identifier || peek(2).d_type == Tok_string || peek(2).d_type == Tok_unsigned_integer ) )  || la.d_type == Tok_ColonEq || la.d_type == Tok_ColonMinus ) {
			if( ( peek(1).d_type == Tok_BEGIN && ( peek(2).d_type == Tok_ACTIVATE || peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_BEGIN || peek(2).d_type == Tok_BOOLEAN || peek(2).d_type == Tok_CHARACTER || peek(2).d_type == Tok_CLASS || peek(2).d_type == Tok_END || peek(2).d_type == Tok_EXTERNAL || peek(2).d_type == Tok_FALSE || peek(2).d_type == Tok_FOR || peek(2).d_type == Tok_GO || peek(2).d_type == Tok_GOTO || peek(2).d_type == Tok_IF || peek(2).d_type == Tok_INSPECT || peek(2).d_type == Tok_INTEGER || peek(2).d_type == Tok_LONG || peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_NEW || peek(2).d_type == Tok_NONE || peek(2).d_type == Tok_NOTEXT || peek(2).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_REACTIVATE || peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_REF || peek(2).d_type == Tok_SHORT || peek(2).d_type == Tok_SWITCH || peek(2).d_type == Tok_Semi || peek(2).d_type == Tok_TEXT || peek(2).d_type == Tok_THIS || peek(2).d_type == Tok_TRUE || peek(2).d_type == Tok_WHILE || peek(2).d_type == Tok_character || peek(2).d_type == Tok_decimal_number || peek(2).d_type == Tok_identifier || peek(2).d_type == Tok_string || peek(2).d_type == Tok_unsigned_integer ) )  ) {
				main_block();
			} else if( la.d_type == Tok_ColonEq ) {
				expect(Tok_ColonEq, false, "unlabelled_basic_statement");
				expression();
				while( la.d_type == Tok_ColonEq ) {
					expect(Tok_ColonEq, false, "unlabelled_basic_statement");
					expression();
				}
			} else if( la.d_type == Tok_ColonMinus ) {
				expect(Tok_ColonMinus, false, "unlabelled_basic_statement");
				expression();
				while( la.d_type == Tok_ColonMinus ) {
					expect(Tok_ColonMinus, false, "unlabelled_basic_statement");
					expression();
				}
			} else
				invalid("unlabelled_basic_statement");
		}
	} else
		invalid("unlabelled_basic_statement");
}

void Parser2::when_clause() {
	expect(Tok_WHEN, false, "when_clause");
	class_identifier();
	expect(Tok_DO, false, "when_clause");
	statement();
}

void Parser2::otherwise_clause() {
	expect(Tok_OTHERWISE, false, "otherwise_clause");
	statement();
}

void Parser2::connection_part() {
	when_clause();
	while( FIRST_when_clause(la.d_type) ) {
		when_clause();
	}
}

void Parser2::connection_statement() {
	expect(Tok_INSPECT, false, "connection_statement");
	expression();
	if( FIRST_connection_part(la.d_type) ) {
		connection_part();
	} else if( la.d_type == Tok_DO ) {
		expect(Tok_DO, false, "connection_statement");
		statement();
	} else
		invalid("connection_statement");
	if( FIRST_otherwise_clause(la.d_type) ) {
		otherwise_clause();
	}
}

void Parser2::activator() {
	if( la.d_type == Tok_ACTIVATE ) {
		expect(Tok_ACTIVATE, false, "activator");
	} else if( la.d_type == Tok_REACTIVATE ) {
		expect(Tok_REACTIVATE, false, "activator");
	} else
		invalid("activator");
}

void Parser2::activation_clause() {
	activator();
	expression();
}

void Parser2::simple_timing_clause() {
	if( la.d_type == Tok_AT ) {
		expect(Tok_AT, false, "simple_timing_clause");
		expression();
	} else if( la.d_type == Tok_DELAY ) {
		expect(Tok_DELAY, false, "simple_timing_clause");
		expression();
	} else
		invalid("simple_timing_clause");
}

void Parser2::timing_clause() {
	simple_timing_clause();
	if( la.d_type == Tok_PRIOR ) {
		expect(Tok_PRIOR, false, "timing_clause");
	}
}

void Parser2::scheduling_clause() {
	if( FIRST_timing_clause(la.d_type) ) {
		timing_clause();
	} else if( la.d_type == Tok_BEFORE ) {
		expect(Tok_BEFORE, false, "scheduling_clause");
		expression();
	} else if( la.d_type == Tok_AFTER ) {
		expect(Tok_AFTER, false, "scheduling_clause");
		expression();
	} else
		invalid("scheduling_clause");
}

void Parser2::activation_statement() {
	activation_clause();
	if( FIRST_scheduling_clause(la.d_type) ) {
		scheduling_clause();
	}
}

void Parser2::specifier() {
	if( la.d_type == Tok_SWITCH ) {
		expect(Tok_SWITCH, false, "specifier");
	} else if( la.d_type == Tok_LABEL ) {
		expect(Tok_LABEL, false, "specifier");
	} else if( ( ( peek(1).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REF && peek(5).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_ARRAY || peek(2).d_type == Tok_ARRAY || peek(1).d_type == Tok_REF && peek(5).d_type == Tok_ARRAY ) )  ) {
		if( FIRST_type(la.d_type) ) {
			type();
		}
		if( la.d_type == Tok_ARRAY ) {
			expect(Tok_ARRAY, false, "specifier");
		} else if( la.d_type == Tok_PROCEDURE ) {
			expect(Tok_PROCEDURE, false, "specifier");
		} else
			invalid("specifier");
	} else if( FIRST_type(la.d_type) ) {
		type();
	} else
		invalid("specifier");
}

void Parser2::specification_part() {
	specifier();
	identifier_list();
	expect(Tok_Semi, false, "specification_part");
}

void Parser2::procedure_specification() {
	expect(Tok_IS, false, "procedure_specification");
	procedure_declaration();
}

void Parser2::external_item() {
	if( ( peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Eq )  ) {
		expect(Tok_identifier, false, "external_item");
		expect(Tok_Eq, false, "external_item");
	}
	external_identifier();
}

void Parser2::external_list() {
	external_item();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "external_list");
		external_item();
	}
}

void Parser2::external_declaration() {
	expect(Tok_EXTERNAL, false, "external_declaration");
	if( la.d_type == Tok_identifier || FIRST_type(la.d_type) || la.d_type == Tok_PROCEDURE ) {
		if( la.d_type == Tok_identifier ) {
			expect(Tok_identifier, false, "external_declaration");
		}
		if( FIRST_type(la.d_type) ) {
			type();
		}
		expect(Tok_PROCEDURE, false, "external_declaration");
		external_list();
		if( FIRST_procedure_specification(la.d_type) ) {
			procedure_specification();
		}
	} else if( la.d_type == Tok_CLASS ) {
		expect(Tok_CLASS, false, "external_declaration");
		external_list();
	} else
		invalid("external_declaration");
}

void Parser2::external_identifier() {
	if( la.d_type == Tok_identifier ) {
		expect(Tok_identifier, false, "external_identifier");
	} else if( la.d_type == Tok_string ) {
		expect(Tok_string, false, "external_identifier");
	} else
		invalid("external_identifier");
}

void Parser2::switch_declaration() {
	expect(Tok_SWITCH, false, "switch_declaration");
	switch_identifier();
	expect(Tok_ColonEq, false, "switch_declaration");
	switch_list();
}

void Parser2::switch_identifier() {
	expect(Tok_identifier, false, "switch_identifier");
}

void Parser2::switch_list() {
	expression();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "switch_list");
		expression();
	}
}

void Parser2::simple_variable() {
	variable_identifier();
}

void Parser2::variable_identifier() {
	expect(Tok_identifier, false, "variable_identifier");
}

void Parser2::type_list_element() {
	simple_variable();
	if( la.d_type == Tok_Eq ) {
		expect(Tok_Eq, false, "type_list_element");
		expression();
	}
}

void Parser2::type_list() {
	type_list_element();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "type_list");
		type_list_element();
	}
}

void Parser2::type_declaration() {
	type();
	type_list();
}

void Parser2::array_list() {
	array_segment();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "array_list");
		array_segment();
	}
}

void Parser2::array_segment() {
	expect(Tok_identifier, false, "array_segment");
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "array_segment");
		expect(Tok_identifier, false, "array_segment");
	}
	if( la.d_type == Tok_Lbrack ) {
		expect(Tok_Lbrack, false, "array_segment");
		bound_pair_list();
		expect(Tok_Rbrack, false, "array_segment");
	} else if( la.d_type == Tok_Lpar ) {
		expect(Tok_Lpar, false, "array_segment");
		bound_pair_list();
		expect(Tok_Rpar, false, "array_segment");
	} else
		invalid("array_segment");
}

void Parser2::bound_pair_list() {
	bound_pair();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "bound_pair_list");
		bound_pair();
	}
}

void Parser2::bound_pair() {
	lower_bound();
	expect(Tok_Colon, false, "bound_pair");
	upper_bound();
}

void Parser2::upper_bound() {
	expression();
}

void Parser2::lower_bound() {
	expression();
}

void Parser2::array_declaration() {
	if( FIRST_type(la.d_type) ) {
		type();
	}
	expect(Tok_ARRAY, false, "array_declaration");
	array_list();
}

void Parser2::type() {
	if( FIRST_value_type(la.d_type) ) {
		value_type();
	} else if( FIRST_reference_type(la.d_type) ) {
		reference_type();
	} else
		invalid("type");
}

void Parser2::value_type() {
	if( la.d_type == Tok_INTEGER ) {
		expect(Tok_INTEGER, false, "value_type");
	} else if( la.d_type == Tok_REAL ) {
		expect(Tok_REAL, false, "value_type");
	} else if( la.d_type == Tok_SHORT ) {
		expect(Tok_SHORT, false, "value_type");
		expect(Tok_INTEGER, false, "value_type");
	} else if( la.d_type == Tok_LONG ) {
		expect(Tok_LONG, false, "value_type");
		expect(Tok_REAL, false, "value_type");
	} else if( la.d_type == Tok_BOOLEAN ) {
		expect(Tok_BOOLEAN, false, "value_type");
	} else if( la.d_type == Tok_CHARACTER ) {
		expect(Tok_CHARACTER, false, "value_type");
	} else
		invalid("value_type");
}

void Parser2::reference_type() {
	if( FIRST_object_reference(la.d_type) ) {
		object_reference();
	} else if( la.d_type == Tok_TEXT ) {
		expect(Tok_TEXT, false, "reference_type");
	} else
		invalid("reference_type");
}

void Parser2::object_reference() {
	expect(Tok_REF, false, "object_reference");
	expect(Tok_Lpar, false, "object_reference");
	qualification();
	expect(Tok_Rpar, false, "object_reference");
}

void Parser2::qualification() {
	class_identifier();
}

void Parser2::label() {
	expect(Tok_identifier, false, "label");
}

void Parser2::if_clause() {
	expect(Tok_IF, false, "if_clause");
	expression();
	expect(Tok_THEN, false, "if_clause");
}

void Parser2::local_object() {
	expect(Tok_THIS, false, "local_object");
	class_identifier();
}

void Parser2::object_generator() {
	expect(Tok_NEW, false, "object_generator");
	class_identifier();
}

void Parser2::actual_parameter_list() {
	actual_parameter();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "actual_parameter_list");
		actual_parameter();
	}
}

void Parser2::actual_parameter() {
	expression();
}

void Parser2::expression() {
	if( FIRST_quaternary_(la.d_type) ) {
		quaternary_();
	} else if( FIRST_if_clause(la.d_type) ) {
		if_clause();
		quaternary_();
		expect(Tok_ELSE, false, "expression");
		expression();
	} else
		invalid("expression");
}

void Parser2::quaternary_() {
	tertiary_();
	while( la.d_type == Tok_OR_ELSE ) {
		expect(Tok_OR_ELSE, false, "quaternary_");
		tertiary_();
	}
}

void Parser2::tertiary_() {
	equivalence_();
	while( la.d_type == Tok_AND_THEN ) {
		expect(Tok_AND_THEN, false, "tertiary_");
		equivalence_();
	}
}

void Parser2::equivalence_() {
	implication();
	while( FIRST_equiv_sym_(la.d_type) ) {
		equiv_sym_();
		implication();
	}
}

void Parser2::equiv_sym_() {
	if( la.d_type == Tok_EQUIV ) {
		expect(Tok_EQUIV, false, "equiv_sym_");
	} else if( la.d_type == Tok_Ueq ) {
		expect(Tok_Ueq, false, "equiv_sym_");
	} else if( la.d_type == Tok_2Eq ) {
		expect(Tok_2Eq, false, "equiv_sym_");
	} else if( la.d_type == Tok_EQV ) {
		expect(Tok_EQV, false, "equiv_sym_");
	} else
		invalid("equiv_sym_");
}

void Parser2::implication() {
	simple_expression_();
	while( FIRST_impl_sym_(la.d_type) ) {
		impl_sym_();
		simple_expression_();
	}
}

void Parser2::impl_sym_() {
	if( la.d_type == Tok_IMPL ) {
		expect(Tok_IMPL, false, "impl_sym_");
	} else if( la.d_type == Tok_Uimpl ) {
		expect(Tok_Uimpl, false, "impl_sym_");
	} else if( la.d_type == Tok_MinusGt ) {
		expect(Tok_MinusGt, false, "impl_sym_");
	} else if( la.d_type == Tok_IMP ) {
		expect(Tok_IMP, false, "impl_sym_");
	} else
		invalid("impl_sym_");
}

void Parser2::simple_expression_() {
	term();
	while( FIRST_adding_operator(la.d_type) || FIRST_or_sym_(la.d_type) ) {
		if( FIRST_adding_operator(la.d_type) ) {
			adding_operator();
		} else if( FIRST_or_sym_(la.d_type) ) {
			or_sym_();
		} else
			invalid("simple_expression_");
		term();
	}
}

void Parser2::adding_operator() {
	if( la.d_type == Tok_Plus ) {
		expect(Tok_Plus, false, "adding_operator");
	} else if( la.d_type == Tok_Minus ) {
		expect(Tok_Minus, false, "adding_operator");
	} else
		invalid("adding_operator");
}

void Parser2::or_sym_() {
	if( la.d_type == Tok_OR ) {
		expect(Tok_OR, false, "or_sym_");
	} else if( la.d_type == Tok_Uor ) {
		expect(Tok_Uor, false, "or_sym_");
	} else if( la.d_type == Tok_Bar ) {
		expect(Tok_Bar, false, "or_sym_");
	} else
		invalid("or_sym_");
}

void Parser2::term() {
	factor();
	while( FIRST_multiplying_operator(la.d_type) || FIRST_and_sym_(la.d_type) ) {
		if( FIRST_multiplying_operator(la.d_type) ) {
			multiplying_operator();
		} else if( FIRST_and_sym_(la.d_type) ) {
			and_sym_();
		} else
			invalid("term");
		factor();
	}
}

void Parser2::multiplying_operator() {
	if( la.d_type == Tok_Star ) {
		expect(Tok_Star, false, "multiplying_operator");
	} else if( la.d_type == Tok_Slash ) {
		expect(Tok_Slash, false, "multiplying_operator");
	} else if( la.d_type == Tok_Percent ) {
		expect(Tok_Percent, false, "multiplying_operator");
	} else if( la.d_type == Tok_Udiv ) {
		expect(Tok_Udiv, false, "multiplying_operator");
	} else if( la.d_type == Tok_Umul ) {
		expect(Tok_Umul, false, "multiplying_operator");
	} else if( la.d_type == Tok_2Slash ) {
		expect(Tok_2Slash, false, "multiplying_operator");
	} else
		invalid("multiplying_operator");
}

void Parser2::and_sym_() {
	if( la.d_type == Tok_AND ) {
		expect(Tok_AND, false, "and_sym_");
	} else if( la.d_type == Tok_Uand ) {
		expect(Tok_Uand, false, "and_sym_");
	} else if( la.d_type == Tok_Amp ) {
		expect(Tok_Amp, false, "and_sym_");
	} else
		invalid("and_sym_");
}

void Parser2::factor() {
	secondary();
	while( FIRST_power_sym_(la.d_type) ) {
		power_sym_();
		secondary();
	}
}

void Parser2::power_sym_() {
	if( la.d_type == Tok_POWER ) {
		expect(Tok_POWER, false, "power_sym_");
	} else if( la.d_type == Tok_Uexp ) {
		expect(Tok_Uexp, false, "power_sym_");
	} else if( la.d_type == Tok_Hat ) {
		expect(Tok_Hat, false, "power_sym_");
	} else if( la.d_type == Tok_2Star ) {
		expect(Tok_2Star, false, "power_sym_");
	} else
		invalid("power_sym_");
}

void Parser2::secondary() {
	if( FIRST_not_sym_(la.d_type) ) {
		not_sym_();
	}
	if( FIRST_adding_operator(la.d_type) ) {
		adding_operator();
	}
	primary();
}

void Parser2::not_sym_() {
	if( la.d_type == Tok_NOT ) {
		expect(Tok_NOT, false, "not_sym_");
	} else if( la.d_type == Tok_Unot ) {
		expect(Tok_Unot, false, "not_sym_");
	} else if( la.d_type == Tok_Bang ) {
		expect(Tok_Bang, false, "not_sym_");
	} else
		invalid("not_sym_");
}

void Parser2::primary() {
	if( FIRST_unsigned_number(la.d_type) ) {
		unsigned_number();
		primary_nlr_();
	} else if( FIRST_logical_value(la.d_type) ) {
		logical_value();
		primary_nlr_();
	} else if( la.d_type == Tok_character ) {
		expect(Tok_character, false, "primary");
		primary_nlr_();
	} else if( FIRST_string_(la.d_type) ) {
		string_();
		primary_nlr_();
	} else if( la.d_type == Tok_NOTEXT ) {
		expect(Tok_NOTEXT, false, "primary");
		primary_nlr_();
	} else if( la.d_type == Tok_NONE ) {
		expect(Tok_NONE, false, "primary");
		primary_nlr_();
	} else if( FIRST_local_object(la.d_type) ) {
		local_object();
		primary_nlr_();
	} else if( FIRST_object_generator(la.d_type) ) {
		object_generator();
		primary_nlr_();
	} else if( la.d_type == Tok_Lpar ) {
		expect(Tok_Lpar, false, "primary");
		expression();
		expect(Tok_Rpar, false, "primary");
		primary_nlr_();
	} else if( la.d_type == Tok_identifier ) {
		expect(Tok_identifier, false, "primary");
		primary_nlr_();
	} else
		invalid("primary");
}

void Parser2::primary_nlr_() {
	if( FIRST_relation_(la.d_type) || FIRST_selector_(la.d_type) || FIRST_qualified_(la.d_type) ) {
		if( FIRST_relation_(la.d_type) ) {
			relation_();
		} else if( FIRST_selector_(la.d_type) ) {
			selector_();
		} else if( FIRST_qualified_(la.d_type) ) {
			qualified_();
		} else
			invalid("primary_nlr_");
		primary_nlr_();
	}
}

void Parser2::relation_() {
	relational_operator();
	simple_expression_();
}

void Parser2::qualified_() {
	expect(Tok_QUA, false, "qualified_");
	class_identifier();
}

void Parser2::selector_() {
	if( la.d_type == Tok_Dot ) {
		expect(Tok_Dot, false, "selector_");
		attribute_identifier();
	} else if( la.d_type == Tok_Lbrack ) {
		expect(Tok_Lbrack, false, "selector_");
		subscript_list();
		expect(Tok_Rbrack, false, "selector_");
	} else if( FIRST_actual_parameter_part(la.d_type) ) {
		actual_parameter_part();
	} else
		invalid("selector_");
}

void Parser2::relational_operator() {
	if( la.d_type == Tok_Lt ) {
		expect(Tok_Lt, false, "relational_operator");
	} else if( la.d_type == Tok_Leq ) {
		expect(Tok_Leq, false, "relational_operator");
	} else if( la.d_type == Tok_Eq ) {
		expect(Tok_Eq, false, "relational_operator");
	} else if( la.d_type == Tok_Geq ) {
		expect(Tok_Geq, false, "relational_operator");
	} else if( la.d_type == Tok_Gt ) {
		expect(Tok_Gt, false, "relational_operator");
	} else if( la.d_type == Tok_LtGt ) {
		expect(Tok_LtGt, false, "relational_operator");
	} else if( la.d_type == Tok_Uleq ) {
		expect(Tok_Uleq, false, "relational_operator");
	} else if( la.d_type == Tok_Ugeq ) {
		expect(Tok_Ugeq, false, "relational_operator");
	} else if( la.d_type == Tok_Uneq ) {
		expect(Tok_Uneq, false, "relational_operator");
	} else if( la.d_type == Tok_BangEq ) {
		expect(Tok_BangEq, false, "relational_operator");
	} else if( la.d_type == Tok_HatEq ) {
		expect(Tok_HatEq, false, "relational_operator");
	} else if( la.d_type == Tok_LESS ) {
		expect(Tok_LESS, false, "relational_operator");
	} else if( la.d_type == Tok_NOTGREATER ) {
		expect(Tok_NOTGREATER, false, "relational_operator");
	} else if( la.d_type == Tok_EQUAL ) {
		expect(Tok_EQUAL, false, "relational_operator");
	} else if( la.d_type == Tok_NOTLESS ) {
		expect(Tok_NOTLESS, false, "relational_operator");
	} else if( la.d_type == Tok_GREATER ) {
		expect(Tok_GREATER, false, "relational_operator");
	} else if( la.d_type == Tok_NOTEQUAL ) {
		expect(Tok_NOTEQUAL, false, "relational_operator");
	} else if( la.d_type == Tok_LT ) {
		expect(Tok_LT, false, "relational_operator");
	} else if( la.d_type == Tok_LE ) {
		expect(Tok_LE, false, "relational_operator");
	} else if( la.d_type == Tok_EQ ) {
		expect(Tok_EQ, false, "relational_operator");
	} else if( la.d_type == Tok_GE ) {
		expect(Tok_GE, false, "relational_operator");
	} else if( la.d_type == Tok_GT ) {
		expect(Tok_GT, false, "relational_operator");
	} else if( la.d_type == Tok_NE ) {
		expect(Tok_NE, false, "relational_operator");
	} else if( la.d_type == Tok_IS ) {
		expect(Tok_IS, false, "relational_operator");
	} else if( la.d_type == Tok_IN ) {
		expect(Tok_IN, false, "relational_operator");
	} else if( la.d_type == Tok_2Eq ) {
		expect(Tok_2Eq, false, "relational_operator");
	} else if( la.d_type == Tok_EqSlashEq ) {
		expect(Tok_EqSlashEq, false, "relational_operator");
	} else
		invalid("relational_operator");
}

void Parser2::logical_value() {
	if( la.d_type == Tok_TRUE ) {
		expect(Tok_TRUE, false, "logical_value");
	} else if( la.d_type == Tok_FALSE ) {
		expect(Tok_FALSE, false, "logical_value");
	} else
		invalid("logical_value");
}

void Parser2::unsigned_number() {
	if( la.d_type == Tok_unsigned_integer ) {
		expect(Tok_unsigned_integer, false, "unsigned_number");
	} else if( la.d_type == Tok_decimal_number ) {
		expect(Tok_decimal_number, false, "unsigned_number");
	} else
		invalid("unsigned_number");
}

void Parser2::class_identifier() {
	expect(Tok_identifier, false, "class_identifier");
}

void Parser2::identifier_list() {
	expect(Tok_identifier, false, "identifier_list");
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "identifier_list");
		expect(Tok_identifier, false, "identifier_list");
	}
}

void Parser2::subscript_list() {
	subscript_expression();
	while( la.d_type == Tok_Comma ) {
		expect(Tok_Comma, false, "subscript_list");
		subscript_expression();
	}
}

void Parser2::subscript_expression() {
	expression();
}

void Parser2::attribute_identifier() {
	expect(Tok_identifier, false, "attribute_identifier");
}

void Parser2::string_() {
	expect(Tok_string, false, "string_");
	while( la.d_type == Tok_string ) {
		expect(Tok_string, false, "string_");
	}
}

