    #include "parser.h"
    #include<iostream>
    #include<vector>
    using namespace std;
    #include <string>

    string tokenToString(TokenType t) {
        switch(t) {
            case INT: return "INT";
            case RETURN: return "RETURN";
            case IDENTIFIER: return "IDENTIFIER";
            case NUMBER: return "NUMBER";
            case PLUS: return "PLUS";
            case EQUAL: return "EQUAL";
            case SEMICOLON: return "SEMICOLON";
            case END: return "END";
            default: return "UNKNOWN";
        }
    }
    Parser::Parser(const vector<Token> &tokens)
        :tokens(tokens),pos(0) {}
    Token Parser::peek(){
        return tokens[pos];
    }
    Token Parser::get(){
        return tokens[pos++];
    }
    Expr* Parser::parsePrimary(){
        Token t=get();
        if(t.token==NUMBER){
            return new NumberExpr(t.value);
        }
        if(t.token==IDENTIFIER){
            return new VariableExpr(t.value);
        }
        return nullptr;
    }
     Expr* Parser:: parseFactor(){
        Token t=get();
        if(t.token==NUMBER){
            return new NumberExpr(t.value);
        }
        if(t.token==IDENTIFIER){
            return new VariableExpr(t.value);
        }
        return nullptr;
    }
     Expr* Parser::parseTerm(){
        Expr* left=parseFactor();
        while(peek().token==MULTIPLY){
            get();
            Expr* right=parseTerm();
            left=new BinaryExpr("*",left,right);
        }
        return left;
    }
   
    Expr* Parser::parseExpression(){
        Expr *left=parseTerm();
        while(peek().token==PLUS){
            get();
            Expr* right=parsePrimary();
            left=new BinaryExpr("+", left, right);
        }
        return left;
    }
    Expr* Parser::parseAdditive(){
        Expr* left=parseTerm();
        while(peek().token==PLUS){
            get();
            Expr* right=parseTerm();
            left=new BinaryExpr("+",left,right);
        }
        return left;
    }
   
    Expr* Parser::parseCondition(){
        Expr* left=parseAdditive();
        while(peek().token==LESS||peek().token==GREATER){
            Token op=get();
            Expr* right=parseAdditive();
            left=new BinaryExpr(op.value,left,right);
        }
        return left;
    }


    Stmt* Parser::parseStatement(){
        if(peek().token==INT){
            get();
            Token name=get();
            get();
            Expr* value=parseExpression();
            get();
            return new VarDecl(name.value, value);
        }
        if(peek().token==RETURN){
            get();
            Expr* expr=parseExpression();
            get();
            return new ReturnStmt(expr);
        }
        if(peek().token==IF){
            get(); //if
            get(); // lparen
            Expr* cond=parseCondition();
            get(); //rparen
            get(); //lbrace
            vector<Stmt*>body;
            while(peek().token!=RBRACE){
                Stmt* s=parseStatement();
                if(s!=nullptr) body.push_back(s);
                if(peek().token==SEMICOLON)get();
            }
            get(); //rbrace
            vector<Stmt*>elseb;
            if(peek().token==ELSE){
                get(); //else
                get();  //lbrace
                while(peek().token!=RBRACE){
                    Stmt* s=parseStatement();
                    if(s!=nullptr) elseb.push_back(s);
                    if(peek().token==SEMICOLON)get();
                }
                get(); //rbrace
            }

            return new IfStmt(cond,body,elseb);
        }


        return nullptr;
    }
    void printExpr(Expr* expr){
        if(auto num=dynamic_cast<NumberExpr*>(expr)){
            cout<<num->value;
        }else if(auto var=dynamic_cast<VariableExpr*>(expr)){
            cout<<var->name;
        }else if(auto bin=dynamic_cast<BinaryExpr*>(expr)){
            cout<<"(";
            printExpr(bin->left);
            cout<<" "<<bin->op<<" ";
            printExpr(bin->right);
            cout<<")";
        }
    }
    void printStmt(Stmt *stmt){
        if(auto var=dynamic_cast<VarDecl*>(stmt)){
            cout<<"int "<<var->name<<" = ";
            printExpr(var->value);
            cout<<endl;
        }else if(auto ret=dynamic_cast<ReturnStmt*>(stmt)){
            cout<<"return ";
            printExpr(ret->expr);
            cout<<endl;
        }
    }
    void Parser::parse(){
        while(peek().token !=END){
        Stmt* stmt=parseStatement();
        if(stmt!=nullptr){
            printStmt(stmt);
        }
        
            if(peek().token==SEMICOLON){
                get();
            }
            
        }
    }
    void printProgram(Program* program){
        for(auto stmt: program->statements){
            printStmt(stmt);
        }
    }

    Program* Parser::parseProgram(){

        Program* program=new Program();
        while(peek().token!=END){
            Stmt* stmt=parseStatement();
            if(stmt!=nullptr){
                program->statements.push_back(stmt);
            }
            if(peek().token==SEMICOLON){
                get();
            }
        }
        return program;


    }