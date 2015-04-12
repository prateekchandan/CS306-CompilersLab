///////////////////////////////////////////////////////////////////////////////
/////////////// GLOBAL VARIABLES FOR SYMBOL TABLE GENERATION //////////////////
///////////////////////////////////////////////////////////////////////////////

int print_symbol_table = 1;										// Flag indicating whether to print symbol table
SCOPE current_scope = SCOPE::GLOBAL;							// For knowing the current scope while parsing

SymbolTable* CurrentSymbolTable = new SymbolTable("Global");	// Pointer to current symbol table (Initially global)
vector<SymbolTable*> SymbolTableStack(1,CurrentSymbolTable);	// Stack of symbol tables (for nested function defn.s)
vector<int> offsetStack(1,0);									// Stack of their corresponding current offsets

TYPE* curr_type;												// Type of variable whose decln. is currently being parsed
int global_offset = 8;											// Current offset of the global symbol table (Initially 0)
int offset_multiplier = 1;										// Value is 1 for params and -1 for funstions

///////////////////////////////////////////////////////////////////////////////
/////////// HELPER FUNCTIONS FOR TYPE CHECKING ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Validates if typecasting is possible
TYPE* is_compatible(TYPE* a , TYPE* b){
	if(a->child!=NULL || b->child != NULL)
		return NULL;
	if(a->basetype == BASETYPE::FLOAT && (b->basetype == BASETYPE::FLOAT || b->basetype == BASETYPE::INT))
		return a;
	if(b->basetype == BASETYPE::FLOAT && (a->basetype == BASETYPE::FLOAT || a->basetype == BASETYPE::INT))
		return b;
	if(a->basetype == BASETYPE::INT && b->basetype == BASETYPE::INT)
		return a;
	else return NULL;
}

bool operator==(TYPE a , TYPE b){
	if(a.child!=NULL || b.child != NULL)
		return false;
	return a.basetype == b.basetype;
}

// Validates if type check is possible , exits otherwise
TYPE* validate(ExpAst* a , ExpAst *b){
	TYPE* temp = is_compatible(a->type,b->type);
	return temp;
}

// get typecasting operator
UNOP_TYPE getTypeCast(TYPE* a){
	if(a->basetype == BASETYPE::INT)
		return UNOP_TYPE::TO_INT;
	if(a->basetype == BASETYPE::FLOAT)
		return UNOP_TYPE::TO_FLOAT;

	cout<<"Error at Line "/*<<line_no<<*/" : Unable to Type cast\n";
	exit(0);
}

TYPE* ExpAstTypeCast(ExpAst **a,ExpAst **b , bool flag = true){
	TYPE* type = validate(*a,*b);
	if(type==NULL)
	{
		return NULL;
	}
	if(type->basetype != BASETYPE::INT && type->basetype != BASETYPE::FLOAT)
		return NULL;

	if(flag && (*a)->type->basetype != type->basetype)
		*a = new UnOp(getTypeCast(type), *a);

	if((*b)->type->basetype != type->basetype)
		*b = new UnOp(getTypeCast(type), *b);

	if(flag)
		return type;
	else
		return  (*a)->type;
}	

vector<BASETYPE> GetFuncParamTypes(string s){
	SymbolTable* temp = CurrentSymbolTable;
	SymbolTableEntry* ret = temp->GetEntry(s);
	if(ret!=NULL)
		return ret->table->get_param_types();

	for (int i = SymbolTableStack.size()-1; i >= 0  ; --i)
	{
		temp = SymbolTableStack[i];
		ret = temp->GetEntry(s);
		if(ret!=NULL)
			return ret->table->get_param_types();
	}
	vector<BASETYPE> temp1;
	return temp1;
}

//Size of Parameter list for a function
int GetFuncParamCount(string s){
	return GetFuncParamTypes(s).size();
}


TYPE* SearchSymbolTable(string s,int line_no,int type=1){
	SymbolTable* temp = CurrentSymbolTable;
	SymbolTableEntry* ret = temp->GetEntry(s);
	if(ret!=NULL){
		if(type != ret->vf)
		{
			cout<<"Error at line "<<line_no<<" : "<<s<<" is a "<<(type==0?"function":"variable");
			cout<<" and not "<<(type==1?"function":"variable")<<"\n";
			exit(0);
		}
		return ret->type;
	}
	for (int i = SymbolTableStack.size()-1; i >= 0  ; --i)
	{
		temp = SymbolTableStack[i];
		ret = temp->GetEntry(s);
		if(ret!=NULL){
			if(type != ret->vf)
			{
				cout<<"Error at line "<<line_no<<" : "<<s<<" is a "<<(type==0?"function":"variable");
				cout<<" and not "<<(type==1?"function":"variable")<<"\n";
				exit(0);
			}
			return ret->type;
		}
	}
	return NULL;
}

//validates if the function signature is correct and then typecasts if required
void validateBasetypes(vector<BASETYPE> b,vector<ExpAst*> &a,int line_no){
	for(int i=0;i<b.size();++i){
		// An array check
		if(a[i]->type==NULL || a[i]->type->child != NULL || (a[i]->type->basetype != BASETYPE::INT && a[i]->type->basetype!= BASETYPE::FLOAT)){
			cout<<"Error at line "<<line_no<<": Passing non int/float values\n";
			exit(0);
		}
		if(b[i] != a[i]->type->basetype){
			TYPE temp(b[i]);
			a[i] = new UnOp(getTypeCast(&temp) , a[i]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//////// DEFINITIONS OF METHODS FOR CLASSES RELATING TO SYMBOL TABLE //////////
///////////////////////////////////////////////////////////////////////////////

// Methods of TYPE class //

void TYPE::print(){
	if(child != NULL){
		cout<<"array("<<size<<",";
			child->print();
		cout<<")";
	}
	else if(basetype == BASETYPE::VOID) cout<<"void";
	else if(basetype == BASETYPE::INT) cout<<"int";
	else if(basetype == BASETYPE::FLOAT) cout<<"float";
}

void TYPE::reverse(){
	if(child == NULL) return;
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

// Methods of SymbolTableEntry class //

void SymbolTableEntry::print(){
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

// Methods of SymbolTable class //

bool SymbolTable::AddEntry(string s,SymbolTableEntry* En1){
	if(En1->type != NULL){
		En1->type->reverse();
	}
	if(Entry.find(s) == Entry.end()){
		Entry[s] = En1;
		return true;
	}
	return false;
}

SymbolTableEntry* SymbolTable::GetEntry(string s){
	if(Entry.find(s)==Entry.end())
		return NULL;
	return Entry[s];
}

void SymbolTable::print(){
	cout<<"\nSymboltable for "<<name<<": \n";
	for (map<string,SymbolTableEntry*>::iterator it=Entry.begin(); it!=Entry.end(); ++it)
   		it->second->print();
   	cout<<endl;
}

void SymbolTable::arg_type_add(BASETYPE b){
	arg_types.push_back(b);
}

vector<BASETYPE> SymbolTable::get_param_types(){
	return arg_types;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
