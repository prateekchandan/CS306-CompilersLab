%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token VOID INT FLOAT FLOAT_CONSTANT INT_CONSTANT AND_OP OR_OP
%token EQ_OP NE_OP LE_OP GE_OP STRING_LITERAL IF ELSE WHILE FOR RETURN IDENTIFIER INC_OP

%polymorphic eAst : ExpAst* ; sAst : StmtAst*; Int : int; Float : float; String : string; Type : TYPE*;
			symbolTableEntry : SymbolTableEntry*;

%type <eAst> expression l_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression postfix_expression logical_and_expression constant_expression expression_list unary_operator
%type <sAst> translation_unit function_definition compound_statement statement_list statement selection_statement iteration_statement assignment_statement 
%type <Int> INT_CONSTANT parameter_list
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
		
		// Print the AST when print_AST is enabled
		if(print_AST) $1->print();
		
		// Print Symbol table when print_symbol_table is enabled
		if(print_symbol_table){
			((BlockAst*)$1)->get_symbol_table()->print();
			cout<<"-----------------------\n";
		}

		$1->gen_code();
		
		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offset_multiplier = 1;
		offsetStack.pop_back();
	}
	| translation_unit function_definition
	{
		$$ = $1 ;
		
		// Print the AST when print_AST is enabled
		if(print_AST) $2->print();

		// Print Symbol table when print_symbol_table is enabled
		if(print_symbol_table){
			((BlockAst*)$2)->get_symbol_table()->print();
			cout<<"-----------------------\n";
		}
		
		$2->gen_code();
		
		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offset_multiplier = 1;
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
		temp->nested_level = SymbolTableStack.size();
		$$->table = temp;
		temp->returnType = curr_type;
		temp->parent = CurrentSymbolTable;

		bool check = CurrentSymbolTable->AddEntry($1,$$);
		if(!check)
			cout<<"Error at line "<<line_no<<" : Function "<<$1<<" Redefined\n";

		SymbolTableStack.push_back(CurrentSymbolTable);
		CurrentSymbolTable = temp;
		offsetStack.push_back(global_offset);
		global_offset = 8;
		offset_multiplier = 1;
	} 
	parameter_list ')'
	{
		current_scope = SCOPE::LOCAL;
		CurrentSymbolTable->return_offset = global_offset;
		global_offset = -4;
		offset_multiplier = -1;
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
		temp->nested_level = SymbolTableStack.size();
		$$->table = temp;
		temp->returnType = curr_type;
		temp->parent = CurrentSymbolTable;

		bool check=CurrentSymbolTable->AddEntry($1,$$);
		if(!check)
			cout<<"Error at line "<<line_no<<" : Function "<<$1<<" Redefined\n";

		SymbolTableStack.push_back(CurrentSymbolTable);
		offsetStack.push_back(global_offset);
		CurrentSymbolTable = temp;
		CurrentSymbolTable->return_offset = 8;
		global_offset = -4;
		offset_multiplier = -1;

		current_scope = SCOPE::LOCAL;
	}
	;

parameter_list
	: parameter_declaration
	{
		current_scope = SCOPE::PARAM;
		CurrentSymbolTable->arg_type_add(curr_type->basetype);
	}
	| parameter_list ',' parameter_declaration 
	{
		current_scope = SCOPE::PARAM;
		CurrentSymbolTable->arg_type_add(curr_type->basetype);
	}
	;

parameter_declaration
	: type_specifier declarator 
	{
		bool check=CurrentSymbolTable->AddEntry($2->symbolName,$2);
		if(!check)
			cout<<"Error at line "<<line_no<<" : "<<$2->symbolName<<" Redefined\n";
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
		$$ = new BlockAst(CurrentSymbolTable);
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
		StmtAst* temp_val = $1;
		$$ = new BlockAst(CurrentSymbolTable);
		((BlockAst*)$$)->add_statement(temp_val);
	}		
	| statement_list statement
	{
		$$ = $1;
		((BlockAst*)$$)->add_statement($2);
	}
	;

statement
	: '{' '}'
	{
		$$ = new Ass(NULL,NULL);
	}
	| '{' statement_list '}'
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
		TYPE* temp = is_compatible(CurrentSymbolTable->returnType,$2->type);
		TYPE* rtype = CurrentSymbolTable->returnType;
		
		if(rtype->child==NULL && rtype->basetype==BASETYPE::VOID){
			cout<<"Error at line "<<line_no<<" : Return not allowed for Void functions\n";
			exit(0);
		}
		if(temp==NULL || (temp->basetype != BASETYPE::INT && temp->basetype != BASETYPE::FLOAT)){
			cout<<"Error at line "<<line_no<<" : Incompatible Return Type\n";
			exit(0);
		}

		if(rtype->basetype != $2->type->basetype)
			$2 = new UnOp(getTypeCast(rtype),$2);
			
		$$ = new ReturnSt($2);
	}
	| IDENTIFIER '(' ')' ';'
    {
		$$ = new FunCallStmt(new Identifier($1));

		// ignore printf
		if($1 != "printf"){
			if(SearchSymbolTable($1,line_no) == NULL){
				cout<<"Error at line "<<line_no<<" : "<<$1<<" Undefined\n";
				exit(0);
			}
			int num = GetFuncParamCount($1);
			if(num != 0){
				cout<<"Error at line "<<line_no<<" : In call to function "<<$1<<": 0 Parameters passed but "<<num<<" required\n";
				exit(0);
			}
			SymbolTable* temp = GetSymbolTableForFunc($1);
			((FunCallStmt*)$$)->table = temp;
			if (temp == NULL){
				cout<<"Error at line "<<line_no<<" : SymbolTable for "<<$1<<" Undefined\n";
				exit(0);
			}
		}
	}
	| IDENTIFIER '(' expression_list ')' ';'
	{
		$$ = new FunCallStmt(new Identifier($1));
		((FunCallStmt*)$$)->set_expression_list(((FunCall*)$3)->get_expression_list());

		// ignore printf
		if($1 != "printf"){
			if(SearchSymbolTable($1,line_no) == NULL){
				cout<<"Error at line "<<line_no<<" : "<<$1<<" Undefined\n";
				exit(0);
			}
			int num = GetFuncParamCount($1);
			int num1 = ((FunCallStmt*)$$)->get_param_count();
			if(num != num1){
				cout<<"Error at line "<<line_no<<" :  In call to function "<<$1<<": "<<num1<<" Parameters passed but "<<num<<" required\n";
				exit(0);
			}
			validateBasetypes(GetFuncParamTypes($1),((FunCallStmt*)$$)->expression_list,line_no);
			SymbolTable* temp = GetSymbolTableForFunc($1);
			((FunCallStmt*)$$)->table = temp;
			if (temp == NULL){
				cout<<"Error at line "<<line_no<<" : SymbolTable for "<<$1<<" Undefined\n";
				exit(0);
			}
		}
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast while assigning\n";
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

		if($1->type->child != NULL || $3->type->child != NULL){
			cout<<"Error at line "<<line_no<<" : Unable to typecast Array\n";
			exit(0);
		}

		if(($1->type->basetype != BASETYPE::INT && $1->type->basetype != BASETYPE::FLOAT) || ($3->type->basetype != BASETYPE::INT && $3->type->basetype != BASETYPE::FLOAT)){
			cout<<"Error at line "<<line_no<<" : Unable to typecast String or Void\n";
			exit(0);
		}

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
		if($1->type->child != NULL || $3->type->child != NULL){
			cout<<"Error at line "<<line_no<<" : Unable to typecast Array\n";
			exit(0);
		}

		if(($1->type->basetype != BASETYPE::INT && $1->type->basetype != BASETYPE::FLOAT) || ($3->type->basetype != BASETYPE::INT && $3->type->basetype != BASETYPE::FLOAT)){
			cout<<"Error at line "<<line_no<<" : Unable to typecast String or Void\n";
			exit(0);
		}

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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
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
		$$ = new FunCall(new Identifier($1));
		$$->type = SearchSymbolTable($1,line_no);

		// ignore printf
		if($1 != "printf"){
			if($$->type == NULL){
				cout<<"Error at line "<<line_no<<" : "<<$1<<" Undefined\n";
				exit(0);
			}
			int num = GetFuncParamCount($1);
			if(num != 0){
				cout<<"Error at line "<<line_no<<" : In call to function "<<$1<<": 0 Parameters passed but "<<num<<" required\n";
				exit(0);
			}
			SymbolTable* temp = GetSymbolTableForFunc($1);
			((FunCall*)$$)->table = temp;
			if (temp == NULL){
				cout<<"Error at line "<<line_no<<" : SymbolTable for "<<$1<<" Undefined\n";
				exit(0);
			}
		}
	}
	| IDENTIFIER '(' expression_list ')'
	{
		$$ = $3;
		((FunCall*)$$)->set_name(new Identifier($1));
		$$->type = new TYPE(BASETYPE::INT);

		// ignore printf
		if($1 != "printf"){
			$$->type = SearchSymbolTable($1,line_no);
			if($$->type == NULL){
				cout<<"Error at line "<<line_no<<" : "<<$1<<" Undefined\n";
				exit(0);
			}

			int num = GetFuncParamCount($1);
			int num1 = ((FunCall*)$3)->get_param_count();
			if(num != num1){
				cout<<"Error at line "<<line_no<<" :  In call to function "<<$1<<": "<<num1<<" Parameters passed but "<<num<<" required\n";
				exit(0);
			}
			validateBasetypes(GetFuncParamTypes($1),((FunCall*)$$)->expression_list,line_no);
			SymbolTable* temp = GetSymbolTableForFunc($1);
			((FunCall*)$$)->table = temp;
			if (temp == NULL){
				cout<<"Error at line "<<line_no<<" : SymbolTable for "<<$1<<" Undefined\n";
				exit(0);
			}
		}
	}
	| l_expression INC_OP
	{
		TYPE *temp = $1->type;
		if($1->type->basetype == BASETYPE::INT)
			$$ = new UnOp(UNOP_TYPE::PP_INT, $1);
		else if($1->type->basetype == BASETYPE::FLOAT)
			$$ = new UnOp(UNOP_TYPE::PP_FLOAT, $1);
		else{
			cout<<"Error at line "<<line_no<<" : Incompatible expression type\n";
			exit(0);
		}
		$$->type = temp;
	}
	;

primary_expression
	: l_expression
	{
		$$ = $1;
	}
	| l_expression '=' expression
	{
		TYPE* temp1 = $1->type;
		TYPE* temp = ExpAstTypeCast(&($1) , &($3) , false);
		if(temp==NULL){
			cout<<"Error at line "<<line_no<<" : Unable to typecast\n";
			exit(0);
		};
		if(temp->basetype == BASETYPE::INT)
			$$ = new Op(OP_TYPE::ASSIGN_INT, $1, $3);
		else
			$$ = new Op(OP_TYPE::ASSIGN_FLOAT, $1, $3);
		if(temp1 == NULL)
		{
			cout<<"Error at line "<<line_no<<": Error fething type of l_expression\n";
			exit(0);
		}
		$$->type = new TYPE(temp1->basetype);
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
		$$->type = SearchSymbolTable($1,line_no,0);
		if($$->type == NULL){
			cout<<"Error at line "<<line_no<<" :"<<$1<<" Undefined\n";
			exit(0);
		}
		// defined_at for Identifier
		((Identifier*)$$)->defined_at = GetSymbolTableForIdentifier($1);

		if(CurrentSymbolTable->GetEntry($1) != NULL){
			$$->mem_offset = CurrentSymbolTable->GetEntry($1)->offset;
		}
	}
	| l_expression '[' expression ']'
	{	
		if($1->type == NULL){
			cout<<"Error at line "<<line_no<<" :Compiler's fault in error detection\n";
			exit(0);
		}
		if($1->type->child == NULL){
			cout<<"Error at line "<<line_no<<" :Not an array\n";
			exit(0);
		}
		TYPE* temp = $1->type->child;
		string name = ((Identifier*)$1)->get_id();

		if($3->type->basetype != BASETYPE::INT){
			cout<<"Error at line "<<line_no<<" :Array Indices Not an Integer\n";
			exit(0);
		}
		if(identifiers.find((ExpAst*)$1)!=identifiers.end()){
			// We are at base case
			$$ = new ArrayRef((Identifier*)$1);
			((ArrayRef*)$$)->add_index($3);
			if(CurrentSymbolTable->GetEntry(name) != NULL){
				$$->mem_offset = CurrentSymbolTable->GetEntry(name)->offset;
			}
			((ArrayRef*)$$)->defined_at = ((Identifier*)$1)->defined_at;
		}
		else{
			// We are recursive step
			$$ = new ArrayRef((ArrayRef*)$1);
			((ArrayRef*)$$)->add_index($3);
			((ArrayRef*)$$)->defined_at = ((ArrayRef*)$1)->defined_at;
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
	| function_definition
	{
		$$ = $1 ;
		
		// Print the AST when print_AST is enabled
		if(print_AST) $1->print();
		
		// Print Symbol table when print_symbol_table is enabled
		if(print_symbol_table){
			((BlockAst*)$1)->get_symbol_table()->print();
			cout<<"-----------------------\n";
		}
		
		$1->gen_code();

		// Restoring Environment
		CurrentSymbolTable = SymbolTableStack.back();
		SymbolTableStack.pop_back();
		global_offset = offsetStack.back();
		offset_multiplier = -1;
		offsetStack.pop_back();

	}
	;

declarator_list
	: declarator
	{
		bool check=CurrentSymbolTable->AddEntry($1->symbolName,$1);
		if(!check)
			cout<<"Error at line "<<line_no<<" :"<<$1->symbolName<<" Redefined\n";
	}
	| declarator_list ',' declarator 
	{
		bool check=CurrentSymbolTable->AddEntry($3->symbolName,$3);
		if(!check)
			cout<<"Error at line "<<line_no<<" :"<<$3->symbolName<<" Redefined\n";
	}
	;

declarator
	: IDENTIFIER
	{
		if(curr_type->basetype == BASETYPE::VOID){
			cout<<"Error at line "<<line_no<<" : Void type declaration not allowed\n";
			exit(0);
		}
		$$ = new SymbolTableEntry;
		$$->symbolName = $1;
		$$->scope = current_scope;
		$$->type = curr_type;
		$$->size = curr_type->size;
		$$->vf = VAR_OR_FUNC::VAR;
		$$->offset = global_offset;
		global_offset += $$->size * offset_multiplier;
	} 
	| declarator '[' constant_expression ']' 
	{
		TYPE* temp = validate($3,$3);
		int fault = 0;
		if(temp == NULL || temp->basetype == BASETYPE::FLOAT || ((IntConst*)$3)->getVal() < 1){
			cout<<"Error at line "<<line_no<<" : Array size is not a Positive Integer\n";
			exit(0);
		}
		temp =  new TYPE($1->type,((IntConst*)$3)->getVal());
		$$ = $1;
		$$->type = temp;
		global_offset -= $$->size * offset_multiplier;
		$$->size *=  temp->size;
		global_offset += $$->size * offset_multiplier;
	}
	;
