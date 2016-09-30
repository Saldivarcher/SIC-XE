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

// A <- (A) + (m..m+2)
int Assembler::add()
{
    instruction_opcode = 0x18;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (A) & (m..m+2)
int Assembler::_and()
{
    instruction_opcode = 0x40;
    instruction_opcode = instruction_opcode << 16;
}

// (A) : (m..m+2)
int Assembler::comp()
{
    instruction_opcode = 0x28;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (A) / (m..m+2)
int Assembler::_div()
{
    instruction_opcode = 0x24;
    instruction_opcode = instruction_opcode << 16;
}

// PC <- m
int Assembler::j()
{
    instruction_opcode = 0x3C;
    instruction_opcode = instruction_opcode << 16;
}

// PC <- m if CC set to =
int Assembler::jeq()
{
    instruction_opcode = 0x30;
    instruction_opcode = instruction_opcode << 16;
}

// PC <- m if CC set to >
int Assembler::jgt()
{
    instruction_opcode = 0x34;
    instruction_opcode = instruction_opcode << 16;
}

// PC <- m if CC set to < 
int Assembler::jlt()
{
    instruction_opcode = 0x38;
    instruction_opcode = instruction_opcode << 16;
}

// L <- (PC); PC <- m
int Assembler::jsub()
{
    instruction_opcode = 0x48;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (m..m+2)
int Assembler::lda()
{
    instruction_opcode = 0x00;
    instruction_opcode = instruction_opcode << 16;
}

// A[rightmost byte] <- (m)
int Assembler::ldch()
{
    instruction_opcode = 0x50;
    instruction_opcode = instruction_opcode << 16;
}

// L <- (m..m+2)
int Assembler::ldl()
{
    instruction_opcode = 0x08;
    instruction_opcode = instruction_opcode << 16;
}

// X <- (m..m+2)
int Assembler::ldx()
{
    instruction_opcode = 0x04;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (A) * (m..m+2)
int Assembler::mul()
{
    instruction_opcode = 0x20;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (A) | (m..m+2)
int Assembler::_or()
{
    instruction_opcode = 0x44;
    instruction_opcode = instruction_opcode << 16;
}

// A[rightmost byte] <- data from device specified by (m)
int Assembler::rd()
{
    instruction_opcode = 0xD8;
    instruction_opcode = instruction_opcode << 16;
}

// PC <- (L)
int Assembler::rsub()
{
    instruction_opcode = 0x4C;
    instruction_opcode = instruction_opcode << 16;
}

// m..m+2 <- (A)
int Assembler::sta()
{
    instruction_opcode = 0x0C;
    instruction_opcode = instruction_opcode << 16;
}

// m <- (A)[rightmost byte]
int Assembler::stch()
{
    instruction_opcode = 0x54;
    instruction_opcode = instruction_opcode << 16;
}

// m..m+2 <- (L)
int Assembler::stl()
{
    instruction_opcode = 14;
    instruction_opcode = instruction_opcode << 16;
}

// m..m+2 <- (SW)
int Assembler::stsw()
{
    instruction_opcode = 0xE8;
    instruction_opcode = instruction_opcode << 16;
}

// m..m+2 <- (X)
int Assembler::stx()
{
    instruction_opcode = 0x10;
    instruction_opcode = instruction_opcode << 16;
}

// A <- (A) - (m..m+2)
int Assembler::sub()
{
    instruction_opcode = 0x1C;
    instruction_opcode = instruction_opcode << 16;
}

// test device specified by m
int Assembler::td()
{
    instruction_opcode = 0xE0;
    instruction_opcode = instruction_opcode << 16;
}

// X <- (X) + 1; (X) : (m..m+2)
int Assembler::tix()
{
    instruction_opcode = 0x2C;
    instruction_opcode = instruction_opcode << 16;
}

// device specified by (m) <- (A)[rightmost byte]
int Assembler::wd()
{
    instruction_opcode = 0xDC;
    instruction_opcode = instruction_opcode << 16;
}