set<ExpAst*> identifiers;
bool seen_identifier = false;

string operator_map[] = {"OR" , "AND" ,"EQ_OP_INT" , "EQ_OP_FLOAT" , "NE_OP_INT" , 
"NE_OP_FLOAT" , "LT_INT" , "LT_FLOAT" , "LE_OP_INT" , "LE_OP_FLOAT" , "GT_INT" , 
"GT_FLOAT" , "GE_OP_INT" , "GE_OP_FLOAT" , "PLUS_INT" , "PLUS_FLOAT" , "MINUS_INT" , 
"MINUS_FLOAT" , "MULT_INT" , "MULT_FLOAT" , "DIV_INT" , "DIV_FLOAT" , "ASSIGN_INT" , 
"ASSIGN_FLOAT" , "UMINUS_INT" , "UMINUS_FLOAT" , "NOT" , "INC_OP_INT" , "INC_OP_FLOAT",
"TO_INT" , "TO_FLOAT"} ;

// Functions for BlockAst //

void BlockAst::add_statement(StmtAst *e){
	statements.push_back(e);
}

void BlockAst::print(){
	cout<<"(Block [";
	int count = statements.size();
	for(int i=0; i<count; i++){
		statements[i]->print();
		if(i != count-1)
			cout<<endl;
	}
	cout<<"])";
}
	
// Print Function for Assignment statement //

void Ass::print(){
	if(left==NULL){
		cout<<"(Empty)";
		return;
	}
	cout<<"(Ass ";
	left->print();
	cout<<" ";
	right->print();
	cout<<")";
}

// Print Function for Return statement //

void ReturnSt::print(){
	cout<<"(Return ";
	exp->print();
	cout<<")";
}

// Print Function for If statement //

void If::print(){
	cout<<"(If ";
	cond->print();
	cout<<endl;
	statement1->print();
	cout<<endl;
	statement2->print();
	cout<<")";
}

// Print Function for While statement //

void While::print(){
	cout<<"(While ";
	cond->print();
	cout<<endl;
	statement->print();
	cout<<")";
}

// Print Function for For statement //

void For::print(){
	cout<<"(For ";
	init->print();
	cout<<endl;
	cond->print();
	cout<<endl;
	step->print();
	cout<<endl;
	statement->print();
	cout<<")";
}

// Print Function for Op statement //

void Op::print(){
	cout<<"("<<operator_map[op_type]<<" ";
	left->print();
	cout<<" ";
	right->print();
	cout<<")";
}

// Functions for UnOp statement //

void UnOp::set_expression(ExpAst *e){
	exp = e;
}

void UnOp::print(){
	cout<<"("<<operator_map[op_type]<<" ";
	exp->print();
	cout<<")";
}

// Print Function for Identifier //

void Identifier::print(){
	cout<<"(Id \""<<id<<"\")";
}
	
// Print & some other functions for FunCall //

void FunCall::print(){
	cout<<"(";
	name->print();
	for(int i=0; i<expression_list.size(); i++){
		cout<<" ";
		expression_list[i]->print();
	}
	cout<<")";
}

void FunCall::set_name(Identifier *i){
	name = i;
}

void FunCall::add_expression(ExpAst *e){
	expression_list.push_back(e);
}

// Print Function for Float Const //

void FloatConst::print(){
	cout<<"(FloatConst "<<val<<")";
}

// Print Function for Int Const //

void IntConst::print(){
	cout<<"(IntConst "<<val<<")";
}

// Print Function for String Const //

void StringConst::print(){
	cout<<"(StringConst "<<val<<")";
}

// Functions for ArrayRef //

void ArrayRef::add_index(ExpAst *e){
	indices.push_back(e);
}

void ArrayRef::print(){
	cout<<"(ArrayRef ";
	name->print();
	for(int i=0; i<indices.size(); i++){
		cout<<" ";
		indices[i]->print();
	}
	cout<<")";
}


/********* GLOBALS FOR SYMBOL TABLE ***********/
SCOPE current_scope = SCOPE::GLOBAL;		// global Scope variable

SymbolTable* CurrentSymbolTable = new SymbolTable("Global");
vector<SymbolTable*> SymbolTableStack(1,CurrentSymbolTable);
vector<int> offsetStack(1,0);

TYPE* curr_type;

int global_offset = 0;



/*********** FUNCTIONS FOR TYPE CHECKING ***********/

// Validates if typecasting is possible
TYPE* is_compatible(TYPE* a , TYPE* b){
	if(a->child!=NULL || b->child != NULL)
		return NULL;
	if(a->basetype == BASETYPE::FLOAT && (b->basetype == BASETYPE::FLOAT || b->basetype == BASETYPE::INT))
		return a;
	if(b->basetype == BASETYPE::FLOAT && (a->basetype == BASETYPE::FLOAT || a->basetype == BASETYPE::INT))
		return b;
	if(a->basetype == BASETYPE::INT && b->basetype == BASETYPE::INT)
		return a;
	else return NULL;
}

bool operator==(TYPE a , TYPE b){
	if(a.child!=NULL || b.child != NULL)
		return false;
	return a.basetype == b.basetype;
}

// Validates if type check is possible , exits otherwise
TYPE* validate(ExpAst* a , ExpAst *b){
	TYPE* temp = is_compatible(a->type,b->type);
	if(temp == NULL){
		cout<<"Error at Line "/*<<line_no<<*/" : Incompatible Types\n";
		exit(0);
	}
	return temp;
}

// get typecasting operator
UNOP_TYPE getTypeCast(TYPE* a){
	if(a->basetype == BASETYPE::INT)
		return UNOP_TYPE::TO_INT;
	if(a->basetype == BASETYPE::FLOAT)
		return UNOP_TYPE::TO_FLOAT;

	cout<<"Error at Line "/*<<line_no<<*/" : Unable to Type cast\n";
	exit(0);
}

TYPE* ExpAstTypeCast(ExpAst **a,ExpAst **b){
	TYPE* type = validate(*a,*b);
	if((*a)->type->basetype != type->basetype)
		*a = new UnOp(getTypeCast(type), *a);

	if((*b)->type->basetype != type->basetype)
		*b = new UnOp(getTypeCast(type), *b);

	return type;
}	

TYPE* SearchSymbolTable(string s){
	SymbolTable* temp = CurrentSymbolTable;
	SymbolTableEntry* ret = temp->GetEntry(s);
	if(ret!=NULL)
		return ret->type;
	for (int i = SymbolTableStack.size()-1; i >= 0  ; --i)
	{
		temp = SymbolTableStack[i];
		ret = temp->GetEntry(s);
		if(ret!=NULL)
			return ret->type;
	}
	return NULL;
}