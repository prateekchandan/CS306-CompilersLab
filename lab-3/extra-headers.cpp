#include <vector>
#include <cstring>

using namespace std;

struct typeExp
{
	string s;
	typeExp(){};
	typeExp(string s1)
	{
		s=s1;
	}
};

class abstract_astnode
{
	public:
		vector<abstract_astnode*> children;
		virtual void print () = 0;
		//virtual std::string generate_code(const symbolTable&) = 0;
		//virtual basic_types getType() = 0;
		//virtual bool checkTypeofAST() = 0;
	protected:
		//virtual void setType(basic_types) = 0;
		typeExp astnode_type;
	private:
		
};

class stmtAst : public abstract_astnode{
	public:
		void print(){};

};

class expAst : public abstract_astnode{
	public:

};

class arrayRef : public abstract_astnode{
	public:
};


vector<abstract_astnode*> node_pointers;
int node_count=0;