%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP
%token EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic eAst : ExpAst* ; sAst : StmtAst*; Int : int; Float : float; String : string; Type : TYPE*;
			symbolTableEntry : SymbolTableEntry*;

%type <eAst> expression l_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression postfix_expression logical_and_expression constant_expression expression_list unary_operator
%type <sAst> translation_unit function_definition compound_statement statement_list statement selection_statement iteration_statement assignment_statement
%type <Int> INT_CONSTANT
%type <Float> FLOAT_CONSTANT
%type <String> STRING_LITERAL IDENTIFIER
%type <Type> type_specifier
%type <symbolTableEntry> declarator fun_declarator;

%%

translation_unit
	: 
	function_definition 
	{
		$$ = $1 ;
		CurrentSymbolTable->print();
		$1->print();
		cout<<endl<<"\n-----------------------\n";

		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offsetStack.pop_back();
	}
	| translation_unit function_definition
	{
		$$ = $1 ;
		CurrentSymbolTable->print();
		$2->print();
		cout<<endl<<"\n-----------------------\n";
		
		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offsetStack.pop_back();

	}
	;

function_definition
	: type_specifier fun_declarator compound_statement
	{
		$$ = $3 ;
	}
	;

fun_declarator
	:
	IDENTIFIER '('
	{
		$$ = new SymbolTableEntry;
		$$->symbolName = $1;
		$$->scope = current_scope;
		$$->vf = VAR_OR_FUNC::FUNC;
		$$->type = curr_type;

		current_scope = SCOPE::PARAM;

		SymbolTable *temp = new SymbolTable($1);
		$$->table = temp;

		bool check=CurrentSymbolTable->AddEntry($1,$$);
		if(!check)
			cout<<"Error : Function "<<$1<<" Redefined\n";

		SymbolTableStack.push_back(CurrentSymbolTable);
		CurrentSymbolTable = temp;
		offsetStack.push_back(global_offset);
		global_offset = 0;

	} 
	parameter_list ')'
	{
		current_scope = SCOPE::LOCAL;
	}
	|
	IDENTIFIER '(' ')' 
	{
		$$ = new SymbolTableEntry;
		$$->symbolName = $1;
		$$->scope = current_scope;
		$$->vf = VAR_OR_FUNC::FUNC;
		$$->type = curr_type;

		current_scope = SCOPE::PARAM;

		SymbolTable *temp = new SymbolTable($1);
		$$->table = temp;

		bool check=CurrentSymbolTable->AddEntry($1,$$);
		if(!check)
			cout<<"Error : Function "<<$1<<" Redefined\n";

		SymbolTableStack.push_back(CurrentSymbolTable);
		CurrentSymbolTable = temp;
		offsetStack.push_back(global_offset);
		global_offset = 0;

		current_scope = SCOPE::LOCAL;
	}
	;

parameter_list
	: parameter_declaration 
	{
		current_scope = SCOPE::PARAM;
	}
	| parameter_list ',' parameter_declaration 
	{
		current_scope = SCOPE::PARAM;
	}
	;

parameter_declaration
	: type_specifier declarator 
	{
		bool check=CurrentSymbolTable->AddEntry($2->symbolName,$2);
		if(!check)
			cout<<"Error at Line "<<line_no<<" :"<<$2->symbolName<<" Redefined\n";
	}
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
	| '{'  statement_list '}' 
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
	|function_definition
	{
		$$ = $1 ;
		CurrentSymbolTable->print();
		$1->print();
		cout<<endl<<"\n-----------------------\n";

		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offsetStack.pop_back();
	}
	;

assignment_statement
	: ';'
	{
		$$ = new Ass(NULL, NULL);
	}								
	|  l_expression '=' expression ';'
	{
		TYPE* temp = validate($1,$3);
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast while assigning\n";
			exit(0);
		}
		
		if(!(*($1->type) == *($3->type)))
			$3 = new UnOp(getTypeCast($1->type), $3);
		
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
		$$->type = new TYPE(BASETYPE::INT);
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
		$$->type = new TYPE(BASETYPE::INT);
	}
	;

equality_expression
	: relational_expression
	{
		$$ = $1;
	} 
	| equality_expression EQ_OP relational_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::EQ_OP_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::EQ_OP_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	| equality_expression NE_OP relational_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::NE_OP_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::NE_OP_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	;
	
relational_expression
	: additive_expression
	{
		$$ = $1;
	}
	| relational_expression '<' additive_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::LT_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::LT_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	| relational_expression '>' additive_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::GT_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::GT_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	| relational_expression LE_OP additive_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::LE_OP_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::LE_OP_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	| relational_expression GE_OP additive_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::GE_OP_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::GE_OP_FLOAT, $1, $3);

		$$->type = new TYPE(BASETYPE::INT);
	}
	;

additive_expression 
	: multiplicative_expression
	{
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		}
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::PLUS_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::PLUS_FLOAT, $1, $3);

		$$->type = new TYPE(temp->basetype);

	}
	| additive_expression '-' multiplicative_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::MINUS_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::MINUS_FLOAT, $1, $3);

		$$->type = new TYPE(temp->basetype);
	}
	;

multiplicative_expression
	: unary_expression
	{
		$$ = $1;
	}
	| multiplicative_expression '*' unary_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::MULT_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::MULT_FLOAT, $1, $3);

		$$->type = new TYPE(temp->basetype);

	}
	| multiplicative_expression '/' unary_expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::DIV_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::DIV_FLOAT, $1, $3);

		$$->type = new TYPE(temp->basetype);

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
		$1->type = $2->type;
		if($2->type->basetype==BASETYPE::FLOAT && ((UnOp*)$1)->get_type()==UNOP_TYPE::UMINUS_INT){
			((UnOp*)$1)->set_type(UNOP_TYPE::UMINUS_FLOAT);
		}
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
		$$->type = SearchSymbolTable($1);
		if($$->type == NULL){
			cout<<"Error at Line "<<line_no<<" :"<<$1<<" Undefined\n";
			exit(0);
		}
	}
	| IDENTIFIER '(' expression_list ')'
	{
		$$ = $3;
		((FunCall*)$$)->set_name(new Identifier($1));
		$$->type = SearchSymbolTable($1);
		if($$->type == NULL){
			cout<<"Error at Line "<<line_no<<" :"<<$1<<" Undefined\n";
			exit(0);
		}
	}
	| l_expression INC_OP
	{
		if($1->type->basetype == BASETYPE::INT)
			$$ = new UnOp(UNOP_TYPE::PP_INT, $1);
		else if($1->type->basetype == BASETYPE::FLOAT)
			$$ = new UnOp(UNOP_TYPE::PP_FLOAT, $1);
		else{
			cout<<"Error at Line "<<line_no<<" : Incompatible expression type\n";
			exit(0);
		}
		$$->type = $1->type;
	}
	;

primary_expression
	: l_expression
	{
		$$ = $1;
	}
	| l_expression '=' expression
	{
		TYPE* temp = ExpAstTypeCast(&($1) , &($3));
		if(temp==NULL){
			cout<<"Error at Line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::ASSIGN_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::ASSIGN_FLOAT, $1, $3);

		$$->type = new TYPE(temp->basetype);
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
		$$->type = SearchSymbolTable($1);
		if($$->type == NULL){
			cout<<"Error at Line "<<line_no<<" :"<<$1<<" Undefined\n";
			exit(0);
		}
	}
	| l_expression '[' expression ']'
	{	
		if($1->type == NULL){
			cout<<"Error at Line "<<line_no<<" :Compiler's fault in error detection\n";
			exit(0);
		}
		if($1->type->child == NULL){
			cout<<"Error at Line "<<line_no<<" :Not an array\n";
			exit(0);
		}
		TYPE* temp = $1->type->child;

		if($3->type->basetype != BASETYPE::INT){
			cout<<"Error at Line "<<line_no<<" :Array Indices Not an Integer\n";
			exit(0);
		}
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
		
		$$->type = temp;
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
		$$ = new UnOp(UNOP_TYPE::UMINUS_INT);
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


type_specifier
	: VOID 	
	{
		$$ = new TYPE(BASETYPE::VOID);
		curr_type = $$;
	}
	| INT   
	{
		$$ = new TYPE(BASETYPE::INT);
		curr_type = $$;
	}
	| FLOAT 
	{
		$$ = new TYPE(BASETYPE::FLOAT);
		curr_type = $$;
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
	{
		bool check=CurrentSymbolTable->AddEntry($1->symbolName,$1);
		if(!check)
			cout<<"Error at Line "<<line_no<<" :"<<$1->symbolName<<" Redefined\n";
	}
	| declarator_list ',' declarator 
	{
		bool check=CurrentSymbolTable->AddEntry($3->symbolName,$3);
		if(!check)
			cout<<"Error at Line "<<line_no<<" :"<<$3->symbolName<<" Redefined\n";
	}
	;

declarator
	: IDENTIFIER
	{
		if(curr_type->basetype == BASETYPE::VOID){
			cout<<"Error at Line "<<line_no<<" : Void type declaration not allowed\n";
			exit(0);
		}
		$$ = new SymbolTableEntry;
		$$->symbolName = $1;
		$$->scope = current_scope;
		$$->type = curr_type;
		$$->size = curr_type->size;
		$$->vf = VAR_OR_FUNC::VAR;
		$$->offset = global_offset;
		global_offset += $$->size;
	} 
	| declarator '[' constant_expression ']' 
	{
		TYPE *temp =  new TYPE($1->type,((IntConst*)$3)->getVal());
		$$ = $1;
		$$->type = temp;
		global_offset -= $$->size;
		$$->size *=  temp->size;
		global_offset += $$->size;
	}
	;
