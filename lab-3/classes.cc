string operator_map[15] = {"OR" , "AND" , "EQ_OP" , "NE_OP" , "LT" , 
	"LE_OP" , "GT" , "GE_OP" , "PLUS" , "MINUS" ,"MULT" , "ASSIGN",
	"UMINUS" , "NOT" , "INC_OP"} ;

void BlockAst::add_statement(StmtAst *e){
	statements.push_back(e);
}

void FunCall::set_name(Identifier *i){
	name = i;
}

void FunCall::add_expression(ExpAst *e){
	expression_list.push_back(e);
}

void ArrayRef::add_index(ExpAst *e){
	indices.push_back(e);
}

void UnOp::set_expression(ExpAst *e){
	exp = e;
}
