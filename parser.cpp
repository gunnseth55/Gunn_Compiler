    #include "parser.h"
    #include<iostream>
    #include<vector>
    using namespace std;
    #include <string>
    #include<unordered_map>
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
        if(pos >= tokens.size()) {
            static Token endToken = {END, ""};
            return endToken;
        }
        return tokens[pos];
    }
    Token Parser::get(){
        if(pos >= tokens.size()) {
            static Token endToken = {END, ""};
            return endToken;
        }
        Token t = tokens[pos];
        pos++;
        return t;
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
        Token t=peek();
        if(t.token==MINUS){
            get();
            Expr* expr=parseFactor();
            return new BinaryExpr("-", new NumberExpr("0"), expr);
        }
        if(t.token==LPAREN){
            get();
            Expr* expr=parseExpression();
            get(); // consume RPAREN
            return expr;
        }
        Token tok=get();
        if(tok.token==NUMBER){
            return new NumberExpr(tok.value);
        }
        if(tok.token==IDENTIFIER){
            return new VariableExpr(tok.value);
        }
        return nullptr;
    }
     Expr* Parser::parseTerm(){
        Expr* left=parseFactor();
        while(peek().token==MULTIPLY||peek().token==DIVIDE){
            Token op=get();
            Expr* right=parseFactor();
            left=new BinaryExpr(op.value,left,right);
        }
        return left;
    }
   
    Expr* Parser::parseExpression(){
        Expr *left=parseTerm();
        while(peek().token==PLUS||peek().token==MINUS){
            Token op=get();
            Expr* right=parseTerm();
            left=new BinaryExpr(op.value, left, right);
        }
        return left;
    }
    Expr* Parser::parseAdditive(){
        Expr* left=parseTerm();
        while(peek().token==PLUS||peek().token==MINUS){
            Token op=get();
            Expr* right=parseTerm();
            left=new BinaryExpr(op.value,left,right);
        }
        return left;
    }
   
    Expr* Parser::parseCondition(){
        Expr* left=parseAdditive();
        while(peek().token==LESS||peek().token==GREATER||peek().token==EQUAL_EQUAL||peek().token==LESS_EQUAL||peek().token==GREATER_EQUAL){
            Token op=get();
            Expr* right=parseAdditive();
            left=new BinaryExpr(op.value,left,right);
        }
        return left;
    }

    Expr* optimizeExpr(Expr* expr, unordered_map<string,string>& constTable){
        if(auto var=dynamic_cast<VariableExpr*>(expr)){
            if(constTable.count(var->name)){
                return new NumberExpr(constTable[var->name]);
            }
        }
        if(auto bin=dynamic_cast<BinaryExpr*>(expr)){
            bin->left=optimizeExpr(bin->left,constTable);
            bin->right=optimizeExpr(bin->right,constTable);
            auto leftnum=dynamic_cast<NumberExpr*>(bin->left);
            auto rightnum=dynamic_cast<NumberExpr*>(bin->right);

            if(leftnum && rightnum){
                int l=stoi(leftnum->value);
                int r=stoi(rightnum->value);
                int result=0;
                if(bin->op=="+")result= l+r;
                if(bin->op=="-")result= l-r;
                if(bin->op=="*")result= l*r;
                if(bin->op=="/")result= l/r;
                if(bin->op=="<")result= l<r;
                if(bin->op==">")result= l>r;
                if(bin->op=="==")result= l==r;
                if(bin->op=="<=")result= l<=r;
                if(bin->op==">=")result= l>=r;
                cout<< "Foldings: "<<l <<" "<<bin->op<< " " <<r <<endl;
                return new NumberExpr(to_string(result));

            }
        }
        return expr;
    }

    void optimizeStatement(Stmt* stmt, unordered_map<string,string> &constTable){
        if(auto var=dynamic_cast<VarDecl*>(stmt)){
            var->value=optimizeExpr(var->value,constTable);
            if(auto num=dynamic_cast<NumberExpr*>(var->value)){
                constTable[var->name]=num->value;
            }

        }else if(auto ret=dynamic_cast<ReturnStmt*>(stmt)){
            ret->expr=optimizeExpr(ret->expr,constTable);
        }else if(auto ifs=dynamic_cast<IfStmt*>(stmt)){
            ifs->condition=optimizeExpr(ifs->condition,constTable);
            for (auto &s:ifs->body){
                optimizeStatement(s,constTable);
            }
            for(auto &s:ifs->elsebody){
                optimizeStatement(s,constTable);
            }
        }
    }
    void Parser::optimizeProgram(Program* pgm){
        unordered_map<string,string>constTable;
        for (auto &stmt:pgm->statements){
            optimizeStatement(stmt,constTable);
        }
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
            while(peek().token!=RBRACE && peek().token!=END){
                Stmt* s=parseStatement();
                if(s!=nullptr) body.push_back(s);
                if(peek().token==SEMICOLON)get();
                else if(s==nullptr && peek().token!=RBRACE && peek().token!=END) get();
            }
            if(peek().token==RBRACE) get(); //rbrace
            vector<Stmt*>elseb;
            if(peek().token==ELSE){
                get(); //else
                get();  //lbrace
                while(peek().token!=RBRACE && peek().token!=END){
                    Stmt* s=parseStatement();
                    if(s!=nullptr) elseb.push_back(s);
                    if(peek().token==SEMICOLON)get();
                    else if(s==nullptr && peek().token!=RBRACE && peek().token!=END) get();
                }
                if(peek().token==RBRACE) get(); //rbrace
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
            else if(stmt==nullptr && peek().token!=END){
                get(); // advance if we couldn't parse a statement
            }
        }
        return program;


    }