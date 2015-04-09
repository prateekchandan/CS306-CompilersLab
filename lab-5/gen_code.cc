/////////////////////////////////////////////////////////////////////////////
// Some global variables/functions required for code generation /////////////
/////////////////////////////////////////////////////////////////////////////

// enum for registers
enum Reg {
	eax, 
	ebx, 
	ecx, 
	edx, 
	esp, 
	ebp
};

// Map of register number to its string
string reg_names[] = {"eax", "ebx", "ecx", "edx", "esp", "ebp"};

// Class for maintenance of registers and their mappings
class reg_maintainer {
	
	set<Reg> free_registers;					// Stack of free registers
	map<Reg,SymbolTableEntry*> assigned_to; 	// Map of register to the variable that it holds
	map<Reg,string> type_of;					// Map of register to the type of variable it holds
	
	public:
	// Constructor initializes the free regs stack
	reg_maintainer(){
		free_registers.insert(eax);
		free_registers.insert(ebx);
		free_registers.insert(ecx);
		free_registers.insert(edx);
		type_of[eax] = "";
		type_of[ebx] = "";
		type_of[ecx] = "";
		type_of[edx] = "";
	}
};

reg_maintainer rm; 			// The register maintainer
vector<string> code;		// Vector of lines for the generated code
int line_num = 1;			// Line number where the next instruction of the code would come

////////////////////////////
///// Helper Functions /////
////////////////////////////

void add_line_to_code(string s, bool stmt_start){
	// Add a line_number tag only at the start of a statement
	if(stmt_start){
		s = "l"+to_string(line_num)+": "+s;
	}
	code.push_back(s);
	line_num++;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of StmtAst class //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void BlockAst::gen_code(){
	for(int i=0; i<statements.size(); i++){
		statements[i]->gen_code();
	}
}

void Ass::gen_code(){
	if(left==NULL && right==NULL) return;		// Empty Statement
	assert(left->is_lval);						// Just check that LHS is an lval
	right->gen_code();
}
	
void ReturnSt::gen_code(){
	exp->gen_code();
}

void If::gen_code(){
	cond->gen_code();
	statement1->gen_code();
	statement2->gen_code();
}

void While::gen_code(){
	cond->gen_code();
	statement->gen_code();
}

void For::gen_code(){
	init->gen_code();
	cond->gen_code();
	statement->gen_code();
	step->gen_code();
}

void FunCallStmt::gen_code(){
	for(int i = 0; i<expression_list.size(); i++){
		expression_list[i]->gen_code();
	}
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of ExpAst class ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void Identifier::gen_code(){
	
}

void Op::gen_code(){

}

void UnOp::gen_code(){
}

void FunCall::gen_code(){
}

void FloatConst::gen_code(){
}

void IntConst::gen_code(){
}

void StringConst::gen_code(){
}

void ArrayRef::gen_code(){
}
