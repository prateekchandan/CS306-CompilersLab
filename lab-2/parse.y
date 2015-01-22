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
		{std::cout<<"dot: translation_unit -> function_definition";}
	| translation_unit function_definition
		{std::cout<<"dot: translation_unit -> {translation_unit function_definition}";}
	;
	
function_definition
	: type_specifier fun_declarator compound_statement
		{std::cout<<"dot: function_definition -> {type_specifier fun_declarator compound_statement}";}
	;

type_specifier
	: VOID
		{std::cout<<"dot: type_specifier -> VOID";}
	| INT
		{std::cout<<"dot: type_specifier -> INT";}
	| FLOAT
		{std::cout<<"dot: type_specifier -> FLOAT";}
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')'
		{std::cout<<"dot: fun_declarator -> {IDENTIFIER \"(\"  parameter_list \")\" }";}
	| IDENTIFIER '(' ')'
		{std::cout<<"dot: fun_declarator -> {IDENTIFIER \"(\" \")\" }";}
	;

parameter_list
	: parameter_declaration
		{std::cout<<"dot: parameter_list -> parameter_declaration";}
	| parameter_list ',' parameter_declaration
		{std::cout<<"dot: parameter_list -> {parameter_list \",\" parameter_declaration}";}
	;

parameter_declaration
	: type_specifier declarator
		{std::cout<<"dot: parameter_declaration -> {type_specifier declarator}";}
	;

declarator
	: IDENTIFIER
		{std::cout<<"dot: declarator -> IDENTIFIER";}
	| declarator '[' constant_expression ']'
		{std::cout<<"dot: declarator \"[\" constant_expression \"]\"";}
	;

constant_expression
	: INT_CONSTANT
		{std::cout<<"dot: constant_expression -> INT_CONSTANT";}
	| FLOAT_CONSTANT
		{std::cout<<"dot: constant_expression -> FLOAT_CONSTANT";}
	;

compound_statement
	: '{' '}'
		{std::cout<<"dot: compound_statement -> {\"{\" \"}\"}";}
	| '{' statement_list '}'
		{std::cout<<"dot: compound_statement -> {\"{\" statement_list \"}\"}";}

	| '{' declaration_list statement_list '}'
		{std::cout<<"dot: compound_statement -> {\"{\" declaration_list statement_list \"}\"}";}
	;

statement_list
	: statement
		{std::cout<<"dot: statement_list -> statement";}
	| statement_list statement
		{std::cout<<"dot: statement_list -> {statement_list statement}";}
	;

statement
	: compound_statement
		{std::cout<<"dot: statement -> compound_statement";}
	| selection_statement
		{std::cout<<"dot: statement -> selection_statement";}
	| iteration_statement
		{std::cout<<"dot: statement -> iteration_statement";}
	| assignment_statement
		{std::cout<<"dot: statement -> assignment_statement";}
	| RETURN expression ';'
		{std::cout<<"dot: statement -> {RETURN expression \";\"}";}
	;

assignment_statement
	: ';'
		{std::cout<<"dot: assignment_statement -> \";\"";}
	|  l_expression '=' expression ';'
		{std::cout<<"dot: assignment_statement -> {l_expression \"=\" expression \";\" }";}
	;

expression
	: logical_and_expression
	| expression OR_OP logical_and_expression
	;

logical_and_expression
	: equality_expression
	| logical_and_expression AND_OP equality_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;
	
relational_expression
	: additive_expression
	| relational_expression '<' additive_expression
	| relational_expression '>' additive_expression
	| relational_expression LE_OP additive_expression
	| relational_expression GE_OP additive_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	| multiplicative_expression '/' unary_expression
	;

unary_expression
	: postfix_expression
	| unary_operator postfix_expression
	;

postfix_expression
	: primary_expression
	| IDENTIFIER '(' ')'
	| IDENTIFIER '(' expression_list ')'
	| l_expression INC_OP
	| l_expression DEC_OP
	;

primary_expression
	: l_expression
	| INT_CONSTANT
	| FLOAT_CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

l_expression
	: IDENTIFIER
	| l_expression '[' expression ']' 
	;
	
expression_list
	: expression
	| expression_list ',' expression
	;
	
unary_operator
	: '-'
	| '!'
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| FOR '(' assignment_statement expression ';' assignment_statement ')' statement
	;

declaration_list
	: declaration
	| declaration_list declaration
	;

declaration
	: type_specifier declarator_list';'
	;

declarator_list
	: declarator
	| declarator_list ',' declarator
	;
