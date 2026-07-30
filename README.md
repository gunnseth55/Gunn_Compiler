It implements the classic front-to-middle-end pipeline — lexer → parser → AST → optimizer → three-address code generation → control-flow graph — and is intended as a learning project for understanding how compilers work under the hood.

Features
Lexer – tokenizes source code into a stream of typed tokens (lexer.cpp / lexer.h, token.h)
Parser – recursive-descent parser that builds an Abstract Syntax Tree (parser.cpp / parser.h, ast.h)
Language constructs supported:
Variable declarations (int a = 2;)
Assignment statements (a = a * 10;)
if statements
while and for loops
return statements
Arithmetic and comparison expressions
AST optimization pass – parser.optimizeProgram() simplifies the tree before code generation
Code generation – lowers the AST into three-address code (TAC) instructions, with SSA-style variable renaming (getNewName / getCurrentName) (codegen.cpp / codegen.h)
Control-flow graph (CFG) construction from the generated instructions (CodeGen::buildCFG)
Project Structure
Gunn_Compiler/
├── token.h        # Token type definitions
├── lexer.h/.cpp   # Source → token stream
├── ast.h          # AST node definitions (IfStmt, VarDecl, AssignStmt, WhileStmt, ReturnStmt, ...)
├── parser.h/.cpp  # Token stream → AST, plus AST-level optimization
├── codegen.h/.cpp # AST → three-address code + CFG construction
├── main.cpp       # Compiler driver / entry point
├── input.txt ... input5.txt   # Sample programs in the toy language
└── compiler.exe   # Prebuilt Windows binary
Example Input
c
int a=2;
for(int x=0;x<2;x=x+1){
    a=a*10;
}
return a;

Running the compiler on a file like this prints:

The full token stream produced by the lexer
The list of top-level AST statement types
The generated three-address code instructions
The resulting control-flow graph
Building

The project has no external dependencies beyond a standard C++ compiler.

Linux / macOS:

bash
g++ -std=c++17 -o compiler main.cpp lexer.cpp parser.cpp codegen.cpp

Windows (MinGW):

bash
g++ -std=c++17 -o compiler.exe main.cpp lexer.cpp parser.cpp codegen.cpp

A prebuilt compiler.exe for Windows is already included in the repo.

Usage

The driver currently reads its source from a hardcoded input file (input4.txt in main.cpp). To compile a different program, either:

Replace the contents of the target input file, or
Edit the filename in main.cpp and rebuild.

Then run:

bash
./compiler
Roadmap / Ideas for Contribution
Accept the input file as a command-line argument instead of hardcoding it
Add more optimization passes (constant folding, dead-code elimination)
Emit real machine code or LLVM IR instead of stopping at three-address code
Add a test suite covering the sample input*.txt programs
