#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <cstdlib>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
/////////// CLASSES HELPFUL IN SYMBOL TABLE GENERATION ////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Classes related to symbol table maintenance //

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

// Class for variable type //
struct TYPE{
	
	int size;
	BASETYPE basetype;
	TYPE* child;
	
	// Constructors for the type class
	TYPE(){
		size = -1;
		child=NULL;
	}
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

	// Methods of the class
	void print();
	void reverse();
};

// Forward declaration //
class SymbolTable;

// Class for an entry of the Symbol Table //
struct SymbolTableEntry
{
	string symbolName;
	VAR_OR_FUNC vf;
	SCOPE scope;
	TYPE *type;
	int size;
	int offset;
	SymbolTable* table;

	// Constructor and destructor for this class
	SymbolTableEntry(){
		table = NULL;
		size = 0; 
		offset=0;
		type =NULL;
	};
	~SymbolTableEntry(){};

	// Print method
	void print();
};

// Class for Symbol Table //
class SymbolTable
{
	string name;
	map<string,SymbolTableEntry*> Entry;
	vector<BASETYPE> arg_types;

	public:
	TYPE* returnType;
	int return_offset;
	
	// Constructors and desctructor of this class
	SymbolTable(string n = "Junk"){
		name = n;
	}
	~SymbolTable();

	// Methods of the class
	bool AddEntry(string s,SymbolTableEntry* En1);
	SymbolTableEntry* GetEntry(string s);
	void print();
	void arg_type_add(BASETYPE b);
	vector<BASETYPE> get_param_types();
};
