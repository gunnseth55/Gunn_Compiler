#ifndef AST_H
#define AST_H
#include<string>
#include<iostream>
#include<vector>
using namespace std;
struct Expr{
    virtual ~Expr(){}
};
struct Stmt{
    virtual ~Stmt(){}
};
struct VarDecl:Stmt{
    string name;
    Expr* value;
    VarDecl(string n , Expr* e):name(n),value(e){}
};
struct ReturnStmt:Stmt{
    Expr* expr;
    ReturnStmt(Expr* e):expr(e){}
};
struct IfStmt:Stmt{
    Expr* condition;
    vector<Stmt*>body;
    vector<Stmt*>elsebody;
    IfStmt(Expr* cond, vector<Stmt*>b,vector<Stmt*>elseb):condition(cond),body(b),elsebody(elseb){}
};

struct WhileStmt:Stmt{
    Expr* condition;
    vector<Stmt*>body;
    WhileStmt(Expr* cond, vector<Stmt*> b):condition(cond),body(b) {}
};
struct NumberExpr:Expr{
    string value;
    NumberExpr(string v):value(v) {}
};
struct VariableExpr:Expr{
    string name;
    VariableExpr(string n):name(n) {}

};
struct AssignStmt:Stmt{
    string name;
    Expr* value;
    AssignStmt(string var, Expr* exp):name(var),value(exp){}
};

struct BinaryExpr:Expr{
    string op;
    Expr *left;
    Expr *right;
    BinaryExpr(string o, Expr* l, Expr* r)
    :op(o),left(l),right(r){}
};


struct Program{
   vector<Stmt*> statements;
};



#endif