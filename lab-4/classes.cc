set<ExpAst*> identifiers;
bool seen_identifier = false;

string operator_map[] = {"OR" , "AND" , "EQ_OP" , "NE_OP" , "LT" , 
	"LE_OP" , "GT" , "GE_OP" , "PLUS" , "MINUS" ,"MULT" , "DIV", "ASSIGN",
	"UMINUS" , "NOT" , "INC_OP"} ;

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

SCOPE current_scope = SCOPE::GLOBAL;
