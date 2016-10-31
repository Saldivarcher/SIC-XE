#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <iomanip>    // for setw, setfill, hex, ...
#include <string>
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
    map<string, string> OPTAB;
    map<string, int> SYMTAB;
    fstream in, interm, listing, object_file;
    int LOCCTR, PREV_LOCCTR, starting_addr, prog_len, operand_addr;
    int BASE;
    string base_operand;
    string label, opcode, operand, program_name, object_code;
    char *file;
    bool indirect, immediate, indexing, base_rel, pc_rel, extended;
    bool format2;

    void init_op_table(map<string, string> &);
    int const_len(string);
    string convert_byte_const(string);
    string convert_word_const(string);
    string trim(string);
    void read_line(fstream &, string &, string &, string &);
    void read_line(fstream &, int &, string &, string &, string &);
    void write_line(fstream &, int, string, string, string, string);

    void write_header_line(fstream &, string, int, int);
    void write_end_record(fstream &, int);
    int get_len(string);
    void print(fstream &, int, int, string);
    void write_text_record_line(fstream &, int, int, string, string, bool);

    void relative_address(int&, int, int, string);

    void open_files_pass1();
    void open_files_pass2();

    map<string, int> init_registers();

    string assemble(string, int);
    int get_addressing();
    void pass1();
    void pass2();

public:
    void run(char*);
    Assembler();
};

#endif