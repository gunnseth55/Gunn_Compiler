#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

enum TokenType{
    INT, RETURN , IDENTIFIER, NUMBER,MULTIPLY, PLUS,MINUS, DIVIDE, EQUAL, ELSE,
    SEMICOLON, IF, LPAREN, RPAREN, LBRACE, RBRACE, LESS,GREATER,EQUAL_EQUAL,LESS_EQUAL,GREATER_EQUAL,END
};
struct Token 
{

   TokenType token;
   string value;
};

#endif
 