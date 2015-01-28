%debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()



%token VOID INT FLOAT
%token IF ELSE WHILE FOR RETURN
%token OR_OP AND_OP EQ_OP NE_OP LE_OP GE_OP
%token INC_OP DEC_OP
%token STRING_LITERAL IDENTIFIER INT_CONSTANT FLOAT_CONSTANT


 
%%

translation_unit
	: function_definition
		{
			$$ = ++node_count;
			std::cout<<"dot: translation_unit_"<<$$<<" [label=\"translation_unit\"]\n";
			std::cout<<"dot: translation_unit_"<<$$<<" -> function_definition_"<<$1<<"\n";
		}
	| translation_unit function_definition
		{
			$$ = ++node_count;
			std::cout<<"dot: translation_unit_"<<$$<<" [label=\"translation_unit\"]\n";
			std::cout<<"dot: translation_unit_"<<$$<<" -> {translation_unit_"<<$1<<" function_definition_"<<$2<<"}\n";
		}
	;
	
function_definition
	: type_specifier fun_declarator compound_statement
		{
			$$=++node_count;
			std::cout<<"dot: function_definition_"<<$$<<" [label=\"function_definition\"]\n";
			std::cout<<"dot: function_definition_"<<$$<<" -> {type_specifier_"<<$1<<" fun_declarator_"<<$2<<" compound_statement_"<<$3<<"}\n";
		}
	;

type_specifier
	: VOID
		{
			$$=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"VOID\"]\n";
			$1=node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| INT
		{
			$$=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"INT\"]\n";
			$1=node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| FLOAT
		{
			$$=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"FLOAT\"]\n";
			$1=node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')'
		{
			$$=++node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" [label=\"fun_declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"IDENTIFIER\"]\n";
			$1=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"(\"]\n";
			$2=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\")\"]\n";
			$4=node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" -> {TERMINAL_"<<$1<<" TERMINAL_"<<$2<<"  parameter_list_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	| IDENTIFIER '(' ')'
		{
			$$=++node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" [label=\"fun_declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"IDENTIFIER\"]\n";
			$1=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"(\"]\n";
			$2=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\")\"]\n";
			$3=node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" -> {TERMINAL_"<<$1<<" TERMINAL_"<<$2<<" TERMINAL_"<<$3<<" }\n";
		}
	;

parameter_list
	: parameter_declaration
		{
			$$=++node_count;
			std::cout<<"dot: parameter_list_"<<$$<<" [label=\"parameter_list\"]\n";
			std::cout<<"dot: parameter_list_"<<$$<<" -> parameter_declaration_"<<$1<<"\n";
		}
	| parameter_list ',' parameter_declaration
		{
			$$=++node_count;
			std::cout<<"dot: parameter_list_"<<$$<<" [label=\"parameter_list\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\",\"]\n";
			$2=node_count;
			std::cout<<"dot: parameter_list_"<<$$<<" -> {parameter_list_"<<$1<<" TERMINAL_"<<$2<<" parameter_declaration_"<<$3<<"}\n";
		}
	;

parameter_declaration
	: type_specifier declarator
		{
			$$=++node_count;
			std::cout<<"dot: parameter_declaration_"<<$$<<" [label=\"parameter_declaration\"]\n";
			std::cout<<"dot: parameter_declaration_"<<$$<<" -> {type_specifier_"<<$1<<" declarator_"<<$2<<"}\n";
		}
	;

declarator
	: IDENTIFIER
		{
			$$=++node_count;
			std::cout<<"dot: declarator_"<<$$<<" [label=\"declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"IDENTIFIER\"]\n";
			$1=node_count;
			std::cout<<"dot: declarator_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| declarator '[' constant_expression ']'
		{
			$$=++node_count;
			std::cout<<"dot: declarator_"<<$$<<" [label=\"declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"[\"]\n";
			$2=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"]\"]\n";
			$4=node_count;
			std::cout<<"dot: declarator_"<<$$<<" -> {declarator_"<<$1<<" TERMINAL_"<<$2<<" constant_expression_"<<$3<<" TERMINAL_"<<$4<<"}\n";
		}
	;

constant_expression
	: INT_CONSTANT
		{
			$$=++node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" [label=\"constant_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"INT_CONSTANT\"]\n";
			$1=node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| FLOAT_CONSTANT
		{
			$$=++node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" [label=\"constant_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"FLOAT_CONSTANT\"]\n";
			$1=node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	;

compound_statement
	: '{' '}'
		{
			$$=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"{\"]\n";
			$1=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"}\"]\n";
			$2=node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" -> {TERMINAL_"<<$1<<" TERMINAL_"<<$2<<"}\n";
		}
	| '{' statement_list '}'
		{
			$$=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"{\"]\n";
			$1=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"}\"]\n";
			$3=node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" -> {TERMINAL_"<<$1<<" statement_list_"<<$2<<" TERMINAL_"<<$3<<"}\n";
		}

	| '{' declaration_list statement_list '}'
		{
			$$=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"{\"]\n";
			$1=node_count;
			std::cout<<"dot: TERMINAL_"<<++node_count<<" [label=\"}\"]\n";
			$4=node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" -> {TERMINAL_"<<$1<<" declaration_list_"<<$2<<" statement_list_"<<$3<<" TERMINAL_"<<$4<<"}\n";
		}
	;

statement_list
	: statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_list_"<<$$<<" [label=\"statement_list\"]\n";
			std::cout<<"dot: statement_list_"<<$$<<" -> statement_"<<$1<<"\n";
		}
	| statement_list statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_list_"<<$$<<" [label=\"statement_list\"]\n";
			std::cout<<"dot: statement_list_"<<$$<<" -> {statement_list_"<<$1<<" statement_"<<$2<<"}\n";
		}
	;

statement
	: compound_statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: statement_"<<$$<<" -> compound_statement_"<<$1<<"\n";
		}
	| selection_statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: statement_"<<$$<<" -> selection_statement_"<<$1<<"\n";
		}
	| iteration_statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: statement -> iteration_statement\n";
		}
	| assignment_statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: statement -> assignment_statement\n";
		}
	| RETURN expression ';'
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: statement -> {RETURN expression \";\"}\n";
		}
	;

assignment_statement
	: ';'
		{std::cout<<"dot: assignment_statement -> \";\"\n";}
	|  l_expression '=' expression ';'
		{std::cout<<"dot: assignment_statement -> {l_expression \"=\" expression \";\" }\n";}
	;

expression
	: logical_and_expression
		{std::cout<<"dot: expression -> logical_and_expression\n";}
	| expression OR_OP logical_and_expression
		{std::cout<<"dot: expression -> {expression OR_OP logical_and_expression}\n";}
	;

logical_and_expression
	: equality_expression
		{std::cout<<"dot: logical_and_expression -> equality_expression\n";}
	| logical_and_expression AND_OP equality_expression
		{std::cout<<"dot: logical_and_expression -> {logical_and_expression AND_OP equality_expression}\n";}
	;

equality_expression
	: relational_expression
		{std::cout<<"dot: equality_expression -> relational_expression\n";}
	| equality_expression EQ_OP relational_expression
		{std::cout<<"dot: equality_expression -> {equality_expression EQ_OP relational_expression}\n";}
	| equality_expression NE_OP relational_expression
		{std::cout<<"dot: equality_expression -> {equality_expression NE_OP relational_expression}\n";}
	;
	
relational_expression
	: additive_expression
		{std::cout<<"dot: relational_expression -> additive_expression\n";}
	| relational_expression '<' additive_expression
		{std::cout<<"dot: relational_expression -> {relational_expression \"<\" additive_expression}\n";}
	| relational_expression '>' additive_expression
		{std::cout<<"dot: relational_expression -> {relational_expression \">\" additive_expression}\n";}
	| relational_expression LE_OP additive_expression
		{std::cout<<"dot: relational_expression -> {relational_expression LE_OP additive_expression}\n";}
	| relational_expression GE_OP additive_expression
		{std::cout<<"dot: relational_expression -> {relational_expression GE_OP additive_expression}\n";}
	;

additive_expression
	: multiplicative_expression
		{std::cout<<"dot: additive_expression -> multiplicative_expression\n";}
	| additive_expression '+' multiplicative_expression
		{std::cout<<"dot: additive_expression -> {additive_expression \"+\" multiplicative_expression}\n";}
	| additive_expression '-' multiplicative_expression
		{std::cout<<"dot: additive_expression -> {additive_expression \"-\" multiplicative_expression}\n";}
	;

multiplicative_expression
	: unary_expression
		{std::cout<<"dot: multiplicative_expression -> unary_expression\n";}
	| multiplicative_expression '*' unary_expression
		{std::cout<<"dot: multiplicative_expression -> {multiplicative_expression \"*\" unary_expression}\n";}
	| multiplicative_expression '/' unary_expression
		{std::cout<<"dot: multiplicative_expression -> {multiplicative_expression \"/\" unary_expression}\n";}
	;

unary_expression
	: postfix_expression
		{std::cout<<"dot: unary_expression -> postfix_expression\n";}
	| unary_operator postfix_expression
		{std::cout<<"dot: unary_expression -> {unary_operator postfix_expression}\n";}
	;

postfix_expression
	: primary_expression
		{std::cout<<"dot: postfix_expression -> primary_expression\n";}
	| IDENTIFIER '(' ')'
		{std::cout<<"dot: postfix_expression -> {IDENTIFIER \"(\" \")\"}\n";}
	| IDENTIFIER '(' expression_list ')'
		{std::cout<<"dot: postfix_expression -> {IDENTIFIER \"(\" expression_list \")\"}\n";}
	| l_expression INC_OP
		{std::cout<<"dot: postfix_expression -> {l_expression INC_OP}\n";}
	| l_expression DEC_OP
		{std::cout<<"dot: postfix_expression -> {l_expression DEC_OP}\n";}
	;

primary_expression
	: l_expression
		{std::cout<<"dot: primary_expression -> l_expression\n";}
	| INT_CONSTANT
		{std::cout<<"dot: primary_expression -> INT_CONSTANT\n";}
	| FLOAT_CONSTANT
		{std::cout<<"dot: primary_expression -> FLOAT_CONSTANT\n";}
	| STRING_LITERAL
		{std::cout<<"dot: primary_expression -> STRING_LITERAL\n";}
	| '(' expression ')'
		{std::cout<<"dot: primary_expression -> { \"(\" expression \")\" }\n";}
	;

l_expression
	: IDENTIFIER
		{std::cout<<"dot: l_expression -> IDENTIFIER\n";}
	| l_expression '[' expression ']' 
		{std::cout<<"dot: l_expression -> { l_expression \"[\" expression \"]\" }\n";}
	;
	
expression_list
	: expression
		{std::cout<<"dot: expression_list -> expression\n";}
	| expression_list ',' expression
		{std::cout<<"dot: expression_list -> { expression_list \",\" expression }\n";}
	;
	
unary_operator
	: '-'
		{std::cout<<"dot: unary_operator -> \"-\"\n";}
	| '!'
		{std::cout<<"dot: unary_operator -> \"!\"\n";}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
		{std::cout<<"dot: selection_statement -> { IF \"(\" expression \")\" statement ELSE statement }\n";}
	;

iteration_statement
	: WHILE '(' expression ')' statement
		{std::cout<<"dot: iteration_statement -> { WHILE \"(\" expression \")\" statement }\n";}
	| FOR '(' assignment_statement expression ';' assignment_statement ')' statement
		{std::cout<<"dot: iteration_statement -> { FOR \"(\" assignment_statement expression \";\" assignment_statement \")\" statement }\n";}
	;

declaration_list
	: declaration
		{std::cout<<"dot: declaration_list -> declaration\n";}
	| declaration_list declaration
		{std::cout<<"dot: declaration_list -> { declaration_list declaration }\n";}
	;

declaration
	: type_specifier declarator_list ';'
		{std::cout<<"dot: declaration -> { type_specifier declarator_list \";\" }\n";}
	;

declarator_list
	: declarator
		{std::cout<<"dot: declarator_list -> declarator\n";}
	| declarator_list ',' declarator
		{std::cout<<"dot: declarator_list -> { declarator_list \",\" declarator }\n";}
	;
