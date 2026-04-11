#include "lexer.h"
#include <cctype>
using namespace std;
Lexer::Lexer(const string &input): input(input), pos(0) {}
char Lexer::peek(){
    if(pos>=input.size())return '\0';
    return input[pos];
}
char Lexer::get(){
    return input[pos++];

}

vector<Token> Lexer::tokenize(){
    vector<Token>tokens;
    while(pos<input.size()){
        char c=peek();
        if(isspace(c)){
            get();
            continue;
        }
        if(isalpha(c)){
            string word;
            while(pos<input.size() && isalnum(peek())) word+=get();

            if(word =="int")tokens.push_back({INT,word});
            else if (word == "return") tokens.push_back({RETURN,word});
            else if (word == "if") tokens.push_back({IF,word});
            else if (word=="else")tokens.push_back({ELSE,word});
            else tokens.push_back({IDENTIFIER,word});
        }else if(isdigit(c)){
            string num;
            while(pos<input.size() && isdigit(peek())) num+=get();
            tokens.push_back({NUMBER,num});
        }else if(c=='+'){
            get();
            tokens.push_back({PLUS,"+"});
        }else if(c=='='){
            get();
            tokens.push_back({EQUAL,"="});
        } else if(c==';'){
            get();
            tokens.push_back({SEMICOLON,";"});
        }else if(c=='<'){
            get();
            tokens.push_back({LESS,"<"});
        }else if(c=='>'){
            get();
            tokens.push_back({GREATER,">"});
        }else if(c=='('){
            get();
            tokens.push_back({LPAREN,"("});
        }else if(c==')'){
            get();
            tokens.push_back({RPAREN,")"});
        }else if(c=='{'){
            get();
            tokens.push_back({LBRACE,"{"});
        }else if(c=='}'){
            get();
            tokens.push_back({RBRACE,"}"});
        }
        
        
        else
        {
            get();
        }
        
    }
    tokens.push_back({END,""});
    return tokens;
}