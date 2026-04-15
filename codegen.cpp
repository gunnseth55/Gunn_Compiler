#include "codegen.h"
#include<iostream>
#include<vector>
#include<set>
#include<map>
using namespace std;

int tempCount=0;
int labelCount=0;

string CodeGen::generateExpr(Expr *expr){
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
            string binop=bin->op;
            string temp="t"+to_string(tempCount++);
            // cout<<temp<<" = "<<left<<" "<<bin->op<<" "<<right<<endl;
            instructions.push_back({binop,left,right,temp});
            return temp;
        }
    }
    return "";
}
string CodeGen::newLabel(){
    return "L"+to_string (labelCount++);
}
void CodeGen::generateStmt(Stmt *stmt){
   if(auto var=dynamic_cast<VarDecl*>(stmt)){
        string val=generateExpr(var->value);
        // cout<<var->name<<" = "<<val<<endl;
        instructions.push_back({"assign", val, "", var->name});
        varTable[var->name]=val;
    }
    else if(auto ret=dynamic_cast<ReturnStmt*>(stmt)){
        string val=generateExpr(ret->expr);
        cout<<"return "<<val<<endl;
        
        // Look up actual value if it's a variable
        string actualValue=val;
        if(varTable.count(val)){
            actualValue=varTable[val];
        }
        instructions.push_back({"return", val, "", ""});
        // cout<<"Expected final value: "<<varTable[val]<<endl;
    }
    else if(auto ifs=dynamic_cast<IfStmt*>(stmt)){
        string cond=generateExpr(ifs->condition);
        // cout<<"DEBUG if cond='"<<cond<<"'\n";
        
        // Handle constant conditions (compile-time known)
        if(cond=="1"){
            for(auto s:ifs->body){
                generateStmt(s);
            }
            return;
        }
        if(cond=="0"){
            for(auto s:ifs->elsebody){
                generateStmt(s);
            }
            return;
        }
        
        // Handle runtime conditions
        string labelEnd=newLabel();
        string labelElse=newLabel();
        // cout<<"ifFalse "<< cond << " goto " << labelElse <<endl;
        instructions.push_back({"ifFalse", cond, "", labelElse});
        for(auto s:ifs->body){
            generateStmt(s);
        }
        // cout<<" goto "<<labelEnd<<endl;
         instructions.push_back({"goto",cond,"",labelEnd});
          instructions.push_back({"label",cond,"",labelEnd});
        // cout<<labelElse<<":"<<endl;
        for(auto s:ifs->elsebody){
            generateStmt(s);
        }
        instructions.push_back({"label",cond,"",labelEnd});
    }
    else if(auto as = dynamic_cast<AssignStmt*>(stmt)){
        string val = generateExpr(as->value);
        // cout << as->name << " = " << val << endl;
        instructions.push_back({"assign",val,"",as->name});
        varTable[as->name]=val;
    }
    else if(auto w=dynamic_cast<WhileStmt*>(stmt)){
        string startLabel=newLabel();
        string endLabel=newLabel();
        // cout<< startLabel<<":"<<endl;
        
        instructions.push_back({"label","","",startLabel});

        // if(auto num=dynamic_cast<NumberExpr*>(w->condition)){
        //     // Constant condition
        //     if(num->value=="1"){
        //         for(auto s:w->body){
        //             generateStmt(s);
        //         }
        //         cout<< "goto " <<startLabel<<endl;
        //         return;
        //     }
        //     cout<< "ifFalse "<< num->value << " goto " <<endLabel <<endl;
        // }else{
        //     // Expression condition - generate it fresh at loop start each iteration
        //     string cond=generateExpr(w->condition);
        //     cout<< "ifFalse "<< cond << " goto " <<endLabel <<endl;
        // }
        string cond=generateExpr(w->condition);
        // cout << "ifFalse " << cond << " goto " << endLabel <<endl;
        instructions.push_back({"ifFalse",cond,"",endLabel});
        for(auto s:w->body){
            generateStmt(s);
        }
        
        // cout<< "goto " <<startLabel<<endl;
        //loop back
        instructions.push_back({"goto","","", startLabel});

        //exit label 
        instructions.push_back({"label","","",endLabel});
        
        // cout<<endLabel<< ":" <<endl;
    }
    
}

void CodeGen::generate(Program *program){
   for(auto stmt:program->statements){
    
    generateStmt(stmt);
   }
}

void CodeGen::buildCFG(){
    set<int>leaders;
    leaders.insert(0);
    for(int i=0;i<instructions.size();i++){
        if(instructions[i].op=="goto"||instructions[i].op=="ifFalse"){
            string label=instructions[i].result;
            for(int j=0;j<instructions.size();j++){
                if(instructions[j].op=="label" && instructions[j].result==label){
                    leaders.insert(j);
                }
            }
            if(i+1<instructions.size()){
                leaders.insert(i+1);
            }
        }
    }
    map<int,vector<Instruction>>blocks;
    int curr_leader=0;
    for(int i=0;i<instructions.size();i++){
        if(leaders.count(i)){
            curr_leader=i;
        }
        blocks[curr_leader].push_back(instructions[i]);
    }

    vector<int>keys;
    for(auto &b:blocks){
        keys.push_back(b.first);
    }
    map<string,int>labelToBlock;
    int id=0;
    for(auto &b:blocks){
        for(auto &inst:b.second){
            if(inst.op=="label"){
                labelToBlock[inst.result]=id;
            }
        }
        id++;
    }
    for(int i=0;i<keys.size();i++){
        int k=keys[i];
        auto &block=blocks[k];
        Instruction last=block.back();
        cout<<"Block "<<i   <<": \n";
        for(auto &inst:block){
            cout<<" "<<inst.op<<" "
            <<inst.arg1<<" "<<inst.arg2<< " "<<inst.result<<endl;
        }
        cout<<"->";
        if(last.op=="goto"){
            cout<<"Block"<<labelToBlock[last.result];
        }
        else if(last.op=="ifFalse"){
            cout<<"Block"<<labelToBlock[last.result];
            if(i+1<keys.size()){
                cout<<" Block"<<i+1;
            }
        }else{
            if(i+1<keys.size()){
                cout<<"Block"<<i+1;
            }
        }
        cout<<"\n";
    }
   
}