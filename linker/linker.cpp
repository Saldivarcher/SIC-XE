#include "linker.h"

Linker::Linker()
{
    memset(mem, '.', sizeof mem);
    PROGADDR = 0x4000;
    CSADDR = PROGADDR;
    EXECADDR = PROGADDR;
    start_addr = 0;
}

void Linker::pass1()
{
    while(!infile.eof())
    {
        read_line();
        if(ESTAB.find(header) == ESTAB.end() and header != "")
        {
            ESTAB[header].control_section = header;
            ESTAB[header].length = CSLTH;
            ESTAB[header].address = CSADDR;
        }
        else if(ESTAB.find(header) != ESTAB.end())
        {
            cout << "Duplicate symbols for header!" << endl;
            exit(1);
        }
        if(record[0] != 'E')
        {
            if(record[0] == 'D')
            {
                parse_record();
                update_estab();
            }
        }
        if(record[0] == 'E')
            CSADDR += CSLTH;
        drec_holder.clear();
    }
    infile.close();
    print_estab();
}

void Linker::pass2()
{
    while(!infile.eof())
    {
        read_line();
        object_code = record;
        if(header != "")
        {
            CSLTH = ESTAB[header].length;
            saved_header = header;
        }
        if(record[0] != 'E')
        {
            if(record[0] == 'T')
            {
                parse_t_record();
                string temp;
                for (auto i : t_rec)
                {
                    for (auto j : i)
                    {
                        temp += j;
                        if (temp.size() == 2)
                        {
                            char c = get_hex(temp);
                            mem[(ESTAB[saved_header].address + start_addr++) - 0x4000] = c;
                            temp.clear();
                        }
                    }
                }
            }
            if(record[0] == 'M')
            {
                string symbol, location, length, sign;
                parse_m_record(symbol, location, length, sign);
                if (ESTAB.find(symbol) != ESTAB.end())
                {

                    int int_location = stoi(location, nullptr, 16);
                    int int_length = stoi(length, nullptr, 16);
                    int actual_location = (int_location + ESTAB[saved_header].address) - 0x4000;

                    // should apply to m records that use extended format
                    if(int_length == 5)
                    {
                        string t;
                        memory_object(t, actual_location);
                        int modified;
                        modified_obj_code(modified, sign, t, symbol);
                        stringstream ss;
                        ss << hex << modified;
                        t = ss.str();
                        t = insert_zero(t);
                        update_mem(t, actual_location);
                    }
                    else
                    {
                        string t;
                        memory_object(t, actual_location);
                        int modified;
                        modified_obj_code(modified, sign, t, symbol);
                        stringstream ss;
                        ss << hex << modified;
                        t = ss.str();
                        t = insert_zero(t);
                        update_mem(t, actual_location);
                    }
                }
                else
                {
                    cout << "Symbol is undefined! " << symbol << endl;
                    exit(3);
                }
            }
            t_rec.clear();
        }
    }
    print_mem();
}

void Linker::update_mem(string t, int actual_location)
{
    string temp;
    for(auto i: t)
    {
        temp += i;
        if(temp.size() == 2)
        {
            char c = get_hex(temp);
            mem[actual_location++] = c;
            temp.clear();
        }
    }
}

void Linker::print_mem()
{
    cout << endl;
    for (int i = 0; i < 0x150; i++)
    {
        if(i == 0)
            cout << i + 0x4000 << " ";
        if(i % 0x10 == 0 and i != 0)
            cout << endl << i + 0x4000 << " ";
        if(mem[i] == '.')
            cout << mem[i];
        else
        {
            string t;
            stringstream ss;
            int test = (int)mem[i];
            if (test < 0)
                test = test - 0xffffff00;
            ss << hex << test;
            t = ss.str();
            if(t.size() == 1)
                t.insert(0, 1, '0');
            cout << t;
        }
    }
    cout << endl;
}


string Linker::insert_zero(string t)
{
    if(t.size() == 4)
        t.insert(0, 2, '0');
    else if(t.size() == 3)
        t.insert(0, 3, '0');
    else if(t.size() == 2)
        t.insert(0, 4, '0');
    else if(t.size() == 1)
        t.insert(0, 5, '0');
    else if(t.size() > 6)
        t = t.substr(2);
    return t;
}

void Linker::memory_object(string &t, int actual_location)
{
    for (int i = actual_location; i < actual_location + 3; i++)
    {
        stringstream ss;
        int value = mem[i];
        if(value < 0)
            value = value - 0xffffff00;
        
        ss << hex << value;
        string s(ss.str());
        if(s.size() == 1)
        {
            s.insert(0, 1, '0');
        }
        t += s;
    }
}

void Linker::modified_obj_code(int &modified, string sign, string t, string symbol)
{
    if (sign[0] == '+')
    {
        int inverse = stoi(t, nullptr, 16);
        if (t[0] == 'f')
        {
            inverse = (0xffffff - inverse);
            inverse = ~inverse;
        }
        modified = inverse + ESTAB[symbol].address;
    }
    else
    {
        int inverse = stoi(t, nullptr, 16);
        if(t[0] == 'f')
        {
            inverse = (0xffffff - inverse);
            inverse = ~inverse;
        }
        modified = inverse - ESTAB[symbol].address;
    }
}

void Linker::parse_m_record(string &sym, string &location, string &length, string& sign)
{
    location = object_code.substr(1, 6);
    length = object_code.substr(7, 2);
    string obj = record.substr(1);
    sym = obj.substr(9);
    sign = obj.substr(8, 1);
}

char Linker::get_hex(string s)
{
    char a = s[0];
    if('0' <= a and a <= '9')
        a = a - '0';
    else if('a' <= a and a <= 'f')
        a = a - 'a' + 10;
    else if('A' <= a and a <= 'F')
        a = a - 'A' + 10;

    char b = s[1];
    if('0' <= b and b <= '9')
        b = b - '0';
    else if('a' <= b and b <= 'f')
        b = b - 'a' + 10;
    else if('A' <= b and b <= 'F')
        b = b - 'A' + 10;

    char c = a;
    c = (c << 4) + b;
    return c;
}

void Linker::parse_t_record()
{
    string rec = record;
    start_addr = stoi(rec.substr(1, 6), nullptr, 16);
    rec = rec.substr(9);
    string temp;
    for(auto i: rec)
        temp += i;
    t_rec.push_back(temp);
}

void Linker::read_line()
{
    getline(infile, header);
    record = header;
    if(header[0] == 'H')
    {
        CSLTH = stoi(header.substr(13), nullptr, 16);
        header = header.substr(1, 6);
        header.erase(remove_if(header.begin(), header.end(),
                               [](char x) { return isspace(x); }),
                               header.end());
        cs = header;
    }
    else
        header = "";
}

void Linker::parse_record()
{
    record = record.substr(1);
    string temp;
    for(auto i: record)
    {
        temp += i;
        if(temp.size() == 6){
            temp.erase(remove_if(temp.begin(), temp.end(),
                                 [](char x) { return isspace(x); }),
                                 temp.end());
            drec_holder.push_back(temp);
            temp.clear();
        }
    }
}

void Linker::update_estab()
{
    string temp;
    for(auto i: drec_holder)
    {
        if(ESTAB.find(i) != ESTAB.end())
        {
            cout << "Duplicate symbol in DREC! " << i << endl;
            exit(2);
        }
        else
        {
            if (isalpha(i[0]))
            {
                ESTAB[i].control_section = cs;
                temp = i;
            }
            else if(isdigit(i[0]))
                ESTAB[temp].address = CSADDR + stoi(i, nullptr, 16);
        }
    }
}

void Linker::print_estab()
{
    cout << "Symbol"
         << "\t\tCS "
         << "\t\tAddress "
         << "\tLength " << endl;
    for (auto i : ESTAB)
    {
        cout << i.first << "\t\t" << i.second.control_section
             << hex << "\t\t" << i.second.address
             << "\t\t" << i.second.length << endl;
    }
}

void Linker::link(char *f)
{
    infile.open(f);
    pass1();
    infile.open(f);
    pass2();
}