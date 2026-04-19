#ifndef CODEGEN_H
#define CODEGEN_H
#include<vector>
#include<unordered_map>
#include "token.h"
#include "ast.h"
using namespace std;
struct Instruction{
    string op;
    string arg1;
    string arg2;
    string result;
};

class CodeGen{
    public: 
    void generate(Program* program);
    vector<Instruction>instructions;
    void buildCFG();
    private:
    unordered_map<string,string> varTable;
    string generateExpr(Expr *expr);
    string getNewName(string var);
    string getCurrentName(string var);
    bool generateStmt(Stmt *stmt);
    string newLabel();
};

#endif