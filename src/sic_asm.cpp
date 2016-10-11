#include "sic_asm.h"

Assembler::Assembler()
{
    instruction["ADD"] = &Assembler::add;     instruction["AND"] = &Assembler::_and;
    instruction["COMP"] = &Assembler::comp;   instruction["DIV"] = &Assembler::_div;
    instruction["J"] = &Assembler::j;         instruction["JEQ"] = &Assembler::jeq;
    instruction["JGT"] = &Assembler::jgt;     instruction["JLT"] = &Assembler::jlt;
    instruction["JSUB"] = &Assembler::jsub;   instruction["LDA"] = &Assembler::lda;
    instruction["LDCH"] = &Assembler::ldch;   instruction["LDL"] = &Assembler::ldl;
    instruction["LDX"] = &Assembler::ldx;     instruction["MUL"] = &Assembler::mul;
    instruction["OR"] = &Assembler::_or;      instruction["RD"] = &Assembler::rd;
    instruction["RSUB"] = &Assembler::rsub;   instruction["STA"] = &Assembler::sta;
    instruction["STCH"] = &Assembler::stch;   instruction["STL"] = &Assembler::stl;
    instruction["STSW"] = &Assembler::stsw;   instruction["STX"] = &Assembler::stx;
    instruction["SUB"] = &Assembler::sub;     instruction["TD"] = &Assembler::td;
    instruction["TIX"] = &Assembler::tix;     instruction["WD"] = &Assembler::wd;
}

void Assembler::assemble(string file)
{
    file = "../res/" + file;

    in_file.open(file.c_str(), ios::in);
    if(!in_file.is_open()){
        cerr << "Could not open the file!" << endl;
        exit(1);
    }

    pass1();
}

/**************** Pass 1 *********************
* 1) Assign addresses to all statements in the program
* 2) Save the values assigned to all labes for use in pass 2
* 3) Perform some processing of assembler directives    
*    ex// BYTE, RESW, etc.
**********************************************/
void Assembler::pass1()
{
    string s;
    for (; in_file.good() ;){
        getline(in_file, s);
        if(s.find("start") != std::string::npos){            
            for(auto &i: s){
                if(i >= 0x30 or i <= 0x39)
                    cout << i << endl;
            }
        }
        else
            locctr = 0;
        
    }
}

void Assembler::pass2()
{
    return;
}

// A <- (A) + (m..m+2)
int32 Assembler::add()
{
    instruction_opcode = 0x18;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (A) & (m..m+2)
int32 Assembler::_and()
{
    instruction_opcode = 0x40;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// (A) : (m..m+2)
int32 Assembler::comp()
{
    instruction_opcode = 0x28;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (A) / (m..m+2)
int32 Assembler::_div()
{
    instruction_opcode = 0x24;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// PC <- m
int32 Assembler::j()
{
    instruction_opcode = 0x3C;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// PC <- m if CC set to =
int32 Assembler::jeq()
{
    instruction_opcode = 0x30;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// PC <- m if CC set to >
int32 Assembler::jgt()
{
    instruction_opcode = 0x34;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// PC <- m if CC set to < 
int32 Assembler::jlt()
{
    instruction_opcode = 0x38;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// L <- (PC); PC <- m
int32 Assembler::jsub()
{
    instruction_opcode = 0x48;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (m..m+2)
int32 Assembler::lda()
{
    instruction_opcode = 0x00;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A[rightmost byte] <- (m)
int32 Assembler::ldch()
{
    instruction_opcode = 0x50;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// L <- (m..m+2)
int32 Assembler::ldl()
{
    instruction_opcode = 0x08;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// X <- (m..m+2)
int32 Assembler::ldx()
{
    instruction_opcode = 0x04;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (A) * (m..m+2)
int32 Assembler::mul()
{
    instruction_opcode = 0x20;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (A) | (m..m+2)
int32 Assembler::_or()
{
    instruction_opcode = 0x44;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A[rightmost byte] <- data from device specified by (m)
int32 Assembler::rd()
{
    instruction_opcode = 0xD8;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// PC <- (L)
int32 Assembler::rsub()
{
    instruction_opcode = 0x4C;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// m..m+2 <- (A)
int32 Assembler::sta()
{
    instruction_opcode = 0x0C;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// m <- (A)[rightmost byte]
int32 Assembler::stch()
{
    instruction_opcode = 0x54;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// m..m+2 <- (L)
int32 Assembler::stl()
{
    instruction_opcode = 14;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// m..m+2 <- (SW)
int32 Assembler::stsw()
{
    instruction_opcode = 0xE8;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// m..m+2 <- (X)
int32 Assembler::stx()
{
    instruction_opcode = 0x10;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// A <- (A) - (m..m+2)
int32 Assembler::sub()
{
    instruction_opcode = 0x1C;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// test device specified by m
int32 Assembler::td()
{
    instruction_opcode = 0xE0;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// X <- (X) + 1; (X) : (m..m+2)
int32 Assembler::tix()
{
    instruction_opcode = 0x2C;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

// device specified by (m) <- (A)[rightmost byte]
int32 Assembler::wd()
{
    instruction_opcode = 0xDC;
    instruction_opcode = instruction_opcode << 16;
    return instruction_opcode;
}

int main(int argv, char *argc[])
{
    string file(argc[1]);
    Assembler a;
    a.assemble(file);
    return 0;
}