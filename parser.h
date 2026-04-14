#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include "ast.h"
#include<vector>

class Parser{

public:
    Parser(const vector<Token> &tokens);
    void parse();
    Program* parseProgram();
    void optimizeProgram(Program* Program);
private:
    vector<Token> tokens;
    int pos;
    Token peek();
    Token get();
    Expr* parseExpression();
    Expr* parseTerm();
    Expr* parseCondition();
    Expr* parseAdditive();
    Expr* parseFactor();
    Expr* parsePrimary();
    Stmt* parseStatement();

    
};

#endif