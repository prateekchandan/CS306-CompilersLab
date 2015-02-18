%debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP
%token EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic eAst : ExpAst* ; sAst : StmtAst*; Int : int; Float : float; String : string;

%type <eAst> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression postfix_expression primary_expression l_expression constant_expression expression_list unary_operator
%type <sAst> selection_statement iteration_statement assignment_statement translation_unit function_definition compound_statement statement statement_list
%type <Int> INT_CONSTANT
%type <Float> FLOAT_CONSTANT
%type <String> STRING_LITERAL IDENTIFIER

%%

translation_unit
	: function_definition 
	{
		$<sAst>$ = $<sAst>1 ;
	}
	| translation_unit function_definition
	;

function_definition
	: type_specifier fun_declarator compound_statement
	{
		$<sAst>$ = $<sAst>3 ;
	}
	;

type_specifier
	: VOID 	
	| INT   
	| FLOAT 
	;

fun_declarator
	: IDENTIFIER '(' parameter_list ')'
	| IDENTIFIER '(' ')' 
	;

parameter_list
	: parameter_declaration 
	| parameter_list ',' parameter_declaration 
	;

parameter_declaration
	: type_specifier declarator 
	;

declarator
	: IDENTIFIER 
	| declarator '[' constant_expression ']' 
	;

constant_expression 
	: INT_CONSTANT 
	{
		$<eAst>$ = new IntConst($<Int>1);
	}
	| FLOAT_CONSTANT
	{
		$<eAst>$ = new FloatConst($<Float>1);
	}
	;

compound_statement
	: '{' '}' 
	{
		$<sAst>$ = new BlockAst();
	}
	| '{' statement_list '}' 
	{
		$<sAst>$ = $<sAst>2;
	}
	| '{' declaration_list statement_list '}'
	{
		$<sAst>$ = $<sAst>3;
	}
	;

statement_list
	: statement
	{
		$<sAst>$ = new BlockAst();
		((BlockAst*)$<sAst>$)->add_statement($<sAst>1);
	}		
	| statement_list statement
	{
		$<sAst>$ = $<sAst>1;
		((BlockAst*)$<sAst>$)->add_statement($<sAst>2);
	}
	;

statement
	: '{' statement_list '}'
	{
		$<sAst>$ = $<sAst>2;
	}
	| selection_statement
	{
		$<sAst>$ = $<sAst>1;
	}
	| iteration_statement
	{
		$<sAst>$ = $<sAst>1;
	}
	| assignment_statement
	{
		$<sAst>$ = $<sAst>1;
	}
	| RETURN expression ';'
	{
		$<sAst>$ = new ReturnSt($<eAst>2);
	}
	;

assignment_statement
	: ';'
	{
		$<sAst>$ = new Ass(NULL, NULL);
	}								
	|  l_expression '=' expression ';'
	{
		$<sAst>$ = new Ass($<eAst>1, $<eAst>3);
	}
	;

expression
	: logical_and_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| expression OR_OP logical_and_expression
	{
		$<eAst>$ = new Op(OP_TYPE::OR_OP, $<eAst>1, $<eAst>3);
	}
	;

logical_and_expression
	: equality_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| logical_and_expression AND_OP equality_expression
	{
		$<eAst>$ = new Op(OP_TYPE::AND_OP, $<eAst>1, $<eAst>3);
	}
	;

equality_expression
	: relational_expression
	{
		$<eAst>$ = $<eAst>1;
	} 
	| equality_expression EQ_OP relational_expression
	{
		$<eAst>$ = new Op(OP_TYPE::EQ_OP, $<eAst>1, $<eAst>3);
	}
	| equality_expression NE_OP relational_expression
	{
		$<eAst>$ = new Op(OP_TYPE::NE_OP, $<eAst>1, $<eAst>3);
	}
	;
	
relational_expression
	: additive_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| relational_expression '<' additive_expression
	{
		$<eAst>$ = new Op(OP_TYPE::LT, $<eAst>1, $<eAst>3);
	}
	| relational_expression '>' additive_expression
	{
		$<eAst>$ = new Op(OP_TYPE::GT, $<eAst>1, $<eAst>3);
	}
	| relational_expression LE_OP additive_expression
	{
		$<eAst>$ = new Op(OP_TYPE::LE_OP, $<eAst>1, $<eAst>3);
	}
	| relational_expression GE_OP additive_expression
	{
		$<eAst>$ = new Op(OP_TYPE::GE_OP, $<eAst>1, $<eAst>3);
	}
	;

additive_expression 
	: multiplicative_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| additive_expression '+' multiplicative_expression
	{
		$<eAst>$ = new Op(OP_TYPE::PLUS, $<eAst>1, $<eAst>3);
	}
	| additive_expression '-' multiplicative_expression
	{
		$<eAst>$ = new Op(OP_TYPE::MINUS, $<eAst>1, $<eAst>3);
	}
	;

multiplicative_expression
	: unary_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| multiplicative_expression '*' unary_expression
	{
		$<eAst>$ = new Op(OP_TYPE::MULT, $<eAst>1, $<eAst>3);
	}
	| multiplicative_expression '/' unary_expression
	;
	
unary_expression
	: postfix_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| unary_operator postfix_expression
	{
		((UnOp*)$<eAst>1)->set_expression($<eAst>2);
		$<eAst>$ = $<eAst>1;
	}
	;

postfix_expression
	: primary_expression
	{
		$<eAst>$ = $<eAst>1;
	}
    | IDENTIFIER '(' ')'
    {
		$<eAst>$ = new FunCall((Identifier*)$<eAst>1);
	}
	| IDENTIFIER '(' expression_list ')'
	{
		$<eAst>$ = $<eAst>3;
		((FunCall*)$<eAst>$)->set_name(new Identifier($1));
	}
	| l_expression INC_OP
	{
		$<eAst>$ = new UnOp(UNOP_TYPE::PP, $<eAst>1);
	}
	;

primary_expression
	: l_expression
	{
		$<eAst>$ = $<eAst>1;
	}
	| l_expression '=' expression
	{
		$<eAst>$ = new Op(OP_TYPE::ASSIGN, $<eAst>1, $<eAst>3);
	}
	| INT_CONSTANT
	{
		$<eAst>$ = new IntConst($<Int>1);
	}
	| FLOAT_CONSTANT
	{
		$<eAst>$ = new FloatConst($<Float>1);
	}
	| STRING_LITERAL
	{
		$<eAst>$ = new StringConst($<String>1);
	}
	| '(' expression ')'
	{
		$<eAst>$ = $2;
	}
	;

l_expression
	: IDENTIFIER
	{
		$<eAst>$ = new Identifier($<String>1);
	}
	| l_expression '[' expression ']'
	{
		//$<eAst>$ = new Index((ArrayRef*)$<eAst>1, $<eAst>3);
	}
	;
        
expression_list
	: expression
	{
		$<eAst>$ = new FunCall();
		((FunCall*)$<eAst>$)->add_expression($<eAst>1);
	}
	| expression_list ',' expression
	{
		$<eAst>$ = $<eAst>1;
		((FunCall*)$<eAst>$)->add_expression($<eAst>3);
	}
	;
        
unary_operator
	: '-'
	{
		$<eAst>$ = new UnOp(UNOP_TYPE::UMINUS);
	}
	| '!'
	{
		$<eAst>$ = new UnOp(UNOP_TYPE::NOT);
	}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	{
		$<sAst>$ = new If($<eAst>3, $<sAst>5, $<sAst>7);
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{
		$<sAst>$ = new While($<eAst>1, $<sAst>5);
	}	
	| FOR '(' expression ';' expression ';' expression ')' statement
	{
		$<sAst>$ = new For($<eAst>3, $<eAst>5, $<eAst>7, $<sAst>9);
	}
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
