#include "codegen.h"
#include<iostream>
#include<vector>
#include<set>
#include<map>
using namespace std;

unordered_map<string,int>version;
unordered_map<string,string>currentName;

int tempCount=0;
int labelCount=0;
vector<string>breakstk;
vector<string>continuestk;


string CodeGen::getNewName(string var){
        version[var]++;
        return var+ to_string(version[var]);

}
string CodeGen::getCurrentName(string var){
    if(currentName.count(var)){
        return currentName[var];

    }
    return var;
}

string CodeGen::generateExpr(Expr *expr){
    if(auto num=dynamic_cast<NumberExpr*>(expr)){
        return num->value;
    }else if(auto var=dynamic_cast<VariableExpr*>(expr)){
       return getCurrentName(var->name); //SSA start point
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
bool CodeGen::generateStmt(Stmt *stmt){
   if(auto var=dynamic_cast<VarDecl*>(stmt)){
        string val=generateExpr(var->value);
        // cout<<var->name<<" = "<<val<<endl;
        string newVar=getNewName(var->name);
        currentName[var->name]=newVar;
        instructions.push_back({"assign", val, "", newVar});
        // varTable[var->name]=val;
    }
    
    else if(auto ifs=dynamic_cast<IfStmt*>(stmt)){
        string cond=generateExpr(ifs->condition);
          bool hasReturn=false;
          if(ifs->elsebody.empty()){
            string labelEnd=newLabel();
            instructions.push_back({"ifFalse",cond,"",labelEnd});
            for(auto s:ifs->body){
                if(generateStmt(s)){
                    hasReturn=true;
                }
            }
            instructions.push_back({"label","","",labelEnd});
          }else{

            string labelElse=newLabel();
        string labelEnd=newLabel();

        auto beforeIf=currentName;
        instructions.push_back({"ifFalse",cond,"",labelElse});
      
        for(auto s:ifs->body){
            generateStmt(s);
        }
        auto thenMap=currentName;
        instructions.push_back({"goto","","",labelEnd});
        currentName=beforeIf;
        instructions.push_back({"label","","",labelElse});
        for(auto s:ifs->elsebody){
            generateStmt(s);
        }
        auto elseMap=currentName;
        for(auto &p:beforeIf){
            string var=p.first;
            string thenVar=thenMap.count(var)?thenMap[var]:beforeIf[var];
            string elseVar=elseMap.count(var)?elseMap[var]:beforeIf[var];
            if(thenVar!=elseVar){
                string newVar=getNewName(var);
                currentName[var]=newVar;
                instructions.push_back({"phi",thenVar,elseVar,newVar});
            }
        }
        instructions.push_back({"label","","",labelEnd});
       
          }
            
        
//         string cond=generateExpr(ifs->condition);
//         // cout<<"DEBUG if cond='"<<cond<<"'\n";
//         string labelElse=newLabel();
//         instructions.push_back({"ifFalse",cond,"",labelElse});
        
//         // Handle constant conditions (compile-time known)
//         // if(cond=="1"){
//         //     for(auto s:ifs->body){
//         //         if(generateStmt(s)) return true;
//         //     }
//         //     return false;
//         // }
//         // if(cond=="0"){
//         //     for(auto s:ifs->elsebody){
//         //         if(generateStmt(s)) return true;
//         //     }
//         //     return false;
//         // }
        
//         // Handle runtime conditions
//         // string labelEnd=newLabel();
//         // string labelElse=newLabel();
//         // // cout<<"ifFalse "<< cond << " goto " << labelElse <<endl;
//         // instructions.push_back({"ifFalse", cond, "", labelElse});
//         // bool bodyReturned = false;
// bool hasReturn=false;
// for(auto s:ifs->body){
//     if(generateStmt(s)){
//        hasReturn= true;
//     }
// }
// //only else
// if(!ifs->elsebody.empty()){
//      string labelEnd = newLabel();
//     instructions.push_back({"goto","","",labelEnd});

//     instructions.push_back({"label","","",labelElse});

//     for(auto s:ifs->elsebody){
//         if(generateStmt(s)){
//             hasReturn= true;
//         }
//     }

//     instructions.push_back({"label","","",labelEnd});
// }else{
//     instructions.push_back({"label","","",labelElse});
// }

// return hasReturn;
// if(!bodyReturned){
//     instructions.push_back({"goto","","",labelEnd});
// }

    //       instructions.push_back({"label","","",labelElse});
    //     // cout<<labelElse<<":"<<endl;
    //    for(auto s:ifs->elsebody){
    // if(generateStmt(s)){
    //     return true;
    // }

    //     }
    //     instructions.push_back({"label","","",labelEnd});
    }
    else if(auto as = dynamic_cast<AssignStmt*>(stmt)){
        string val = generateExpr(as->value);
        // cout << as->name << " = " << val << endl;
        string newVar=getNewName(as->name);
        currentName[as->name]=newVar;
        instructions.push_back({"assign",val,"",newVar});
        // varTable[as->name]=val;
    }
    else if(auto w=dynamic_cast<WhileStmt*>(stmt)){
        string startLabel=newLabel();
        string endLabel=newLabel();
        // cout<< startLabel<<":"<<endl;
        continuestk.push_back(startLabel);
        breakstk.push_back(endLabel);
        auto beforeLoop=currentName;
        instructions.push_back({"label","","",startLabel});
        unordered_map<string,string>phiMap;
        for(auto &p:beforeLoop){
            string var=p.first;
            string phiVar=getNewName(var);
            phiMap[var]=phiVar;
            currentName[var]=phiVar;
            instructions.push_back({"phi",beforeLoop[var],"?",phiVar});
        }
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
        bool hasReturn=false;
        for(auto s:w->body){
           generateStmt(s);
        }
        auto afterLoop=currentName;
        for(auto &p:phiMap){
            string var=p.first;
            string phiVar=p.second;
            string afterVar = afterLoop.count(var) ? afterLoop[var] : beforeLoop[var];
            for(auto &inst:instructions){
                if(inst.op=="phi" && inst.result==phiVar){
                    inst.arg2=afterVar;
                }
            }
        }
        // cout<< "goto " <<startLabel<<endl;
        //loop back
        instructions.push_back({"goto","","", startLabel});

        //exit label 
        instructions.push_back({"label","","",endLabel});
        
        // cout<<endLabel<< ":" <<endl;
        continuestk.pop_back();
        breakstk.pop_back();
       
    }else if(auto f=dynamic_cast<ForStmt*>(stmt)){
        generateStmt(f->init);
        string startLabel=newLabel();
        string updateLabel=newLabel();
        string endLabel=newLabel();
        continuestk.push_back(updateLabel);
        breakstk.push_back(endLabel);

        auto beforeLoop=currentName;
        instructions.push_back({"label","","",startLabel});
        unordered_map<string,string>phiMap;
        for(auto &p:beforeLoop){
            string var=p.first;
            string phiVar=getNewName(var);
            phiMap[var]=phiVar;
            currentName[var]=phiVar;
            instructions.push_back({"phi",beforeLoop[var],"?",phiVar});
        }
        string cond=generateExpr(f->condition);
        instructions.push_back({"ifFalse",cond,"",endLabel});
       
        for(auto s:f->body){
           generateStmt(s);
        }   
        instructions.push_back({"goto","","",updateLabel});
        instructions.push_back({"label","","",updateLabel});
         generateStmt(f->update);
         auto afterLoop=currentName;
         for(auto &p:phiMap){
            string var=p.first;
            string phiVar=p.second;
            string afterVar = afterLoop.count(var) ? afterLoop[var] : beforeLoop[var];
            for(auto &inst:instructions){
                if(inst.op=="phi" &&   inst.result==phiVar){
                    inst.arg2=afterVar;
                }
            }
         }
        // instructions.push_back({"label","","",updateLabel});
       
        instructions.push_back({"goto","","",startLabel});
        instructions.push_back({"label","","",endLabel});
        continuestk.pop_back();
        breakstk.pop_back();
        
    }else if(auto br=dynamic_cast<BreakStmt*>(stmt)){
        if(!breakstk.empty()){
            instructions.push_back({"goto","","",breakstk.back()});
        }
    }else if(auto cn=dynamic_cast<ContinueStmt*>(stmt)){
        if(!continuestk.empty()){
            instructions.push_back({"goto","","",continuestk.back()});
        }
    }else if(auto ret=dynamic_cast<ReturnStmt*>(stmt)){
        string val=generateExpr(ret->expr);
        // cout<<"return "<<val<<endl;
        
        // // Look up actual value if it's a variable
        // string actualValue=val;
        // if(varTable.count(val)){
        //     actualValue=varTable[val];
        // }
        instructions.push_back({"return", val, "", ""});
        // cout<<"Expected final value: "<<varTable[val]<<endl;
        return true;
    }
    return false;
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

for(int i=0;i<keys.size();i++){
    int k = keys[i];

    for(auto &inst : blocks[k]){
        if(inst.op=="label"){
            labelToBlock[inst.result] = i;
        }
    }
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
        }else if(last.op=="return"){
            //nothing
        }else{
            if(i+1<keys.size()){
                cout<<"Block"<<i+1;
            }
        }
        cout<<"\n";
    }
   
}