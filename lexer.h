#ifndef LEXER_H
#define LEXER_H

#include<vector>
#include "token.h"
using namespace std;
class Lexer{
    public:
        Lexer(const string &input);
        vector<Token> tokenize();
    private:
        string input;
        int pos;
        char peek();
        char get();

};  

#endif
