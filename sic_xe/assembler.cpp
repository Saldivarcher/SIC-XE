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

void Assembler::init_op_table()
{
    // sic instructions
    OPTAB["STL"]  = "14";
    OPTAB["JSUB"] = "48";
    OPTAB["LDA"]  = "00";
    OPTAB["COMP"] = "28";
    OPTAB["JEQ"]  = "30";
    OPTAB["J"]    = "3C";
    OPTAB["STA"]  = "0C";
    OPTAB["LDCH"] = "50";
    OPTAB["LDL"]  = "08";
    OPTAB["RSUB"] = "4C";
    OPTAB["LDX"]  = "04";
    OPTAB["TD"]   = "E0";
    OPTAB["RD"]   = "D8";
    OPTAB["STCH"] = "54";
    OPTAB["TIX"]  = "2C";
    OPTAB["STA"]  = "0C";
    OPTAB["STX"]  = "10";
    OPTAB["JLT"]  = "38";
    OPTAB["WD"]   = "DC";
    OPTAB["ADD"]  = "18";
    OPTAB["AND"]  = "40";
    OPTAB["DIV"]  = "28";
    OPTAB["MUL"]  = "29";
    OPTAB["OR"]   = "44";
    OPTAB["STSW"] = "E8";
    OPTAB["SUB"]  = "1C";
    OPTAB["MUL"]  = "20";


    // new XE machine
    OPTAB["ADDF"]   = "58";
    OPTAB["ADDR"]   = "90";
    OPTAB["CLEAR"]  = "B4";
    OPTAB["COMPF"]  = "88";
    OPTAB["COMPR"]  = "A0";
    OPTAB["DIVF"]   = "64";
    OPTAB["DIVR"]   = "9C";
    OPTAB["FIX"]    = "C4";
    OPTAB["FLOAT"]  = "C0";
    OPTAB["HIO"]    = "F4";
    OPTAB["LDB"]    = "68";
    OPTAB["LDF"]    = "70";
    OPTAB["LDS"]    = "6C";
    OPTAB["LDT"]    = "74";
    OPTAB["LPS"]    = "D0";
    OPTAB["MULF"]   = "60";
    OPTAB["MULR"]   = "98";
    OPTAB["NORM"]   = "C8";
    OPTAB["RMO"]    = "AC";
    OPTAB["SHIFTL"] = "A4";
    OPTAB["SHIFTR"] = "A8";
    OPTAB["SIO"]    = "F0";
    OPTAB["SSK"]    = "EC";
    OPTAB["STB"]    = "78";
    OPTAB["STF"]    = "80";
    OPTAB["STI"]    = "D4";
    OPTAB["STS"]    = "7C";
    OPTAB["STT"]    = "84";
    OPTAB["SUBF"]   = "5C";
    OPTAB["SUBR"]   = "94";
    OPTAB["SVC"]    = "B0";
    OPTAB["TIO"]    = "2C";
    OPTAB["TIXR"]   = "B8";
}

int Assembler::const_len()
{
    if (operand[0] == 'C' and operand[1] == '\'' and operand[operand.length() - 1] == '\'')
        return operand.length() - 3;
    if (operand[0] == 'X' and operand[1] == '\'' and operand[operand.length() - 1] == '\'')
        return (operand.length() - 3) / 2;
    return -1;
}

string Assembler::convert_byte_const()
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

string Assembler::convert_word_const()
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

void Assembler::make_mod_line()
{
    m_file << 'M'
        << setw(6) << setfill('0') << hex << LOCCTR + 1
        << setw(2) << setfill('0') << '5' << endl;
}

void Assembler::write_mod_line()
{
    string s;
    ifstream m(string(file) + ".m");
    while (getline(m, s))
        object_file << s << endl;

    string rem = "rm " + string(file) + ".m";
    const char *rm = rem.c_str();
    system(rm);
}

/*
* Helper function that grabs the boolean values of the 
* register, and puts it into array of bools or 'bitset'.
* Then I turn the bitset into the corresponding integer
*/
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
        // extended, so 6 flag bits + 20 address bits
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
        // i will be the value gathered from [n|i|x|b|p|e|   disp    ]
        int i = get_addressing();
        stringstream tt;
        ss << setw(6) << setfill('0') << hex << operand;
        string test = "";
        obj_code += ss.str();

        // I'm telling the stoi fun that I am using base 16
        int n = stoi(obj_code, nullptr, 16);
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

        // I'm telling the stoi fun that I am using base 16
        int n = stoi(obj_code, nullptr, 16);
        n |= i;
        tt << hex << n;
        obj_code = "";
        obj_code = tt.str();

        // obj_code gets truncated when opcode[0] == '0'
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


// setting register to corresponding values
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


/*
* This functon calculates the relative address of an instruction
* based on whether it is pc_rel or base_rel. If not give user an error
* telling them to use format4/extended
*/
void Assembler::relative_address(int &operand_addr, int pc, int base, string opcode)
{
    int pc_   = operand_addr - pc;
    int base_ = operand_addr - base;

    // format2, extended, and immediate instr
    // don't need relative addresses
    if (format2 or extended or immediate)
        return;
    if (opcode == "RSUB")
        return;

    // indexing is always base_rel
    if ((pc_ >= -2048 and pc_ <= 2047) and !indexing)
    {
        // negative value was a bigger than 12 bits
        // so I inverted than got the negative value
        // into 12 bits
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
        exit(10);
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
                LOCCTR += const_len();
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

    // setting base to its operands location
    BASE = SYMTAB[base_operand];

    // inserting registers and thier values into symtab
    // A = 0, X = 1, ..., PC = 8, SW = 9
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
            // remove the '+' from the opcode
            // and set e bit = 1
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

                    // only going to get operands that don't
                    // have 'special' characters infront 
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
                object_code = convert_word_const();
            else if (opcode == "BYTE")
                object_code = convert_byte_const();
            // if object_code doesn't fit into current Text rec...
            // add object_code to Text rec
            write_text_record_line(object_file, LOCCTR, starting_addr, object_code, opcode, false);

            
            // only making modifications on instructions that
            // jump to a subroutine, and assuming user knows
            // that they must make it extended when doing that
            // jump instruction
            if(extended and SYMTAB.find(operand) != SYMTAB.end())
                make_mod_line();
        }
        if(extended)
            opcode = temp;
        write_line(listing, LOCCTR, label, opcode, operand, object_code);
        read_line(interm, LOCCTR, label, opcode, operand);
    }
    // write last Text rec to obj prog
    write_text_record_line(object_file, LOCCTR, starting_addr, object_code, opcode, true);
    write_mod_line();

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
    init_op_table();
    
    pass1();
    pass2();
}