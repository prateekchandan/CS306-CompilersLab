#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cstdlib>
#include <string>
#include <assert.h>
using namespace std;

#define INF 1000000000

/////////////////////////////////////////////////////////////////////////////
// DEFINITIONS FOR AST //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

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
	virtual void gen_code() = 0;
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
	virtual void gen_code() = 0;
};

class ExpAst : public abstract_astnode {
	public:
	TYPE* type;
	int reg_addr = -1;
	int mem_addr = INF;
	bool is_lval = false;
	bool is_leaf = false;
	
	virtual void print () = 0;
	virtual void gen_code() = 0;
};
/////////////////////////////////////////////////////////////////////////////

// Declaring Identifier class here itself, as it is required for FunCallStmt //

class Identifier : public ExpAst {
	
	protected:
	string id;
	
	public:
	Identifier(){
		is_lval = true;
		is_leaf = true;
	}
	Identifier(string s){
		is_lval = true;
		is_leaf = true;
		id = s;
	}
	
	void print();
	void gen_code();
};

/////////////////////////////////////////////////////////////////////////////
// Child classes of StmtAst class ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class BlockAst : public StmtAst {
	protected:
	vector<StmtAst*> statements;
	SymbolTable* curr;
	
	public:
	BlockAst(){}
	BlockAst(SymbolTable* s){
		curr = s;
	}
	void print();
	void add_statement(StmtAst *e);
	//int get_stmtlist_length();
	//StmtAst* get_stmt_i(int i);
	void gen_code();
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
	void gen_code();
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
	void gen_code();	
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
	void gen_code();	
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
	void gen_code();	
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
	void gen_code();	
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
	void gen_code();	
};

/////////////////////////////////////////////////////////////////////////////
// Child classes of ExpAst class ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
 
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
	void gen_code();	
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
	void gen_code();	
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
	void gen_code();	
};

class FloatConst : public ExpAst {
	
	protected:
	float val;
	
	public:
	FloatConst(){
		is_leaf = true;
	}
	FloatConst(float f){
		is_leaf = true;
		val = f;
		type = new TYPE(BASETYPE::FLOAT);
	}
	void print();
	float getVal() { return val; }
	void gen_code();	
};

class IntConst : public ExpAst {
	
	protected:
	int val;
	
	public:
	IntConst(){
		is_leaf = true;
	}
	IntConst(int i){
		is_leaf = true;
		val = i;
		type = new TYPE(BASETYPE::INT);
	}
	void print();
	int getVal() { return val; }
	void gen_code();
};

class StringConst : public ExpAst {
	
	protected:
	string val;
	
	public:
	StringConst(){
		is_leaf = true;
	}
	StringConst(string s){
		is_leaf = true;
		val = s;
		type = new TYPE(BASETYPE::STRING);
	}
	void print();
	string getVal() { return val; }
	void gen_code();
};

class ArrayRef : public ExpAst {
	
	protected:
	Identifier *name;
	vector<ExpAst*> indices;
	
	public:
	ArrayRef(){
		is_lval = true;
		is_leaf = true;
	}
	ArrayRef(Identifier *i){
		is_lval = true;
		is_leaf = true;
		name = i;
	}
	ArrayRef(ArrayRef* child){
		indices = child->indices;
		name = child->name;
	}
	void add_index(ExpAst *e);
	void print();
	void gen_code();
};
