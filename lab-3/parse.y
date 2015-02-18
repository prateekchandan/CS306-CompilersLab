%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP
%token EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic eAst : ExpAst* ; sAst : StmtAst*; Int : int; Float : float; String : string;

%type <eAst> expression l_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression postfix_expression logical_and_expression constant_expression expression_list unary_operator
%type <sAst> translation_unit function_definition compound_statement statement_list statement selection_statement iteration_statement assignment_statement
%type <Int> INT_CONSTANT
%type <Float> FLOAT_CONSTANT
%type <String> STRING_LITERAL IDENTIFIER

%%

translation_unit
	: function_definition 
	{
		$$ = $1 ;
		$$->print();
		cout<<endl;
	}
	| translation_unit function_definition
	;

function_definition
	: type_specifier fun_declarator compound_statement
	{
		$$ = $3 ;
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
		$$ = new IntConst($<Int>1);
	}
	| FLOAT_CONSTANT
	{
		$$ = new FloatConst($<Float>1);
	}
	;

compound_statement
	: '{' '}' 
	{
		$$ = new BlockAst();
	}
	| '{' statement_list '}' 
	{
		$$ = $2;
	}
	| '{' declaration_list statement_list '}'
	{
		$$ = $3;
	}
	;

statement_list
	: statement
	{
		StmtAst* temp_val = $$;
		$$ = new BlockAst();
		((BlockAst*)$$)->add_statement(temp_val);
	}		
	| statement_list statement
	{
		$$ = $1;
		((BlockAst*)$$)->add_statement($2);
	}
	;

statement
	: '{' statement_list '}'
	{
		$$ = $2;
	}
	| selection_statement
	{
		$$ = $1;
	}
	| iteration_statement
	{
		$$ = $1;
	}
	| assignment_statement
	{
		$$ = $1;
	}
	| RETURN expression ';'
	{
		$$ = new ReturnSt($2);
	}
	;

assignment_statement
	: ';'
	{
		$$ = new Ass(NULL, NULL);
	}								
	|  l_expression '=' expression ';'
	{
		$$ = new Ass($1, $3);
	}
	;

expression
	: logical_and_expression
	{
		$$ = $1;
	}
	| expression OR_OP logical_and_expression
	{
		$$ = new Op(OP_TYPE::OR_OP, $1, $3);
	}
	;

logical_and_expression
	: equality_expression
	{
		$$ = $1;
	}
	| logical_and_expression AND_OP equality_expression
	{
		$$ = new Op(OP_TYPE::AND_OP, $1, $3);
	}
	;

equality_expression
	: relational_expression
	{
		$$ = $1;
	} 
	| equality_expression EQ_OP relational_expression
	{
		$$ = new Op(OP_TYPE::EQ_OP, $1, $3);
	}
	| equality_expression NE_OP relational_expression
	{
		$$ = new Op(OP_TYPE::NE_OP, $1, $3);
	}
	;
	
relational_expression
	: additive_expression
	{
		$$ = $1;
	}
	| relational_expression '<' additive_expression
	{
		$$ = new Op(OP_TYPE::LT, $1, $3);
	}
	| relational_expression '>' additive_expression
	{
		$$ = new Op(OP_TYPE::GT, $1, $3);
	}
	| relational_expression LE_OP additive_expression
	{
		$$ = new Op(OP_TYPE::LE_OP, $1, $3);
	}
	| relational_expression GE_OP additive_expression
	{
		$$ = new Op(OP_TYPE::GE_OP, $1, $3);
	}
	;

additive_expression 
	: multiplicative_expression
	{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression
	{
		$$ = new Op(OP_TYPE::PLUS, $1, $3);
	}
	| additive_expression '-' multiplicative_expression
	{
		$$ = new Op(OP_TYPE::MINUS, $1, $3);
	}
	;

multiplicative_expression
	: unary_expression
	{
		$$ = $1;
	}
	| multiplicative_expression '*' unary_expression
	{
		$$ = new Op(OP_TYPE::MULT, $1, $3);
	}
	| multiplicative_expression '/' unary_expression
	{
		$$ = new Op(OP_TYPE::DIV, $1, $3);
	}
	;
	
unary_expression
	: postfix_expression
	{
		$$ = $1;
	}
	| unary_operator postfix_expression
	{
		((UnOp*)$1)->set_expression($2);
		$$ = $1;
	}
	;

postfix_expression
	: primary_expression
	{
		$$ = $1;
	}
    | IDENTIFIER '(' ')'
    {
		$$ = new FunCall(new Identifier());
	}
	| IDENTIFIER '(' expression_list ')'
	{
		$$ = $3;
		((FunCall*)$$)->set_name(new Identifier($1));
	}
	| l_expression INC_OP
	{
		$$ = new UnOp(UNOP_TYPE::PP, $1);
	}
	;

primary_expression
	: l_expression
	{
		$$ = $1;
	}
	| l_expression '=' expression
	{
		$$ = new Op(OP_TYPE::ASSIGN, $1, $3);
	}
	| INT_CONSTANT
	{
		$$ = new IntConst($<Int>1);
	}
	| FLOAT_CONSTANT
	{
		$$ = new FloatConst($<Float>1);
	}
	| STRING_LITERAL
	{
		$$ = new StringConst($<String>1);
	}
	| '(' expression ')'
	{
		$$ = $2;
	}
	;

l_expression
	: IDENTIFIER
	{
		$$ = new Identifier($1);
		identifiers.insert((ExpAst*)$$);
	}
	| l_expression '[' expression ']'
	{
		if(identifiers.find((ExpAst*)$1)!=identifiers.end()){
			// We are at base case
			$$ = new ArrayRef((Identifier*)$1);
			((ArrayRef*)$$)->add_index($3);
		}
		else{
			// We are recursive step
			$$ = new ArrayRef((ArrayRef*)$1);
			((ArrayRef*)$$)->add_index($3);
		}
	}
	;
        
expression_list
	: expression
	{
		ExpAst* temp_val = $1;
		$$ = new FunCall();
		((FunCall*)$$)->add_expression(temp_val);
	}
	| expression_list ',' expression
	{
		$$ = $1;
		((FunCall*)$$)->add_expression($3);
	}
	;
        
unary_operator
	: '-'
	{
		$$ = new UnOp(UNOP_TYPE::UMINUS);
	}
	| '!'
	{
		$$ = new UnOp(UNOP_TYPE::NOT);
	}
	;

selection_statement
	: IF '(' expression ')' statement ELSE statement
	{
		$$ = new If($3, $5, $7);
	}
	;

iteration_statement
	: WHILE '(' expression ')' statement
	{
		$$ = new While($3, $5);
	}	
	| FOR '(' expression ';' expression ';' expression ')' statement
	{
		$$ = new For($3, $5, $7, $9);
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
