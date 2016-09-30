#ifndef SIC_ASM_H
#define SIC_ASM_H

#include <iostream>
#include <map>
#include <string>

using namespace std;

// 24 bit int
struct int24 
{
    unsigned int data : 24;
};

class Assembler
{
private:
    // function pointer to point to the instructions
    typedef int (Assembler::*FP)();
    map<string, FP> instruction;

    map<string, int> symtable;

    int locctr;
    int24 instruction_opcode;

    void pass1();
    void pass2();

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
    
};

#endif 