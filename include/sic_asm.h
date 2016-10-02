#ifndef SIC_ASM_H
#define SIC_ASM_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class Assembler
{
private:
    // function pointer to point to the instructions
    typedef int (Assembler::*FP)();
    map<string, FP> instruction;

    // used during the pass1 to gather all the symbols
    map<string, int> symtable;

    // the location counter for 'START'
    int locctr;

    // 24-bit int
    int instruction_opcode:24;

    void pass1();
    void pass2();

    // got to figure out how to correctly format and print
    void print();

    // instruction set
    int add();  int _and();
    int comp(); int _div();
    int j();    int jeq();
    int jgt();  int jlt();
    int jsub(); int lda();
    int ldch(); int ldl();
    int ldx();  int mul();  
    int _or();  int rd();   
    int rsub(); int sta();  
    int stch(); int stl();  
    int stsw(); int stx(); 
    int sub();  int td();   
    int tix();  int wd();

public:
    Assembler();
    void assemble(string);
    
};

#endif 