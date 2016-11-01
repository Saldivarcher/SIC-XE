#include "assembler.h"

Assembler::Assembler()
{
    format2 = false;

    indirect = false;
    immediate = false;
    indexing = false;
    base_rel = false;
    pc_rel = false;
    extended = false;
}

void Assembler::init_op_table(map<string, string> & op_table)
{
    // sic instructions
    op_table["STL"]  = "14";
    op_table["JSUB"] = "48";
    op_table["LDA"]  = "00";
    op_table["COMP"] = "28";
    op_table["JEQ"]  = "30";
    op_table["J"]    = "3C";
    op_table["STA"]  = "0C";
    op_table["LDCH"] = "50";
    op_table["LDL"]  = "08";
    op_table["RSUB"] = "4C";
    op_table["LDX"]  = "04";
    op_table["TD"]   = "E0";
    op_table["RD"]   = "D8";
    op_table["STCH"] = "54";
    op_table["TIX"]  = "2C";
    op_table["STA"]  = "0C";
    op_table["STX"]  = "10";
    op_table["JLT"]  = "38";
    op_table["WD"]   = "DC";
    op_table["ADD"]  = "18";
    op_table["AND"]  = "40";
    op_table["DIV"]  = "28";
    op_table["MUL"]  = "29";
    op_table["OR"]   = "44";
    op_table["STSW"] = "E8";
    op_table["SUB"]  = "1C";
    op_table["MUL"]  = "20";


    // new XE machine
    op_table["ADDF"]   = "58";
    op_table["ADDR"]   = "90";
    op_table["CLEAR"]  = "B4";
    op_table["COMPF"]  = "88";
    op_table["COMPR"]  = "A0";
    op_table["DIVF"]   = "64";
    op_table["DIVR"]   = "9C";
    op_table["FIX"]    = "C4";
    op_table["FLOAT"]  = "C0";
    op_table["HIO"]    = "F4";
    op_table["LDB"]    = "68";
    op_table["LDF"]    = "70";
    op_table["LDS"]    = "6C";
    op_table["LDT"]    = "74";
    op_table["LPS"]    = "D0";
    op_table["MULF"]   = "60";
    op_table["MULR"]   = "98";
    op_table["NORM"]   = "C8";
    op_table["RMO"]    = "AC";
    op_table["SHIFTL"] = "A4";
    op_table["SHIFTR"] = "A8";
    op_table["SIO"]    = "F0";
    op_table["SSK"]    = "EC";
    op_table["STB"]    = "78";
    op_table["STF"]    = "80";
    op_table["STI"]    = "D4";
    op_table["STS"]    = "7C";
    op_table["STT"]    = "84";
    op_table["SUBF"]   = "5C";
    op_table["SUBR"]   = "94";
    op_table["SVC"]    = "B0";
    op_table["TIO"]    = "2C";
    op_table["TIXR"]   = "B8";
}

int Assembler::const_len(string operand)
{
    if (operand[0] == 'C' and operand[1] == '\'' and operand[operand.length() - 1] == '\'')
        return operand.length() - 3;
    if (operand[0] == 'X' and operand[1] == '\'' and operand[operand.length() - 1] == '\'')
        return (operand.length() - 3) / 2;
    return -1;
}

string Assembler::convert_byte_const(string operand)
{
    if (operand[0] == 'X')
        return operand.substr(2, operand.length() - 3);
    if (operand[0] == 'C'){
        stringstream ss;
        for (int i = 2; i < operand.length() - 1; i++)
            ss << setw(2) << setfill('0') << hex << (int)operand[i];
        return ss.str();
    }
    return "00";
}

string Assembler::convert_word_const(string operand)
{
    int value = stoi(operand);
    stringstream ss;

    ss << setw(6) << setfill('0') << hex << value;
    return ss.str();
}

string Assembler::trim(string in)
{
    stringstream ss;
    string out;
    ss << in;


    ss >> out;
    return out;
}

// used for assembly file
void Assembler::read_line(fstream &in, string & label, string &opcode, string &operand)
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

// used on interm file
void Assembler::read_line(fstream &in, int &LOCCTR, string &label, string &opcode, string &operand)
{
    LOCCTR = -1;
    label = "";
    opcode = "";
    operand = "";

    string line;
    getline(in, line);
    if(!in.eof()){
        stringstream converter(line.substr(0, 4));
        converter >> hex >> LOCCTR;
        label = trim(line.substr(5, 10));
        opcode = trim(line.substr(16, 10));
        operand = trim(line.substr(27, 10));
    }
}

void Assembler::write_line(fstream &out, int LOCCTR, string label, string opcode, string operand, string obj_code = "")
{
    if(opcode != "BASE"){
        out << setw(4) << setfill('0') << right << hex << LOCCTR << ' '
            << setw(10) << setfill(' ') << left << label << ' '
            << setw(10) << opcode << ' ' << setw(10) << operand << ' '
            << setw(10) << obj_code << endl;
    } else {
        string temp = "";
        out << setw(4) << setfill(' ') << temp <<' '
            << setw(10) << setfill(' ') << left << label << ' '
            << setw(10) << opcode << ' ' << setw(10) << operand << ' '
            << setw(10) << obj_code << endl;
    } 
}

void Assembler::write_header_line(fstream &out, string prog_name, int starting_add, int program_len)
{
    out << 'H'
        << setw(6) << setfill(' ') << left << prog_name
        << setw(6) << setfill('0') << right << hex << starting_add
        << setw(6) << setfill('0') << hex << program_len << endl;
}

void Assembler::write_end_record(fstream &out, int starting_add)
{
    out << 'E'
        << setw(6) << setfill('0') << hex << starting_add
        << endl;
}

int Assembler::get_len(string object_code)
{
    return object_code.length() / 2;
}

void Assembler::print(fstream &out, int actual_starting_add, int length, string full_object_code)
{
    out << 'T'
        << setw(6) << setfill('0') << hex << actual_starting_add
        << setw(2) << setfill('0') << hex << length
        << hex << full_object_code
        << endl;
}

void Assembler::write_text_record_line(fstream &out, int locctr, int starting_add, string object_code, string opcode, bool last_line_flag)
{
    // getting the opcodes then putting it at the end
    static string full_object_code = "";
    full_object_code += object_code;
    
    // putting the
    static int actual_starting_add = 0;
    
    // just so it only sets the very first call
    if(locctr == starting_add)
        actual_starting_add = starting_add;

    // grabbing the full length of the "full object code"
    static int length = 0;
    length = get_len(full_object_code);

    // since I set length to 0 after each print
    // the starting address will always have a length <= 3
    if(length <= 3)
        actual_starting_add = locctr;

    if(length == 0x1E){
        print(out, actual_starting_add, length, full_object_code);
        length = 0;
        full_object_code = "";
    }
    else if(opcode == "RESW" or opcode == "RESB"){
        if(!full_object_code.empty()){
            print(out, actual_starting_add, length, full_object_code);
            length = 0;
            full_object_code = "";
        }
    }
    else if(length > 0x1E){
        size_t pos = full_object_code.find(object_code);
        if(pos != string::npos)
            full_object_code.erase(pos, object_code.length());
        length = get_len(full_object_code);
        print(out, actual_starting_add, length, full_object_code);
        length = 0;
        full_object_code = "";
        full_object_code += object_code;
        actual_starting_add = locctr;
    }
    else if(last_line_flag){
        size_t pos = full_object_code.find(object_code);
        if(pos != string::npos)
            full_object_code.erase(pos, object_code.length());
        length = get_len(full_object_code);
        print(out, actual_starting_add, length, full_object_code);
        length = 0;
        full_object_code = "";
    }
}

void Assembler::make_mod_line(fstream &out)
{
    out << 'M'
        << setw(6) << setfill('0') << hex << LOCCTR + 1
        << setw(2) << setfill('0') << '5' << endl;
}

void Assembler::write_mod_line(fstream &out)
{
    string s;
    ifstream m(string(file) + ".m");
    while (getline(m, s))
        out << s << endl;

    string rem = "rm " + string(file) + ".m";
    const char *rm = rem.c_str();
    system(rm);
}

int Assembler::get_addressing()
{
    unsigned long temp;
    if (!immediate and !indirect)
        immediate = indirect = true;

    if(!extended)
    {
        if(opcode == "LDB")
            pc_rel = 1;
        bitset<18> b;
        b[17] = indirect;
        b[16] = immediate;
        b[15] = indexing;
        b[14] = base_rel;
        b[13] = pc_rel;
        b[12] = extended;
        temp = b.to_ulong();
        int n = (int)temp;

        return n;
    }
    else
    {
        bitset<26> b;
        b[25] = indirect;
        b[24] = immediate;
        b[23] = indexing;
        b[22] = base_rel;
        b[21] = pc_rel;
        b[20] = extended;
        temp = b.to_ulong();
        int n = (int)temp;

        return n;
    }
}

string Assembler::assemble(string op, int operand)
{
    string obj_code = op;
    stringstream ss;
    if (format2)
    {
        if(opcode != "COMPR"){
            ss << setw(2) << setfill('0') << left << hex << operand;
            obj_code += ss.str();
            return obj_code;
        } else {
            ss << setw(2) << setfill('0') << right << hex << operand;
            obj_code += ss.str();
            return obj_code;
        }
    }
    else if(extended)
    {
        int i = get_addressing();
        stringstream tt;
        ss << setw(6) << setfill('0') << hex << operand;
        string test = "";
        obj_code += ss.str();
        long n = stol(obj_code, nullptr, 16);
        n |= i;
        tt << hex << n;
        obj_code = "";
        obj_code = tt.str();
        return obj_code;
    }
    else
    {
        int i = get_addressing();
        stringstream tt;
        ss << setw(4) << setfill('0') << hex << operand;
        string test = "";
        obj_code += ss.str();
        int size = obj_code.size();
        long n = stol(obj_code, nullptr, 16);
        n |= i;
        tt << hex << n;
        obj_code = "";
        obj_code = tt.str();
        if(obj_code.size() != size)
            obj_code.insert(0, 1, '0');
        return obj_code;
    }
}

void Assembler::open_files_pass1()
{
    in.open(file, ios::in);
    if(!in.is_open())
    {
        cout << "couldn't open assembly file: " << file << endl;
        exit(2);
    }
    interm.open((string(file)+".interm").c_str(), ios::out);
    if (!interm.is_open())
    {
        cout << "Couldn't open intermediate file: " << string(file)+".interm for output.\n";
        exit(3);
    }
}

void Assembler::open_files_pass2()
{
    interm.open((string(file)+".interm").c_str(), ios::in);
    if (!interm.is_open())
    {
        cout << "Couldn't open intermediate file: " << string(file)+".interm for input.\n";
        exit(6);
    }
    listing.open((string(file)+".list").c_str(), ios::out);
    if(!listing.is_open())
    {
        cout << "Couldn't open " << file << ".list for output\n";
        exit(7);
    }

    object_file.open((string(file)+".o"),ios::out);
    if(!object_file.is_open())
    {
        cout << "Couldn't open " << string(file) << ".o for output\n";
        exit(8);
    }

    m_file.open((string(file) + ".m"), ios::out);
}

map<string, int> Assembler::init_registers()
{
    map<string, int> r;
    r["A"]  = 0; r["X"]  = 1;
    r["L"]  = 2; r["B"]  = 3;
    r["S"]  = 4; r["T"]  = 5;
    r["F"]  = 6; r["PC"] = 8;
    r["SW"] = 9;

    return r;
}

void Assembler::relative_address(int &operand_addr, int pc, int base, string opcode)
{
    int pc_   = operand_addr - pc;
    int base_ = operand_addr - base;

    if (format2 or extended or immediate)
        return;
    if (opcode == "RSUB")
        return;

    if ((pc_ >= -2048 and pc_ <= 2047) and !indexing)
    {
        if (pc_ < 0)
        {
            pc_ = ~pc_;
            pc_ = 0xFFF - pc_;
        }
        operand_addr = pc_;
        pc_rel = true;
    }
    else if((base_ >= 0 and base_ <= 4095) or indexing) 
    {
        operand_addr = base_;
        base_rel = true;
    }
    else
    {
        cout << opcode << " " << operand << endl;
        cout << "You have to use format 4!" << endl;
        return;
    }
}

void Assembler::pass1()
{
    open_files_pass1();
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
            if (label != "")
            {
                if (SYMTAB.find(label) == SYMTAB.end())
                    SYMTAB[label] = LOCCTR;
                else 
                {
                    cout << "Duplicate symbol: " << label << endl;
                    exit(4);
                }
            }
            if (opcode == "CLEAR" or opcode == "COMPR" or opcode == "TIXR")
                LOCCTR += 2;
            else if (OPTAB.find(opcode) != OPTAB.end())
                LOCCTR += 3;
            else if (opcode == "WORD")
                LOCCTR += 3;
            else if (opcode == "RESW")
                LOCCTR += (3 * stoi(operand));
            else if (opcode == "RESB")
                LOCCTR += stoi(operand);
            else if (opcode == "BYTE")
                LOCCTR += const_len(operand);
            else if(opcode == "BASE"){
                BASE = 0;
                base_operand = operand;
            }
            else if(opcode[0] == '+')
            {
                extended = true;
                LOCCTR += 4;
            }
            else
            {
                cout << "Invalid opcode: " << opcode << endl;
                exit(5);
            }
            write_line(interm, PREV_LOCCTR, label, opcode, operand);
        }
        read_line(in, label, opcode, operand);
    }
    write_line(interm, LOCCTR, label, opcode, operand);
    prog_len = LOCCTR - starting_addr;

    BASE = SYMTAB[base_operand];
    map<string, int> registers = init_registers();
    SYMTAB.insert(registers.begin(), registers.end());

    in.close();
    interm.close();
}

void Assembler::pass2()
{
    string temp = "";
    open_files_pass2();
    read_line(interm, LOCCTR, label, opcode, operand);
    if(opcode == "START")
    {
        write_line(listing, LOCCTR, label, opcode, operand);
        read_line(interm, LOCCTR, label, opcode, operand);
    }

    // write Header rec
    write_header_line(object_file, program_name, starting_addr, prog_len);

    // init first Text rec
    write_text_record_line(object_file, LOCCTR, starting_addr, object_code, opcode, false);
    while (!interm.eof() and opcode != "END")
    {
        indirect = immediate = indexing = base_rel = pc_rel = extended = false;
        format2 = false;
        object_code = "";
        if (label == "" or label[0] != '.') // not comment
        {

            if (opcode[0] == '+')
            {
                extended = true;
                temp = opcode;
                opcode = opcode.substr(1);
            }

            if (OPTAB[opcode] != "")
            {
                if (operand != "")
                {
                    if (opcode == "CLEAR" or opcode == "TIXR")
                        format2 = true;
                    // if ,X at end of operand, remove and pass index flag to assemble()
                    // using find because "substr" would work on "CLEAR X"
                    else if (operand.find(",X") != string::npos )
                    {
                        operand = operand.substr(0, operand.length() - 2);
                        indexing = true;
                    }
                    else
                        indexing = false;

                    if (SYMTAB.find(operand) != SYMTAB.end())
                    {
                        operand_addr = SYMTAB[operand];
                        if(indexing)
                            operand += ",X";
                    }
                    else if(operand[0] == '#')
                    {
                        immediate = true;
                        if (isdigit(operand[1]))
                            operand_addr = stoi(operand.substr(1, operand.length()));
                    }
                    else if (operand[0] == '@'){
                        indirect = true;
                        operand_addr = SYMTAB[operand.substr(1)];
                    }
                    else if (opcode == "COMPR")
                    {
                        operand_addr = 0;
                        operand_addr = operand_addr << SYMTAB[operand.substr(0, 1)];
                        operand_addr += SYMTAB[operand.substr(operand.length()-1)];
                        format2 = true;
                    }
                    else
                    {
                        cout << "Undefined symbol: " << operand << endl;
                        exit(8);
                    }
                }
                else
                    operand_addr = 0;

                int pc = LOCCTR + 3;
                relative_address(operand_addr, pc, BASE, opcode);
                object_code = assemble(OPTAB[opcode], operand_addr);
            }
            else if (opcode == "WORD")
                object_code = convert_word_const(operand);
            else if (opcode == "BYTE")
                object_code = convert_byte_const(operand);
            // if object_code doesn't fit into current Text rec...
            // add object_code to Text rec
            write_text_record_line(object_file, LOCCTR, starting_addr, object_code, opcode, false);

            
            if(extended and SYMTAB.find(operand) != SYMTAB.end())
                make_mod_line(m_file);
        }
        if(extended)
            opcode = temp;
        write_line(listing, LOCCTR, label, opcode, operand, object_code);
        read_line(interm, LOCCTR, label, opcode, operand);
    }
    // write last Text rec to obj prog
    write_text_record_line(object_file, LOCCTR, starting_addr, object_code, opcode, true);
    write_mod_line(object_file);

    // write End rec to obj prog
    write_end_record(object_file, starting_addr);
    write_line(listing, LOCCTR, label, opcode, operand);
    interm.close();
    listing.close();
    m_file.close();
}

void Assembler::run(char* f)
{
    file = f;
    init_op_table(OPTAB);
    
    pass1();
    pass2();
}