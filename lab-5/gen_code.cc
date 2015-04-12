/////////////////////////////////////////////////////////////////////////////
// Some global variables/functions required for code generation /////////////
/////////////////////////////////////////////////////////////////////////////

#define INF 1000000000

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
	
	public:
	stack<Reg> free_registers;					// Stack of free registers
	map<Reg,SymbolTableEntry*> assigned_to; 	// Map of register to the variable that it holds
	map<Reg,string> type_of;					// Map of register to the type of variable it holds
	
	// Constructor initializes the free regs stack
	reg_maintainer(){
		free_registers.push(edx);
		free_registers.push(ecx);
		free_registers.push(ebx);
		free_registers.push(eax);
		type_of[eax] = "";
		type_of[ebx] = "";
		type_of[ecx] = "";
		type_of[edx] = "";
	}
	
	// Get the top-most free register
	Reg get_top(){
		assert(free_registers.size()>0);
		return free_registers.top();
	}
	
	// Pop the top-most free register and return it
	Reg pop_top(){
		assert(free_registers.size()>0);
		Reg r = free_registers.top();
		free_registers.pop();
		return r;
	}
	
	// Push a register on top of the free-register stack
	void push_top(Reg r){
		free_registers.push(r);
	}
	
	// Swap the top 2 registers of the free-register stack
	void swap_top(){
		assert(free_registers.size()>1);
		Reg temp1 = free_registers.top();
		free_registers.pop();
		Reg temp2 = free_registers.top();
		free_registers.pop();
		free_registers.push(temp1);
		free_registers.push(temp2);
	}
	
	// Get free register count
	int free_reg_count(){
		return free_registers.size();
	}
};

reg_maintainer rm; 								// The register maintainer
vector<string> code;							// Vector of lines for the generated code
int line_to_be_printed = 0;						// Shows the line number to be printed next (in the code vector)
int line_num = 0;								// Line number where the next instruction of the code would come
SymbolTable *currentST;							// The current symbol table under consideration (used for variable lookup)

// For printing the code for functions as they are parsed //
void print_code(){
	cout<<endl;
	int i;
	for(i=line_to_be_printed; i<code.size(); i++){
		cout<<code[i]<<endl;
	}
	assert(line_to_be_printed<=code.size());
	line_to_be_printed = code.size();
}

// For setting current symbol table
void set_global_ST(SymbolTable *g){
	currentST = g;
}

////////////////////////////
///// Helper Functions /////
////////////////////////////

// Function to add a line of code to the global code vector

int add_line_to_code(string s, bool stmt_start){
	if(stmt_start){		// Add line number tag
		s = to_string(line_num)+":\t"+s;
	}
	else s = "\t"+s;
	code.push_back(s);
	line_num++;
	return line_num-1;
}

// Various polymorhic definitions of make_instruction function

string make_instr(string c, Reg r1, Reg r2){
	string s = c+"("+reg_names[r1]+", "+reg_names[r2]+");";
	return s;
}

string make_instr(string c, int i, Reg r){
	string s = c+"("+to_string(i)+", "+reg_names[r]+");";
	return s;
}

string make_instr(string c, float f, Reg r){
	string s = c+"("+to_string(f)+", "+reg_names[r]+");";
	return s;
}

string make_instr(string c, Reg r, int i){
	string s = c+"("+reg_names[r]+", "+to_string(i)+")";
	if(c.compare("ind")) s = s+";";
	return s;
}

string make_instr(string c, Reg r, string t){
	string s = c+"("+reg_names[r]+", "+t+");";
	return s;
}

string make_instr(string c, int i, string t){
	string s = c+"("+to_string(i)+", "+t+");";
	return s;
}

string make_instr(string c, float f, string t){
	string s = c+"("+to_string(f)+", "+t+");";
	return s;
}

string make_instr(string c, string t, Reg r){
	string s = c+"("+t+", "+reg_names[r]+");";
	return s;
}

string make_instr(string c, string t, int i){
	string s = c+"("+t+", "+to_string(i)+");";
	return s;
}

string make_instr(string c, Reg r){
	string s = c+"("+reg_names[r]+")";
	if(c.compare("ind")) s = s+";";
	return s;
}

string make_instr(string c, int i){
	string s = c+"("+to_string(i)+");";
	return s;
}

string make_instr(string c, float f){
	string s = c+"("+to_string(f)+");";
	return s;
}

string make_instr(string c, string t){
	string s;
	if(!c.compare("j") || !c.compare("je") || !c.compare("jl") || !c.compare("jg") ||
	!c.compare("jne") || !c.compare("jle") || !c.compare("jge"))
		s = c+"(\""+t+"\");";
	else s = c+"("+t+");";
	return s;
}

string make_instr(string c, char ch){
	string s = c+"('"+ch+"');";
	return s;
}

string make_instr(string c){
	string s = c+"();";
	return s;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of StmtAst class //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void BlockAst::gen_code(){
	SymbolTable *temp;
	bool changed_scope = (currentST != symbolTable);
	
	// If scope changed, means function definition has been encountered
	if(changed_scope){
		temp = currentST;
		currentST = symbolTable;			// set the current symbol table
		
		string fun_sig = "void "+currentST->get_name()+"()\n{";
		code.push_back(fun_sig);
		line_num++;
		
		string s = make_instr("pushi",ebp);
		add_line_to_code(s,false);
		s = make_instr("move",esp,ebp);
		add_line_to_code(s,false);
		// create space on stack for local variables here ////////////
		add_line_to_code("",false);
	}
	
	for(int i=0; i<statements.size(); i++){
		statements[i]->gen_code();
	}
	
	if(changed_scope){
		code.push_back("e:");
		line_num++;
		// pop off local variables from the stack here /////////////
		
		string s = make_instr("move",make_instr("ind",ebp),ebp);
		add_line_to_code(s,false);
		s = make_instr("popi",1);
		add_line_to_code(s,false);
		s = "\treturn;\n}\n";
		code.push_back(s);
		line_num++;
		
		currentST = temp;
	}
	return;
}

void Ass::gen_code(){
	if(left==NULL && right==NULL) return;					// Empty Statement
	assert(left->is_identifier || left->is_arrayref);		// Just check that LHS is an lval
	
	string dtype = (left->type->basetype==BASETYPE::INT)?"i":"f";
	string s;
	Reg r = rm.get_top();
	
	if(left->is_identifier){
		if(right->is_const){
			// If right side is constant, use immediate operation (also put that value in top register for return of this node)
			if(!dtype.compare("i")){
				s = make_instr("storei", right->vali, make_instr("ind",ebp,left->mem_offset));
				add_line_to_code(s,true);
				s = make_instr("move", right->vali, r);
			}
			else{
				s = make_instr("storef", right->valf, make_instr("ind",ebp,left->mem_offset));
				add_line_to_code(s,true);
				s = make_instr("move", right->valf, r);
			}
			add_line_to_code(s,true);
		}
		else{
			// Else first generate code for the right side
			right->gen_code();
			s = make_instr("store"+dtype, r, make_instr("ind",ebp,left->mem_offset));
			add_line_to_code(s,true);
		}
	}
	else if(left->is_arrayref){
		((ArrayRef*)left)->gen_code_addr();
		if(right->is_const){
			// If right side is constant, use immediate operation (also put that value in top register for return of this node)
			if(!dtype.compare("i")){
				s = make_instr("storei",right->vali, make_instr("ind",r));
				add_line_to_code(s,true);
				s = make_instr("move", right->vali, r);
			}
			else{
				s = make_instr("storef",right->valf, make_instr("ind",r));
				add_line_to_code(s,true);
				s = make_instr("move", right->valf, r);
			}
			add_line_to_code(s,true);
		}
		else{
			// Else save the lval index onto the stack & evaluate right side
			s = make_instr("push"+dtype,r); 	// save the index
			add_line_to_code(s,true);
			right->gen_code();					// generate code for rhs
			rm.pop_top();						// pop the top register r
			Reg l = rm.get_top();				// get the current top register
			rm.push_top(r);						// put back the register r on top
			s = make_instr("load"+dtype, make_instr("ind",esp), l); // load the saved index in current top register l
			add_line_to_code(s,true);
			s = make_instr("pop"+dtype, 1);		// pop the saved value from stack
			add_line_to_code(s,true);
			s = make_instr("store"+dtype, r, make_instr("ind",l)); // store the calculated rhs value in lhs's index
			add_line_to_code(s,true);
		}
	}
	else assert(0);
	
	add_line_to_code("",false);
	return;
}
	
void ReturnSt::gen_code(){
	assert(currentST->returnType->basetype!=BASETYPE::VOID);

	string s;
	Reg r = rm.get_top();			// Top register (will have the answer of evaluated exp)
	string dtype = (exp->type->basetype==BASETYPE::INT)?"i":"f";
	
	exp->gen_code();

	s = make_instr("store"+dtype,r,make_instr("ind",ebp,currentST->return_offset));
	add_line_to_code(s,true);
	s = make_instr("j","e");
	add_line_to_code(s,false);
	
	add_line_to_code("",false);
	return;
}

void If::gen_code(){
	cond->gen_code();
	statement1->gen_code();
	statement2->gen_code();
	
	add_line_to_code("",false);
	return;
}

void While::gen_code(){
	cond->gen_code();
	statement->gen_code();
	
	add_line_to_code("",false);
	return;
}

void For::gen_code(){
	init->gen_code();
	cond->gen_code();
	statement->gen_code();
	step->gen_code();
	
	add_line_to_code("",false);
	return;
}

void FunCallStmt::gen_code(){
	
	string s;
	Reg r = rm.get_top();					// Top register (will have to evaluate answer in this, if FunCall)
	vector<int> arg_types;					// for storing list of types of arguments (0=int, 1=float)
	bool is_printf = (!name->get_id().compare("printf")); 	// Check if function is printf
	
	////////////////////////////////////////////////////
	// Save the registers which are in use (TODO) //////
	////////////////////////////////////////////////////
	
	// Note: No need to push space for return value if FunCallStmt //
	
	// Evaluate arguments from right to left and push onto the stack
	for(int i=expression_list.size()-1; i>=0; i--){
		bool is_int = (expression_list[i]->type->basetype == BASETYPE::INT);
		bool is_string = (expression_list[i]->type->basetype == BASETYPE::STRING);
		
		if(expression_list[i]->is_const){
			if(is_string){
				assert(is_printf);
				s = make_instr("print_string",((StringConst*)expression_list[i])->getVal());
			}
			if(is_int){
				if(is_printf) s = make_instr("print_int",expression_list[i]->vali);
				else s = make_instr("pushi",expression_list[i]->vali);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) s = make_instr("print_float",expression_list[i]->valf);
				else s = make_instr("pushf",expression_list[i]->valf);
				arg_types.push_back(1);
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) s = make_instr("print_int",r);
				else s = make_instr("pushi",r);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) s = make_instr("print_float",r);
				else s = make_instr("pushf",r);
				arg_types.push_back(1);
			}
		}
		add_line_to_code(s,true);
	}

	// Return if the function is printf, as it's job is over
	if(is_printf){
		add_line_to_code("",false);
		return;
	}
	
	// Push the static link (TODO)
	s = make_instr("pushi",0);
	add_line_to_code(s,true);
	
	// Make a call to the function
	s = make_instr(name->get_id());
	add_line_to_code(s,true);

	
	// Pop static link & parameters from the stack (initial count=1 is for popping the static link)
	int mode = 0;								// 0 means int, 1 means float;
	string dtype = (mode?"f":"i");
	int count = 1;								// Count of consecutive parameters on the stack of same mode
	for(int i=arg_types.size()-1; i>=0; i--){
		
		if(arg_types[i]==mode) count++;
		else{
			if(count!=0){
				s = make_instr("pop"+dtype,count);
				add_line_to_code(s,true);
			}
			mode = arg_types[i];
			dtype = (mode?"f":"i");
			count = 1;
		}
	}
	if(count!=0){
		s = make_instr("pop"+dtype,count);
		add_line_to_code(s,true);
	}

	////////////////////////////////////////////////////
	// Load the registers which were saved (TODO) //////
	////////////////////////////////////////////////////
	
	add_line_to_code("",false);
	return;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of ExpAst class ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void Identifier::gen_code(){

	assert(type->child == NULL);					// Check that it is not an array type	
	Reg r = rm.get_top();							// Get the top-most free register
	
	if(type->basetype == BASETYPE::INT){			// If type is int, use loadi
		add_line_to_code(make_instr("loadi",make_instr("ind",ebp,mem_offset),r), true);
	}
	else if(type->basetype == BASETYPE::FLOAT){	// If type is float, use loadf
		add_line_to_code(make_instr("loadf",make_instr("ind",ebp,mem_offset),r), true);
	}
	else assert(0);
	
	reg_addr = r;									// Set the address attribute of the node
	return;
}

void Op::gen_code(){

	if(op_type==OR_OP || op_type==AND_OP){
		return;
	}
	if(op_type==ASSIGN_INT || op_type==ASSIGN_FLOAT){
		
		string dtype = (op_type==ASSIGN_INT) ? "i":"f";
		string s;
		
		if(left->is_identifier){
			Reg r = rm.get_top();
			if(right->is_const){
				// If right side is constant, use immediate operation (also put that value in top register for return of this node)
				if(op_type==ASSIGN_INT){
					s = make_instr("storei", right->vali, make_instr("ind",ebp,left->mem_offset));
					add_line_to_code(s,true);
					s = make_instr("move", right->vali, r);
				}
				else{
					s = make_instr("storef", right->valf, make_instr("ind",ebp,left->mem_offset));
					add_line_to_code(s,true);
					s = make_instr("move", right->valf, r);
				}
				add_line_to_code(s,true);
			}
			else{
				// Else first generate code for the right side
				right->gen_code();
				s = make_instr("store"+dtype, r, make_instr("ind",ebp,left->mem_offset));
				add_line_to_code(s,true);
			}
			// Now mark that the value of this node is kept on the top free register
			reg_addr = r;
		}
		else if(left->is_arrayref){
			((ArrayRef*)left)->gen_code_addr();
			Reg r = rm.get_top();
			if(right->is_const){
				// If right side is constant, use immediate operation (also put that value in top register for return of this node)
				if(op_type==ASSIGN_INT){
					s = make_instr("storei",right->vali, make_instr("ind",r));
					add_line_to_code(s,true);
					s = make_instr("move", right->vali, r);
				}
				else{
					s = make_instr("storef",right->valf, make_instr("ind",r));
					add_line_to_code(s,true);
					s = make_instr("move", right->valf, r);
				}
				add_line_to_code(s,true);
			}
			else{
				// Else save the lval index onto the stack & evaluate right side
				s = make_instr("push"+dtype,r); 	// save the index
				add_line_to_code(s,true);
				right->gen_code();					// generate code for rhs
				rm.pop_top();						// pop the top register r
				Reg l = rm.get_top();				// get the current top register
				rm.push_top(r);						// put back the register r on top
				s = make_instr("load"+dtype, make_instr("ind",esp), l); // load the saved index in current top register l
				add_line_to_code(s,true);
				s = make_instr("pop"+dtype, 1);		// pop the saved value from stack
				add_line_to_code(s,true);
				s = make_instr("store"+dtype, r, make_instr("ind",l)); // store the calculated rhs value in lhs's index
				add_line_to_code(s,true);
			}
			reg_addr = r;
		}
		else assert(0);
		return;
	}
	// comparator ops have to be filled in here //
	if(op_type==PLUS_INT || op_type==PLUS_FLOAT || op_type==MULT_INT || op_type==MULT_FLOAT ||
		op_type==MINUS_INT || op_type==MINUS_FLOAT || op_type==DIV_INT || op_type==DIV_FLOAT){
		
		assert(!is_const);
		string op, dtype;
		
		// Find the suitable op
		if(op_type==PLUS_INT || op_type==PLUS_FLOAT || 
			op_type==MINUS_INT || op_type==MINUS_FLOAT) op = "add";
		else if(op_type==MULT_INT || op_type==MULT_FLOAT) op = "mul";
		else op = "div";
		
		// Find the suitable datatype
		if(op_type==PLUS_INT || op_type==MULT_INT || op_type==MINUS_INT || op_type==DIV_INT) dtype="i";
		else dtype = "f";
		
		// Booleans for checking if sub or div operation
		bool is_sub = (op_type==MINUS_INT || op_type==MINUS_FLOAT);
		bool is_div = (op_type==DIV_INT || op_type==DIV_FLOAT);
		string s;
		
		if(left->is_const){				// If Left register has constant value
			right->gen_code();
			Reg r = rm.get_top();
			if(!dtype.compare("i")){
				if(is_sub){
					s = make_instr("muli", -1, r);
					add_line_to_code(s,true);
				}
				s = make_instr(op+dtype,left->vali,r);
			}
			else{
				if(is_sub){
					s = make_instr("mulf", -1.0f, r);
					add_line_to_code(s,true);
				}
				s = make_instr(op+dtype,left->valf,r);
			}
			add_line_to_code(s,true);
			reg_addr = r;
			return;
		}
		if(right->is_const){			// If Right register has constant value
			Reg r = rm.get_top();
			if(!dtype.compare("i")){
				if(is_div){
					rm.swap_top();		// Swap registers and evaluate left side if div operation
					Reg l = rm.get_top();
					left->gen_code();
					rm.swap_top();
					s = make_instr("move",right->vali,r);
					add_line_to_code(s,true);
					s = make_instr("divi",l,r);
				}
				else{
					left->gen_code();	// Evaluate left side first if not div operation
					if(is_sub) s = make_instr(op+dtype,-right->vali,r);
					else s = make_instr(op+dtype,right->vali,r);
				}
			}
			else{
				if(is_div){
					rm.swap_top();
					Reg l = rm.get_top();
					left->gen_code();
					rm.swap_top();
					s = make_instr("move",right->valf,r);
					add_line_to_code(s,true);
					s = make_instr("divf",l,r);
				}
				else{
					left->gen_code();	// Evaluate left side first if not div operation
					if(is_sub) s = make_instr(op+dtype,-right->valf,r);
					else s = make_instr(op+dtype,right->valf,r);
				}
			}
			add_line_to_code(s,true);
			reg_addr = r;
			return;
		}

		// Both sides have non-constant values //
		if(rm.free_reg_count()>2 || left->is_identifier){		// If Current node has more than 2 registers ||
			right->gen_code();									// Current node has 2 registers, but left is identifier
			Reg r = rm.pop_top();
			left->gen_code();
			Reg l = rm.get_top();
			rm.push_top(r);
			
			if(is_sub){												// negate the value of right register if sub operation
				if(!dtype.compare("i")){
					s = make_instr("muli", -1, r);
					add_line_to_code(s,true);
				}
				else{
					s = make_instr("mulf", -1.0f, r);
					add_line_to_code(s,true);
				}
			}
			
			s = make_instr(op+dtype,l,r);
			add_line_to_code(s,true);
			reg_addr = r;
		}
		else{													// Current node has exactly 2 registers & left is not identifier
			right->gen_code();
			Reg r = rm.get_top();
			s = make_instr("push"+dtype,r);						// gen code and store right part on stack
			add_line_to_code(s,true);
			rm.swap_top();											// swap top 2 regs of reg-stack
			left->gen_code();										// gen code for left side
			Reg l = rm.get_top();
			s = make_instr("load"+dtype,make_instr("ind",esp),r);	// load back right answer from stack
			add_line_to_code(s,true);
			s = make_instr("pop"+dtype,1);							// pop the stored value from stack
			add_line_to_code(s,true);
			
			if(is_sub){												// negate the value of right register if sub operation
				if(!dtype.compare("i")){
					s = make_instr("muli", -1, r);
					add_line_to_code(s,true);
				}
				else{
					s = make_instr("mulf", -1.0f, r);
					add_line_to_code(s,true);
				}
			}
			
			s = make_instr(op+dtype,l,r);							// r <- (l op r)
			add_line_to_code(s,true);
			rm.swap_top();											// swap back the top 2 registers into original position
			reg_addr = r;
		}
		return;
	}
}

void UnOp::gen_code(){
	if(op_type==UMINUS_INT || op_type==UMINUS_FLOAT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		string s;
		
		if(op_type==UMINUS_INT) s = make_instr("muli",-1,r);
		else s = make_instr("mulf",-1.0f,r);
		add_line_to_code(s,true);
		reg_addr = r;
		return;
	}
	if(op_type==TO_INT || op_type==TO_FLOAT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		string s;
		
		if(op_type==TO_INT) s = make_instr("floatToint",r);
		else s = make_instr("intTofloat",r);
		add_line_to_code(s,true);
		reg_addr = r;
		return;
	}
	if(op_type==PP_INT || op_type==PP_FLOAT){
		exp->gen_code();
		
		Reg r = rm.get_top();
		string s;
		string dtype = (op_type==PP_INT) ? "i" : "f";
		
		// If lhs is identifier, then r contains its value in it
		if(exp->is_identifier){
			if(op_type==PP_INT) s = make_instr("addi",1,r);
			else s = make_instr("addf",1.0f,r);
			add_line_to_code(s,true);
			s = make_instr("store"+dtype, r, make_instr("ind",ebp,exp->mem_offset));
			add_line_to_code(s,true);
		}
		// But if lhs is arrayref, then r now contains its memory address (viz. pointer)
		else if(exp->is_arrayref){
			rm.pop_top();											// pop register r (contains mem. pointer)
			Reg l = rm.get_top();									// get top register l
			rm.push_top(r);											// place back r on top
			s = make_instr("load"+dtype, make_instr("ind",r), l);	// l <- *(r)
			add_line_to_code(s,true);
			if(op_type==PP_INT) s = make_instr("addi",1,r);			// l <- l + 1
			else s = make_instr("addf",1.0f,r);
			add_line_to_code(s,true);
			s = make_instr("store"+dtype,l, make_instr("ind",r));	// *(r) <- l
			add_line_to_code(s,true);
			s = make_instr("move",l,r);								// r <- l (preparing value in r for return from this node)
			add_line_to_code(s,true);
		}
		else assert(0);
		
		reg_addr = r;
		return;
	}
	if(op_type==NOT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		string s;
		string dtype = (type->basetype==BASETYPE::INT) ? "i" : "f";
		
		s = make_instr("cmp"+dtype,0,r);
		add_line_to_code(s,true);
		s = make_instr("je",to_string(line_num+3));
		add_line_to_code(s,true);
		s = (!dtype.compare("i")) ? make_instr("move",0,r) : make_instr("move",0.0f,r);
		add_line_to_code(s,true);
		s = make_instr("j",to_string(line_num+2));
		add_line_to_code(s,true);
		s = (!dtype.compare("i")) ? make_instr("move",1,r) : make_instr("move",1.0f,r);
		add_line_to_code(s,true);
		reg_addr = r;
		return;
	}
}

void FunCall::gen_code(){
	
	Reg r = rm.get_top();				// Top register ( will have to evaluate asnwer in this)
	string s;
	vector<int> arg_types;									// for storing list of types of arguments
	
	int ret_type = type->basetype;							// return type of this function expression
	assert(ret_type!=BASETYPE::VOID);
	string dtype = (ret_type==BASETYPE::INT) ? "i" : "f";
	
	bool is_printf = (!name->get_id().compare("printf")); 	// Check if function is printf
	
	////////////////////////////////////////////////////
	// Save the registers which are in use (TODO) //////
	////////////////////////////////////////////////////
	
	// Push space for return value (if not printf function)
	if(!printf){
		s = make_instr("push"+dtype,1);
		add_line_to_code(s,true);
	}
	
	// Evaluate arguments from right to left and push onto the stack
	for(int i=expression_list.size()-1; i>=0; i--){
		bool is_int = (expression_list[i]->type->basetype == BASETYPE::INT);
		bool is_string = (expression_list[i]->type->basetype == BASETYPE::STRING);
		
		if(expression_list[i]->is_const){
			if(is_string){
				assert(is_printf);
				s = make_instr("print_string",((StringConst*)expression_list[i])->getVal());
			}
			else if(is_int){
				if(is_printf) s = make_instr("print_int",expression_list[i]->vali);
				else s = make_instr("pushi",expression_list[i]->vali);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) s = make_instr("print_float",expression_list[i]->valf);
				else s = make_instr("pushf",expression_list[i]->valf);
				arg_types.push_back(1);
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) s = make_instr("print_int",r);
				else s = make_instr("pushi",r);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) s = make_instr("print_float",r);
				else s = make_instr("pushf",r);
				arg_types.push_back(1);
			}
		}
		add_line_to_code(s,true);
	}
	
	// Return if the function is printf, as it's job is over
	if(is_printf){
		s = make_instr("move",0,r);
		add_line_to_code(s,true);
		return;
	}
	
	// Push the static link (TODO)
	s = make_instr("pushi",0);
	add_line_to_code(s,true);
	
	// Make a call to the function
	s = make_instr(name->get_id());
	add_line_to_code(s,true);
	
	// Pop static link & parameters from the stack (initial count=1 is for popping the static link)
	int mode = 0;								// 0 means int, 1 means float;
	string dtype1 = (mode?"f":"i");
	int count = 1;								// Count of consecutive parameters on the stack of same mode
	for(int i=arg_types.size()-1; i>=0; i--){
		
		if(arg_types[i]==mode) count++;
		else{
			if(count!=0){
				s = make_instr("pop"+dtype1,count);
				add_line_to_code(s,true);
			}
			mode = arg_types[i];
			dtype1 = (mode?"f":"i");
			count = 1;
		}
	}
	if(count!=0){
		s = make_instr("pop"+dtype1,count);
		add_line_to_code(s,true);
	}
	
	// Pick up the return value from stack and pop it off
	if(ret_type==BASETYPE::INT){
		s = make_instr("load"+dtype,make_instr("ind",esp),r);
		add_line_to_code(s,true);
		s = make_instr("pop"+dtype,1);
		add_line_to_code(s,true);
	}

	////////////////////////////////////////////////////
	// Load the registers which were saved (TODO) //////
	////////////////////////////////////////////////////
	
	reg_addr = r;
	return;
}

void FloatConst::gen_code(){ assert(0);}
void IntConst::gen_code(){ assert(0);}
void StringConst::gen_code(){ assert(0);}

// This function should return value of the array ref in the top register r
void ArrayRef::gen_code(){
	
	gen_code_addr();					// First call gen_code_addr to get pointer to the required location
	Reg r = rm.get_top();				// Top register (currently contains pointer to the required value)
	string dtype = (type->basetype==BASETYPE::INT)?"i":"f";
	
	string s = make_instr("load"+dtype,make_instr("ind",r),r); // r <- *(r)
	add_line_to_code(s,true);
	reg_addr = r;
	return;
}

// This function should return address of the array ref in the top register r
void ArrayRef::gen_code_addr(){
	
	Reg r = rm.get_top();				// Top register (will have to evaluate address of array ref in this)
	string s;
	vector<int> dimension;				// for storing dimensions of array
	
	TYPE *t = currentST->GetEntry(name->get_id())->type->child; 	// start from the 2nd dimension (from left)
	assert(t!=NULL);												// assert that this is not a base type
	for(; t != NULL; t = t->child) dimension.push_back(t->size); 	// keep collecting the dimensions towards right
	int indc = indices.size()-1;
	
	// Now find out products of sizes starting from right end (for use later)
	for(int i=dimension.size()-2; i>=0;i--) dimension[i] *= dimension[i+1];
	
	// Case where you have > 2 free registers
	if(rm.free_reg_count()>2){
		
		// Evaluate the rightmost index using all registers
		if(indices[indc]->is_const)		// If constant, use move
			s = make_instr("move",dimension[indc]*indices[indc]->vali,r);
		else{							// Else, evaluate & multiply with sizeof(basetype) viz. dimension[indc]
			indices[indc]->gen_code();
			s = make_instr("muli",dimension[indc],r);
		}
		add_line_to_code(s,true);
		
		rm.pop_top();			// Pop top register and keep it for accumulating sum
		Reg l = rm.get_top();	// l = new top register
		
		// Now start evaluating from second last index
		for(int i=indc-1; i>=0; i--){
			if(indices[i]->is_const){
				s = make_instr("addi",(indices[i]->vali)*dimension[i],r);	// r <- r + dim[i]*ind[i]
			}
			else{
				indices[i]->gen_code();
				s = make_instr("muli",dimension[i],l);						// l <- l*dim[i]
				add_line_to_code(s,true);
				s = make_instr("addi",l,r);									// r <- l + r
			}	
			add_line_to_code(s,true);
		}
	}
	
	// Case where you have = 2 free registers
	else{
		s = make_instr("pushi",0);		// Accumulate the sum on a stack location & initialize it to 0
		add_line_to_code(s,true);
		
		// Now start evaluating from last index
		for(int i=indc; i>=0; i--){
			if(indices[i]->is_const){
				s = make_instr("loadi",make_instr("ind",esp),r);			// r <- *(esp)
				add_line_to_code(s,true);
				s = make_instr("addi",(indices[i]->vali)*dimension[i],r);	// r <- r + dim[i]*ind[i]
				add_line_to_code(s,true);
				s = make_instr("stori",r,make_instr("ind",esp));			// *(esp) <- r
			}
			else{
				indices[i]->gen_code();										// r <- ind[i]
				s = make_instr("muli",dimension[i],r);						// r <- dimension[i]*r
				add_line_to_code(s,true);
				
				rm.pop_top();												// pop(r)
				Reg l = rm.get_top();										// l = get new top register
				rm.push_top(r);												// push(r)
				
				s = make_instr("loadi",make_instr("ind",esp),l);			// l <- *(esp)
				add_line_to_code(s,true);
				s = make_instr("addi",r,l);									// l <- r + l
				add_line_to_code(s,true);
				s = make_instr("stori",l,make_instr("ind",esp));			// *(esp) <- l
			}	
			add_line_to_code(s,true);
		}
	}
	
	s = make_instr("muli",-1,r);			// Negate r, as the index grows towards lower addresses of stack
	add_line_to_code(s,true);
	s = make_instr("addi",mem_offset,r);	// Add mem_offset of array-base wrt ebp
	add_line_to_code(s,true);
	s = make_instr("addi",ebp,r);			// Get absolute address by r <- ebp + r
	add_line_to_code(s,true);
	
	reg_addr = r;
	return;
}
