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
#include "SimLexer.h"
#include <QtDebug>
using namespace Sim;

// FIRST set helper functions (from SimParser2.cpp)

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

static inline bool FIRST_switch_declaration(int tt) {
    return tt == Tok_SWITCH;
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_tertiary_(int tt) {
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_equivalence_(int tt) {
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_equiv_sym_(int tt) {
    return tt == Tok_EQUIV || tt == Tok_Ueq || tt == Tok_2Eq || tt == Tok_EQV;
}

static inline bool FIRST_implication(int tt) {
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_impl_sym_(int tt) {
    return tt == Tok_IMPL || tt == Tok_Uimpl || tt == Tok_MinusGt || tt == Tok_IMP;
}

static inline bool FIRST_simple_expression_(int tt) {
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_adding_operator(int tt) {
    return tt == Tok_Plus || tt == Tok_Minus;
}

static inline bool FIRST_or_sym_(int tt) {
    return tt == Tok_OR || tt == Tok_Uor || tt == Tok_Bar;
}

static inline bool FIRST_term(int tt) {
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
    case Tok_character:
    case Tok_Unot:
    case Tok_FALSE:
    case Tok_Minus:
    case Tok_decimal_number:
        return true;
    default: return false;
    }
}

static inline bool FIRST_multiplying_operator(int tt) {
    switch(tt){
    case Tok_Star:
    case Tok_Slash:
    case Tok_Percent:
    case Tok_Udiv:
    case Tok_Umul:
    case Tok_2Slash:
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

static inline bool FIRST_string_(int tt) {
    return tt == Tok_string;
}

// Parser implementation

Parser3::Parser3(Scanner* s, AstModel* m) : scanner(s), mdl(m), thisMod(0) {
}

Parser3::~Parser3() {
    if (thisMod)
        Declaration::deleteAll(thisMod);
}

Declaration* Parser3::RunParser() {
    errors.clear();
    next();
    return module();
}

Declaration* Parser3::takeResult() {
    Declaration* res = thisMod;
    thisMod = 0;
    return res;
}

void Parser3::next() {
    cur = la;
    la = scanner->next();
    while (la.d_type == Tok_Invalid) {
        errors << Error(la.d_val, toRowCol(la), la.d_sourcePath);
        la = scanner->next();
    }
}

Token Parser3::peek(int off) {
    if (off == 1)
        return la;
    else if (off == 0)
        return cur;
    else
        return scanner->peek(off - 1);
}

void Parser3::error(const Token& t, const QString& msg) {
    errors << Error(msg, toRowCol(t), t.d_sourcePath);
}

void Parser3::invalid(const char* what) {
    errors << Error(QString("invalid %1").arg(what), toRowCol(la), la.d_sourcePath);
}

bool Parser3::expect(int tt, bool pkw, const char* where) {
    if (la.d_type == tt) {
        next();
        return true;
    } else {
        errors << Error(QString("'%1' expected in %2").arg(tokenTypeString(tt)).arg(where),
                       toRowCol(la), la.d_sourcePath);
        return false;
    }
}

RowCol Parser3::toRowCol(const Token& t) const {
    return RowCol(t.d_lineNr, t.d_colNr);
}

bool Parser3::versionCheck(SimulaVersion minVersion, const char* feature) {
    // Check if current version supports this feature (requires at least minVersion)
    // Returns true if OK, false if version too old
    // Note: Sim70 < Sim75 < Sim86
    SimulaVersion currentVersion = mdl ? Sim86 : Sim86; // TODO: get from mdl
    if (currentVersion < minVersion) {
        if (feature)
            error(la, QString("'%1' requires Simula %2 or later").arg(feature).arg(
                minVersion == Sim75 ? "75" : "86"));
        return false;
    }
    return true;
}

bool Parser3::versionMax(SimulaVersion maxVersion, const char* feature) {
    // Check if current version allows this feature (requires at most maxVersion)
    // Returns true if OK, false if version too new
    SimulaVersion currentVersion = mdl ? Sim86 : Sim86; // TODO: get from mdl
    if (currentVersion > maxVersion) {
        if (feature)
            error(la, QString("'%1' is not available in Simula %2").arg(feature).arg(
                currentVersion == Sim75 ? "75" : "86"));
        return false;
    }
    return true;
}

// Module parsing

Declaration* Parser3::module() {
    Declaration* mod = new Declaration(Declaration::Module);
    if (thisMod)
        Declaration::deleteAll(thisMod);
    thisMod = mod;
    
    mod->path = new QString();
    *mod->path = scanner->source().toUtf8();
    
    mdl->openScope(mod);
    
    if (FIRST_external_head(la.d_type)) {
        external_head();
    }
    
    module_body_();

    while (la.d_type == Tok_Semi) {
        expect(Tok_Semi, false, "module");
        if (FIRST_module_body_(la.d_type)) {
            module_body_();

        }
    }
    
    mdl->closeScope();
    return mod;
}

void Parser3::module_body_() {
    if ((peek(1).d_type == Tok_CLASS || peek(1).d_type == Tok_identifier) &&
        (peek(2).d_type == Tok_CLASS || peek(2).d_type == Tok_identifier)) {
        class_declaration();
    } else if (FIRST_procedure_declaration(la.d_type)) {
        procedure_declaration();
    } else if (FIRST_program(la.d_type)) {
        program();
    } else {
        invalid("module_body_");
    }
}

void Parser3::external_head() {
    external_declaration();
    expect(Tok_Semi, false, "external_head");
    while (FIRST_external_declaration(la.d_type)) {
        external_declaration();
        expect(Tok_Semi, false, "external_head");
    }
}

void Parser3::program() {
    
    // Handle labels
    while ((peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon)) {
        const Token lbl = label();
        expect(Tok_Colon, false, "program");
        // Create label declaration
        Declaration* lblDecl = mdl->addDecl(lbl.d_id, lbl.d_val, Declaration::LabelDecl);
        lblDecl->pos = toRowCol(cur);
    }
    
    Declaration* prog = mdl->addDecl("","", Declaration::Program);
    prog->pos = toRowCol(la); // begin
    mdl->openScope(prog);
    prog->body = block();
    mdl->closeScope();
}

// Statement parsing

Statement* Parser3::while_statement() {
    expect(Tok_WHILE, false, "while_statement");
    RowCol pos = toRowCol(cur);
    
    Expression* cond = expression();
    expect(Tok_DO, false, "while_statement");
    Statement* body = statement();
    
    Statement* stmt = new Statement(Statement::While, pos);
    stmt->cond = cond;
    stmt->body = body;
    return stmt;
}

Statement* Parser3::block() {
    Token prefixName;
    QList<Expression*> args;
    
    if (FIRST_block_prefix(la.d_type)) {
        block_prefix(prefixName, args);
    }
    return main_block(prefixName.d_id, args);
}

void Parser3::block_prefix(Token& prefixName, QList<Expression*>& args) {
    prefixName = class_identifier();
    if (FIRST_actual_parameter_part(la.d_type)) {
        actual_parameter_part(args);
    }
}

void Parser3::actual_parameter_part(QList<Expression*>& args) {
    expect(Tok_Lpar, false, "actual_parameter_part");
    actual_parameter_list(args);
    expect(Tok_Rpar, false, "actual_parameter_part");
}

Statement* Parser3::main_block(Atom prefixName, const QList<Expression*>& args) {
    expect(Tok_BEGIN, false, "main_block");
    RowCol pos = toRowCol(cur);
    
    Statement* blk = new Statement(Statement::Block, pos);
    
    // Store prefix info if present
    if (prefixName != 0) {
        Expression* prefixExpr = new Expression(Expression::Identifier, pos);
        prefixExpr->a = prefixName;
        blk->prefix = prefixExpr;
        if(!args.isEmpty())
        {
            blk->args = args.first();
            Expression* e = blk->args;
            for( int i = 1; i < args.size(); i++ )
            {
                e->next = args[i];
                e = e->next;
            }
        }
    }
    
    // Open a new scope for the block
    Declaration* blockScope = mdl->addDecl("", "", Declaration::Block);
    blockScope->pos = pos;
    mdl->openScope(blockScope);
    
    // Parse declarations
    if (((peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN ||
          peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_CLASS ||
          peek(1).d_type == Tok_EXTERNAL || peek(1).d_type == Tok_INTEGER ||
          peek(1).d_type == Tok_LONG || peek(1).d_type == Tok_PROCEDURE ||
          peek(1).d_type == Tok_REAL || peek(1).d_type == Tok_REF ||
          peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_SWITCH ||
          peek(1).d_type == Tok_TEXT) ||
         (peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_CLASS))) {
        declaration();
        while ((peek(1).d_type == Tok_Semi &&
                ((peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_BOOLEAN ||
                  peek(2).d_type == Tok_CHARACTER || peek(2).d_type == Tok_CLASS ||
                  peek(2).d_type == Tok_EXTERNAL || peek(2).d_type == Tok_INTEGER ||
                  peek(2).d_type == Tok_LONG || peek(2).d_type == Tok_PROCEDURE ||
                  peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_REF ||
                  peek(2).d_type == Tok_SHORT || peek(2).d_type == Tok_SWITCH ||
                  peek(2).d_type == Tok_TEXT) ||
                 (peek(2).d_type == Tok_identifier && peek(3).d_type == Tok_CLASS)))) {
            expect(Tok_Semi, false, "main_block");
            declaration();
        }
        expect(Tok_Semi, false, "main_block");
    }
    
    blk->body = compound_tail();
    
    mdl->closeScope();
    blk->scope = blockScope;
    return blk;
}

Statement* Parser3::compound_tail() {
    Statement* first = 0;
    Statement* last = 0;
    
    if (FIRST_statement(la.d_type) ||
        (peek(1).d_type == Tok_Semi && !(peek(2).d_type == Tok_END) && !(peek(2).d_type == Tok_INNER))) {
        first = statement();
        last = first;
        while ((peek(1).d_type == Tok_Semi && !(peek(2).d_type == Tok_END) && !(peek(2).d_type == Tok_INNER))) {
            expect(Tok_Semi, false, "compound_tail");
            Statement* s = statement();
            if (s) {
                if (last)
                    last->append(s);
                else
                    first = s;
                last = s;
            }
        }
    }
    
    // Handle INNER
    if ((peek(1).d_type == Tok_Semi && peek(2).d_type == Tok_INNER)) {
        expect(Tok_Semi, false, "compound_tail");
        expect(Tok_INNER, false, "compound_tail");
        Statement* inner = new Statement(Statement::Inner, toRowCol(cur));
        if (last)
            last->append(inner);
        else
            first = inner;
        last = inner;
        
        while ((peek(1).d_type == Tok_Semi && !(peek(2).d_type == Tok_END))) {
            expect(Tok_Semi, false, "compound_tail");
            Statement* s = statement();
            if (s) {
                last->append(s);
                last = s;
            }
        }
    }
    
    if (la.d_type == Tok_Semi) {
        expect(Tok_Semi, false, "compound_tail");
    }
    expect(Tok_END, false, "compound_tail");
    
    return first;
}

// Declaration parsing

void Parser3::declaration() {
    if (FIRST_switch_declaration(la.d_type)) {
        switch_declaration();
    } else if (FIRST_external_declaration(la.d_type)) {
        external_declaration();
    } else if (((peek(1).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_PROCEDURE ||
                 (peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_LONG) && peek(3).d_type == Tok_PROCEDURE ||
                 peek(1).d_type == Tok_REF && peek(5).d_type == Tok_PROCEDURE))) {
        procedure_declaration();
    } else if (((peek(1).d_type == Tok_ARRAY || peek(2).d_type == Tok_ARRAY ||
                 (peek(1).d_type == Tok_SHORT || peek(1).d_type == Tok_LONG) && peek(3).d_type == Tok_ARRAY ||
                 peek(1).d_type == Tok_REF && peek(5).d_type == Tok_ARRAY))) {
        array_declaration();
    } else if (((peek(1).d_type == Tok_CLASS || peek(2).d_type == Tok_CLASS))) {
        class_declaration();
    } else if (FIRST_type_declaration(la.d_type)) {
        type_declaration();
    } else {
        invalid("declaration");
    }
}

void Parser3::class_declaration() {
    Token prefixName = prefix();
        
    Declaration* classDecl = main_part();
    
    // Link prefix if present
    if (!prefixName.d_val.isEmpty()) {
        // Will be resolved during semantic analysis
        classDecl->nameRef = prefixName.d_id;
    }
}

Token Parser3::prefix() {
    if (FIRST_class_identifier(la.d_type)) {
        return class_identifier();
    }
    return Token();
}

Declaration* Parser3::main_part() {
    expect(Tok_CLASS, false, "main_part");
    
    expect(Tok_identifier, false, "main_part");

    Declaration* classDecl = mdl->addDecl(cur.d_id, cur.d_val, Declaration::Class);
    classDecl->pos = toRowCol(cur);

    mdl->openScope(classDecl);
    
    formal_parameter_part(classDecl);
    expect(Tok_Semi, false, "main_part");
    
    if (FIRST_value_part(la.d_type)) {
        value_part(classDecl);
    }
    
    while (((peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN ||
             peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER ||
             peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG ||
             peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL ||
             peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT ||
             peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT) &&
            (peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER ||
             peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE ||
             peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_identifier))) {
        specification_part(classDecl);
    }
    
    if (FIRST_protection_part(la.d_type)) {
        if (!versionCheck(Sim75, "HIDDEN/PROTECTED")) {
            // Skip but continue parsing
        }
        protection_part(classDecl);
        expect(Tok_Semi, false, "main_part");
    }
    
    virtual_part(classDecl);
    classDecl->body = class_body();

#if 0
    // repack block scope to class level
    if( classDecl->body && classDecl->body->kind == Statement::Block && classDecl->body->scope )
    {
        // TODO: maybe I should leave it as is and consider body->scope for member search
        // the problem is that the virtual decl uses the same name as the proc an shadows the latter,
        // so we should remove it an apply the flag to the proc
        Declaration* d = classDecl->body->scope->link;
        while(d)
        {
            d->outer = classDecl;
            d = d->next;
        }
        if( classDecl->body->scope->link )
            classDecl->appendMember(classDecl->body->scope->link);
        classDecl->body->scope->link = 0;
        classDecl->body->scope = 0;
        Q_ASSERT(classDecl->body->next == 0);
        Statement* s = classDecl->body->body;
        classDecl->body->body = 0;
        delete classDecl->body;
        classDecl->body = s;
    }
#endif
    
    mdl->closeScope();

    return classDecl;
}

void Parser3::protection_part(Declaration* classDecl) {
    protection_specification(classDecl);
    while ((peek(1).d_type == Tok_Semi && (peek(2).d_type == Tok_HIDDEN || peek(2).d_type == Tok_PROTECTED))) {
        expect(Tok_Semi, false, "protection_part");
        protection_specification(classDecl);
    }
}

void Parser3::protection_specification(Declaration* classDecl) {
    Declaration::Visi visi = Declaration::NA;
    bool hidden = false;
    bool prot = false;
    
    if (la.d_type == Tok_HIDDEN) {
        expect(Tok_HIDDEN, false, "protection_specification");
        hidden = true;
        if (la.d_type == Tok_PROTECTED) {
            expect(Tok_PROTECTED, false, "protection_specification");
            prot = true;
        }
    } else if (la.d_type == Tok_PROTECTED) {
        expect(Tok_PROTECTED, false, "protection_specification");
        prot = true;
        if (la.d_type == Tok_HIDDEN) {
            expect(Tok_HIDDEN, false, "protection_specification");
            hidden = true;
        }
    } else {
        invalid("protection_specification");
        return;
    }
    
    if (hidden)
        visi = Declaration::Hidden;
    else if (prot)
        visi = Declaration::Protected;
    
    QList<Token> ids = identifier_list();
    // TODO: Mark the identified members with visibility
    // This will be resolved during semantic analysis
}

Statement* Parser3::class_body() {
    if (FIRST_statement(la.d_type)) {
        return statement();
    }
    return 0;
}

void Parser3::virtual_part(Declaration* classDecl) {
    if (la.d_type == Tok_VIRTUAL) {
        expect(Tok_VIRTUAL, false, "virtual_part");
        expect(Tok_Colon, false, "virtual_part");
        virtual_spec(classDecl);
        expect(Tok_Semi, false, "virtual_part");
        while (((peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN ||
                 peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER ||
                 peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG ||
                 peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL ||
                 peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT ||
                 peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT) &&
                (peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER ||
                 peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE ||
                 peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_Semi ||
                 peek(2).d_type == Tok_identifier))) {
            virtual_spec(classDecl);
            expect(Tok_Semi, false, "virtual_part");
        }
    }
}

void Parser3::virtual_spec(Declaration* classDecl) {
    bool isArray = false;
    bool isProcedure = false;
    Type* t = specifier(isArray, isProcedure);
    
    QList<Token> ids = identifier_list();
    
    for (int i = 0; i < ids.size(); i++) {
        Declaration* virtDecl = mdl->addDecl(ids[i].d_id, ids[i].d_val, Declaration::VirtualSpec);
        virtDecl->pos = toRowCol(ids[i]);
        virtDecl->isVirtual = true;
        virtDecl->setType(t);
    }
    
    if (FIRST_procedure_specification(la.d_type)) {
        // SIM86 feature
        if (versionCheck(Sim86, "IS procedure_specification")) {
            // procedure_specification will be parsed
        }
        // For now, skip the procedure specification parsing
        // as it's complex and version-specific
        if (la.d_type == Tok_IS) {
            expect(Tok_IS, false, "virtual_spec");
            procedure_declaration();
        }
    }
}

void Parser3::procedure_declaration() {
    Type* retType = 0;
    if (FIRST_type(la.d_type)) {
        retType = type();
    }
    
    expect(Tok_PROCEDURE, false, "procedure_declaration");
        
    Declaration* procDecl = procedure_heading();
    
    procDecl->setType(retType ? retType : mdl->getType(Type::NoType));
    
    mdl->openScope(procDecl);
    procDecl->body = procedure_body();
    mdl->closeScope();
}

Declaration* Parser3::procedure_heading() {
    expect(Tok_identifier, false, "procedure_heading");
    Declaration* procDecl = mdl->addDecl(cur.d_id, cur.d_val, Declaration::Procedure);
    procDecl->pos = toRowCol(cur);
    
    mdl->openScope(procDecl);
    formal_parameter_part(procDecl);
    expect(Tok_Semi, false, "procedure_heading");
    
    if (FIRST_mode_part(la.d_type)) {
        mode_part(procDecl);
    }
    
    while (((peek(1).d_type == Tok_ARRAY || peek(1).d_type == Tok_BOOLEAN ||
             peek(1).d_type == Tok_CHARACTER || peek(1).d_type == Tok_INTEGER ||
             peek(1).d_type == Tok_LABEL || peek(1).d_type == Tok_LONG ||
             peek(1).d_type == Tok_PROCEDURE || peek(1).d_type == Tok_REAL ||
             peek(1).d_type == Tok_REF || peek(1).d_type == Tok_SHORT ||
             peek(1).d_type == Tok_SWITCH || peek(1).d_type == Tok_TEXT) &&
            (peek(2).d_type == Tok_ARRAY || peek(2).d_type == Tok_INTEGER ||
             peek(2).d_type == Tok_Lpar || peek(2).d_type == Tok_PROCEDURE ||
             peek(2).d_type == Tok_REAL || peek(2).d_type == Tok_identifier))) {
        specification_part(procDecl);
    }
    mdl->closeScope();
    return procDecl;
}

void Parser3::mode_part(Declaration* procDecl) {
    if (la.d_type == Tok_NAME) {
        name_part(procDecl);
        if (FIRST_value_part(la.d_type)) {
            value_part(procDecl);
        }
    } else if (la.d_type == Tok_VALUE) {
        value_part(procDecl);
        if (FIRST_name_part(la.d_type)) {
            name_part(procDecl);
        }
    }
}

void Parser3::value_part(Declaration* procDecl) {
    expect(Tok_VALUE, false, "value_part");
    QList<Token> ids = identifier_list();
    expect(Tok_Semi, false, "value_part");
    
    // Mark parameters as value mode
    Declaration* param = procDecl->link;
    while (param) {
        for (int i = 0; i < ids.size(); i++) {
            if (param->name.constData() == ids[i].d_val.constData()) {
                param->mode = Declaration::ModeValue;
                break;
            }
            // TODO: check if found, report otherwise
        }
        param = param->next;
    }
}

void Parser3::name_part(Declaration* procDecl) {
    expect(Tok_NAME, false, "name_part");
    QList<Token> ids = identifier_list();
    expect(Tok_Semi, false, "name_part");
    
    // Mark parameters as name mode
    Declaration* param = procDecl->link;
    while (param) {
        for (int i = 0; i < ids.size(); i++) {
            if (param->name.constData() == ids[i].d_val.constData()) {
                param->mode = Declaration::ModeName;
                break;
            }
            // TODO: check if found, report otherwise
        }
        param = param->next;
    }
}

void Parser3::formal_parameter_part(Declaration* procDecl) {
    if (FIRST_formal_parameter_part(la.d_type)) {
        expect(Tok_Lpar, false, "formal_parameter_part");
        if (FIRST_formal_parameter_list(la.d_type)) {
            formal_parameter_list(procDecl);
        }
        expect(Tok_Rpar, false, "formal_parameter_part");
    }
}

void Parser3::formal_parameter_list(Declaration* procDecl) {
    formal_parameter(procDecl);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "formal_parameter_list");
        formal_parameter(procDecl);
    }
}

void Parser3::formal_parameter(Declaration* procDecl) {
    expect(Tok_identifier, false, "formal_parameter");
    
    Declaration* param = mdl->addDecl(cur.d_id, cur.d_val,Declaration::Parameter);
    param->pos = toRowCol(cur);
    param->outer = procDecl;
}

Statement* Parser3::procedure_body() {
    return statement();
}

Statement* Parser3::statement() {
    // Handle labels
    while ((peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon)) {
        const Token lbl = label();
        expect(Tok_Colon, false, "statement");
        // Create label declaration in current scope
        Declaration* lblDecl = mdl->addDecl(lbl.d_id, lbl.d_val, Declaration::LabelDecl);
        lblDecl->pos = toRowCol(cur);
    }
    
    if (FIRST_Common_Base_statement(la.d_type)) {
        return Common_Base_statement();
    } else if (FIRST_while_statement(la.d_type)) {
        return while_statement();
    }
    
    // Dummy statement (empty)
    return new Statement(Statement::Dummy, toRowCol(la));
}

Statement* Parser3::unconditional_statement() {
    return unlabelled_basic_statement();
}

Statement* Parser3::Common_Base_statement() {
    if (FIRST_unconditional_statement(la.d_type)) {
        return unconditional_statement();
    } else if (FIRST_Common_Base_conditional_statement(la.d_type)) {
        return Common_Base_conditional_statement();
    } else if (FIRST_for_statement(la.d_type)) {
        return for_statement();
    } else {
        invalid("Common_Base_statement");
        return 0;
    }
}

Statement* Parser3::Common_Base_conditional_statement() {
    Expression* cond = if_clause();
    RowCol pos = cond ? cond->pos : toRowCol(la);
    
    // Handle labels after IF // TODO
    while ((peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Colon)) {
        const Token lbl = label();
        expect(Tok_Colon, false, "Common_Base_conditional_statement");
    }
    
    Statement* ifStmt = new Statement(Statement::If, pos);
    ifStmt->cond = cond;
    
    if (FIRST_unconditional_statement(la.d_type)) {
        ifStmt->body = unconditional_statement();
        if (la.d_type == Tok_ELSE) {
            expect(Tok_ELSE, false, "Common_Base_conditional_statement");
            ifStmt->elseStmt = statement();
        }
    } else if (la.d_type == Tok_ELSE) {
        // Empty then part
        expect(Tok_ELSE, false, "Common_Base_conditional_statement");
        ifStmt->elseStmt = statement();
    } else if (FIRST_for_statement(la.d_type)) {
        ifStmt->body = for_statement();
    } else if (FIRST_while_statement(la.d_type)) {
        ifStmt->body = while_statement();
    } else {
        invalid("Common_Base_conditional_statement");
    }
    
    return ifStmt;
}

Statement* Parser3::for_statement() {
    Expression* varExpr = 0;
    bool isRefAssign = false;
    QList<Expression*> forList;
    
    for_clause(varExpr, isRefAssign, forList);
    
    Statement* forStmt = new Statement(Statement::For, varExpr ? varExpr->pos : toRowCol(la));
    forStmt->var = varExpr;
    
    if( !forList.isEmpty() )
    {
        forStmt->list = forList.first();
        Expression* e = forStmt->list;
        for( int i = 1; i <  forList.size(); i++ )
        {
            e->next = forList[i];
            e = e->next;
        }
    }
    
    forStmt->body = statement();
    return forStmt;
}

void Parser3::for_clause(Expression*& varExpr, bool& isRefAssign, QList<Expression*>& forList) {
    expect(Tok_FOR, false, "for_clause");
    
    expect(Tok_identifier, false, "for_clause");
    varExpr = new Expression(Expression::Identifier, toRowCol(cur));
    varExpr->a = cur.d_id;
    
    for_right_part(isRefAssign, forList);
    expect(Tok_DO, false, "for_clause");
}

void Parser3::for_right_part(bool& isRefAssign, QList<Expression*>& forList) {
    if (la.d_type == Tok_ColonEq) {
        expect(Tok_ColonEq, false, "for_right_part");
        isRefAssign = false;
        value_for_list(forList);
    } else if (la.d_type == Tok_ColonMinus) {
        expect(Tok_ColonMinus, false, "for_right_part");
        isRefAssign = true;
        object_for_list(forList);
    } else {
        invalid("for_right_part");
    }
}

void Parser3::value_for_list(QList<Expression*>& forList) {
    Expression* elem = value_for_list_element();
    if (elem)
        forList.append(elem);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "value_for_list");
        elem = value_for_list_element();
        if (elem)
            forList.append(elem);
    }
}

void Parser3::object_for_list(QList<Expression*>& forList) {
    Expression* elem = object_for_list_element();
    if (elem) forList.append(elem);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "object_for_list");
        elem = object_for_list_element();
        if (elem) forList.append(elem);
    }
}

Expression* Parser3::value_for_list_element() {
    Expression* expr = expression();
    
    if (la.d_type == Tok_STEP) {
        // syntax: expression STEP expression UNTIL expression
        expect(Tok_STEP, false, "value_for_list_element");
        Expression* step = expression();
        expect(Tok_UNTIL, false, "value_for_list_element");
        Expression* until = expression();
        // Create a compound expression for STEP...UNTIL
        if (expr) {
            Expression* stepUntil = new Expression(Expression::StepUntil);
            stepUntil->lhs = expr; // start
            stepUntil->rhs = step; // step
            stepUntil->condition = until;
            expr = stepUntil;
        }else
        {
            delete step;
            delete until;
        }
    } else if (la.d_type == Tok_WHILE) {
        expect(Tok_WHILE, false, "value_for_list_element");
        Expression* whileCond = expression();
        if (expr) {
            Expression* whileLoop = new Expression(Expression::WhileLoop);
            whileLoop->lhs = expr;
            whileLoop->condition = whileCond;
            expr = whileLoop;
        }else
            delete whileCond;
    }
    
    return expr;
}

Expression* Parser3::object_for_list_element() {
    Expression* expr = expression();
    
    if (la.d_type == Tok_WHILE) {
        expect(Tok_WHILE, false, "object_for_list_element");
        Expression* whileCond = expression();
        if (expr) {
            Expression* whileLoop = new Expression(Expression::WhileLoop);
            whileLoop->lhs = expr;
            whileLoop->condition = whileCond;
            expr = whileLoop;
        }
    }
    
    return expr;
}

Statement* Parser3::go_to_statement() {
    if (la.d_type == Tok_GOTO) {
        expect(Tok_GOTO, false, "go_to_statement");
    } else if (la.d_type == Tok_GO) {
        expect(Tok_GO, false, "go_to_statement");
        expect(Tok_TO, false, "go_to_statement");
    } else {
        invalid("go_to_statement");
        return 0;
    }
    
    RowCol pos = toRowCol(cur);
    Expression* target = expression();
    
    Statement* stmt = new Statement(Statement::Goto, pos);
    stmt->lhs = target;
    return stmt;
}

Statement* Parser3::unlabelled_basic_statement() {
    if (FIRST_go_to_statement(la.d_type)) {
        return go_to_statement();
    } else if (FIRST_activation_statement(la.d_type)) {
        return activation_statement();
    } else if (FIRST_connection_statement(la.d_type)) {
        return connection_statement();
    } else if (FIRST_main_block(la.d_type)) {
        QList<Expression*> args;
        Statement* res = main_block(0, args);
        return res;
    } else if (FIRST_primary(la.d_type)) {
        RowCol pos = toRowCol(la);
        Expression* prim = primary();
        
        // Check for prefixed block: identifier BEGIN or identifier(args) BEGIN
        if (FIRST_main_block(la.d_type) ) {
            Atom prefixName;
            QList<Expression*> args;
            
            if (prim->kind == Expression::Identifier) {
                // Simple prefix: identifier BEGIN
                prefixName = prim->a;
            } else if (prim->kind == Expression::Call && prim->lhs && 
                       prim->lhs->kind == Expression::Identifier) {
                // Prefix with parameters: identifier(args) BEGIN
                prefixName = prim->lhs->a;
                // Collect args from the call expression
                Expression* arg = prim->rhs;
                while (arg) {
                    Expression* nextArg = arg->next;
                    arg->next = 0; // Detach from chain
                    args.append(arg);
                    arg = nextArg;
                }
                prim->rhs = 0; // Prevent deletion of args
            }
            
            if (prefixName != 0) {
                delete prim;
                return main_block(prefixName, args);
            }else
            {
                qDeleteAll(args);
                delete prim;
                invalid("block prefix");
                return 0;
            }
        }else if (la.d_type == Tok_ColonEq) {
            // Value assignment
            expect(Tok_ColonEq, false, "unlabelled_basic_statement");
            Expression* rhs = expression();
            
            // Handle multiple assignments (SIM75+)
            while (la.d_type == Tok_ColonEq) {
                if (!versionCheck(Sim75, "multiple assignment")) {
                    break;
                }
                expect(Tok_ColonEq, false, "unlabelled_basic_statement");
                Expression* next = expression();
                // Chain assignments
                Expression* assign = new Expression(Expression::AssignVal, pos);
                assign->lhs = rhs;
                assign->rhs = next;
                rhs = assign;
            }
            
            Statement* stmt = new Statement(Statement::Assign, pos);
            stmt->rhs = new Expression(Expression::AssignVal, pos);
            stmt->rhs->lhs = prim;
            stmt->rhs->rhs = rhs;
            return stmt;
        } else if (la.d_type == Tok_ColonMinus) {
            // Reference assignment
            expect(Tok_ColonMinus, false, "unlabelled_basic_statement");
            Expression* rhs = expression();
            
            // Handle multiple assignments (SIM75+)
            while (la.d_type == Tok_ColonMinus) {
                if (!versionCheck(Sim75, "multiple assignment")) {
                    break;
                }
                expect(Tok_ColonMinus, false, "unlabelled_basic_statement");
                Expression* next = expression();
                Expression* assign = new Expression(Expression::AssignRef, pos);
                assign->lhs = rhs;
                assign->rhs = next;
                rhs = assign;
            }
            
            Statement* stmt = new Statement(Statement::Assign, pos);
            stmt->rhs = new Expression(Expression::AssignRef, pos);
            stmt->rhs->lhs = prim;
            stmt->rhs->rhs = rhs;
            return stmt;
        } else {
            // Procedure call or just an expression statement
            Statement* stmt = new Statement(Statement::Call, pos);
            stmt->rhs = prim;
            return stmt;
        }
    } else {
        invalid("unlabelled_basic_statement");
        return 0;
    }
}

Connection* Parser3::when_clause() {
    expect(Tok_WHEN, false, "when_clause");
    RowCol pos = toRowCol(cur);
    
    Token className = class_identifier();
    expect(Tok_DO, false, "when_clause");
    Statement* body = statement();
    
    Connection* conn = new Connection();
    conn->pos = pos;
    conn->className = className.d_id;
    conn->body = body;
    conn->next = 0;
    return conn;
}

Statement* Parser3::otherwise_clause() {
    expect(Tok_OTHERWISE, false, "otherwise_clause");
    return statement();
}

Statement* Parser3::connection_part() {
    Connection* first = when_clause();
    Connection* last = first;
    
    while (FIRST_when_clause(la.d_type)) {
        Connection* conn = when_clause();
        last->next = conn;
        last = conn;
    }
    
    // Return as a statement with connections
    Statement* stmt = new Statement(Statement::Inspect, first->pos);
    stmt->conn = first;
    return stmt;
}

Statement* Parser3::connection_statement() {
    expect(Tok_INSPECT, false, "connection_statement");
    RowCol pos = toRowCol(cur);
    
    Expression* obj = expression();
    
    Statement* stmt = new Statement(Statement::Inspect, pos);
    stmt->obj = obj;
    
    if (FIRST_connection_part(la.d_type)) {
        // WHEN clauses
        Connection* first = when_clause();
        Connection* last = first;
        while (FIRST_when_clause(la.d_type)) {
            Connection* conn = when_clause();
            last->next = conn;
            last = conn;
        }
        // Store connections chain directly
        stmt->conn = first;
    } else if (la.d_type == Tok_DO) {
        expect(Tok_DO, false, "connection_statement");
        stmt->body = statement();
    }
    
    if (FIRST_otherwise_clause(la.d_type)) {
        stmt->otherwise = otherwise_clause();
    }
    
    return stmt;
}

bool Parser3::activator() {
    if (la.d_type == Tok_ACTIVATE) {
        expect(Tok_ACTIVATE, false, "activator");
        return false;
    } else if (la.d_type == Tok_REACTIVATE) {
        expect(Tok_REACTIVATE, false, "activator");
        return true;
    }
    invalid("activator");
    return false;
}

Statement* Parser3::activation_statement() {
    bool isReactivate = activator();
    RowCol pos = toRowCol(cur);
    
    Expression* obj = expression();
    
    Statement* stmt = new Statement(Statement::Activate, pos);
    stmt->activate = new ActivateData();
    stmt->activate->obj = obj;
    stmt->re = isReactivate;
    
    if (FIRST_scheduling_clause(la.d_type)) {
        scheduling_clause(stmt);
    }
    
    return stmt;
}

void Parser3::simple_timing_clause(Statement* stmt) {
    Q_ASSERT( stmt->kind == Statement::Activate && stmt->activate );
    if (la.d_type == Tok_AT) {
        expect(Tok_AT, false, "simple_timing_clause");
        stmt->activate->at = expression();
    } else if (la.d_type == Tok_DELAY) {
        expect(Tok_DELAY, false, "simple_timing_clause");
        stmt->activate->delay = expression();
    } else {
        invalid("simple_timing_clause");
    }
}

void Parser3::timing_clause(Statement* stmt) {
    simple_timing_clause(stmt);
    if (la.d_type == Tok_PRIOR) {
        expect(Tok_PRIOR, false, "timing_clause");
        stmt->prior = true;
    }
}

void Parser3::scheduling_clause(Statement* stmt) {
    Q_ASSERT( stmt->kind == Statement::Activate && stmt->activate );
    if (FIRST_timing_clause(la.d_type)) {
        timing_clause(stmt);
    } else if (la.d_type == Tok_BEFORE) {
        expect(Tok_BEFORE, false, "scheduling_clause");
        stmt->activate->priorObj = expression();
    } else if (la.d_type == Tok_AFTER) {
        expect(Tok_AFTER, false, "scheduling_clause");
        stmt->activate->priorObj = expression();
    } else {
        invalid("scheduling_clause");
    }
}

Type* Parser3::specifier(bool& isArray, bool& isProcedure) {
    isArray = false;
    isProcedure = false;
    
    if (la.d_type == Tok_SWITCH) {
        expect(Tok_SWITCH, false, "specifier");
        return new Type(Type::Switch);
    } else if (la.d_type == Tok_LABEL) {
        expect(Tok_LABEL, false, "specifier");
        return mdl->getType(Type::Label);
    } else if (((peek(1).d_type == Tok_PROCEDURE || peek(2).d_type == Tok_PROCEDURE ||
                 peek(1).d_type == Tok_REF && peek(5).d_type == Tok_PROCEDURE ||
                 peek(1).d_type == Tok_ARRAY || peek(2).d_type == Tok_ARRAY ||
                 peek(1).d_type == Tok_REF && peek(5).d_type == Tok_ARRAY))) {
        Type* t = 0;
        if (FIRST_type(la.d_type)) {
            t = type();
        }
        if (la.d_type == Tok_ARRAY) {
            expect(Tok_ARRAY, false, "specifier");
            isArray = true;
        } else if (la.d_type == Tok_PROCEDURE) {
            expect(Tok_PROCEDURE, false, "specifier");
            isProcedure = true;
        } else {
            invalid("specifier");
        }
        return t;
    } else if (FIRST_type(la.d_type)) {
        return type();
    } else {
        invalid("specifier");
        return 0;
    }
}

void Parser3::specification_part(Declaration* parent) {
    bool isArray = false;
    bool isProcedure = false;
    Type* type = specifier(isArray, isProcedure);
    
    QList<Token> ids = identifier_list();
    expect(Tok_Semi, false, "specification_part");

    // Update parameter types
    bool typeUsed = false;
    for (int i = 0; i < ids.size(); i++) {
        Declaration* param = parent->link;
        while (param) {
            // TODO: check if found, report otherwise
            if (param->name.constData() == ids[i].d_val.constData()) {
                if (isArray) {
                    Type* arrType = new Type(Type::Array);
                    arrType->setType(type);
                    typeUsed = true;
                    param->setType(arrType);
                    param->kind = Declaration::Array;
                } else if (isProcedure) {
                    Type* procType = new Type(Type::Procedure);
                    procType->setType(type);
                    param->setType(procType);
                    typeUsed = true;
                } else {
                    param->setType(type);
                    typeUsed = true;
                }
                break;
            }
            param = param->next;
        }
    }
    if( type && !typeUsed && !type->owned )
        delete type;
}

void Parser3::procedure_specification(Declaration* virtSpec) {
    expect(Tok_IS, false, "procedure_specification");
    procedure_declaration();
    // TODO Link the procedure to the virtual spec
}

Declaration* Parser3::external_item() {
    Token localName;
    if ((peek(1).d_type == Tok_identifier && peek(2).d_type == Tok_Eq)) {
        expect(Tok_identifier, false, "external_item");
        localName = cur;
        expect(Tok_Eq, false, "external_item");
    }  
    Token extName = external_identifier();
    Token name = localName.d_type == Tok_identifier ? localName : extName;
    if(name.d_type != Tok_identifier)
        error(name, "external identifier expected");
    Declaration* extDecl = mdl->addDecl( name.d_id, name.d_val, Declaration::ExternalProc);
    extDecl->pos = toRowCol(name);
    extDecl->nameRef = extName.d_id;
    return extDecl;
}

QList<Declaration*> Parser3::external_list() {
    QList<Declaration*> res;
    res << external_item();
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "external_list");
        res << external_item();
    }
    return res;
}

void Parser3::external_declaration() {
    expect(Tok_EXTERNAL, false, "external_declaration");
        
    if (la.d_type == Tok_identifier || FIRST_type(la.d_type) || la.d_type == Tok_PROCEDURE) {
        // SIM86: optional kind identifier
        Atom kind = 0;
        if (la.d_type == Tok_identifier) {
            if (versionCheck(Sim86, "external kind")) {
                expect(Tok_identifier, false, "external_declaration");
                kind = cur.d_id;
            }
        }
        
        Type* t = 0;
        if (FIRST_type(la.d_type)) {
            t = type();
        }
        
        expect(Tok_PROCEDURE, false, "external_declaration");

        if( kind != 0 && t == 0 )
        {
            Declaration* proc = external_item();
            // TODO: this is ambiguous in the standard.
            if (FIRST_procedure_specification(la.d_type)) {
                if (versionCheck(Sim86, "IS procedure_specification")) {
                    procedure_specification(0); // TODO kind
                }
            }
        }else
        {
            QList<Declaration*> list = external_list();
            foreach(Declaration* proc, list)
            {
                proc->setType(t);
                // TODO kind
            }
        }

    } else if (la.d_type == Tok_CLASS) {
        expect(Tok_CLASS, false, "external_declaration");
        QList<Declaration*> list = external_list();
        foreach(Declaration* cls, list)
            cls->kind = Declaration::ExternalClass;
    } else {
        invalid("external_declaration");
    }
}

Token Parser3::external_identifier() {
    if (la.d_type == Tok_identifier) {
        expect(Tok_identifier, false, "external_identifier");
        return cur;
    } else if (la.d_type == Tok_string) {
        if (!versionCheck(Sim86, "string external identifier")) {
            // Still parse it
        }
        expect(Tok_string, false, "external_identifier");
        return cur;
    } else {
        invalid("external_identifier");
        return Token();
    }
}

Declaration* Parser3::switch_declaration() {
    expect(Tok_SWITCH, false, "switch_declaration");
    RowCol pos = toRowCol(cur);
    
    expect(Tok_identifier, false, "switch_declaration");
    const Token name = cur;
    
    expect(Tok_ColonEq, false, "switch_declaration");
    
    Declaration* switchDecl = mdl->addDecl(name.d_id, name.d_val,Declaration::Switch);
    switchDecl->pos = toRowCol(name);
    switchDecl->setType(new Type(Type::Switch)); // TODO: does this make any sense?
    
    switch_list(switchDecl);
    
    return switchDecl;
}

void Parser3::switch_list(Declaration* switchDecl) {
    Expression* first = expression();
    Expression* last = first;
    
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "switch_list");
        Expression* e = expression();
        if (last) {
            last->next = e;
            last = e;
        } else {
            first = e;
            last = e;
        }
    }
    
    switchDecl->list = first;
}

void Parser3::type_list_element(Type* t) {
    expect(Tok_identifier, false, "type_list_element");
    const Token name = cur;
    
    Declaration* varDecl = mdl->addDecl(name.d_id, name.d_val,Declaration::Variable);
    varDecl->pos = toRowCol(name);
    varDecl->setType(t);
    
    if (la.d_type == Tok_Eq) {
        // SIM86 feature: initializer
        if (versionCheck(Sim86, "variable initializer")) {
            expect(Tok_Eq, false, "type_list_element");
            varDecl->init = expression();
        }
    }
}

void Parser3::type_list(Type* t) {
    type_list_element(t);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "type_list");
        type_list_element(t);
    }
}

void Parser3::type_declaration() {
    Type* t = type();
    type_list(t);
}

void Parser3::array_list(Type* elemType) {
    array_segment(elemType);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "array_list");
        array_segment(elemType);
    }
}

void Parser3::array_segment(Type* elemType) {
    QList<Token> names;
    
    expect(Tok_identifier, false, "array_segment");
    names.append(cur);
    
    // Collect all identifiers that share the same bounds
    // Grammar: array_segment ::= identifier { ',' identifier } bounds
    // Continue while we see ", identifier" followed by comma, '[', or '('
    // (bounds come at the end, shared by all identifiers)
    while (la.d_type == Tok_Comma && peek(2).d_type == Tok_identifier) {
        int tt3 = peek(3).d_type;
        // If identifier is followed by comma, '[', or '(' it's part of this segment
        if (tt3 == Tok_Comma || tt3 == Tok_Lbrack || tt3 == Tok_Lpar) {
            expect(Tok_Comma, false, "array_segment");
            expect(Tok_identifier, false, "array_segment");
            names.append(cur);
        } else {
            // Something else follows - not part of this segment
            break;
        }
    }
    
    QList<Expression*> bounds;
    
    if (la.d_type == Tok_Lbrack) {
        expect(Tok_Lbrack, false, "array_segment");
        bound_pair_list(bounds);
        expect(Tok_Rbrack, false, "array_segment");
    } else if (la.d_type == Tok_Lpar) {
        // SIM75+ feature
        if (!versionCheck(Sim75, "() array bounds")) {
            // Still parse it
        }
        expect(Tok_Lpar, false, "array_segment");
        bound_pair_list(bounds);
        expect(Tok_Rpar, false, "array_segment");
    } else {
        invalid("array_segment");
    }
    
    // Create array declarations for each name
    for (int i = 0; i < names.size(); i++) {
        Declaration* arrDecl = mdl->addDecl(names[i].d_id, names[i].d_val, Declaration::Array);
        arrDecl->pos = toRowCol(names[i]);
        
        Type* arrType = new Type(Type::Array);
        arrType->setType(elemType);
        // Store bounds in the type
        if (!bounds.isEmpty()) {
            arrType->setExpr(bounds.first()); // First bound pair's lower bound
            Expression* e = arrType->getExpr();
            for( int i = 1; i < bounds.size(); i++ )
            {
                e->next = bounds[i];
                e = e->next;
            }
        }
        arrDecl->setType(arrType);
    }
}

void Parser3::bound_pair_list(QList<Expression*>& bounds) {
    bound_pair(bounds);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "bound_pair_list");
        bound_pair(bounds);
    }
}

void Parser3::bound_pair(QList<Expression*>& bounds) {
    Expression* lower = expression();
    expect(Tok_Colon, false, "bound_pair");
    Expression* upper = expression();
    
    bounds.append(lower);
    bounds.append(upper);
}

Declaration* Parser3::array_declaration() {
    Type* elemType = 0;
    if (FIRST_type(la.d_type)) {
        elemType = type();
    }
    
    expect(Tok_ARRAY, false, "array_declaration");
    array_list(elemType);
    
    return 0; // Arrays are added directly to scope
}

Type* Parser3::type() {
    if (FIRST_value_type(la.d_type)) {
        return value_type();
    } else if (FIRST_reference_type(la.d_type)) {
        return reference_type();
    } else {
        invalid("type");
        return 0;
    }
}

Type* Parser3::value_type() {
    if (la.d_type == Tok_INTEGER) {
        expect(Tok_INTEGER, false, "value_type");
        return mdl->getType(Type::Integer);
    } else if (la.d_type == Tok_REAL) {
        expect(Tok_REAL, false, "value_type");
        return mdl->getType(Type::Real);
    } else if (la.d_type == Tok_SHORT) {
        expect(Tok_SHORT, false, "value_type");
        if (!versionCheck(Sim75, "SHORT INTEGER")) {
            // Still parse it
        }
        expect(Tok_INTEGER, false, "value_type");
        return mdl->getType(Type::ShortInteger);
    } else if (la.d_type == Tok_LONG) {
        expect(Tok_LONG, false, "value_type");
        if (!versionCheck(Sim75, "LONG REAL")) {
            // Still parse it
        }
        expect(Tok_REAL, false, "value_type");
        return mdl->getType(Type::LongReal);
    } else if (la.d_type == Tok_BOOLEAN) {
        expect(Tok_BOOLEAN, false, "value_type");
        return mdl->getType(Type::Boolean);
    } else if (la.d_type == Tok_CHARACTER) {
        expect(Tok_CHARACTER, false, "value_type");
        return mdl->getType(Type::Character);
    } else {
        invalid("value_type");
        return 0;
    }
}

Type* Parser3::reference_type() {
    if (FIRST_object_reference(la.d_type)) {
        return object_reference();
    } else if (la.d_type == Tok_TEXT) {
        expect(Tok_TEXT, false, "reference_type");
        return mdl->getType(Type::Text);
    } else {
        invalid("reference_type");
        return 0;
    }
}

Type* Parser3::object_reference() {
    expect(Tok_REF, false, "object_reference");
    expect(Tok_Lpar, false, "object_reference");
    Token qual = qualification();
    expect(Tok_Rpar, false, "object_reference");
    
    Type* refType = new Type(Type::Ref);
    refType->setExpr(new Expression(Expression::TypeRef));
    refType->getExpr()->a = qual.d_id;
    return refType;
}

Token Parser3::qualification() {
    return class_identifier();
}

Token Parser3::label() {
    expect(Tok_identifier, false, "label");
    return cur;
}

Expression* Parser3::if_clause() {
    expect(Tok_IF, false, "if_clause");
    Expression* cond = expression();
    expect(Tok_THEN, false, "if_clause");
    return cond;
}

Expression* Parser3::local_object() {
    expect(Tok_THIS, false, "local_object");
    RowCol pos = toRowCol(cur);
    Token className = class_identifier();
    
    Expression* expr = new Expression(Expression::This, pos);
    expr->a = className.d_id;
    return expr;
}

Expression* Parser3::object_generator() {
    expect(Tok_NEW, false, "object_generator");
    RowCol pos = toRowCol(cur);
    Token className = class_identifier();
    
    Expression* expr = new Expression(Expression::New, pos);
    expr->a = className.d_id;
    return expr;
}

void Parser3::actual_parameter_list(QList<Expression*>& args) {
    Expression* arg = actual_parameter();
    if (arg)
        args.append(arg);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "actual_parameter_list");
        arg = actual_parameter();
        if (arg)
            args.append(arg);
    }
}

Expression* Parser3::actual_parameter() {
    return expression();
}

// Expression parsing

Expression* Parser3::expression() {
    if (FIRST_quaternary_(la.d_type)) {
        return quaternary_();
    } else if (FIRST_if_clause(la.d_type)) {
        Expression* cond = if_clause();
        Expression* thenExpr = quaternary_();
        expect(Tok_ELSE, false, "expression");
        Expression* elseExpr = expression();
        
        Expression* ifExpr = new Expression(Expression::IfExpr, cond ? cond->pos : toRowCol(la));
        ifExpr->condition = cond;
        ifExpr->lhs = thenExpr;
        ifExpr->rhs = elseExpr;
        return ifExpr;
    } else {
        invalid("expression");
        return 0;
    }
}

Expression* Parser3::quaternary_() {
    Expression* left = tertiary_();
    while (la.d_type == Tok_OR_ELSE) {
        expect(Tok_OR_ELSE, false, "quaternary_");
        if (!versionCheck(Sim86, "OR ELSE")) {
            // Still parse it
        }
        Expression* right = tertiary_();
        Expression* op = new Expression(Expression::Or, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression* Parser3::tertiary_() {
    Expression* left = equivalence_();
    while (la.d_type == Tok_AND_THEN) {
        expect(Tok_AND_THEN, false, "tertiary_");
        if (!versionCheck(Sim86, "AND THEN")) {
            // Still parse it
        }
        Expression* right = equivalence_();
        Expression* op = new Expression(Expression::And, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression* Parser3::equivalence_() {
    Expression* left = implication();
    while (FIRST_equiv_sym_(la.d_type)) {
        Expression::Kind kind = equiv_sym_();
        Expression* right = implication();
        Expression* op = new Expression(kind, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression::Kind Parser3::equiv_sym_() {
    if (la.d_type == Tok_EQUIV) {
        expect(Tok_EQUIV, false, "equiv_sym_");
    } else if (la.d_type == Tok_Ueq) {
        expect(Tok_Ueq, false, "equiv_sym_");
    } else if (la.d_type == Tok_2Eq) {
        expect(Tok_2Eq, false, "equiv_sym_");
    } else if (la.d_type == Tok_EQV) {
        expect(Tok_EQV, false, "equiv_sym_");
    } else {
        invalid("equiv_sym_");
    }
    return Expression::Eqv;
}

Expression* Parser3::implication() {
    Expression* left = simple_expression_();
    while (FIRST_impl_sym_(la.d_type)) {
        Expression::Kind kind = impl_sym_();
        Expression* right = simple_expression_();
        Expression* op = new Expression(kind, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression::Kind Parser3::impl_sym_() {
    if (la.d_type == Tok_IMPL) {
        expect(Tok_IMPL, false, "impl_sym_");
    } else if (la.d_type == Tok_Uimpl) {
        expect(Tok_Uimpl, false, "impl_sym_");
    } else if (la.d_type == Tok_MinusGt) {
        expect(Tok_MinusGt, false, "impl_sym_");
    } else if (la.d_type == Tok_IMP) {
        expect(Tok_IMP, false, "impl_sym_");
    } else {
        invalid("impl_sym_");
    }
    return Expression::Imp;
}

Expression* Parser3::simple_expression_() {
    Expression* left = term();
    while (FIRST_adding_operator(la.d_type) || FIRST_or_sym_(la.d_type)) {
        Expression::Kind kind;
        if (FIRST_adding_operator(la.d_type)) {
            kind = adding_operator();
        } else {
            kind = or_sym_();
        }
        Expression* right = term();
        Expression* op = new Expression(kind, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression::Kind Parser3::adding_operator() {
    if (la.d_type == Tok_Plus) {
        expect(Tok_Plus, false, "adding_operator");
        return Expression::Plus;
    } else if (la.d_type == Tok_Minus) {
        expect(Tok_Minus, false, "adding_operator");
        return Expression::Minus;
    } else {
        invalid("adding_operator");
        return Expression::Invalid;
    }
}

Expression::Kind Parser3::or_sym_() {
    if (la.d_type == Tok_OR) {
        expect(Tok_OR, false, "or_sym_");
    } else if (la.d_type == Tok_Uor) {
        expect(Tok_Uor, false, "or_sym_");
    } else if (la.d_type == Tok_Bar) {
        expect(Tok_Bar, false, "or_sym_");
    } else {
        invalid("or_sym_");
    }
    return Expression::Or;
}

Expression* Parser3::term() {
    Expression* left = factor();
    while (FIRST_multiplying_operator(la.d_type) || FIRST_and_sym_(la.d_type)) {
        Expression::Kind kind;
        if (FIRST_multiplying_operator(la.d_type)) {
            kind = multiplying_operator();
        } else {
            kind = and_sym_();
        }
        Expression* right = factor();
        Expression* op = new Expression(kind, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression::Kind Parser3::multiplying_operator() {
    if (la.d_type == Tok_Star) {
        expect(Tok_Star, false, "multiplying_operator");
        return Expression::Mul;
    } else if (la.d_type == Tok_Slash) {
        expect(Tok_Slash, false, "multiplying_operator");
        return Expression::Div;
    } else if (la.d_type == Tok_Percent) {
        expect(Tok_Percent, false, "multiplying_operator");
        return Expression::IntDiv;
    } else if (la.d_type == Tok_Udiv) {
        expect(Tok_Udiv, false, "multiplying_operator");
        return Expression::Div;
    } else if (la.d_type == Tok_Umul) {
        expect(Tok_Umul, false, "multiplying_operator");
        return Expression::Mul;
    } else if (la.d_type == Tok_2Slash) {
        expect(Tok_2Slash, false, "multiplying_operator");
        return Expression::IntDiv;
    } else {
        invalid("multiplying_operator");
        return Expression::Invalid;
    }
}

Expression::Kind Parser3::and_sym_() {
    if (la.d_type == Tok_AND) {
        expect(Tok_AND, false, "and_sym_");
    } else if (la.d_type == Tok_Uand) {
        expect(Tok_Uand, false, "and_sym_");
    } else if (la.d_type == Tok_Amp) {
        expect(Tok_Amp, false, "and_sym_");
    } else {
        invalid("and_sym_");
    }
    return Expression::And;
}

Expression* Parser3::factor() {
    Expression* left = secondary();
    while (FIRST_power_sym_(la.d_type)) {
        Expression::Kind kind = power_sym_();
        Expression* right = secondary();
        Expression* op = new Expression(kind, left ? left->pos : toRowCol(cur));
        op->lhs = left;
        op->rhs = right;
        left = op;
    }
    return left;
}

Expression::Kind Parser3::power_sym_() {
    if (la.d_type == Tok_POWER) {
        expect(Tok_POWER, false, "power_sym_");
    } else if (la.d_type == Tok_Uexp) {
        expect(Tok_Uexp, false, "power_sym_");
    } else if (la.d_type == Tok_Hat) {
        expect(Tok_Hat, false, "power_sym_");
    } else if (la.d_type == Tok_2Star) {
        expect(Tok_2Star, false, "power_sym_");
    } else {
        invalid("power_sym_");
    }
    return Expression::Exp;
}

Expression* Parser3::secondary() {
    Expression::Kind notKind = Expression::Invalid;
    Expression::Kind signKind = Expression::Invalid;
    RowCol pos = toRowCol(la);
    
    if (FIRST_not_sym_(la.d_type)) {
        notKind = not_sym_();
    }
    if (FIRST_adding_operator(la.d_type)) {
        signKind = adding_operator();
    }
    
    Expression* prim = primary();
    
    // Apply unary operators
    if (signKind == Expression::Minus && prim) {
        Expression* neg = new Expression(Expression::Minus, pos);
        neg->rhs = prim;
        prim = neg;
    }
    
    if (notKind == Expression::Not && prim) {
        Expression* notExpr = new Expression(Expression::Not, pos);
        notExpr->rhs = prim;
        prim = notExpr;
    }
    
    return prim;
}

Expression::Kind Parser3::not_sym_() {
    if (la.d_type == Tok_NOT) {
        expect(Tok_NOT, false, "not_sym_");
    } else if (la.d_type == Tok_Unot) {
        expect(Tok_Unot, false, "not_sym_");
    } else if (la.d_type == Tok_Bang) {
        expect(Tok_Bang, false, "not_sym_");
    } else {
        invalid("not_sym_");
    }
    return Expression::Not;
}

Expression* Parser3::primary() {
    Expression* result = 0;
    
    if (FIRST_unsigned_number(la.d_type)) {
        result = unsigned_number();
    } else if (FIRST_logical_value(la.d_type)) {
        result = logical_value();
    } else if (la.d_type == Tok_character) {
        expect(Tok_character, false, "primary");
        result = new Expression(Expression::CharConst, toRowCol(cur));
        result->u = QString::fromUtf8(cur.d_val)[0].unicode();
    } else if (FIRST_string_(la.d_type)) {
        result = string_();
    } else if (la.d_type == Tok_NOTEXT) {
        expect(Tok_NOTEXT, false, "primary");
        result = new Expression(Expression::Notext, toRowCol(cur));
        result->a = 0;
    } else if (la.d_type == Tok_NONE) {
        expect(Tok_NONE, false, "primary");
        result = new Expression(Expression::None, toRowCol(cur));
        result->a = 0;
    } else if (FIRST_local_object(la.d_type)) {
        result = local_object();
    } else if (FIRST_object_generator(la.d_type)) {
        result = object_generator();
        // Check for actual parameters
        if (FIRST_actual_parameter_part(la.d_type)) {
            QList<Expression*> args;
            actual_parameter_part(args);
            // Convert to Call expression
            Expression* call = new Expression(Expression::Call, result->pos);
            call->lhs = result;
            // Store args
            if (!args.isEmpty()) {
                call->rhs = args[0];
                Expression* e = call->rhs;
                for (int i = 1; i < args.size(); i++) {
                    e->next = args[i];
                    e = e->next;
                }
            }
            result = call;
        }
    } else if (la.d_type == Tok_Lpar) {
        expect(Tok_Lpar, false, "primary");
        result = expression();
        expect(Tok_Rpar, false, "primary");
    } else if (la.d_type == Tok_identifier) {
        expect(Tok_identifier, false, "primary");
        result = new Expression(Expression::Identifier, toRowCol(cur));
        result->a = cur.d_id;
    } else {
        invalid("primary");
        return 0;
    }
    
    return primary_nlr_(result);
}

Expression* Parser3::primary_nlr_(Expression* lhs) {
    if (FIRST_relation_(la.d_type) || FIRST_selector_(la.d_type) || FIRST_qualified_(la.d_type)) {
        Expression* result = 0;
        if (FIRST_relation_(la.d_type)) {
            result = relation_(lhs);
        } else if (FIRST_selector_(la.d_type)) {
            result = selector_(lhs);
        } else if (FIRST_qualified_(la.d_type)) {
            result = qualified_(lhs);
        }
        return primary_nlr_(result);
    }
    return lhs;
}

Expression* Parser3::relation_(Expression* lhs) {
    Expression::Kind kind = relational_operator();
    Expression* rhs = simple_expression_();
    
    Expression* rel = new Expression(kind, lhs ? lhs->pos : toRowCol(cur));
    rel->lhs = lhs;
    rel->rhs = rhs;
    return rel;
}

Expression* Parser3::qualified_(Expression* lhs) {
    expect(Tok_QUA, false, "qualified_");
    RowCol pos = toRowCol(cur);
    Token className = class_identifier();
    
    Expression* qua = new Expression(Expression::Qua, pos);
    qua->lhs = lhs;
    qua->a = className.d_id;
    return qua;
}

Expression* Parser3::selector_(Expression* lhs) {
    if (la.d_type == Tok_Dot) {
        expect(Tok_Dot, false, "selector_");
        RowCol pos = toRowCol(cur);
        Token attr = attribute_identifier();
        
        Expression* dot = new Expression(Expression::Dot, pos);
        dot->lhs = lhs;
        dot->a = attr.d_id;
        return dot;
    } else if (la.d_type == Tok_Lbrack) {
        expect(Tok_Lbrack, false, "selector_");
        RowCol pos = toRowCol(cur);
        QList<Expression*> subs;
        subscript_list(subs);
        expect(Tok_Rbrack, false, "selector_");
        
        Expression* subscript = new Expression(Expression::Subscript, pos);
        subscript->lhs = lhs;
        if (!subs.isEmpty()) {
            subscript->rhs = subs[0];
            for (int i = 1; i < subs.size(); i++) {
                Expression::append(subscript->rhs, subs[i]);
            }
        }
        return subscript;
    } else if (la.d_type == Tok_Lpar) {
        // Function/procedure call
        RowCol pos = toRowCol(la);
        QList<Expression*> args;
        actual_parameter_part(args);
        
        Expression* call = new Expression(Expression::Call, pos);
        call->lhs = lhs;
        if (!args.isEmpty()) {
            call->rhs = args[0];
            Expression* e = call->rhs;
            for (int i = 1; i < args.size(); i++) {
                e->next = args[i];
                e = e->next;
            }
        }
        return call;
    } else {
        invalid("selector_");
        return lhs;
    }
}

Expression::Kind Parser3::relational_operator() {
    Expression::Kind kind = Expression::Invalid;
    
    if (la.d_type == Tok_Lt || la.d_type == Tok_LESS || la.d_type == Tok_LT) {
        next();
        kind = Expression::Lt;
    } else if (la.d_type == Tok_Leq || la.d_type == Tok_Uleq || la.d_type == Tok_NOTGREATER || la.d_type == Tok_LE) {
        next();
        kind = Expression::Leq;
    } else if (la.d_type == Tok_Eq || la.d_type == Tok_EQUAL || la.d_type == Tok_EQ) {
        next();
        kind = Expression::Eq;
    } else if (la.d_type == Tok_Geq || la.d_type == Tok_Ugeq || la.d_type == Tok_NOTLESS || la.d_type == Tok_GE) {
        next();
        kind = Expression::Geq;
    } else if (la.d_type == Tok_Gt || la.d_type == Tok_GREATER || la.d_type == Tok_GT) {
        next();
        kind = Expression::Gt;
    } else if (la.d_type == Tok_LtGt || la.d_type == Tok_Uneq || la.d_type == Tok_NOTEQUAL || 
               la.d_type == Tok_NE || la.d_type == Tok_BangEq || la.d_type == Tok_HatEq) {
        next();
        kind = Expression::Neq;
    } else if (la.d_type == Tok_IS) {
        next();
        kind = Expression::Eq; // IS is reference equality
    } else if (la.d_type == Tok_IN) {
        next();
        kind = Expression::Eq; // IN is class membership
    } else if (la.d_type == Tok_2Eq) {
        next();
        kind = Expression::RefEq;
    } else if (la.d_type == Tok_EqSlashEq) {
        next();
        kind = Expression::RefNeq;
    } else {
        invalid("relational_operator");
    }
    
    return kind;
}

Expression* Parser3::logical_value() {
    if (la.d_type == Tok_TRUE) {
        expect(Tok_TRUE, false, "logical_value");
        Expression* expr = new Expression(Expression::BoolConst, toRowCol(cur));
        expr->u = true;
        return expr;
    } else if (la.d_type == Tok_FALSE) {
        expect(Tok_FALSE, false, "logical_value");
        Expression* expr = new Expression(Expression::BoolConst, toRowCol(cur));
        expr->u = false;
        return expr;
    } else {
        invalid("logical_value");
        return 0;
    }
}

Expression* Parser3::unsigned_number() {
    if (la.d_type == Tok_unsigned_integer) {
        expect(Tok_unsigned_integer, false, "unsigned_number");
        Expression* expr = new Expression(Expression::UnsignedConst, toRowCol(cur));
        expr->u = cur.d_val.toULongLong();
        return expr;
    } else if (la.d_type == Tok_decimal_number) {
        expect(Tok_decimal_number, false, "unsigned_number");
        Expression* expr = new Expression(Expression::RealConst, toRowCol(cur));
        expr->r = cur.d_val.toDouble();
        return expr;
    } else {
        invalid("unsigned_number");
        return 0;
    }
}

Token Parser3::class_identifier() {
    expect(Tok_identifier, false, "class_identifier");
    return cur;
}

QList<Token> Parser3::identifier_list() {
    QList<Token> ids;
    expect(Tok_identifier, false, "identifier_list");
    ids.append(cur);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "identifier_list");
        expect(Tok_identifier, false, "identifier_list");
        ids.append(cur);
    }
    return ids;
}

void Parser3::subscript_list(QList<Expression*>& subs) {
    Expression* e = subscript_expression();
    if (e)
        subs.append(e);
    while (la.d_type == Tok_Comma) {
        expect(Tok_Comma, false, "subscript_list");
        e = subscript_expression();
        if (e)
            subs.append(e);
    }
}

Expression* Parser3::subscript_expression() {
    return expression();
}

Token Parser3::attribute_identifier() {
    expect(Tok_identifier, false, "attribute_identifier");
    return cur;
}

Expression* Parser3::string_() {
    expect(Tok_string, false, "string_");
    Expression* expr = new Expression(Expression::StringConst, toRowCol(cur));
    QByteArray val = cur.d_val;
    
    // Handle string concatenation
    while (la.d_type == Tok_string) {
        expect(Tok_string, false, "string_");
        val += cur.d_val;
    }
    
    expr->a = Lexer::toId(val);
    return expr;
}
