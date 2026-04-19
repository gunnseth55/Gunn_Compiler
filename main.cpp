#include<iostream>
#include<fstream>
#include<sstream>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
using namespace std;

void printProgram(Program* program);


int main()
{

  ifstream file("input4.txt");
  stringstream buffer;
  buffer<< file.rdbuf();
  string code=buffer.str();
  Lexer lexer (code);
 
 
  auto tokens=lexer.tokenize();
  Parser parser(tokens);
  cout<<"Tokens count:"<<tokens.size()<<endl;
  for(auto &t:tokens){
    cout<<"TOK("<<t.token<<","<<t.value<<") ";
  }
  cout<<"\n";
   Program* program=parser.parseProgram();
  cout<<"Program statements: "<<program->statements.size()<<"\n";
  for(auto stmt: program->statements){
    if(dynamic_cast<IfStmt*>(stmt)) cout<<"Stmt IfStmt\n";
    else if(dynamic_cast<VarDecl*>(stmt)) cout<<"Stmt VarDecl\n";
    else if(dynamic_cast<AssignStmt*>(stmt)) cout<<"Stmt AssignStmt\n";
    else if(dynamic_cast<WhileStmt*>(stmt)) cout<<"Stmt WhileStmt\n";
    else if(dynamic_cast<ReturnStmt*>(stmt)) cout<<"Stmt ReturnStmt\n";
    else cout<<"Stmt Unknown\n";
  }
  parser.optimizeProgram(program);
  CodeGen cg;
  cg.generate(program);
  cg.buildCFG();
  return 0;
}