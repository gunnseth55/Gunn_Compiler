#ifndef CODEGEN_H
#define CODEGEN_H
#include<vector>
#include<unordered_map>
#include "token.h"
#include "ast.h"
using namespace std;

class CodeGen{
    public: 
    void generate(Program* program);
    private:
    unordered_map<string,string> varTable;
    string generateExpr(Expr *expr);
    void generateStmt(Stmt *stmt);
    string newLabel();
};

#endif