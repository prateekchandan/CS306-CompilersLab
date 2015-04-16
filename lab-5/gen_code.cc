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
	
	// Function for printing map of registers to types they hols
	void print_reg_types(){
		for(int i = 0; i<4; i++)
			cout<<reg_names[i]<<": "<<is_int_type[(Reg)(i)]<<endl;
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
			in_use[free_registers[i]] = 0;
		}
		for(int i=0; i<4; i++){
			if(!in_use[i]) continue;
			Reg curr = (Reg)i;
			is_int[i] = is_int_type[curr];
			string dtype = is_int[i] ? "i" : "f";
			make_instr("push"+dtype,curr);
			reset();
		}
	}
	
	// Set the free_registers stack & types of used registers
	void set(bool in_use[], bool is_int[]){
		free_registers.clear();
		for(int i=3; i>=0; i--){
			if(!in_use[i]) free_registers.push_back((Reg)i);
			else is_int_type[(Reg)i] = is_int[i];
		}
	}
	
	// Method to load back state of registers
	void load_registers(bool in_use[], bool is_int[]){
		set(in_use,is_int);
		for(int i=3; i>=0; i--){
			if(!in_use[i]) continue;
			Reg curr = (Reg)i;
			string dtype = is_int[i] ? "i" : "f";
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
bool put_exit_label = 0;						// Boolean to indicate whether the instruction to come needs an exit label

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

	if(put_exit_label){			// Add "e" as the exit tag
		code->push_back("");
		line_num++;
		s = "e:"+get_space(8)+s;
		put_exit_label = false;
		put_label = false;
	}
	else if(put_label){			// Add line number tag
		code->push_back("");
		line_num++;
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
}

// Backpatch a single unfilled instruction with the label
void back_patch(int line, string label){
	(*code)[line] = (*code)[line] + "(" + label + ");" ;
}

// Copy the first list into the second one
void copy_list(vector<int> &src, vector<int> &dest){
	for(int i=0; i<src.size(); i++) dest.push_back(src[i]);
	src.clear();
}

// Merge the 1st and 2nd list and put it in the 3rd list
void merge(vector<int> &s1, vector<int> &s2, vector<int> &d){
	for(int i=0; i<s1.size(); i++) d.push_back(s1[i]);
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
		
		bool is_float = true;
		int count = 0;
		string s,dtype;
		
		while(1){
			if(it==locals.end()){
				if(locals.empty()) break;
				it--;
			}
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
	}
	
	int i, backpatch_index = -1;
	int label_num_before;
	statements.back()->is_last = true;
	
	for(i=0; i<statements.size(); i++){
		/* backpatch the previous statement (if non-empty) with current label num
		if(i>0 && !statements[i-1]->next_list.empty()){
			back_patch(statements[i-1]->next_list,"l"+to_string(label_num));
			put_label = true;
		}
		*/
		label_num_before = label_num;					// save label number before generating code for stmt
		if(backpatch_index != -1) put_label = true;		// if some previous statement needs backpatching, set put_label to true
		
		statements[i]->gen_code();						// generate code of the statement
		
		// put_label is true => (stmt[i] needs backpatching from within || some stmt before needs backpatching)
		if(put_label){
			// stmt filled the label or itself now needs backpatching
			if(label_num_before!=label_num || !statements[i]->next_list.empty()){
				if(backpatch_index != -1) back_patch(statements[backpatch_index]->next_list,"l"+to_string(label_num_before));
				backpatch_index = i;
			}
		}
		else{
			if(backpatch_index != -1){
				back_patch(statements[backpatch_index]->next_list,"l"+to_string(label_num_before));
				backpatch_index = -1;
			}
			if(!statements[i]->next_list.empty()) backpatch_index = i;
		}

		// If this is the last statement of the block
		if(i == statements.size()-1){		// for last statement
			if(backpatch_index != -1){
				if(changed_scope) back_patch(statements[backpatch_index]->next_list,"e");
				else copy_list(statements[backpatch_index]->next_list,next_list);
			}
		}
	}
	
	if(changed_scope){
		string s;
		put_exit_label = 1;
		
		// pop off local variables from the stack
		bool is_float = last_float_pushed;
		int c_start = counts_pushed.size()-1;
		for(int i=c_start; i>=0; i--){
			string dtype = is_float?"f":"i";
			is_float = !is_float;
			make_instr("pop"+dtype,counts_pushed[i]);
		}
		
		// Reset back ebp (if not main() function) and return 
		if(func_name.compare("main")){
			make_instr("loadi",make_index(ebp),ebp);
			make_instr("popi",1);
		}
		add_line_to_code("return;\n}\n");
		
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
	return;
}
	
void ReturnSt::gen_code(){
	assert(currentST->returnType->basetype!=BASETYPE::VOID);

	Reg r = rm.get_top();			// Top register (will have the answer of evaluated exp)
	bool is_int = (exp->type->basetype==BASETYPE::INT);
	string dtype = is_int?"i":"f";

	if(exp->is_const){
		bool zero_val = is_int ? (exp->vali==0) : (exp->valf==0);
		if(!zero_val){
			if(is_int) make_instr("storei",exp->vali,make_index(ebp,currentST->return_offset));
			else make_instr("storef",exp->valf,make_index(ebp,currentST->return_offset));
		}
	}
	else{
		exp->gen_code();
		make_instr("store"+dtype,r,make_index(ebp,currentST->return_offset));
	}
	
	if(!is_last) make_instr("j","e");
	return;
}

void If::gen_code(){
	bool is_int = (cond->type->basetype==BASETYPE::INT);
	
	if(cond->is_const){
		bool is_false = (is_int ? (cond->vali == 0) : (cond->valf == 0.0f));
		if(is_false){
			statement2->gen_code();
			copy_list(statement2->next_list,next_list);
		}
		else{
			statement1->gen_code();
			copy_list(statement1->next_list,next_list);
		}
	}
	else{
		cond->is_cond = true;											// inform cond that it is to be evaluated as conditional
		cond->fall = true;												// call gen_code on cond expression with fall marked true
		cond->gen_code();
		back_patch(cond->true_list,"l"+to_string(label_num));			// back patch cond's truelist now with the label of stmt-1
		put_label = true;
		statement1->gen_code();											// generate code for stmt-1 with put label marked true
		int line = add_line_to_code("j");								// add an unconditional jump statement to go out
		next_list.push_back(line);										// add this jump statement to the If statement's next list
		back_patch(cond->false_list,"l"+to_string(label_num));			// back patch cond's falselist now with the label of stmt-2
		put_label = true;
		statement2->gen_code();											// generate code for stmt-2 with put label marked true
		merge(statement1->next_list,statement2->next_list,next_list);	// merge & add next lists of stmt-1 and stmt-2 into next list
	}
	return;
}

void While::gen_code(){
	bool is_int = (cond->type->basetype==BASETYPE::INT);
	
	if(cond->is_const){
		bool is_false = (is_int ? (cond->vali == 0) : (cond->valf == 0.0f));
		if(is_false); // do nothing
		else{
			put_label = true;											// set this to true, as statement needs a label
			int stmt_label = label_num;									// note the to be given label number to the stmt
			statement->gen_code();										// generate code for statement
			add_line_to_code("j(l"+to_string(stmt_label)+");");			// add an unconditional jump after stmt leading back to it
			back_patch(statement->next_list,"l"+to_string(stmt_label));	// backpatch next list of stmt back to itself
		}
	}
	else{
		cond->is_cond = true;											// inform cond that it is to be evaluated as conditional
		put_label = true;												// set this to true, as cond evaluation needs a label
		int cond_label = label_num;										// note the 'to be given' label number
		cond->fall = true;												// call gen_code on cond expression with fall marked true
		cond->gen_code();
		back_patch(cond->true_list,"l"+to_string(label_num));			// back patch cond's truelist with the label stmt would take
		put_label = true;
		statement->gen_code();											// generate code for stmt
		back_patch(statement->next_list,"l"+to_string(cond_label));		// backpatch next list of statement to cond
		add_line_to_code("j(l"+to_string(cond_label)+");");				// add an unconditional jump statement back to cond
		copy_list(cond->false_list, next_list);							// copy false_list of cond into the while's next list
	}
	return;
}

void For::gen_code(){
	
	int cond_label;
	if(cond->is_const){
		// If cond is always false, return without producing any code
		int is_int = (cond->type->basetype==BASETYPE::INT);
		if(is_int && cond->vali==0) return;
		if(!is_int && cond->valf==0.0) return;
		// Else, cond is always true, so make the 'for' loop indefinitely
		init->gen_code();													// generate code for init (clearly as a normal expression)
		cond_label = label_num;												// note the label number of stmt (as cond's code is empty)
	}
	else{
		init->gen_code();													// generate code for init (clearly as a normal expression)
		cond_label = label_num;												// note the 'to be given' label number of cond
		put_label = true;													// set this to true, as cond evaluation needs a label
		cond->is_cond = true;												// inform cond that it is to be evaluated as conditional
		cond->fall = true;													// set fall of cond expression to true
		cond->gen_code();													// generate code for cond
		back_patch(cond->true_list,"l"+to_string(label_num));				// back patch cond's truelist with the label stmt would take
	}
	
	put_label = true;
	statement->gen_code();												// generate code for statement with put label marked true
	back_patch(statement->next_list,"l"+to_string(label_num));			// backpatch next list of stmt with the label step would take
	
	put_label = true;
	step->gen_code();													// generate code for step with put label marked true
	add_line_to_code("j(l"+to_string(cond_label)+");");					// add an unconditional jump statement back to cond
	
	copy_list(cond->false_list,next_list);								// copy false list of cond into next list of the 'for' stmt
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
				else{
					make_instr("pushi",expression_list[i]->vali);
					arg_types.push_back(0);
				}
			}
			else{
				if(is_printf) make_instr("print_float",expression_list[i]->valf);
				else{
					make_instr("pushf",expression_list[i]->valf);
					arg_types.push_back(1);
				}
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) make_instr("print_int",r);
				else{
					make_instr("pushi",r);
					arg_types.push_back(0);
				}
			}
			else{
				if(is_printf) make_instr("print_float",r);
				else{
					make_instr("pushf",r);
					arg_types.push_back(1);
				}
			}
		}
		
		if(is_printf) i++;
		else i--;
	}

	// Return if the function is printf, as it's job is over
	if(is_printf) return;
	
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
	rm.load_registers(in_use, is_int);
	return;
}

/////////////////////////////////////////////////////////////////////////////
// gen_code for child classes of ExpAst class ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void Identifier::gen_code(){
	assert(type->child == NULL);					// Check that it is not an array type	
	Reg r = rm.get_top();							// Get the top-most free register
	bool is_int = (type->basetype == BASETYPE::INT);
	
	if(is_int) make_instr("loadi",make_index(ebp,mem_offset),r);
	else if(type->basetype == BASETYPE::FLOAT) make_instr("loadf",make_index(ebp,mem_offset),r);
	else assert(0);
	
	// If node is a conditional, then add control flow instructions
	if(is_cond){
		is_int ? make_instr("cmpi",0,r) : make_instr("cmpf",0.0f,r);
		if(fall) false_list.push_back(add_line_to_code("je"));
		else true_list.push_back(add_line_to_code("jne"));
	}
	reg_addr = r;
	return;
}

void Op::gen_code(){
	
	assert(!is_const);
	bool is_int = (type->basetype == BASETYPE::INT);
	string op, dtype = is_int ? "i" : "f";
	Reg r = rm.get_top();
	
	if(op_type==ASSIGN_INT || op_type==ASSIGN_FLOAT){
		if(left->is_identifier){
			if(right->is_const){
				// If right side is constant, use immediate operation (also put that value in top register for return of this node)
				if(is_int){
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
		}
		else if(left->is_arrayref){
			((ArrayRef*)left)->gen_code_addr();
			if(right->is_const){
				// If right side is constant, use immediate operation (also put that value in top register for return of this node)
				if(is_int){
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
		}
		else assert(0);
	}
	
	else if(op_type==PLUS_INT || op_type==PLUS_FLOAT || op_type==MULT_INT || op_type==MULT_FLOAT ||
		op_type==MINUS_INT || op_type==MINUS_FLOAT || op_type==DIV_INT || op_type==DIV_FLOAT){
		
		// Find the suitable op
		if(op_type==PLUS_INT || op_type==PLUS_FLOAT || 
			op_type==MINUS_INT || op_type==MINUS_FLOAT) op = "add";
		else if(op_type==MULT_INT || op_type==MULT_FLOAT) op = "mul";
		else op = "div";
		
		// Booleans for checking if sub or div operation
		bool is_sub = (op_type==MINUS_INT || op_type==MINUS_FLOAT);
		bool is_div = (op_type==DIV_INT || op_type==DIV_FLOAT);
		
		if(left->is_const || right->is_const){
			if(left->is_const){				// If Left register has constant value
				right->gen_code();
				if(is_int){
					if(is_sub) make_instr("muli", -1, r);
					make_instr(op+dtype,left->vali,r);
				}
				else{
					if(is_sub) make_instr("mulf", -1.0f, r);
					make_instr(op+dtype,left->valf,r);
				}
			}
			else if(right->is_const){		// If Right register has constant value
				if(is_int){
					if(is_div){
						rm.swap_top();			// Swap registers and evaluate left side if div operation
						Reg l = rm.get_top();
						left->gen_code();
						rm.swap_top();
						make_instr("move",right->vali,r);
						make_instr("divi",l,r);
					}
					else{
						left->gen_code();		// Evaluate left side first if not div operation
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
			}
		}
		
		// Both sides have non-constant values //
		else{
			if(rm.free_reg_count()>2 || left->is_identifier){		// If Current node has more than 2 registers ||
				right->gen_code();									// Current node has 2 registers, but left is identifier
				rm.pop_top();
				left->gen_code();
				Reg l = rm.get_top();
				rm.push_top(r);
				
				if(is_sub){												// negate the value of right register if sub operation
					if(is_int) make_instr("muli", -1, r);
					else make_instr("mulf", -1.0f, r);
				}
				make_instr(op+dtype,l,r);
			}
			else{											// Current node has exactly 2 registers & left is not identifier
				right->gen_code();
				make_instr("push"+dtype,r);								// gen code and store right part on stack
				rm.swap_top();											// swap top 2 regs of reg-stack
				left->gen_code();										// gen code for left side
				Reg l = rm.get_top();
				make_instr("load"+dtype,make_index(esp),r);				// load back right answer from stack
				make_instr("pop"+dtype,1);								// pop the stored value from stack
				
				if(is_sub){												// negate the value of right register if sub operation
					if(is_int) make_instr("muli", -1, r);
					else make_instr("mulf", -1.0f, r);
				}
				make_instr(op+dtype,l,r);								// r <- (l op r)
				rm.swap_top();											// swap back the top 2 registers into original position
			}
		}
	}
	
	else if(op_type==OR_OP || op_type==AND_OP){
		Reg r = rm.get_top();
		bool is_and = (op_type==AND_OP);
		bool l_const = left->is_const, r_const = right->is_const;
		
		// If node is a conditional, then add control flow instructions
		if(is_cond){
			int curr_label;
			if(!l_const){
				left->is_cond = true;				// set cond flag of left to true
				left->fall = r_const?fall:is_and;	// set the fall value suitably
				left->gen_code();					// gen code for left expression
				curr_label = label_num;				// save the 'to be given' label to right expression
			}
			else{
				right->is_cond = true;			// set cond flag of left to true
				right->fall = fall;				// set the fall value suitably
				right->gen_code();				// gen code for right expression
				copy_list(right->true_list,true_list);	// copy the true, false lists
				copy_list(right->false_list,false_list);
				return;
			}
			
			if(!r_const){
				right->is_cond = true;			// set cond flag of left to true
				right->fall = fall;				// set the fall value suitably
				put_label = true;				// mark put label as true for right's code
				right->gen_code();				// gen code for right expression
			}
			else{
				copy_list(left->true_list,true_list);	// copy the true, false lists
				copy_list(left->false_list,false_list);
				return;
			}
			
			// do the back patching, copying and merging of lists suitably
			if(is_and){
				back_patch(left->true_list,"l"+to_string(curr_label));
				copy_list(right->true_list, true_list);
				merge(left->false_list,right->false_list,false_list);
			}
			else{
				back_patch(left->false_list,"l"+to_string(curr_label));
				copy_list(right->false_list, false_list);
				merge(left->true_list,right->true_list,true_list);
			}
			return;
		}
		
		// Else evaluate answer in a register as an integer
		else{
			string dtype1 = (left->type->basetype==BASETYPE::INT) ? "i" : "f";
			string dtype2 = (right->type->basetype==BASETYPE::INT) ? "i" : "f";
			
			if(l_const || r_const){
				if(l_const) right->gen_code();
				else left->gen_code();
				make_instr("cmpi",0,r);
				add_line_to_code("je(l"+to_string(label_num)+");");
				make_instr("move",1,r);
				put_label = true;
			}
			else{
				left->gen_code();
				make_instr("cmpi",0,r);
				if(is_and){
					int line = add_line_to_code("je");
					right->gen_code();
					make_instr("cmpi",0,r);
					add_line_to_code("je("+to_string(label_num)+");");
					back_patch(line,"l"+to_string(label_num));
					make_instr("move",1,r);
					put_label = true;
				}
				else{
					int line = add_line_to_code("jne");
					right->gen_code();
					make_instr("cmpi",0,r);
					add_line_to_code("jne(l"+to_string(label_num)+");");
					add_line_to_code("j("+to_string(label_num+1)+");");
					put_label = true;
					make_instr("move",1,r);
					put_label = true;
				}
			}
		}
		reg_addr = r;
		return;
	}
	
	// If you have reached here, means that the operator is a comparison operator
	else{
		dtype =  (is_int ? "i" : "f");
		int cmp_type = op_type/2 - 1;
		bool l_const = left->is_const, r_const = right->is_const;
		string jump, jump_inv, jump_swap, jump_inv_swap;
		
		// Find the suitable jump operation & its negated jump operation
		switch(cmp_type){
			case 0: jump = "je"; jump_inv = "jne"; jump_swap = "jne"; jump_inv_swap = "je"; break;
			case 1: jump = "jne"; jump_inv = "je"; jump_swap = "je"; jump_inv_swap = "jne"; break;
			case 2: jump = "jl"; jump_inv = "jge"; jump_swap = "jle"; jump_inv_swap = "jg"; break;
			case 3: jump = "jle"; jump_inv = "jg"; jump_swap = "jl"; jump_inv_swap = "jge"; break;
			case 4: jump = "jg"; jump_inv = "jle"; jump_swap = "jge"; jump_inv_swap = "jl"; break;
			case 5: jump = "jge"; jump_inv = "jl"; jump_swap = "jg"; jump_inv_swap = "jle"; break;
			default:;
		}
		
		// If the node is to be evaluated as a conditional
		if(is_cond){
			if(l_const){
				right->gen_code();
				if(is_int) make_instr("cmp"+dtype,left->vali,r);
				else make_instr("cmp"+dtype,left->valf,r);
				if(fall) false_list.push_back(add_line_to_code(jump_inv));
				else true_list.push_back(add_line_to_code(jump));
			}
			else if(r_const){
				left->gen_code();
				if(is_int) make_instr("cmp"+dtype,right->vali,r);
				else make_instr("cmp"+dtype,right->valf,r);
				if(fall) false_list.push_back(add_line_to_code(jump_swap));
				else true_list.push_back(add_line_to_code(jump_inv_swap));
			}
			else{
				// more than 2 registers are available or left side is identifier
				if(rm.free_reg_count()>2 || left->is_identifier){
					right->gen_code();
					rm.pop_top();
					left->gen_code();
					Reg l = rm.get_top();
					rm.push_top(r);
					make_instr("cmp"+dtype,l,r);
					if(fall) false_list.push_back(add_line_to_code(jump_inv));
					else true_list.push_back(add_line_to_code(jump));
				}
				// exactly 2 registers are available and left is not identifier
				else{
					right->gen_code();
					make_instr("push"+dtype,r);
					left->gen_code();
					rm.pop_top();
					Reg l = rm.get_top();
					rm.push_top(r);
					make_instr("load"+dtype,make_index(esp),l);
					make_instr("pop"+dtype,1);
					make_instr("cmp"+dtype,r,l);
					if(fall) false_list.push_back(add_line_to_code(jump_inv));
					else true_list.push_back(add_line_to_code(jump));
				}
			}
		}
		
		// If the node is to be evaluated as an expression
		else{
			if(l_const){			// left operand is constant
				right->gen_code();
				if(is_int) make_instr("cmp"+dtype,left->vali,r);
				else make_instr("cmp"+dtype,left->valf,r);
			}
			else if(r_const){		// right operand is constant
				left->gen_code();
				if(is_int) make_instr("cmp"+dtype,right->vali,r);
				else make_instr("cmp"+dtype,right->valf,r);
			}
			else{					// both operands are non-constants
				// more than 2 registers are available or left side is identifier
				if(rm.free_reg_count()>2 || left->is_identifier){
					right->gen_code();
					rm.pop_top();
					left->gen_code();
					Reg l = rm.get_top();
					rm.push_top(r);
					make_instr("cmp"+dtype,l,r);
				}
				// exactly 2 registers are available and left is not identifier
				else{
					right->gen_code();
					make_instr("push"+dtype,r);
					left->gen_code();
					rm.pop_top();
					Reg l = rm.get_top();
					rm.push_top(r);
					make_instr("load"+dtype,make_index(esp),l);
					make_instr("pop"+dtype,1);
					make_instr("cmp"+dtype,r,l);
				}
			}
			
			if(r_const) add_line_to_code(jump_inv+"(l"+to_string(label_num)+");");
			else add_line_to_code(jump+"(l"+to_string(label_num)+");");
			if(is_int) make_instr("move",0,r);
			else make_instr("move",0.0f,r);
			add_line_to_code("j(l"+to_string(label_num+1)+");");
			put_label = true;
			if(is_int) make_instr("move",1,r);
			else make_instr("move",1.0f,r);
			put_label = true;
		}
		return;
	}
	
	// If node is a conditional, then add control flow instructions
	if(is_cond){
		is_int ? make_instr("cmpi",0,r) : make_instr("cmpf",0.0f,r);
		if(fall) false_list.push_back(add_line_to_code("je"));
		else true_list.push_back(add_line_to_code("jne"));
	}
	reg_addr = r;
	return;
}

void UnOp::gen_code(){
	
	assert(!is_const);
	Reg r = rm.get_top();
	bool is_int = (type->basetype==BASETYPE::INT);
	string dtype = is_int ? "i" : "f";
	
	if(op_type==UMINUS_INT || op_type==UMINUS_FLOAT){
		exp->gen_code();
		if(is_int) make_instr("muli",-1,r);
		else make_instr("mulf",-1.0f,r);
	}
	else if(op_type==TO_INT || op_type==TO_FLOAT){
		exp->gen_code();
		if(is_int) make_instr("floatToint",r);
		else make_instr("intTofloat",r);
	}
	else if(op_type==PP_INT || op_type==PP_FLOAT){
		exp->gen_code();
		
		// If lhs is identifier, then r contains its value in it
		if(exp->is_identifier){
			if(is_int) make_instr("addi",1,r);									// increment value of r by 1
			else make_instr("addf",1.0f,r);
			
			make_instr("store"+dtype, r, make_index(ebp,exp->mem_offset));		// store incremented value back in lval
			if(is_int) make_instr("addi",-1,r);									// put decremented (i.e. old) value in r
			else make_instr("addf",-1.0f,r);
		}
		// But if lhs is arrayref, then r now contains its memory address (viz. pointer)
		else if(exp->is_arrayref){
			rm.pop_top();											// pop register r (contains mem. pointer)
			Reg l = rm.get_top();									// get top register l
			rm.push_top(r);											// place back r on top
			make_instr("load"+dtype, make_index(r), l);				// l <- *(r)
			if(is_int) make_instr("addi",1,r);						// l <- l + 1 (increment value by 1)
			else make_instr("addf",1.0f,r);
			make_instr("store"+dtype,l, make_index(r));				// *(r) <- l
			make_instr("move",l,r);									// r <- l (preparing value in r for return from this node)
			
			if(is_int) make_instr("addi",-1,r);						// r <- r - 1 (decrement value by 1 and get old value for return)
			else make_instr("addf",-1.0f,r);
		}
		else assert(0);
	}
	else{											//This is the case of op_type==NOT
		if(is_cond){
			exp->is_cond = true;
			exp->fall = !fall;
		}
		exp->gen_code();
		
		// If node is a conditional, then add control flow instructions
		if(is_cond){
			is_int ? make_instr("cmpi",0,r) : make_instr("cmpf",0.0f,r);
			if(fall) false_list.push_back(add_line_to_code("jne"));
			else true_list.push_back(add_line_to_code("je"));
			
			copy_list(exp->true_list, false_list);	// add exp's false list in not's true list
			copy_list(exp->false_list, true_list);  // add exp's true list in not's false list
		}
		else{
			make_instr("cmp"+dtype,0,r);									// (r==0)?
			int l1 = add_line_to_code("je(l"+to_string(label_num)+");");	// If yes, then jump to r=1
			is_int ? make_instr("move",0,r) : make_instr("move",0.0f,r);	// If not, then set r=0
			make_instr("j","l"+to_string(label_num+1));						// and then jump outside unconditionally
			put_label = true;												// mark put_label as true because (r=1) needs a label
			is_int ? make_instr("move",1,r) : make_instr("move",1.0f,r);	// r=1
			put_label = true;												// mark put_label as true because next instr. needs a label
		}
		reg_addr = r;
		return;
	}
	
	// If node is a conditional, then add control flow instructions
	if(is_cond){
		is_int ? make_instr("cmpi",0,r) : make_instr("cmpf",0.0f,r);
		if(fall) false_list.push_back(add_line_to_code("je"));
		else true_list.push_back(add_line_to_code("jne"));
	}
	reg_addr = r;
	return;
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
	if(!is_printf) make_instr("push"+dtype,0);
	
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
				else{
					make_instr("pushi",expression_list[i]->vali);
					arg_types.push_back(0);
				}
			}
			else{
				if(is_printf) make_instr("print_float",expression_list[i]->valf);
				else{
					make_instr("pushf",expression_list[i]->valf);
					arg_types.push_back(1);
				}
			}
		}
		else{
			expression_list[i]->gen_code();
			if(is_int){
				if(is_printf) make_instr("print_int",r);
				else{
					make_instr("pushi",r);
					arg_types.push_back(0);
				}
			}
			else{
				if(is_printf) make_instr("print_float",r);
				else{
					make_instr("pushf",r);
					arg_types.push_back(1);
				}
			}
		}
		
		if(is_printf) i++;
		else i--;
	}
	
	// Return if the function is printf, as it's job is over
	if(is_printf){
		make_instr("move",0,r1);
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
	make_instr("load"+dtype,make_index(esp),r1);
	make_instr("pop"+dtype,1);

	// Load the registers which were saved
	rm.load_registers(in_use, is_int);
	
	// If node is a conditional, then add control flow instructions
	if(is_cond){
		(ret_type==BASETYPE::INT) ? make_instr("cmpi",0,r1) : make_instr("cmpf",0.0f,r1);
		if(fall) false_list.push_back(add_line_to_code("je"));
		else true_list.push_back(add_line_to_code("jne"));
	}
	reg_addr = r1;
	return;
}

void FloatConst::gen_code(){ assert(0);}
void IntConst::gen_code(){ cout<<val<<endl; assert(0);}
void StringConst::gen_code(){ assert(0);}

// This function should return value of the array ref in the top register r
void ArrayRef::gen_code(){
	
	gen_code_addr();					// First call gen_code_addr to get pointer to the required location
	Reg r = rm.get_top();				// Top register (currently contains pointer to the required value)
	string dtype = (type->basetype==BASETYPE::INT)?"i":"f";
	
	make_instr("load"+dtype,make_index(r),r); // r <- *(r)
	
	// If node is a conditional, then add control flow instructions
	if(is_cond){
		(type->basetype==BASETYPE::INT) ? make_instr("cmpi",0,r) : make_instr("cmpf",0.0f,r);
		if(fall) false_list.push_back(add_line_to_code("je"));
		else true_list.push_back(add_line_to_code("jne"));
	}
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
		rm.push_top(r);			// push back the top register r
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
