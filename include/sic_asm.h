#ifndef SIC_ASM_H
#define SIC_ASM_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

using namespace std;
typedef int32_t int32;

class Assembler
{
private:
    // function point32er to point32 to the instructions
    typedef int32 (Assembler::*FP)();
    map<string, FP> instruction;

    // used during the pass1 to gather all the symbols
    map<string, int32> symtable;

    // the location counter for 'START'
    int32 locctr, program_length;

    // 24-bit int32
    int32 instruction_opcode:24;

    void pass1();
    void pass2();

    // got to figure out how to correctly format and print32
    void print();

    ifstream in_file;
    ofstream interm_file, listing_file, object_program;

    // instruction set
    int32 add();  int32 _and();
    int32 comp(); int32 _div();
    int32 j();    int32 jeq();
    int32 jgt();  int32 jlt();
    int32 jsub(); int32 lda();
    int32 ldch(); int32 ldl();
    int32 ldx();  int32 mul();  
    int32 _or();  int32 rd();   
    int32 rsub(); int32 sta();  
    int32 stch(); int32 stl();  
    int32 stsw(); int32 stx(); 
    int32 sub();  int32 td();   
    int32 tix();  int32 wd();

public:
    Assembler();
    void assemble(string);
};

#endif 