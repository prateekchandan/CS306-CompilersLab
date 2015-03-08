%%

\"(\\[nt"\\]|[^"\\])*\"				{
									Parser::d_val__ = std::string(matched());
									return Parser::STRING_LITERAL;
									}

\/\/[^\n]*[\n]						// skip line comments
\/\*([^*]*\*+[^*/])*[^*]*\*+\/		// skip block comments
[ \t\n]								// skip whitespaces

void								return Parser::VOID;
int									return Parser::INT;
float								return Parser::FLOAT;
return								return Parser::RETURN;
if									return Parser::IF;
else								return Parser::ELSE;
while								return Parser::WHILE;
for									return Parser::FOR;

"||"								return Parser::OR_OP;
"&&"								return Parser::AND_OP;
"=="								return Parser::EQ_OP;
"!="								return Parser::NE_OP;
"<="								return Parser::LE_OP;
">="								return Parser::GE_OP;
"++"								return Parser::INC_OP;

[[:alpha:]_][[:alpha:][:digit:]_]*										{
																			Parser::d_val__ = std::string(matched());
																			return Parser::IDENTIFIER;
																		}
(([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*))				{
																			istringstream sstr(matched());
																			int value;
																			sstr >> value;
																			Parser::d_val__ = value;
																			return Parser::INT_CONSTANT;
																		}
([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?)|([0-9]+\.?[0-9]*([eE][-+]?[0-9]+)?)	{
																			istringstream sstr(matched());
																			float value;
																			sstr >> value;
																			Parser::d_val__ = value;
																			return Parser::FLOAT_CONSTANT;
																		}

.									return matched()[0];
