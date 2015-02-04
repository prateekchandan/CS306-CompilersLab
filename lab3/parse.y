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
			
			std::cout<<"dot: translation_unit_"<<$$<<" -> { translation_unit_"<<$1<<" function_definition_"<<$2<<" }\n";
		}
	;
	
function_definition
	: type_specifier fun_declarator compound_statement
		{
			$$=++node_count;
			std::cout<<"dot: function_definition_"<<$$<<" [label=\"function_definition\"]\n";
			
			std::cout<<"dot: function_definition_"<<$$<<" -> { type_specifier_"<<$1<<" fun_declarator_"<<$2
			<<" compound_statement_"<<$3<<" }\n";
		}
	;

type_specifier
	: VOID
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"VOID\"]\n";

			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| INT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"INT\"]\n";

			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
			std::cout<<"token_printing "<<endl;
		}
	| FLOAT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: type_specifier_"<<$$<<" [label=\"type_specifier\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"FLOAT\"]\n";

			std::cout<<"dot: type_specifier_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')'
		{
			$$=++node_count;
			$1=++node_count;
			$2=++node_count;
			$4=++node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" [label=\"fun_declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IDENTIFIER\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\")\"]\n";
			
			std::cout<<"dot: fun_declarator_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
			<<"  parameter_list_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	| IDENTIFIER '(' ')'
		{
			$$=++node_count;
			$1=++node_count;
			$2=++node_count;
			$3=++node_count;
			std::cout<<"dot: fun_declarator_"<<$$<<" [label=\"fun_declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IDENTIFIER\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
			std::cout<<"dot: TERMINAL_"<<$3<<" [label=\")\"]\n";
			
			std::cout<<"dot: fun_declarator_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
			<<" TERMINAL_"<<$3<<" }\n";
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
			$2=++node_count;
			std::cout<<"dot: parameter_list_"<<$$<<" [label=\"parameter_list\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\",\"]\n";

			std::cout<<"dot: parameter_list_"<<$$<<" -> { parameter_list_"<<$1<<" TERMINAL_"<<
			$2<<" parameter_declaration_"<<$3<<" }\n";
		}
	;

parameter_declaration
	: type_specifier declarator
		{
			$$=++node_count;
			std::cout<<"dot: parameter_declaration_"<<$$<<" [label=\"parameter_declaration\"]\n";
			
			std::cout<<"dot: parameter_declaration_"<<$$<<" -> { type_specifier_"<<$1
			 <<" declarator_"<<$2<<" }\n";
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
			$2=++node_count;
			$4=++node_count;
			std::cout<<"dot: declarator_"<<$$<<" [label=\"declarator\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"[\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\"]\"]\n";
			
			std::cout<<"dot: declarator_"<<$$<<" -> { declarator_"<<$1<<" TERMINAL_"<<$2
			<<" constant_expression_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	;

constant_expression
	: INT_CONSTANT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" [label=\"constant_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"INT_CONSTANT\"]\n";
			
			std::cout<<"dot: constant_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| FLOAT_CONSTANT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: constant_expression_"<<$$<<" [label=\"constant_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"FLOAT_CONSTANT\"]\n";
			
			std::cout<<"dot: constant_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	;

compound_statement
	: '{' '}'
		{
			$$=++node_count;
			$1=++node_count;
			$2=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"{\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"}\"]\n";
			
			std::cout<<"dot: compound_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2<<" }\n";
		}
	| '{' statement_list '}'
		{
			$$=++node_count;
			$1=++node_count;
			$3=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"{\"]\n";
			std::cout<<"dot: TERMINAL_"<<$3<<" [label=\"}\"]\n";

			std::cout<<"dot: compound_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" statement_list_"<<$2
			<<" TERMINAL_"<<$3<<" }\n";
		}

	| '{' declaration_list statement_list '}'
		{
			$$=++node_count;
			$1=++node_count;
			$4=++node_count;
			std::cout<<"dot: compound_statement_"<<$$<<" [label=\"compound_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"{\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\"}\"]\n";
			
			std::cout<<"dot: compound_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" declaration_list_"<<$2
			<<" statement_list_"<<$3<<" TERMINAL_"<<$4<<" }\n";
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
			
			std::cout<<"dot: statement_list_"<<$$<<" -> { statement_list_"<<$1<<" statement_"<<$2<<" }\n";
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

			std::cout<<"dot: statement_"<<$$<<" -> iteration_statement_"<<$1<<"\n";
		}
	| assignment_statement
		{
			$$=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			
			std::cout<<"dot: statement_"<<$$<<" -> assignment_statement_"<<$1<<"\n";
		}
	| RETURN expression ';'
		{
			$$=++node_count;
			$1=++node_count;
			$3=++node_count;
			std::cout<<"dot: statement_"<<$$<<" [label=\"statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"RETURN\"]\n";
			std::cout<<"dot: TERMINAL_"<<$3<<" [label=\";\"]\n";
			
			std::cout<<"dot: statement_"<<$$<<" -> { TERMINAL_"<<$1<<" expression_"<<$2<<" TERMINAL_"<<$3<<" }\n";
		}
	;

assignment_statement
	: ';'
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: assignment_statement_"<<$$<<" [label=\"assignment_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\";\"]\n";

			std::cout<<"dot: assignment_statement_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	|  l_expression '=' expression ';'
		{
			$$=++node_count;
			$2=++node_count;
			$4=++node_count;
			std::cout<<"dot: assignment_statement_"<<$$<<" [label=\"assignment_statement\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"=\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\";\"]\n";
			
			std::cout<<"dot: assignment_statement_"<<$$<<" -> { l_expression_"<<$1<<" TERMINAL_"<<$2
			<<" expression_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	;

expression
	: logical_and_expression
		{
			$$=++node_count;
			std::cout<<"dot: expression_"<<$$<<" [label=\"expression\"]\n";
			
			std::cout<<"dot: expression_"<<$$<<" -> logical_and_expression_"<<$1<<"\n";
		}
	| expression OR_OP logical_and_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: expression_"<<$$<<" [label=\"expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"OR_OP\"]\n";

			std::cout<<"dot: expression_"<<$$<<" -> { expression_"<<$1<<" TERMINAL_"<<$2
			<<" logical_and_expression_"<<$3<<" }\n";
		}
	;

logical_and_expression
	: equality_expression
		{
			$$=++node_count;
			std::cout<<"dot: logical_and_expression_"<<$$<<" [label=\"logical_and_expression\"]\n";
			
			std::cout<<"dot: logical_and_expression_"<<$$<<" -> equality_expression_"<<$1<<"\n";
		}
	| logical_and_expression AND_OP equality_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: logical_and_expression_"<<$$<<" [label=\"logical_and_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"AND_OP\"]\n";

			std::cout<<"dot: logical_and_expression_"<<$$<<" -> { logical_and_expression_"<<$1
			<<" TERMINAL_"<<$2<<" equality_expression_"<<$3<<" }\n";
		}
	;

equality_expression
	: relational_expression
		{
			$$=++node_count;
			std::cout<<"dot: equality_expression_"<<$$<<" [label=\"equality_expression\"]\n";
			
			std::cout<<"dot: equality_expression_"<<$$<<" -> relational_expression_"<<$1<<"\n";
		}
	| equality_expression EQ_OP relational_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: equality_expression_"<<$$<<" [label=\"equality_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"EQ_OP\"]\n";

			std::cout<<"dot: equality_expression_"<<$$<<" -> { equality_expression_"<<$1
			<<" TERMINAL_"<<$2<<" relational_expression_"<<$3<<" }\n";
		}
	| equality_expression NE_OP relational_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: equality_expression_"<<$$<<" [label=\"equality_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"NE_OP\"]\n";

			std::cout<<"dot: equality_expression_"<<$$<<" -> { equality_expression_"<<$1
			<<" TERMINAL_"<<$2<<" relational_expression_"<<$3<<" }\n";
		}
	;
	
relational_expression
	: additive_expression
		{
			$$=++node_count;
			std::cout<<"dot: relational_expression_"<<$$<<" [label=\"relational_expression\"]\n";
			std::cout<<"dot: relational_expression_"<<$$<<" -> additive_expression_"<<$1<<"\n";
		}
	| relational_expression '<' additive_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: relational_expression_"<<$$<<" [label=\"relational_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"<\"]\n";

			std::cout<<"dot: relational_expression_"<<$$<<" -> { relational_expression_"<<$1
			<<" TERMINAL_"<<$2<<" additive_expression_"<<$3<<" }\n";
		}
	| relational_expression '>' additive_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: relational_expression_"<<$$<<" [label=\"relational_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\">\"]\n";
			
			std::cout<<"dot: relational_expression_"<<$$<<" -> { relational_expression_"<<$1
			<<" TERMINAL_"<<$2<<" additive_expression_"<<$3<<" }\n";
		}
	| relational_expression LE_OP additive_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: relational_expression_"<<$$<<" [label=\"relational_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"LE_OP\"]\n";

			std::cout<<"dot: relational_expression_"<<$$<<" -> { relational_expression_"<<$1
			<<" TERMINAL_"<<$2<<" additive_expression_"<<$3<<" }\n";
		}
	| relational_expression GE_OP additive_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: relational_expression_"<<$$<<" [label=\"relational_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"GE_OP\"]\n";

			std::cout<<"dot: relational_expression_"<<$$<<" -> { relational_expression_"<<$1
			<<" TERMINAL_"<<$2<<" additive_expression_"<<$3<<" }\n";
		}
	;


additive_expression
	: multiplicative_expression
		{
			$$=++node_count;
			std::cout<<"dot: additive_expression_"<<$$<<" [label=\"additive_expression\"]\n";
			
			std::cout<<"dot: additive_expression_"<<$$<<" -> multiplicative_expression_"<<$1<<"\n";
		}
	| additive_expression '+' multiplicative_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: additive_expression_"<<$$<<" [label=\"additive_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"+\"]\n";
			
			std::cout<<"dot: additive_expression_"<<$$<<" -> { additive_expression_"<<$1
			<<" TERMINAL_"<<$2<<" multiplicative_expression_"<<$3<<" }\n";
		}
	| additive_expression '-' multiplicative_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: additive_expression_"<<$$<<" [label=\"additive_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"-\"]\n";

			std::cout<<"dot: additive_expression_"<<$$<<" -> { additive_expression_"<<$1
			<<" TERMINAL_"<<$2<<" multiplicative_expression_"<<$3<<" }\n";
		}
	;

multiplicative_expression
	: unary_expression
		{
			$$=++node_count;
			std::cout<<"dot: multiplicative_expression_"<<$$<<" [label=\"multiplicative_expression\"]\n";
			
			std::cout<<"dot: multiplicative_expression_"<<$$<<" -> unary_expression_"<<$1<<"\n";
		}
	| multiplicative_expression '*' unary_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: multiplicative_expression_"<<$$<<" [label=\"multiplicative_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"*\"]\n";
			
			std::cout<<"dot: multiplicative_expression_"<<$$<<" -> { multiplicative_expression_"<<$1
			<<" TERMINAL_"<<$2<<" unary_expression_"<<$3<<" }\n";
		}
	| multiplicative_expression '/' unary_expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: multiplicative_expression_"<<$$<<" [label=\"multiplicative_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"/\"]\n";

			std::cout<<"dot: multiplicative_expression_"<<$$<<" -> { multiplicative_expression_"<<$1
			<<" TERMINAL_"<<$2<<" unary_expression_"<<$3<<" }\n";
		}
	;
	
unary_expression
	: postfix_expression
		{
			$$=++node_count;
			std::cout<<"dot: unary_expression_"<<$$<<" [label=\"unary_expression\"]\n";
			
			std::cout<<"dot: unary_expression_"<<$$<<" -> postfix_expression_"<<$1<<"\n";
		}
	| unary_operator postfix_expression
		{
			$$=++node_count;
			std::cout<<"dot: unary_expression_"<<$$<<" [label=\"unary_expression\"]\n";
			
			std::cout<<"dot: unary_expression_"<<$$<<" -> { unary_operator_"<<$1<<" postfix_expression_"<<$2<<" }\n";
		}
	;
	


postfix_expression
	: primary_expression
		{
			$$=++node_count;
			std::cout<<"dot: postfix_expression_"<<$$<<" [label=\"postfix_expression\"]\n";
			
			std::cout<<"dot: postfix_expression_"<<$$<<" -> primary_expression_"<<$1<<"\n";
		}
	| IDENTIFIER '(' ')'
		{
			$$=++node_count;
			$1=++node_count;
			$2=++node_count;
			$3=++node_count;
			std::cout<<"dot: postfix_expression_"<<$$<<" [label=\"postfix_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IDENTIFIER\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
			std::cout<<"dot: TERMINAL_"<<$3<<" [label=\")\"]\n";
			
			std::cout<<"dot: postfix_expression_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2<<" TERMINAL_"<<$3<<" }\n";
		}
	| IDENTIFIER '(' expression_list ')'
		{
			$$=++node_count;
			$1=++node_count;
			$2=++node_count;
			$4=++node_count;
			std::cout<<"dot: postfix_expression_"<<$$<<" [label=\"postfix_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IDENTIFIER\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\")\"]\n";
			
			std::cout<<"dot: postfix_expression_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
			<<" expression_list_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	| l_expression INC_OP
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: postfix_expression_"<<$$<<" [label=\"postfix_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"INC_OP\"]\n";
			
			std::cout<<"dot: postfix_expression_"<<$$<<" -> { l_expression_"<<$1<<" TERMINAL_"<<$2<<" }\n";
		}
	| l_expression DEC_OP
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: postfix_expression_"<<$$<<" [label=\"postfix_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"DEC_OP\"]\n";
			
			std::cout<<"dot: postfix_expression_"<<$$<<" -> { l_expression_"<<$1<<" TERMINAL_"<<$2<<" }\n";
		}
	;


primary_expression
	: l_expression
		{
			$$=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> l_expression_"<<$1<<"\n";
		}
	| l_expression '=' expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"=\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> { l_expression_"<<$1<<" TERMINAL_"<<$2<<" expression_"<<$3<<" }\n";
		}
	| INT_CONSTANT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"INT_CONSTANT\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| FLOAT_CONSTANT
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"FLOAT_CONSTANT\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| STRING_LITERAL
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"STRING_LITERAL\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| '(' expression ')'
		{
			$$=++node_count;
			$1=++node_count;
			$3=++node_count;
			std::cout<<"dot: primary_expression_"<<$$<<" [label=\"primary_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"(\"]\n";
			std::cout<<"dot: TERMINAL_"<<$3<<" [label=\")\"]\n";
			
			std::cout<<"dot: primary_expression_"<<$$<<" -> { TERMINAL_"<<$1<<" expression_"<<$2<<" TERMINAL_"<<$3<<" }\n";
		}
	;

l_expression
	: IDENTIFIER
		{
			$$=++node_count;
			$1=++node_count;
			std::cout<<"dot: l_expression_"<<$$<<" [label=\"l_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IDENTIFIER\"]\n";
			
			std::cout<<"dot: l_expression_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| l_expression '[' expression ']' 
		{
			$$=++node_count;
			$2=++node_count;
			$4=++node_count;
			std::cout<<"dot: l_expression_"<<$$<<" [label=\"l_expression\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"[\"]\n";
			std::cout<<"dot: TERMINAL_"<<$4<<" [label=\"]\"]\n";
			
			std::cout<<"dot: l_expression_"<<$$<<" { l_expression_"<<$1<<" TERMINAL_"<<$2
			<<" expression_"<<$3<<" TERMINAL_"<<$4<<" }\n";
		}
	;
	
expression_list
	: expression
		{
			$$=++node_count;
			std::cout<<"dot: expression_list_"<<$$<<" [label=\"expression_list\"]\n";
			
			std::cout<<"dot: expression_list_"<<$$<<" -> expression_"<<$1<<"\n";
		}
	| expression_list ',' expression
		{
			$$=++node_count;
			$2=++node_count;
			std::cout<<"dot: expression_list_"<<$$<<" [label=\"expression_list\"]\n";
			std::cout<<"dot: TERMINAL_"<<$2<<" [label=\",\"]\n";
			
			std::cout<<"dot: expression_list_"<<$$<<" -> { expression_list_"<<$1<<" TERMINAL_"<<$2<<" expression_"<<$3<<" }\n";
		}
	;
	
unary_operator
	: '-'
		{
		    $$=++node_count;
		    $1=++node_count;
		    std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"-\"]\n";
		    
		    std::cout<<"dot: unary_operator_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	| '!'
		{
		    $$=++node_count;
		    $1=++node_count;
		    std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"!\"]\n";
		    
		    std::cout<<"dot: unary_operator_"<<$$<<" -> TERMINAL_"<<$1<<"\n";
		}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
		{
		    $$=++node_count;
		    $1=++node_count;
		    $2=++node_count;
		    $4=++node_count;
		    $6=++node_count;
		    std::cout<<"dot: selection_statement_"<<$$<<" [label=\"selection_statement\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"IF\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$4<<" [label=\")\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$6<<" [label=\"ELSE\"]\n";
		    
		    std::cout<<"dot: selection_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
		    <<" expression_"<<$3<<" TERMINAL_"<<$4<<" statement_"<<$5<<" TERMINAL_"<<$6
		    <<" statement_"<<$7<<" }\n";
	    }
	;

iteration_statement
	: WHILE '(' expression ')' statement
		{
		    $$=++node_count;
		    $1=++node_count;
		    $2=++node_count;
		    $4=++node_count;
		    std::cout<<"dot: iteration_statement_"<<$$<<" [label=\"iteration_statement\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"WHILE\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$4<<" [label=\")\"]\n";
		    
		    std::cout<<"dot: iteration_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
		    <<" expression_"<<$3<<" TERMINAL_"<<$4<<" statement_"<<$5<<" }\n";
	    }
	| FOR '(' expression ';' expression ';' expression ')' statement
		{
		    $$=++node_count;
		    $1=++node_count;
		    $2=++node_count;
		    $4=++node_count;
		    $6=++node_count;
		    $8=++node_count;
		    std::cout<<"dot: iteration_statement_"<<$$<<" [label=\"iteration_statement\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$1<<" [label=\"FOR\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$2<<" [label=\"(\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$4<<" [label=\";\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$6<<" [label=\";\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$8<<" [label=\")\"]\n";
		    
		    std::cout<<"dot: iteration_statement_"<<$$<<" -> { TERMINAL_"<<$1<<" TERMINAL_"<<$2
		    <<" expression_"<<$3<<" TERMINAL_"<<$4<<" expression_"<<$5
		    <<" TERMINAL_"<<$6<<" expression_"<<$7<<" TERMINAL_"<<$8<<" statement_"<<$9<<" }\n";
		}
	;

declaration_list
	: declaration
		{
		    $$=++node_count;
		    std::cout<<"dot: declaration_list_"<<$$<<" [label=\"declaration_list\"]\n";
		    
		    std::cout<<"dot: declaration_list_"<<$$<<" -> declaration_"<<$1<<"\n";
	    }
	| declaration_list declaration
		{
		    $$=++node_count;
		    std::cout<<"dot: declaration_list_"<<$$<<" [label=\"declaration_list\"]\n";
		    
		    std::cout<<"dot: declaration_list_"<<$$<<" -> { declaration_list_"<<$1<<" declaration_"<<$2<<" }\n";
		}
	;

declaration
	: type_specifier declarator_list ';'
		{
		    $$=++node_count;
		    $3=++node_count;
		    std::cout<<"dot: declaration_"<<$$<<" [label=\"declarator_list\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$3<<" [label=\";\"]\n";
		    
		    std::cout<<"dot: declaration_"<<$$<<" -> { type_specifier_"<<$1<<" declarator_list_"<<$2<<" TERMINAL_"<<$3<<" }\n";
	    }
	;

declarator_list
	: declarator
		{
		    $$=++node_count;
		    std::cout<<"dot: declarator_list_"<<$$<<" [label=\"declarator_list\"]\n";
		    
		    std::cout<<"dot: declarator_list_"<<$$<<" -> declarator_"<<$1<<"\n";
		}
	| declarator_list ',' declarator
		{
		    $$=++node_count;
		    $2=++node_count;
		    std::cout<<"dot: declarator_list_"<<$$<<" [label=\"declarator_list\"]\n";
		    std::cout<<"dot: TERMINAL_"<<$2<<" [label=\",\"]\n";
		    
		    std::cout<<"dot: declarator_list_"<<$$<<" -> { declarator_list_"<<$1<<" TERMINAL_"<<$2<<" declarator_"<<$3<<" }\n";
	    }
	;
