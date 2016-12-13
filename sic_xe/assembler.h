#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <iomanip>    // for setw, setfill, hex, ...
#include <string>
#include <cstdio>
#include <map>
#include <cstdlib>    // for exit
#include <fstream>    // for ifstream
#include <sstream>    // for stringstream
#include <ctype.h>    // for isdigit
#include <bitset>

using namespace std;

class Assembler
{
private:

    bool indirect, immediate, indexing, base_rel, pc_rel, extended, format2;
    char *file;
    int LOCCTR, PREV_LOCCTR, starting_addr, prog_len, operand_addr, BASE;
    fstream in, interm, listing, object_file, m_file;
    string label, opcode, operand, program_name, object_code, base_operand;

    map<string, string> OPTAB;
    map<string, int> SYMTAB;


    string convert_byte_const();
    string convert_word_const();
    string trim(string);
    string assemble(string, int);

    map<string, int> init_registers();

    int get_addressing();
    int const_len();
    int get_len(string);

    void init_op_table();
    void read_line(fstream &, string &, string &, string &);
    void read_line(fstream &, int &, string &, string &, string &);
    void write_line(fstream &, int, string, string, string, string);
    void write_header_line(fstream &, string, int, int);
    void write_end_record(fstream &, int);
    void print(fstream &, int, int, string);
    void write_text_record_line(fstream &, int, int, string, string, bool);
    void write_mod_line();
    void make_mod_line();
    void relative_address(int&, int, int, string);

    void open_files_pass1();
    void open_files_pass2();

    void pass1();
    void pass2();

public:
    void run(char*);
    Assembler();
};

#endif