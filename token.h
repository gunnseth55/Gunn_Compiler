#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum TokenType{
    INT, RETURN , IDENTIFIER, NUMBER,MULTIPLY, PLUS, EQUAL, ELSE,
    SEMICOLON, IF, LPAREN, RPAREN, LBRACE, RBRACE, LESS,GREATER,END
};
struct Token 
{

   TokenType token;
   string value;
};

#endif
 