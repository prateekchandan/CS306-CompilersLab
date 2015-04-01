#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cstdlib>
using namespace std;



// Classes and global variables related to symbol table maintenance
enum SCOPE{
	GLOBAL = 0,
	LOCAL = 1,
	PARAM = 2,
};

enum VAR_OR_FUNC{
	VAR = 0,
	FUNC = 1
};

enum BASETYPE{
	VOID = 0,
	INT = 1,
	FLOAT = 2,
	STRING = 3
};

struct TYPE{
	int size;
	BASETYPE basetype;
	TYPE* child;
	
	TYPE(){size = -1; child=NULL;}
	TYPE(BASETYPE type){
		child = NULL;
		switch(type){
			case BASETYPE::VOID: // VOID
				size = 0;
				basetype = BASETYPE::VOID;
				break;
			case BASETYPE::INT: // INT
				size = 4;
				basetype = BASETYPE::INT;
				break;
			case BASETYPE::FLOAT: // FLOAT
				size = 4;
				basetype = BASETYPE::FLOAT;
				break;
			default:
				basetype = type;
				size = 0;
		}
	}
	TYPE(TYPE* c,int s){
		child = c;
		size = s;
	}

	void print(){
		if(child != NULL){
			cout<<"array("<<size<<",";
				child->print();
			cout<<")";
		}
		else if(basetype == BASETYPE::VOID) cout<<"void";
		else if(basetype == BASETYPE::INT) cout<<"int";
		else if(basetype == BASETYPE::FLOAT) cout<<"float";
	}

	void reverse(){
		if(child == NULL)
			return ;
		vector<int> sizes;
		TYPE* temp = this;
		while(temp->child!=NULL){
			sizes.push_back(temp->size);
			temp = temp->child;
		}
		temp = this;
		for (int i = sizes.size()-1; i >= 0; --i)
		{
			temp->size = sizes[i];
			temp= temp->child;
		}
	}
};

class SymbolTable;

// symbolTable Entry class
struct SymbolTableEntry
{
	string symbolName;
	VAR_OR_FUNC vf;
	SCOPE scope;
	TYPE *type;
	int size;
	int offset;
	SymbolTable* table;

	SymbolTableEntry(){
		table = NULL;
		size = 0; 
		offset=0;
		type =NULL;
	};

	~SymbolTableEntry(){};

	void print(){
		cout<<symbolName<<"\t";
		switch(scope){
			case SCOPE::LOCAL : cout<<"local"; break;
			case SCOPE::PARAM : cout<<"param"; break;
			case SCOPE::GLOBAL : cout<<"global"; break;
		}
		
		cout<<" size:"<<size;
		cout<<"\t";
		switch(vf){
			case VAR_OR_FUNC::VAR : cout<<"variable"; break;
			case VAR_OR_FUNC::FUNC : cout<<"function"; break;
		}
		cout<<"\t";
		cout<<offset;
		cout<<"\t";
		
		if(type!=NULL)
			type->print();
		
		cout<<endl;
	}	
};

class SymbolTable
{
	string name;
	map<string,SymbolTableEntry*> Entry;
	vector<BASETYPE> arg_types;

	public:
	SymbolTable(string n = "Junk"){
		name = n;
	};
	~SymbolTable();

	bool AddEntry(string s,SymbolTableEntry* En1){
		if(En1->type != NULL){
			En1->type->reverse();
		}
		if(Entry.find(s) == Entry.end()){
			Entry[s] = En1;
			return true;
		}
		return false;
	}

	SymbolTableEntry* GetEntry(string s){
		if(Entry.find(s)==Entry.end())
			return NULL;
		return Entry[s];
	}
	void print(){
		cout<<"\nSymboltable for "<<name<<": \n";
		for (map<string,SymbolTableEntry*>::iterator it=Entry.begin(); it!=Entry.end(); ++it)
    		it->second->print();
    	cout<<endl;
	}


	void arg_type_add(BASETYPE b){
		arg_types.push_back(b);
	}
	vector<BASETYPE> get_param_types(){
		return arg_types;
	}
};


/***********************************************
************* DEFINITIONS FOR AST **************
************************************************/

// Some enum types for operators
enum OP_TYPE{
	OR_OP = 0,
	AND_OP,
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
	PLUS_INT,
	PLUS_FLOAT,
	MINUS_INT,
	MINUS_FLOAT,
	MULT_INT,
	MULT_FLOAT,
	DIV_INT,
	DIV_FLOAT,
	ASSIGN_INT,
	ASSIGN_FLOAT
};

enum UNOP_TYPE{
	UMINUS_INT = 24,
	UMINUS_FLOAT,
	NOT,
	PP_INT,
	PP_FLOAT,
	TO_INT,
	TO_FLOAT
};

// Abstract class for a node in the AST /////////////////////////////////////
class abstract_astnode {
	public:
	virtual void print () = 0;
	//virtual std::string generate_code(const symbolTable&) = 0;
	//virtual basic_types getType() = 0;
	//virtual bool checkTypeofAST() = 0;

	protected:
	//virtual void setType(basic_types) = 0;

	private:
	//typeExp astnode_type;
};
/////////////////////////////////////////////////////////////////////////////

// These are the high-level classes inherited from the abstract node class //

class StmtAst : public abstract_astnode {
	public:
	virtual void print () = 0;
};

class ExpAst : public abstract_astnode {
	public:
		TYPE* type;
	virtual void print () = 0;
};

// Declaring Identifier class here itself, as it is required for FunCallStmt
class Identifier : public ExpAst {
	
	protected:
	string id;
	
	public:
	Identifier() {}
	Identifier(string s){
		id = s;
	}
	void print();
};

/////////////////////////////////////////////////////////////////////////////

// Child classes of StmtAst class //

class BlockAst : public StmtAst {
	protected:
	vector<StmtAst*> statements;
	
	public:
	BlockAst() {}
	void print();
	void add_statement(StmtAst *e);
};

class Ass : public StmtAst {
	
	protected:
	ExpAst *left;
	ExpAst *right;
	
	public:
	Ass() {}
	Ass(ExpAst *l, ExpAst *r) {
		left = l;
		right = r;
	}
	void print();
};
	
class ReturnSt : public StmtAst {
	
	protected:
	ExpAst *exp;
	
	public:
	ReturnSt() {}
	ReturnSt(ExpAst *r) {
		exp = r;
	}
	void print();
};

class If : public StmtAst {
	
	protected:
	ExpAst *cond;
	StmtAst *statement1;
	StmtAst *statement2;
	
	public:
	If() {}
	If(ExpAst *c, StmtAst *s1, StmtAst *s2) {
		cond = c;
		statement1 = s1;
		statement2 = s2;
	}
	void print();
};

class While : public StmtAst {
	
	protected:
	ExpAst *cond;
	StmtAst *statement;
	
	public:
	While() {}
	While(ExpAst *c, StmtAst *s) {
		cond = c;
		statement = s;
	}
	void print();
};

class For : public StmtAst {
	
	protected:
	ExpAst *init;
	ExpAst *cond;
	ExpAst *step;
	StmtAst *statement;
	
	public:
	For() {}
	For(ExpAst *i, ExpAst *c, ExpAst *s, StmtAst *st) {
		init = i;
		cond = c;
		step = s;
		statement = st;
	}
	void print();
};

class FunCallStmt : public StmtAst {
	
	protected:
	Identifier *name;
	
	public:
	vector<ExpAst*> expression_list;
	FunCallStmt() {}
	FunCallStmt(Identifier *i){
		name = i;
	}
	void print();
	void set_name(Identifier *i);
	void add_expression(ExpAst *e);
	void set_expression_list(vector<ExpAst*> exps);
	int get_param_count();	
};
/////////////////////////////////////////////////////////////////////////////

// Child classes of ExpAst class //
 
class Op : public ExpAst {
	
	protected:
	int op_type;
	ExpAst *left;
	ExpAst *right;
	
	public:
	Op() {}
	Op(int o, ExpAst *l, ExpAst *r) {
		op_type = o;
		left = l;
		right = r;
	}
	void print();
};

class UnOp : public ExpAst {
	
	protected:
	int op_type;
	ExpAst *exp;
	
	public:
	UnOp() {}
	UnOp(int o) {
		op_type = o;
	}
	UnOp(int o, ExpAst *e) {
		op_type = o;
		exp = e;
	}
	int get_type(){
		return op_type;
	}
	void set_type(int o){
		op_type = o;
	}
	void set_expression(ExpAst *e);
	void print();
};

class FunCall : public ExpAst {
	
	protected:
	Identifier *name;
	
	public:
	vector<ExpAst*> expression_list;
	FunCall() {}
	FunCall(Identifier *i){
		name = i;
	}
	void print();
	void set_name(Identifier *i);
	void add_expression(ExpAst *e);
	int get_param_count();
	vector<ExpAst*> get_expression_list();
};

class FloatConst : public ExpAst {
	
	protected:
	float val;
	
	public:
	FloatConst() {}
	FloatConst(float f){
		val = f;
		type = new TYPE(BASETYPE::FLOAT);
	}
	void print();
	float getVal(){return val;}
};

class IntConst : public ExpAst {
	
	protected:
	int val;
	
	public:
	IntConst() {}
	IntConst(int i){
		val = i;
		type = new TYPE(BASETYPE::INT);
	}
	void print();
	int getVal(){return val;}
};

class StringConst : public ExpAst {
	
	protected:
	string val;
	
	public:
	StringConst() {}
	StringConst(string s){
		val = s;
		type = new TYPE(BASETYPE::STRING);
	}
	void print();
	string getVal(){return val;}
};

class ArrayRef : public ExpAst {
	
	protected:
	Identifier *name;
	vector<ExpAst*> indices;
	
	public:
	ArrayRef() {}
	ArrayRef(Identifier *i){
		name = i;
	}
	ArrayRef(ArrayRef* child){
		indices = child->indices;
		name = child->name;
	}
	void add_index(ExpAst *e);
	void print();
};




