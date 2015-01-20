%%

\"(\\[nt"\\]|[^"\\])*\"						return STRING_LITERAL;

\/\/[^\n]*[\n]						// skip line comments
\/\*([^*]*\*+[^*/])*[^*]*\*+\/		// skip block comments
[ \t\n]								// skip whitespaces

void								return VOID;
int									return INT;
float								return FLOAT;
return								return RETURN;
if									return IF;
else								return ELSE;
while								return WHILE;
for									return FOR;

"||"								return OR_OP;
"&&"								return AND_OP;
"=="								return EQ_OP;
"!="								return NE_OP;
"<="								return LE_OP;
">="								return GE_OP;
"++"								return INC_OP;
"--"								return DEC_OP;

[[:alpha:]_][[:alpha:][:digit:]_]*													return IDENTIFIER;
[-+]?(([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*))						return INT_CONSTANT;
[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?|[-+]?[0-9]+\.?[0-9]*([eE][-+]?[0-9]+)?		return FLOAT_CONSTANT;

.									return matched()[0];
