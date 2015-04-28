#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cstdlib>
#include <string>
#include <utility>
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
	vector<int> next_list;
	bool is_last = false;	// marks if statement is the last in the block
	
	virtual void print () = 0;
	virtual void gen_code() = 0;
};

class ExpAst : public abstract_astnode {
	
	public:
	TYPE* type;
	int reg_addr = -1;			// Register containing the node's value (if register is used)
	int mem_offset = INF;		// ebp offset of location containing node's value (if stack is used)
	
	int vali;
	float valf;
	
	bool is_identifier = false;
	bool is_const = false;
	bool is_arrayref = false;
	
	bool is_cond = false;		// tells whether the expression is to be evaluated as normal exp or as condition exp
	// These are required for short-circuiting of logical exps
	bool fall;					// required for fall through
	vector<int> true_list;
	vector<int> false_list;
	
	virtual void print () = 0;
	virtual void gen_code() = 0;
};
/////////////////////////////////////////////////////////////////////////////

// Declaring Identifier class here itself, as it is required for FunCallStmt //

class Identifier : public ExpAst {
	
	protected:
	string id;
	
	public:

	SymbolTable* defined_at;

	Identifier(){
		is_identifier = true;
	}
	Identifier(string s){
		is_identifier = true;
		id = s;
	}
	
	string get_id();
	void print();
	void gen_code();
	void gen_code_addr();
};

/////////////////////////////////////////////////////////////////////////////
// Child classes of StmtAst class ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class BlockAst : public StmtAst {
	protected:
	vector<StmtAst*> statements;
	SymbolTable* symbolTable;
	
	public:
	BlockAst(){}
	BlockAst(SymbolTable* s){
		symbolTable = s;
	}
	void print();
	void add_statement(StmtAst *e);
	SymbolTable* get_symbol_table();
	void gen_code();
};

class Ass : public StmtAst {
	
	protected:
	ExpAst *left;
	ExpAst *right;
	
	public:
	Ass(){}
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
	ReturnSt(){}
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
	SymbolTable* table;
	
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
		if(l->is_const && r->is_const){
			is_const = true;
			if(o==AND_OP){
				bool l_zero = (left->type->basetype==INT) ? (l->vali==0) : (l->valf==0);
				bool r_zero = (right->type->basetype==INT) ? (r->vali==0) : (r->valf==0);
				vali = (l_zero || r_zero) ? 0 : 1;
			}
			if(o==OR_OP){
				bool l_zero = (left->type->basetype==INT) ? (l->vali==0) : (l->valf==0);
				bool r_zero = (right->type->basetype==INT) ? (r->vali==0) : (r->valf==0);
				vali = (l_zero && r_zero) ? 0 : 1;
			}
			if(o==PLUS_INT) vali = l->vali + r->vali;
			if(o==PLUS_FLOAT) valf = l->valf + r->valf;
			if(o==MINUS_INT) vali = l->vali - r->vali;
			if(o==MINUS_FLOAT) valf = l->valf - r->valf;
			if(o==MULT_INT) vali = l->vali * r->vali;
			if(o==MULT_FLOAT) valf = l->valf * r->valf;
			if(o==DIV_INT) vali = l->vali / r->vali;
			if(o==DIV_FLOAT) valf = l->valf / r->valf;
			if(o==EQ_OP_INT) vali = (l->vali == r->vali);
			if(o==EQ_OP_FLOAT) vali = (l->valf == r->valf);
			if(o==NE_OP_INT) vali = (l->vali != r->vali);
			if(o==NE_OP_FLOAT) vali = (l->valf != r->valf);
			if(o==LT_INT) vali = (l->vali < r->vali);
			if(o==LT_FLOAT) vali = (l->valf < r->valf);
			if(o==LE_OP_INT) vali = (l->vali <= r->vali);
			if(o==LE_OP_FLOAT) vali = (l->valf <= r->valf);
			if(o==GT_INT) vali = (l->vali > r->vali);
			if(o==GT_FLOAT) vali = (l->valf > r->valf);
			if(o==GE_OP_INT) vali = (l->vali >= r->vali);
			if(o==GE_OP_FLOAT) vali = (l->valf >= r->valf);
		}
		else if(l->is_const || r->is_const){
			int l_nonzero = (left->type->basetype==INT) ? (l->vali!=0) : (l->valf!=0);
			int r_nonzero = (right->type->basetype==INT) ? (r->vali!=0) : (r->valf!=0);
			if(o==OR_OP) is_const = (l->is_const) ? l_nonzero : r_nonzero;
			if(o==AND_OP) is_const = (l->is_const) ? !l_nonzero : !r_nonzero;
		}
		return;
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
		if(o==TO_INT) type = new TYPE(INT);
		if(o==TO_FLOAT) type = new TYPE(FLOAT);
		exp = e;
		
		if(e->is_const){
			is_const = true;
			if(o==UMINUS_INT) vali = -e->vali;
			if(o==UMINUS_FLOAT) valf = -e->valf;
			if(o==TO_INT) vali = e->valf;
			if(o==TO_FLOAT) valf = e->vali;
			if(o==NOT){
				if(e->type->basetype==BASETYPE::INT){
					if(e->vali != 0) vali = 0;
					else vali = 1;
				}
				if(e->type->basetype==BASETYPE::FLOAT){
					if(e->valf != 0.0) valf = 0.0;
					else valf = 1.0;
				}
			}
		}
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
	SymbolTable* table;
	
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
		is_const = true;
		val = 0.0;
		valf = 0.0;
		type = new TYPE(BASETYPE::FLOAT);
	}
	FloatConst(float f){
		is_const = true;
		val = f;
		valf = f;
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
		is_const = true;
		val = 0;
		vali = 0;
		type = new TYPE(BASETYPE::INT);
	}
	IntConst(int i){
		is_const = true;
		val = i;
		vali = i;
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
		is_const = true;
		val = "";
		type = new TYPE(BASETYPE::STRING);
	}
	StringConst(string s){
		is_const = true;
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

	SymbolTable* defined_at;

	ArrayRef(){
		is_arrayref = true;
	}
	ArrayRef(Identifier *i){
		mem_offset = i->mem_offset;
		is_arrayref = true;
		name = i;
	}
	ArrayRef(ArrayRef* child){
		mem_offset = child->mem_offset;
		is_arrayref = true;
		indices = child->indices;
		name = child->name;
	}
	void add_index(ExpAst *e);
	void print();
	void gen_code();
	void gen_code_addr();
};
