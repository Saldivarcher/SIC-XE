/****************************************************************
Kay Zemoudeh
Oct 1, 2016
assembler.cpp

Compile:
g++ -std=c++11 assembler.cpp -o assembler

Run:
assemble copy.as

This is a 2-pass assembler for SIC (Simple Instructional Computer)
as defined by Leland Beck in "System Software", 3rd ed.
Every instruction is 3 bytes long.
Assembly lines consist of an optional label starting
in column 1, opcode, optional operand, and optional comment.
Label, opcode, and operand lengths are <= 10.
****************************************************************/

#include <iostream>
#include <iomanip>    // for setw, setfill, hex, ...
#include <string>
#include <algorithm>  // for :find
#include <map>
#include <cstdlib>    // for exit
#include <fstream>    // for ifstream
#include <sstream>    // for stringstream

using namespace std;

void init_op_table(map<string, string> & op_table)
{
    // minimum number of opcodes to assemble Fig 2.1
    op_table["STL"] = "14";
    op_table["JSUB"] = "48";
    op_table["LDA"] = "00";
    op_table["COMP"] = "28";
    op_table["JEQ"] = "30";
    op_table["J"] = "3C";
    op_table["STA"] = "0C";
    op_table["LDCH"] = "50";
    op_table["LDL"] = "08";
    op_table["RSUB"] = "4C";
    op_table["LDX"] = "04";
    op_table["TD"] = "E0";
    op_table["RD"] = "D8";
    op_table["STCH"] = "54";
    op_table["TIX"] = "2C";
    op_table["STA"] = "0C";
    op_table["STX"] = "10";
    op_table["JLT"] = "38";
    op_table["WD"] = "DC";
}

// returns length of constants such as C'EOF' = 3, X'05' = 1
// almost no error check, e.g. hex const must have an even number of chars
int const_len(string operand)
{
    if (operand[0] == 'C' and operand[1] == '\'' and operand[operand.length()-1] == '\'')
        return operand.length() - 3;            
    if (operand[0] == 'X' and operand[1] == '\'' and operand[operand.length()-1] == '\'')
        return (operand.length() - 3)/2;            
    return -1; // error flag
}

string convert_byte_const(string operand)
{
    if (operand[0] == 'X') // assuming the next and last char is '
        return operand.substr(2, operand.length()-3);
    if (operand[0] == 'C') // assuming the next and last char is '
    {
        stringstream ss;
        for (int i = 2; i < operand.length()-1; i++)
            ss << setw(2) << setfill('0') << hex << (int) operand[i];
        return ss.str();
    }
    return "00"; // error flag
}

string convert_word_const(string operand)
{
    int value = stoi(operand);
    stringstream ss;

    ss << setw(6) << setfill('0') << hex << value;
    return ss.str();
}

string trim(string in)
{
    stringstream ss;
    string out;

    ss << in;
    // can't return ss.str() as above bc ss still has blanks in it
    ss >> out;

    return out;
}

void read_line(fstream &in, string & label, string &opcode, string &operand)
{
    label = "";
    opcode = "";
    operand = "";

    string line;
    getline(in, line);
    if (!in.eof())
    {
        stringstream line_string(line);
        if (line[0] == ' ' or line[0] == '\t') // no label
            line_string >> opcode >> operand;
        else
            line_string >> label >> opcode >> operand;
        
        if(opcode == "RSUB")
            operand = "";
    }
}

void read_line(fstream &in, int &LOCCTR, string &label, string &opcode, string &operand)
{
    LOCCTR = -1;
    label = "";
    opcode = "";
    operand = "";

    string line;
    getline(in, line);
    if (!in.eof())
    {
        stringstream converter(line.substr(0, 4)); // LOCCTR values in interm file are in hex
        converter >> hex >> LOCCTR;
        label = trim(line.substr(5, 10));
        opcode = trim(line.substr(16, 10));
        operand = trim(line.substr(27, 10));
    }
}

void write_line(fstream &out, int LOCCTR, string label, string opcode, string operand, string obj_code = "")
{
    out << setw(4) << setfill('0') << hex << LOCCTR << ' ' 
        << setw(10) << setfill(' ') << left << label << ' ' 
        << setw(10) << opcode << ' ' << setw(10) << operand << ' ' 
        << setw(10) << obj_code << endl;
}

void write_header_line(fstream &out, string prog_name, int starting_add, int program_len)
{
    out << 'H'
        << setw(6) << setfill(' ') << left << prog_name
        << setw(6) << setfill('0') << right << hex << starting_add
        << setw(6) << setfill('0') << hex << program_len << endl;
}

void write_end_record(fstream &out, int starting_add)
{
    out << 'E'
        << setw(6) << setfill('0') << hex << starting_add
        << endl;
}

int get_obj_code_len(string object_code)
{
    return object_code.length() / 2;
}

/*
    TODO:
        save the right starting address
*/

void write_text_record_line(fstream &out, int starting_add, string object_code, string opcode, bool last_line_flag)
{
    static string full_object_code = "";
    full_object_code += object_code;
    static int length = 0;
    length = get_obj_code_len(full_object_code);
    if(length == 0x1E){
        out << 'T'
            << setw(6) << setfill('0') << hex << starting_add
            << setw(2) << setfill('0') << hex << length
            << hex << full_object_code
            << endl;
        length = 0;
        full_object_code = "";
    }
    else if(opcode == "RESW" or opcode == "RESB"){
        if(!full_object_code.empty()){
            out << 'T'
                << setw(6) << setfill('0') << hex << starting_add
                << setw(2) << setfill('0') << hex << length
                << hex << full_object_code
                << endl;
            length = 0;
            full_object_code = "";
        }
    }
    else if(length > 0x1E){
        size_t pos = full_object_code.find(object_code);
        if(pos != string::npos)
            full_object_code.erase(pos, object_code.length());
        length = get_obj_code_len(full_object_code);
        out << 'T'
            << setw(6) << setfill('0') << hex << starting_add
            << setw(2) << setfill('0') << hex << length
            << hex << full_object_code
            << endl;
        length = 0;
        full_object_code = "";
        full_object_code += object_code;
    }
    else if(last_line_flag){
        size_t pos = full_object_code.find(object_code);
        if(pos != string::npos)
            full_object_code.erase(pos, object_code.length());
        length = get_obj_code_len(full_object_code);
        out << 'T'
            << setw(6) << setfill('0') << hex << starting_add
            << setw(2) << setfill('0') << hex << length
            << hex << full_object_code
            << endl;
        length = 0;
        full_object_code = "";
    }
}


string assemble(string opcode, int operand, bool indexing)
{
    string obj_code = opcode;
    if (indexing)
        operand |= 0x8000; // turn on bit 15
    stringstream ss;
    ss << setw(4) << setfill('0') << hex << operand;
    obj_code += ss.str();
    return obj_code;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Try again. How to run:\nassemble filename.as\n";
        exit(1);
    }

    fstream in, interm, listing, object_file;
    
    map<string, string> OPTAB;
    map<string, int> SYMTAB;
    int LOCCTR, PREV_LOCCTR;
    int starting_addr, prog_length;

    init_op_table(OPTAB);

    string label, opcode, operand;
    
    //added by me
    string program_name;

    // ************** Pass 1
    in.open(argv[1], ios::in);
    if (!in.is_open())
    {
        cout << "Couldn't open assembly file: " << argv[1] << " for input.\n";
        exit(2);
    }    

    interm.open((string(argv[1])+".interm").c_str(), ios::out);
    if (!interm.is_open())
    {
        cout << "Couldn't open intermediate file: " << string(argv[1])+".interm for output.\n";
        exit(3);
    }    

    read_line(in, label, opcode, operand);
    if (opcode == "START")
    {
        stringstream converter(operand); // operand is in hex
        converter >> hex >> starting_addr;
        LOCCTR = starting_addr;
        
        //added by me
        program_name = label;

        write_line(interm, LOCCTR, label, opcode, operand);
        read_line(in, label, opcode, operand);
    }
    else
    {
        LOCCTR = 0;
        starting_addr = 0;
    }

    while (!in.eof() and opcode != "END")
    {
        PREV_LOCCTR = LOCCTR;
        if (label == "" or label[0] != '.') // not comment
        {
            if (label != ""){
                if (SYMTAB.find(label) == SYMTAB.end())
                    SYMTAB[label] = LOCCTR;
                else {
                        cout << "Duplicate symbol: " << label << endl;
                        exit(4);
                     }
            }
            if (OPTAB.find(opcode) != OPTAB.end())
                LOCCTR += 3;
            else if (opcode == "WORD")
                LOCCTR += 3;
            else if (opcode == "RESW")
                LOCCTR += (3 * stoi(operand));
            else if (opcode == "RESB")
                LOCCTR += stoi(operand);
            else if (opcode == "BYTE")
                LOCCTR += const_len(operand);
            else {
                    cout << "Invalid opcode: " << opcode << endl;
                    exit(5);
                 }
            write_line(interm, PREV_LOCCTR, label, opcode, operand);
        }
        read_line(in, label, opcode, operand);
    }
    write_line(interm, LOCCTR, label, opcode, operand);
    prog_length = LOCCTR - starting_addr;
    in.close();
    interm.close();

    // ************** Pass 2
    interm.open((string(argv[1])+".interm").c_str(), ios::in);
    if (!interm.is_open())
    {
        cout << "Couldn't open intermediate file: " << string(argv[1])+".interm for input.\n";
        exit(6);
    }

    listing.open((string(argv[1])+".list").c_str(), ios::out);
    if (!listing.is_open())
    {
        cout << "Couldn't open " << argv[1] << ".list for output.\n";
        exit(7);
    }
    
    int operand_addr;
    bool indexing;
    string object_code;

    read_line(interm, LOCCTR, label, opcode, operand);
    if (opcode == "START")
    {
        write_line(listing, LOCCTR, label, opcode, operand);
        read_line(interm, LOCCTR, label, opcode, operand);
    }

    string obj_file = string(argv[1]);
    obj_file = obj_file.substr(0 ,obj_file.length() - 4);
    obj_file += ".o";

    object_file.open(obj_file,ios::out);
    if(!object_file.is_open())
    {
        cout << "Couldn't open " << obj_file << " for output\n";
        exit(8);
    }
    
    // write Header rec
    write_header_line(object_file, program_name, starting_addr, prog_length);

    // init first Text rec
    write_text_record_line(object_file, LOCCTR, object_code, opcode, true);
    while (!in.eof() and opcode != "END")
    {
        object_code = "";
        if (label == "" or label[0] != '.') // not comment
        {
            if (OPTAB[opcode] != "")
            {
                if (operand != "")
                {
                    // if ,X at end of operand, remove and pass index flag to assemble()
                    if (operand.substr(operand.length()-2) == ",X")
                    {
                        operand = operand.substr(0, operand.length()-2);
                        indexing = true;
                    } else
                        indexing = false;
                    if (SYMTAB.find(operand) != SYMTAB.end()){
                        operand_addr = SYMTAB[operand];
                        if(indexing)
                            operand += ",X";
                    }
                    else {
                        cout << "Undefined symbol: " << operand << endl;
                        exit(8);
                    }
                }
                else
                    operand_addr = 0;
                object_code = assemble(OPTAB[opcode], operand_addr, indexing);
            }
            else if (opcode == "WORD")
                object_code = convert_word_const(operand);
            else if (opcode == "BYTE")
                object_code = convert_byte_const(operand);
            // if object_code doesn't fit into current Text rec...
            // add object_code to Text rec
            write_text_record_line(object_file, LOCCTR, object_code, opcode, false);
            }
        write_line(listing, LOCCTR, label, opcode, operand, object_code);
        read_line(interm, LOCCTR, label, opcode, operand);
    }
    // write last Text rec to obj prog
    write_text_record_line(object_file, LOCCTR, object_code, opcode, true);
    // write End rec to obj prog
    write_end_record(object_file, starting_addr);
    write_line(listing, LOCCTR, label, opcode, operand);
    interm.close();
    listing.close();
} // main
