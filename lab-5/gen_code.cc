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

// Some forward declarations
void make_instr(string c, string t, Reg r);
void make_instr(string c, Reg r);
void make_instr(string c, int i);
string make_index(Reg r);
string make_index(Reg r, int offset);

// Class for maintenance of registers and their mappings
class reg_maintainer {
	
	public:
	vector<Reg> free_registers;					// Stack of free registers
	map<Reg,SymbolTableEntry*> assigned_to; 	// Map of register to the variable that it holds
	map<Reg,int> is_int_type;					// Map of register to bool indicating if register holds int value
	
	// Constructor initializes the free regs stack
	reg_maintainer(){
		free_registers.push_back(edx);
		free_registers.push_back(ecx);
		free_registers.push_back(ebx);
		free_registers.push_back(eax);
		is_int_type[eax] = 1;
		is_int_type[ebx] = 1;
		is_int_type[ecx] = 1;
		is_int_type[edx] = 1;
	}
	
	// Get the top-most free register
	Reg get_top(){
		assert(free_registers.size()>0);
		return free_registers.back();
	}
	
	// Pop the top-most free register and return it
	Reg pop_top(){
		assert(free_registers.size()>0);
		Reg r = free_registers.back();
		free_registers.pop_back();
		return r;
	}
	
	// Push a register on top of the free-register stack
	void push_top(Reg r){
		free_registers.push_back(r);
	}
	
	// Swap the top 2 registers of the free-register stack
	void swap_top(){
		assert(free_registers.size()>1);
		Reg temp1 = free_registers.back();
		free_registers.back() = free_registers[free_registers.size()-2];
		free_registers[free_registers.size()-2] = temp1;
	}
	
	// Get free register count
	int free_reg_count(){
		return free_registers.size();
	}
	
	// Reset the free register stack to default
	void reset(){
		free_registers.clear();
		free_registers.push_back(edx);
		free_registers.push_back(ecx);
		free_registers.push_back(ebx);
		free_registers.push_back(eax);
	}
		
	// Method to save current state of registers
	void save_registers(bool in_use[],bool is_int[]){
		for(int i=0; i<free_reg_count(); i++){
			in_use[3-free_registers[i]] = 0;
		}
		for(int i=0; i<4; i++){
			if(!in_use[i]) continue;
			Reg curr = (Reg)(3-i);
			is_int[i] = is_int_type[curr];
			string dtype = is_int[i] ? "i" : "f";
			make_instr("push"+dtype,curr);
			reset();
		}
	}
	
	// Set the free_registers stack & types of used registers
	void set(bool in_use[], bool is_int[]){
		free_registers.clear();
		for(int i=0; i<4; i++){
			if(!in_use[i]) free_registers.push_back((Reg)(3-i));
			else is_int_type[(Reg)(3-i)] = is_int[3-i];
		}
	}
	
	// Method to load back state of registers
	void load_registers(bool in_use[], bool is_int[]){
		set(in_use,is_int);
		for(int i=3; i>=0; i--){
			if(!in_use[i]) continue;
			Reg curr = (Reg)(3-i);
			string dtype = is_int_type[curr] ? "i" : "f";
			make_instr("load"+dtype,make_index(esp),curr);
			make_instr("pop"+dtype,1);
		}
	}
};

reg_maintainer rm; 								// The register maintainer
vector<vector<string> *> code_stack;			// Stack of vectors for code (required because of nested function defns)
vector<string> *code;							// Pointer to vector of lines of the code of current function
vector<int> label_stack;						// Stack of current labels for nested function
int label_num = 0;								// Label number that would be used by the next label
int line_num = 0;								// Line number where the next instruction of the code would come
bool put_label = 0;								// Boolean to indicate whether the instruction to come has to be put a label
SymbolTable *currentST = NULL;					// The current symbol table under consideration (used for variable lookup)

// For printing the code for functions as they are parsed //
void print_code(){
	int i;
	for(i=0; i<code->size(); i++){
		cout<<(*code)[i]<<endl;
	}
	code->clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////// Helper Functions ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Function to get a string of space characters
string get_space(int c){
	string s = "";
	for(int i=0; i<c; i++) s = s + " ";
	return s;
}
	
// Function to add a line of code to the global code vector
int add_line_to_code(string s){

	if(put_label){		// Add line number tag
		s = "l"+to_string(label_num)+":"+get_space(8-to_string(label_num).length())+s;
		label_num++;
		put_label = false;
	}
	else{
		s = get_space(10)+s;
	}
	code->push_back(s);
	line_num++;
	return line_num-1;
}

// Backpatch a list of unfilled instructions with the label
void back_patch(vector<int> &lines, string label){
	for(int i=0; i<lines.size(); i++){
		(*code)[lines[i]] = (*code)[lines[i]] + "(" + label + ");" ;
	}
	lines.clear();
	return;
}

// Copy the first list into the second one
void copy_list(vector<int> &src, vector<int> &dest){
	dest = src;
	src.clear();
	return;
}

// Merge the 1st and 2nd list and put it in the 3rd list
void merge(vector<int> &s1, vector<int> &s2, vector<int> &d){
	d = s1;
	s1.clear();
	for(int i=0; i<s2.size(); i++) d.push_back(s2[i]);
	s2.clear();
}

// Various polymorhic definitions of make_instruction function

void make_instr(string c, Reg r1, Reg r2){
	if(!c.compare("move")) rm.is_int_type[r2] = rm.is_int_type[r1];
	string s = c+"("+reg_names[r1]+", "+reg_names[r2]+");";
	add_line_to_code(s);
}

void make_instr(string c, int i, Reg r){
	string s = c+"("+to_string(i)+", "+reg_names[r]+");";
	add_line_to_code(s);
}

void make_instr(string c, float f, Reg r){
	string s = c+"("+to_string(f)+", "+reg_names[r]+");";
	add_line_to_code(s);
}

void make_instr(string c, Reg r, int i){
	string s = c+"("+reg_names[r]+", "+to_string(i)+");";
	add_line_to_code(s);
}

void make_instr(string c, Reg r, string t){
	string s = c+"("+reg_names[r]+", "+t+");";
	add_line_to_code(s);
}

void make_instr(string c, int i, string t){
	string s = c+"("+to_string(i)+", "+t+");";
	add_line_to_code(s);
}

void make_instr(string c, float f, string t){
	string s = c+"("+to_string(f)+", "+t+");";
	add_line_to_code(s);
}

void make_instr(string c, string t, Reg r){
	if(!c.compare("loadi")) rm.is_int_type[r] = 1;
	else rm.is_int_type[r] = 0;
	string s = c+"("+t+", "+reg_names[r]+");";
	add_line_to_code(s);
}

void make_instr(string c, string t, int i){
	string s = c+"("+t+", "+to_string(i)+");";
	add_line_to_code(s);
}

void make_instr(string c, Reg r){
	if(!c.compare("intTofloat")) rm.is_int_type[r] = 0;
	if(!c.compare("floatToint")) rm.is_int_type[r] = 1;
	
	string s = c+"("+reg_names[r]+");";
	add_line_to_code(s);
}

void make_instr(string c, int i){
	string s = c+"("+to_string(i)+");";
	add_line_to_code(s);
}

void make_instr(string c, float f){
	string s = c+"("+to_string(f)+");";
	add_line_to_code(s);
}

void make_instr(string c, string t){
	string s = c+"("+t+");";
	add_line_to_code(s);
}

void make_instr(string c, char ch){
	string s = c+"('"+ch+"');";
	add_line_to_code(s);
}

void make_instr(string c){
	string s = c+"();";
	add_line_to_code(s);
}

// Functions to make index instructions

string make_index(Reg r){
	string s = "ind("+reg_names[r]+")";
	return s;
}

string make_index(Reg r, int offset){
	string s = "ind("+reg_names[r]+", "+to_string(offset)+")";
	return s;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of StmtAst class //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void BlockAst::gen_code(){

	SymbolTable *temp;
	bool changed_scope = (currentST != symbolTable);	// Mark true if entered a new function body
	map<int,pair<bool,int> > locals;			// map used for setting up locals on stack (if block ast is func defn.)
	bool last_float_pushed;						// The basetype of the last local pushed onto stack is float ?
	vector<int> counts_pushed;					// counts of consecutive ints/floats pushed onto stack as locals
	string func_name;							// name of the function, if this is a function defn.
	
	// If scope changed, means function definition has been encountered
	if(changed_scope){
		temp = currentST;						// save previous ST pointer

		// create new vector for this function's code & make it the current code pointer
		if(temp!=NULL){
			code_stack.push_back(code);
			label_stack.push_back(label_num);	// save the label num of the current function
		}
		code = new vector<string>();
		label_num = 0;
		line_num = 0;							// line_num doesn't need saving, as it is = code.size()
		
		currentST = symbolTable;				// Set the symbol table pointer to the current one
		func_name = currentST->get_name(); 		// Obtain the name of the function
		
		// Add the function signature line to the code
		string fun_sig = "void "+func_name+"()\n{";
		code->push_back(fun_sig);
		line_num++;

		// Set up dynamic link of this called function (if it is not main)
		if(!func_name.compare("main")){
			assert(temp==NULL);			// assert that main function is not nested within another function
		}
		else{
			make_instr("pushi",ebp);
			make_instr("move",esp,ebp);
		}
		
		// Create space on stack for local variables here
		currentST->get_local_offsets(locals);
		map<int,pair<bool,int> >::iterator it = locals.end();
		it--;
		
		bool is_float = true;
		int count = 0;
		string s,dtype;
		
		while(1){
			if((it->second).first == is_float){
				count += (it->second).second;
			}
			else{
				if(count!=0){
					// Push count of consecutive ints (or floats) placed on stack
					counts_pushed.push_back(count);
				}
				is_float = (it->second).first;
				dtype = (is_float?"f":"i");
				count = (it->second).second;
			}
			
			for(int i=0; i<(it->second).second; i++)
				make_instr("push"+dtype,0);

			
			if(it==locals.begin()){
				if(count!=0){
					// Push count of consecutive ints (or floats) placed on stack
					counts_pushed.push_back(count);
				}
				last_float_pushed = is_float;
				break;
			}
			it--;
		}
		add_line_to_code("");
	}
	
	// Now call gen_code on each of the block's statement
	for(int i=0; i<statements.size(); i++){
		put_label = true;
		statements[i]->gen_code();
		if(i < statements.size()-1){	// for all statements except last one
			back_patch(statements[i]->next_list,"l"+to_string(label_num));
		}
		else{							// for last statement
			if(changed_scope) back_patch(statements[i]->next_list,"e");
			else copy_list(statements[i]->next_list,next_list);
		}
	}
	
	if(changed_scope){
		string s;
		
		// pop off local variables from the stack
		bool is_float = last_float_pushed;
		int c_start = counts_pushed.size()-1;
		for(int i=c_start; i>=0; i--){
			string dtype = is_float?"f":"i";
			is_float = !is_float;
			
			if(i==c_start){
				s = "e:"+get_space(8)+"pop"+dtype+"("+to_string(counts_pushed[i])+");";
				code->push_back(s);
				line_num++;
			}
			else make_instr("pop"+dtype,counts_pushed[i]);
		}
		
		// Reset back ebp (if not main() function) and return 
		if(func_name.compare("main")){
			make_instr("loadi",make_index(ebp),ebp);
			make_instr("popi",1);
		}
		s = get_space(10)+"return;\n}\n";
		code->push_back(s);
		line_num++;
		
		print_code();										// print this function's code
		if(temp!=NULL){
			code = code_stack[code_stack.size()-1];			// If there's an outer function, bring its code back
			code_stack.pop_back();							// and pop the top element of code stack
			line_num = code->size();
			label_num = label_stack[label_stack.size()-1];	// If there's an outer function, bring its label num back
			label_stack.pop_back();							// and pop the top element of label stack
		}
		else{
			code = NULL;									// Else make code pointer NULL
		}
		currentST = temp;		// set back the previous symbol table
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
			if(!dtype.compare("i"))
				make_instr("storei", right->vali, make_index(ebp,left->mem_offset));
			else
				make_instr("storef", right->valf, make_index(ebp,left->mem_offset));
		}
		else{
			// Else first generate code for the right side
			right->gen_code();
			make_instr("store"+dtype, r, make_index(ebp,left->mem_offset));
		}
	}
	else if(left->is_arrayref){
		((ArrayRef*)left)->gen_code_addr();
		if(right->is_const){
			// If right side is constant, use immediate operation (also put that value in top register for return of this node)
			if(!dtype.compare("i"))
				make_instr("storei",right->vali, make_index(r));
			else
				make_instr("storef",right->valf, make_index(r));
		}
		else{
			// Else save the lval index onto the stack & evaluate right side
			make_instr("push"+dtype,r); 						// save the index
			right->gen_code();									// generate code for rhs
			rm.pop_top();										// pop the top register r
			Reg l = rm.get_top();								// get the current top register
			rm.push_top(r);										// put back the register r on top
			make_instr("load"+dtype, make_index(esp), l); 		// load the saved index in current top register l
			make_instr("pop"+dtype, 1);							// pop the saved value from stack
			make_instr("store"+dtype, r, make_index(l)); 		// store the calculated rhs value in lhs's index
		}
	}
	else assert(0);
	
	add_line_to_code("");
	return;
}
	
void ReturnSt::gen_code(){
	assert(currentST->returnType->basetype!=BASETYPE::VOID);

	string s;
	Reg r = rm.get_top();			// Top register (will have the answer of evaluated exp)
	string dtype = (exp->type->basetype==BASETYPE::INT)?"i":"f";

	if(exp->is_const){
		if(!dtype.compare("i"))
			make_instr("storei",exp->vali,make_index(ebp,currentST->return_offset));
		else
			make_instr("storef",exp->valf,make_index(ebp,currentST->return_offset));
	}
	else{
		exp->gen_code();
		make_instr("store"+dtype,r,make_index(ebp,currentST->return_offset));
	}
	
	make_instr("j","e");
	add_line_to_code("");
	return;
}

void If::gen_code(){
	cond->gen_code();
	statement1->gen_code();
	statement2->gen_code();
	
	add_line_to_code("");
	return;
}

void While::gen_code(){
	cond->gen_code();
	statement->gen_code();
	
	add_line_to_code("");
	return;
}

void For::gen_code(){
	init->gen_code();
	cond->gen_code();
	statement->gen_code();
	step->gen_code();
	
	add_line_to_code("");
	return;
}

void FunCallStmt::gen_code(){
	
	string s;
	Reg r1 = rm.get_top();					// Top register (will have to evaluate answer in this, if FunCall)
	vector<int> arg_types;					// for storing list of types of arguments (0=int, 1=float)
	
	assert(name->get_id().compare("main"));					// check that main is not called by any function
	bool is_printf = (!name->get_id().compare("printf")); 	// Check if function is printf
	
	// Save the registers which are in use (if function is not printf)
	bool in_use[4] = {1,1,1,1};							// for finding which registers are in use among [edx,ecx,ebx,eax]
	bool is_int[4];										// for storing the current types of the registers
	if(!is_printf) rm.save_registers(in_use,is_int);
	
	// Note: No need to push space for return value if FunCallStmt //
	
	// Evaluate arguments from right to left and push onto the stack
	Reg r = rm.get_top();					// current top register (after register saving)
	int init_val = (is_printf ? 0 : expression_list.size()-1);
	
	for(int i=init_val;;){
		if(is_printf && i>=expression_list.size()) break;
		if(!is_printf && i==-1) break;

		bool is_int = (expression_list[i]->type->basetype == BASETYPE::INT);
		bool is_string = (expression_list[i]->type->basetype == BASETYPE::STRING);
		
		if(expression_list[i]->is_const){
			if(is_string){
				assert(is_printf);
				make_instr("print_string",((StringConst*)expression_list[i])->getVal());
			}
			else if(is_int){
				if(is_printf) make_instr("print_int",expression_list[i]->vali);
				else make_instr("pushi",expression_list[i]->vali);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) make_instr("print_float",expression_list[i]->valf);
				else make_instr("pushf",expression_list[i]->valf);
				arg_types.push_back(1);
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) make_instr("print_int",r);
				else make_instr("pushi",r);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) make_instr("print_float",r);
				else make_instr("pushf",r);
				arg_types.push_back(1);
			}
		}
		
		if(is_printf) i++;
		else i--;
	}

	// Return if the function is printf, as it's job is over
	if(is_printf){
		add_line_to_code("");
		return;
	}
	
	// Calculate and Push the static link (TODO)
	make_instr("pushi",0);
	
	// Make a call to the function
	make_instr(name->get_id());
	
	// Pop static link & parameters from the stack (initial count=1 is for popping the static link)
	int mode = 0;								// 0 means int, 1 means float;
	string dtype = (mode?"f":"i");
	int count = 1;								// Count of consecutive parameters on the stack of same mode
	for(int i=arg_types.size()-1; i>=0; i--){
		
		if(arg_types[i]==mode) count++;
		else{
			if(count!=0){
				make_instr("pop"+dtype,count);
			}
			mode = arg_types[i];
			dtype = (mode?"f":"i");
			count = 1;
		}
	}
	if(count!=0) make_instr("pop"+dtype,count);

	// Load the registers which were saved
	if(!is_printf) rm.load_registers(in_use, is_int);
	
	add_line_to_code("");
	return;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of ExpAst class ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void Identifier::gen_code(){

	assert(type->child == NULL);					// Check that it is not an array type	
	Reg r = rm.get_top();							// Get the top-most free register
	
	if(type->basetype == BASETYPE::INT){			// If type is int, use loadi
		make_instr("loadi",make_index(ebp,mem_offset),r);
	}
	else if(type->basetype == BASETYPE::FLOAT){		// If type is float, use loadf
		make_instr("loadf",make_index(ebp,mem_offset),r);
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
					make_instr("storei", right->vali, make_index(ebp,left->mem_offset));
					make_instr("move", right->vali, r);
				}
				else{
					make_instr("storef", right->valf, make_index(ebp,left->mem_offset));
					make_instr("move", right->valf, r);
				}
			}
			else{
				// Else first generate code for the right side
				right->gen_code();
				make_instr("store"+dtype, r, make_index(ebp,left->mem_offset));
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
					make_instr("storei",right->vali, make_index(r));
					make_instr("move", right->vali, r);
				}
				else{
					make_instr("storef",right->valf, make_index(r));
					make_instr("move", right->valf, r);
				}
			}
			else{
				// Else save the lval index onto the stack & evaluate right side
				make_instr("push"+dtype,r); 						// save the index
				right->gen_code();									// generate code for rhs
				rm.pop_top();										// pop the top register r
				
				Reg l = rm.get_top();								// get the current top register
				rm.push_top(r);										// put back the register r on top
				make_instr("load"+dtype, make_index(esp), l); 		// load the saved index in current top register l
				make_instr("pop"+dtype, 1);							// pop the saved value from stack
				make_instr("store"+dtype, r, make_index(l)); 		// store the calculated rhs value in lhs's index
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
				if(is_sub) make_instr("muli", -1, r);
				make_instr(op+dtype,left->vali,r);
			}
			else{
				if(is_sub) make_instr("mulf", -1.0f, r);
				make_instr(op+dtype,left->valf,r);
			}
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
					make_instr("move",right->vali,r);
					make_instr("divi",l,r);
				}
				else{
					left->gen_code();	// Evaluate left side first if not div operation
					if(is_sub) make_instr(op+dtype,-right->vali,r);
					else make_instr(op+dtype,right->vali,r);
				}
			}
			else{
				if(is_div){
					rm.swap_top();
					Reg l = rm.get_top();
					left->gen_code();
					rm.swap_top();
					make_instr("move",right->valf,r);
					make_instr("divf",l,r);
				}
				else{
					left->gen_code();	// Evaluate left side first if not div operation
					if(is_sub) make_instr(op+dtype,-right->valf,r);
					else make_instr(op+dtype,right->valf,r);
				}
			}

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
				if(!dtype.compare("i"))	make_instr("muli", -1, r);
				else make_instr("mulf", -1.0f, r);
			}
			
			make_instr(op+dtype,l,r);
			reg_addr = r;
		}
		else{											// Current node has exactly 2 registers & left is not identifier
			right->gen_code();
			Reg r = rm.get_top();
			make_instr("push"+dtype,r);								// gen code and store right part on stack
			rm.swap_top();											// swap top 2 regs of reg-stack
			left->gen_code();										// gen code for left side
			Reg l = rm.get_top();
			make_instr("load"+dtype,make_index(esp),r);				// load back right answer from stack
			make_instr("pop"+dtype,1);								// pop the stored value from stack
			
			if(is_sub){												// negate the value of right register if sub operation
				if(!dtype.compare("i")) make_instr("muli", -1, r);
				else make_instr("mulf", -1.0f, r);
			}
			
			make_instr(op+dtype,l,r);								// r <- (l op r)
			rm.swap_top();											// swap back the top 2 registers into original position
			reg_addr = r;
		}
		return;
	}
	
	// If you have reached here, means that the operator is a comparison operator
	else{
		/*
		EQ_OP_INT,
	EQ_OP_FLOAT,
	NE_OP_INT,
	NE_OP_FLOAT,
	LT_INT,
	LT_FLOAT,
	LE_OP_INT,
	LE_OP_FLOAT,
	GT_INT,
	GT_FLOAT,
	GE_OP_INT,
	GE_OP_FLOAT,
		assert(!is_const);
		bool is_int = (op_type%2==0);
		string op="cmp", dtype =  (is_int ? "i" : "f");
		
		// Find the suitable op
		if(op_type==EQ_OP_INT || op_type==PLUS_FLOAT || 
			op_type==MINUS_INT || op_type==MINUS_FLOAT) op = "add";
		else if(op_type==MULT_INT || op_type==MULT_FLOAT) op = "mul";
		else op = "div";
		
		// Find the suitable datatype
		if(op_type==PLUS_INT || op_type==MULT_INT || op_type==MINUS_INT || op_type==DIV_INT) dtype="i";
		else dtype = "f";
		
		return;
		*/
	}
	
}

void UnOp::gen_code(){
	if(op_type==UMINUS_INT || op_type==UMINUS_FLOAT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		if(op_type==UMINUS_INT) make_instr("muli",-1,r);
		else make_instr("mulf",-1.0f,r);
		reg_addr = r;
		return;
	}
	if(op_type==TO_INT || op_type==TO_FLOAT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		if(op_type==TO_INT) make_instr("floatToint",r);
		else make_instr("intTofloat",r);
		reg_addr = r;
		return;
	}
	if(op_type==PP_INT || op_type==PP_FLOAT){
		exp->gen_code();
		
		Reg r = rm.get_top();
		string dtype = (op_type==PP_INT) ? "i" : "f";
		
		// If lhs is identifier, then r contains its value in it
		if(exp->is_identifier){
			if(op_type==PP_INT) make_instr("addi",1,r);							// increment value of r by 1
			else make_instr("addf",1.0f,r);
			
			make_instr("store"+dtype, r, make_index(ebp,exp->mem_offset));		// store incremented value back in lval
			if(op_type==PP_INT) make_instr("addi",-1,r);						// put decremented (i.e. old) value in r
			else make_instr("addf",-1.0f,r);
		}
		// But if lhs is arrayref, then r now contains its memory address (viz. pointer)
		else if(exp->is_arrayref){
			rm.pop_top();											// pop register r (contains mem. pointer)
			Reg l = rm.get_top();									// get top register l
			rm.push_top(r);											// place back r on top
			make_instr("load"+dtype, make_index(r), l);				// l <- *(r)
			if(op_type==PP_INT) make_instr("addi",1,r);				// l <- l + 1 (increment value by 1)
			else make_instr("addf",1.0f,r);
			make_instr("store"+dtype,l, make_index(r));				// *(r) <- l
			make_instr("move",l,r);									// r <- l (preparing value in r for return from this node)
			
			if(op_type==PP_INT) make_instr("addi",-1,r);			// r <- r - 1 (decrement value by 1 and get old value for return)
			else make_instr("addf",-1.0f,r);
		}
		else assert(0);
		
		reg_addr = r;
		return;
	}
	if(op_type==NOT){
		assert(!is_const);
		exp->gen_code();
		
		Reg r = rm.get_top();
		bool is_int = (type->basetype==BASETYPE::INT);
		string dtype = is_int ? "i" : "f";
		
		make_instr("cmp"+dtype,0,r);
		make_instr("je","l"+to_string(line_num+3));
		is_int ? make_instr("move",0,r) : make_instr("move",0.0f,r);
		make_instr("j","l"+to_string(line_num+2));
		is_int ? make_instr("move",1,r) : make_instr("move",1.0f,r);
		reg_addr = r;
		return;
	}
}

void FunCall::gen_code(){
	
	Reg r1 = rm.get_top();									// Top register ( will have to evaluate asnwer in this)
	vector<int> arg_types;									// for storing list of types of arguments
	
	int ret_type = type->basetype;							// return type of this function expression
	assert(ret_type!=BASETYPE::VOID);
	string dtype = (ret_type==BASETYPE::INT) ? "i" : "f";
	
	assert(name->get_id().compare("main"));					// check that main is not called by any function
	bool is_printf = (!name->get_id().compare("printf")); 	// Check if function is printf
	
	// Save the registers which are in use (if function is not printf)
	bool in_use[4] = {1,1,1,1};							// for finding which registers are in use among [edx,ecx,ebx,eax]
	bool is_int[4];										// for storing the current types of the registers
	if(!is_printf) rm.save_registers(in_use,is_int);
	
	// Push space for return value (if not printf function)
	if(!printf) make_instr("push"+dtype,1);
	
	// Evaluate arguments from right to left and push onto the stack
	Reg r = rm.get_top();					// current top register (after register saving)
	int init_val = (is_printf ? 0 : expression_list.size()-1);
	
	for(int i=init_val;;){
		if(is_printf && i>=expression_list.size()) break;
		if(!is_printf && i==-1) break;
		
		bool is_int = (expression_list[i]->type->basetype == BASETYPE::INT);
		bool is_string = (expression_list[i]->type->basetype == BASETYPE::STRING);
		
		if(expression_list[i]->is_const){
			if(is_string){
				assert(is_printf);
				make_instr("print_string",((StringConst*)expression_list[i])->getVal());
			}
			else if(is_int){
				if(is_printf) make_instr("print_int",expression_list[i]->vali);
				else make_instr("pushi",expression_list[i]->vali);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) make_instr("print_float",expression_list[i]->valf);
				else make_instr("pushf",expression_list[i]->valf);
				arg_types.push_back(1);
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) make_instr("print_int",r);
				else make_instr("pushi",r);
				arg_types.push_back(0);
			}
			else{
				if(is_printf) make_instr("print_float",r);
				else make_instr("pushf",r);
				arg_types.push_back(1);
			}
		}
		
		if(is_printf) i++;
		else i--;
	}
	
	// Return if the function is printf, as it's job is over
	if(is_printf){
		make_instr("move",0,r);
		return;
	}
	
	// Push the static link (TODO)
	make_instr("pushi",0);
	
	// Make a call to the function
	make_instr(name->get_id());
	
	// Pop static link & parameters from the stack (initial count=1 is for popping the static link)
	int mode = 0;								// 0 means int, 1 means float;
	string dtype1 = (mode?"f":"i");
	int count = 1;								// Count of consecutive parameters on the stack of same mode
	for(int i=arg_types.size()-1; i>=0; i--){
		
		if(arg_types[i]==mode) count++;
		else{
			if(count!=0) make_instr("pop"+dtype1,count);
			mode = arg_types[i];
			dtype1 = (mode?"f":"i");
			count = 1;
		}
	}
	if(count!=0) make_instr("pop"+dtype1,count);
	
	// Pick up the return value from stack and pop it off
	if(ret_type==BASETYPE::INT){
		make_instr("load"+dtype,make_index(esp),r1);
		make_instr("pop"+dtype,1);
	}

	// Load the registers which were saved
	if(!is_printf) rm.load_registers(in_use, is_int);
	
	reg_addr = r1;
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
	
	make_instr("load"+dtype,make_index(r),r); // r <- *(r)
	reg_addr = r;
	return;
}

// This function should return address of the array ref in the top register r
void ArrayRef::gen_code_addr(){
	
	Reg r = rm.get_top();				// Top register (will have to evaluate address of array ref in this)
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
			make_instr("move",dimension[indc]*indices[indc]->vali,r);
		else{							// Else, evaluate & multiply with sizeof(basetype) viz. dimension[indc]
			indices[indc]->gen_code();
			make_instr("muli",dimension[indc],r);
		}
		
		rm.pop_top();			// Pop top register and keep it for accumulating sum
		Reg l = rm.get_top();	// l = new top register
		
		// Now start evaluating from second last index
		for(int i=indc-1; i>=0; i--){
			if(indices[i]->is_const){
				make_instr("addi",(indices[i]->vali)*dimension[i],r);	// r <- r + dim[i]*ind[i]
			}
			else{
				indices[i]->gen_code();
				make_instr("muli",dimension[i],l);						// l <- l*dim[i]
				make_instr("addi",l,r);									// r <- l + r
			}	
		}
	}
	
	// Case where you have = 2 free registers
	else{
		make_instr("pushi",0);		// Accumulate the sum on a stack location & initialize it to 0
		
		// Now start evaluating from last index
		for(int i=indc; i>=0; i--){
			if(indices[i]->is_const){
				make_instr("loadi",make_index(esp),r);					// r <- *(esp)
				make_instr("addi",(indices[i]->vali)*dimension[i],r);	// r <- r + dim[i]*ind[i]
				make_instr("storei",r,make_index(esp));					// *(esp) <- r
			}
			else{
				indices[i]->gen_code();										// r <- ind[i]
				make_instr("muli",dimension[i],r);							// r <- dimension[i]*r
				
				rm.pop_top();												// pop(r)
				Reg l = rm.get_top();										// l = get new top register
				rm.push_top(r);												// push(r)
				
				make_instr("loadi",make_index(esp),l);						// l <- *(esp)
				make_instr("addi",r,l);										// l <- r + l
				make_instr("storei",l,make_index(esp));						// *(esp) <- l
			}
		}
	}
	
	make_instr("muli",-1,r);			// Negate r, as the index grows towards lower addresses of stack
	make_instr("addi",mem_offset,r);	// Add mem_offset of array-base wrt ebp
	make_instr("addi",ebp,r);			// Get absolute address by r <- ebp + r
	
	reg_addr = r;
	return;
}
