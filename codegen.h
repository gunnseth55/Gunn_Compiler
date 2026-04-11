#ifndef CODEGEN_H
#define CODEGEN_H
#include<vector>
#include "token.h"
#include "ast.h"
class CodeGen{
    public: 
    void generate(Program* program);
};

#endif