#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
using namespace std;

// Some enum types for operators
enum OP_TYPE{
	OR_OP = 0,
	AND_OP = 1,
	EQ_OP = 2,
	NE_OP = 3,
	LT = 4,
	LE_OP = 5,
	GT = 6,
	GE_OP = 7,
	PLUS = 8,
	MINUS = 9,
	MULT = 10,
	DIV = 11,
	ASSIGN = 12
};

enum UNOP_TYPE{
	UMINUS = 13,
	NOT = 14,
	PP = 15
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
	virtual void print () = 0;
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
	void set_expression(ExpAst *e);
	void print();
};

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

class FunCall : public ExpAst {
	
	protected:
	Identifier *name;
	vector<ExpAst*> expression_list;
	
	public:
	FunCall() {}
	FunCall(Identifier *i){
		name = i;
	}
	void print();
	void set_name(Identifier *i);
	void add_expression(ExpAst *e);
};

class FloatConst : public ExpAst {
	
	protected:
	float val;
	
	public:
	FloatConst() {}
	FloatConst(float f){
		val = f;
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
	FLOAT = 2
};

struct TYPE{
	int size;
	BASETYPE basetype;
	TYPE* child;
	TYPE(){size = -1;}
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

	SymbolTableEntry(){table = NULL;};
	~SymbolTableEntry(){};
	void print(){
		cout<<symbolName<<"\t";
		switch(scope){
			case SCOPE::LOCAL : cout<<"local"; break;
			case SCOPE::PARAM : cout<<"param"; break;
			case SCOPE::GLOBAL : cout<<"global"; break;
		}
		cout<<"\t";
		type->print();
		cout<<" size:"<<size;
		cout<<"\t";
		switch(vf){
			case VAR_OR_FUNC::VAR : cout<<"variable"; break;
			case VAR_OR_FUNC::FUNC : cout<<"function"; break;
		}
		cout<<"\t";
		cout<<offset;
		cout<<endl;
	}
	
};

class SymbolTable
{
	map<string,SymbolTableEntry*> Entry;
public:
	SymbolTable(){};
	~SymbolTable();

	bool AddEntry(string s,SymbolTableEntry* En1){
		if(Entry.find(s) == Entry.end()){
			Entry[s] = En1;
			return true;
		}
		return false;
	}
};





