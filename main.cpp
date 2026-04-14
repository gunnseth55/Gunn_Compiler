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

  ifstream file("input.txt");
  stringstream buffer;
  buffer<< file.rdbuf();
  string code=buffer.str();
  Lexer lexer (code);
 
 
  auto tokens=lexer.tokenize();
  Parser parser(tokens);
  cout<<"Tokens count:"<<tokens.size()<<endl;
   Program* program=parser.parseProgram();
  parser.optimizeProgram(program);
  CodeGen cg;
  cg.generate(program);
  return 0;
}