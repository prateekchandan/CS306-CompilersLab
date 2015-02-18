#include <iostream>
#include <vector>
using namespace std;

// Some global variable to hold map of ENUM_TYPES to Strings
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
	ASSIGN = 11
};

enum UNOP_TYPE{
	UMINUS = 12,
	NOT = 13,
	PP = 14
};

// Abstract class for a node in the AST /////////////////////////////////////
class abstract_astnode {
	public:
	//virtual void print () = 0;
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
};

class ExpAst : public abstract_astnode {
};

/////////////////////////////////////////////////////////////////////////////

// Child classes of StmtAst class //

class BlockAst : public StmtAst {
	protected:
	vector<StmtAst*> statements;
	
	public:
	BlockAst() {}
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
};
	
class ReturnSt : public StmtAst {
	
	protected:
	ExpAst *exp;
	
	public:
	ReturnSt() {}
	ReturnSt(ExpAst *r) {
		exp = r;
	}
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
};

class Identifier : public ExpAst {
	
	protected:
	string id;
	
	public:
	Identifier() {}
	Identifier(string s){
		id = s;
	}
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
};

class IntConst : public ExpAst {
	
	protected:
	int val;
	
	public:
	IntConst() {}
	IntConst(int i){
		val = i;
	}
};

class StringConst : public ExpAst {
	
	protected:
	string val;
	
	public:
	StringConst() {}
	StringConst(string s){
		val = s;
	}
};

class ArrayRef : public ExpAst {
	
	protected:
	string id;
	vector<ExpAst*> indices;
	
	public:
	ArrayRef() {}
	ArrayRef(string s, vector<ExpAst*> &i){
		id = s;
		indices = i;
	}
	void add_index(ExpAst *e);
};
