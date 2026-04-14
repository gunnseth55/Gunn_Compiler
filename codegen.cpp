#include "codegen.h"
#include<iostream>
using namespace std;

int tempCount=0;
int labelCount=0;

string generateExpr(Expr *expr){
    if(auto num=dynamic_cast<NumberExpr*>(expr)){
        return num->value;
    }else if(auto var=dynamic_cast<VariableExpr*>(expr)){
       return var->name;
    }else if(auto bin=dynamic_cast<BinaryExpr*>(expr)){
       string left=generateExpr(bin->left);
       string right=generateExpr(bin->right);
        if((isdigit(left[0])||left[0]=='-') && (isdigit(right[0])||right[0]=='-')){
            int l=stoi(left);
            int r=stoi(right);
            int result=0;
            if(bin->op=="+")result=l+r;
            else if(bin->op=="-")result=l-r;
            else if(bin->op=="*")result=l*r;
            else if(bin->op=="/")result=l/r;
            else if(bin->op=="<")result=l<r;
            else if(bin->op==">")result=l>r;
            else if(bin->op=="==")result=l==r;
            else if(bin->op=="<=")result=l<=r;
            else if(bin->op==">=")result=l>=r;
            return to_string(result);
        }
        else{
            string temp="t"+to_string(tempCount++);
            cout<<temp<<" = "<<left<<" "<<bin->op<<" "<<right<<endl;
            return temp;
        }
    }
    return "";
}
string newLabel(){
    return "L"+to_string (labelCount++);
}
void generateStmt(Stmt *stmt){
   if(auto var=dynamic_cast<VarDecl*>(stmt)){
        string val=generateExpr(var->value);
        cout<<var->name<<" = "<<val<<endl;
    }
    else if(auto ret=dynamic_cast<ReturnStmt*>(stmt)){
        string val=generateExpr(ret->expr);
        cout<<"return "<<val<<endl;
    }
    else if(auto ifs=dynamic_cast<IfStmt*>(stmt)){
        string cond;
        if(auto num=dynamic_cast<NumberExpr*>(ifs->condition)){
            cond=num->value;
        }else{
            cond=generateExpr(ifs->condition);
        }
        if(cond=="1"){
             for(auto s:ifs->body){
            generateStmt(s);
        }
        return;
        }
       
        // cout<<" goto "<<labelEnd<<endl;
        // cout<<labelElse<<":"<<endl;
        if(cond=="0"){
             for(auto s:ifs->elsebody){
            generateStmt(s);
        }
        return;
        }
        string labelEnd=newLabel();
        string labelElse=newLabel();
        cout<<"ifFalse "<< cond << " goto " << labelElse <<endl;
        for(auto s:ifs->body){
            generateStmt(s);
        }
        cout<<" goto "<<labelEnd<<endl;
        cout<<labelElse<<":"<<endl;
        for(auto s:ifs->elsebody){
            generateStmt(s);
        }
        cout<<labelEnd<<":"<<endl;
    }
    
}

void CodeGen::generate(Program *program){
   for(auto stmt:program->statements){
    
    generateStmt(stmt);
   }
}