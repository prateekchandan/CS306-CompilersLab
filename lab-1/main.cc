#include <iostream>
#include "Scanner.h"
using namespace std;
int main()
{
	Scanner scanner;					// define a Scanner object
	while (int token = scanner.lex()) 	// get all tokens
		{
			string const &text = scanner.matched();
			switch (token)
			{
			case Scanner::STRING_LITERAL:
				cout << "STRING: " << text << '\n';
				break;
			case Scanner::IDENTIFIER:
				cout << "IDENTIFIER: " << text << '\n';
				break;
			case Scanner::INT_CONSTANT:
				cout << "INT_CONSTANT: " << text << '\n';
				break;
			case Scanner::FLOAT_CONSTANT:
				cout << "FLOAT_CONSTANT: " << text << '\n';
				break;
			case Scanner::VOID:
				cout << "VOID" << '\n';
				break;
			case Scanner::INT:
				cout << "INT" << '\n';
				break;
			case Scanner::FLOAT:
				cout << "FLOAT" << '\n';
				break;
			case Scanner::RETURN:
				cout << "RETURN" << '\n';
				break;
			case Scanner::IF:
				cout << "IF" << '\n';
				break;
			case Scanner::ELSE:
				cout << "ELSE" << '\n';
				break;
			case Scanner::WHILE:
				cout << "WHILE" << '\n';
				break;
			case Scanner::FOR:
				cout << "FOR" << '\n';
				break;
			case Scanner::OR_OP:
				cout << "OR_OP" << '\n';
				break;
			case Scanner::AND_OP:
				cout << "AND_OP" << '\n';
				break;
			case Scanner::EQ_OP:
				cout << "EQ_OP" << '\n';
				break;
			case Scanner::NE_OP:
				cout << "NE_OP" << '\n';
				break;
			case Scanner::LE_OP:
				cout << "LE_OP" << '\n';
				break;
			case Scanner::GE_OP:
				cout << "GE_OP" << '\n';
				break;
			case Scanner::INC_OP:
				cout << "INC_OP" << '\n';
				break;
			case Scanner::DEC_OP:
				cout << "DEC_OP" << '\n';
				break;
			default:
				cout << "character:  " << text << "\n";
			}
		}
}
