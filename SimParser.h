

// This file was automatically generated by Coco/R; don't modify it.
#if !defined(Sim_COCO_PARSER_H__)
#define Sim_COCO_PARSER_H__

#include <QStack>
#include <Simula/SimSynTree.h>


#define PARSER_NS Sim
#include <Simula/SimLexer.h>

namespace Sim {



class Parser {
private:
	enum {
		_EOF=0,
		_T_Literals_=1,
		_T_Bang=2,
		_T_BangEq=3,
		_T_Percent=4,
		_T_Amp=5,
		_T_Lpar=6,
		_T_Rpar=7,
		_T_Star=8,
		_T_2Star=9,
		_T_Plus=10,
		_T_Comma=11,
		_T_Minus=12,
		_T_MinusGt=13,
		_T_Dot=14,
		_T_Slash=15,
		_T_2Slash=16,
		_T_Colon=17,
		_T_ColonMinus=18,
		_T_ColonEq=19,
		_T_Semi=20,
		_T_Lt=21,
		_T_Leq=22,
		_T_LtGt=23,
		_T_Eq=24,
		_T_EqSlashEq=25,
		_T_2Eq=26,
		_T_Gt=27,
		_T_Geq=28,
		_T_Lbrack=29,
		_T_Rbrack=30,
		_T_Hat=31,
		_T_HatEq=32,
		_T_Bar=33,
		_T_Unot=34,
		_T_Umul=35,
		_T_Udiv=36,
		_T_Uexp=37,
		_T_Uand=38,
		_T_Uor=39,
		_T_Uneq=40,
		_T_Ueq=41,
		_T_Uleq=42,
		_T_Ugeq=43,
		_T_Uimpl=44,
		_T_Keywords_=45,
		_T_ACTIVATE=46,
		_T_AFTER=47,
		_T_AND=48,
		_T_AND_THEN=49,
		_T_ARRAY=50,
		_T_AT=51,
		_T_BEFORE=52,
		_T_BEGIN=53,
		_T_BOOLEAN=54,
		_T_CHARACTER=55,
		_T_CLASS=56,
		_T_COMMENT=57,
		_T_DELAY=58,
		_T_DO=59,
		_T_ELSE=60,
		_T_END=61,
		_T_EQ=62,
		_T_EQUAL=63,
		_T_EQUIV=64,
		_T_EQV=65,
		_T_EXTERNAL=66,
		_T_FALSE=67,
		_T_FOR=68,
		_T_GE=69,
		_T_GO=70,
		_T_GOTO=71,
		_T_GREATER=72,
		_T_GT=73,
		_T_HIDDEN=74,
		_T_IF=75,
		_T_IMP=76,
		_T_IMPL=77,
		_T_IN=78,
		_T_INNER=79,
		_T_INSPECT=80,
		_T_INTEGER=81,
		_T_IS=82,
		_T_LABEL=83,
		_T_LE=84,
		_T_LESS=85,
		_T_LONG=86,
		_T_LT=87,
		_T_NAME=88,
		_T_NE=89,
		_T_NEW=90,
		_T_NONE=91,
		_T_NOT=92,
		_T_NOTEQUAL=93,
		_T_NOTEXT=94,
		_T_NOTGREATER=95,
		_T_NOTLESS=96,
		_T_OR=97,
		_T_OR_ELSE=98,
		_T_OTHERWISE=99,
		_T_POWER=100,
		_T_PRIOR=101,
		_T_PROCEDURE=102,
		_T_PROTECTED=103,
		_T_QUA=104,
		_T_REACTIVATE=105,
		_T_REAL=106,
		_T_REF=107,
		_T_SHORT=108,
		_T_STEP=109,
		_T_SWITCH=110,
		_T_TEXT=111,
		_T_THEN=112,
		_T_THIS=113,
		_T_TO=114,
		_T_TRUE=115,
		_T_UNTIL=116,
		_T_VALUE=117,
		_T_VIRTUAL=118,
		_T_WHEN=119,
		_T_WHILE=120,
		_T_is=121,
		_T_Specials_=122,
		_T_decimal_number=123,
		_T_unsigned_integer=124,
		_T_string=125,
		_T_character=126,
		_T_identifier=127,
		_T_Comment=128,
		_T_Eof=129,
		_T_MaxToken_=130
	};
	int maxT;

	int errDist;
	int minErrDist;

	void SynErr(int n, const char* ctx = 0);
	void Get();
	void Expect(int n, const char* ctx = 0);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);
    void SynErr(const QString& sourcePath, int line, int col, int n, PARSER_NS::Errors* err, const char* ctx, const QString& = QString() );

public:
	PARSER_NS::Lexer *scanner;
	PARSER_NS::Errors  *errors;

	PARSER_NS::Token d_cur;
	PARSER_NS::Token d_next;
	QList<PARSER_NS::Token> d_comments;
	struct TokDummy
	{
		int kind;
	};
	TokDummy d_dummy;
	TokDummy *la;			// lookahead token
	
	int peek( quint8 la = 1 );

    void RunParser()
    {
        d_stack.push(&d_root);
        Parse();
        d_stack.pop();
    }
        
Sim::SynTree d_root;
	QStack<Sim::SynTree*> d_stack;
	void addTerminal() {
		Sim::SynTree* n = new Sim::SynTree( d_cur ); d_stack.top()->d_children.append(n);
	}



	Parser(PARSER_NS::Lexer *scanner,PARSER_NS::Errors*);
	~Parser();
	void SemErr(const char* msg);

	void module();
	void external_head();
	void module_body_();
	void class_declaration();
	void procedure_declaration();
	void program();
	void external_declaration();
	void label();
	void block();
	void while_statement();
	void expression();
	void statement();
	void block_prefix();
	void main_block();
	void class_identifier();
	void actual_parameter_part();
	void actual_parameter_list();
	void declaration();
	void compound_tail();
	void switch_declaration();
	void array_declaration();
	void type_declaration();
	void prefix();
	void main_part();
	void formal_parameter_part();
	void value_part();
	void specification_part();
	void protection_part();
	void virtual_part();
	void class_body();
	void protection_specification();
	void identifier_list();
	void virtual_spec();
	void specifier();
	void procedure_specification();
	void type();
	void procedure_heading();
	void procedure_body();
	void procedure_identifier();
	void mode_part();
	void name_part();
	void formal_parameter_list();
	void formal_parameter();
	void parameter_delimiter();
	void Common_Base_statement();
	void unconditional_statement();
	void unlabelled_basic_statement();
	void Common_Base_conditional_statement();
	void for_statement();
	void if_clause();
	void for_clause();
	void simple_variable();
	void for_right_part();
	void value_for_list();
	void object_for_list();
	void value_for_list_element();
	void object_for_list_element();
	void go_to_statement();
	void activation_statement();
	void connection_statement();
	void primary();
	void when_clause();
	void otherwise_clause();
	void connection_part();
	void activator();
	void activation_clause();
	void simple_timing_clause();
	void timing_clause();
	void scheduling_clause();
	void external_item();
	void external_identifier();
	void external_list();
	void switch_identifier();
	void switch_list();
	void variable_identifier();
	void type_list_element();
	void type_list();
	void array_list();
	void array_segment();
	void bound_pair_list();
	void bound_pair();
	void lower_bound();
	void upper_bound();
	void value_type();
	void reference_type();
	void object_reference();
	void qualification();
	void local_object();
	void object_generator();
	void actual_parameter();
	void quaternary_();
	void tertiary_();
	void equivalence_();
	void implication();
	void equiv_sym_();
	void simple_expression_();
	void impl_sym_();
	void term();
	void adding_operator();
	void or_sym_();
	void factor();
	void multiplying_operator();
	void and_sym_();
	void secondary();
	void power_sym_();
	void not_sym_();
	void unsigned_number();
	void primary_nlr_();
	void logical_value();
	void string_();
	void relation_();
	void selector_();
	void qualified_();
	void relational_operator();
	void attribute_identifier();
	void subscript_list();
	void subscript_expression();

	void Parse();

}; // end Parser

} // namespace


#endif

