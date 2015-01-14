#include <iostream>
#include "Scanner.h"
using namespace std;
int main()
{
  Scanner scanner;   // define a Scanner object
  while (int token = scanner.lex()) // get all tokens
    {
      string const &text = scanner.matched();
      switch (token)
		{
		case Scanner::STRING_LITERAL:
		  cout << "string: " << text << '\n';
		  break;
		case Scanner::IDENTIFIER:
		  cout << "identifier: " << text << '\n';
		  break;
		case Scanner::INT_CONSTANT:
		  cout << "integer: " << text << '\n';
		  break;
		case Scanner::FLOAT_CONSTANT:
		  cout << "float: " << text << '\n';
		  break;
		default:
		  cout << "char. token: `" << text << "'\n";
		}
    }
}
