#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <iomanip>
#include <algorithm>

using namespace std;

class estab_data
{
public:
    string control_section;
    int address;
    int length;

    estab_data():
        control_section(""), address(0), length(0){}

    estab_data(string cs, int a, int l):
        control_section(cs), address(a), length(l){}
};


void read_line(ifstream &in, string &header, string &record, string &cs, int &cslen)
{
    getline(in, header);
    record = header;
    if (header[0] == 'H')
    {
        cslen = stoi(header.substr(13), nullptr, 16);
        header = header.substr(1, 6);
        header.erase(remove_if(header.begin(), header.end(),
                               [](char x) { return isspace(x); }),
                               header.end());
        cs = header;
    }
    else
        header = "";
}

void parse_record(string record, list<string> &l)
{
    record = record.substr(1);
    string temp;
    for (auto i : record)
    {
        temp += i;
        if(temp.size() == 6){
            temp.erase(remove_if(temp.begin(), temp.end(),
                                 [](char x) { return isspace(x); }),
                                 temp.end());
            l.push_back(temp);
            temp.clear();
        }
    }
}

void parse_t_record(string record, list<string> &l, int &start_addr)
{
    // getting the address from the T record
    start_addr = stoi(record.substr(1, 6), nullptr, 16);
    record = record.substr(9);
    string temp;
    for (auto i : record)
        temp += i;
    l.push_back(temp);
}

void parse_m_record(string record, string &sym, string &location, string &length, string& sign)
{
    location = record.substr(1, 6);
    length = record.substr(7, 2);
    record = record.substr(1);
    sym = record.substr(9);
    sign = record.substr(8, 1);
}

void print_mem(char mem[])
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

string insert_zero(string t)
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

char get_hex(string s)
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

int main(int argv, char **argc)
{
    map<string, estab_data> ESTAB;
    list<string> drec_holder;
    char mem[10000];
    memset(mem, '.', sizeof mem);

    int CSLTH;
    int PROGADDR = 0x4000;
    int CSADDR = PROGADDR;

    // Pass 1
    ifstream infile;
    infile.open(argc[1]);

    string header, record, whole_record, cs;
    while (!infile.eof())
    {
        read_line(infile, header, record, cs, CSLTH);
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
        if (record[0] != 'E')
        {
            if (record[0] == 'D')
            {
                parse_record(record, drec_holder);
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
        }
        // clearing the list that holds D records
        if(record[0] == 'E')
            CSADDR += CSLTH;
        drec_holder.clear();
    }

    infile.close();
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
    
    // Pass 2
    CSADDR = PROGADDR;
    int EXECADDR = PROGADDR;
    int start_addr = 0;

    list<string> t_rec;

    string object_code, saved_header;
    infile.open(argc[1]);
    while (!infile.eof())
    {
        read_line(infile, header, record, cs, CSLTH);
        object_code = record;
        if (header != ""){
            CSLTH = ESTAB[header].length;
            saved_header = header;
        }
        if (record[0] != 'E')
        {
            if (record[0] == 'T')
            {
                parse_t_record(object_code, t_rec, start_addr);
                // this is where you link!!
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
            if (record[0] == 'M')
            {
                string symbol, location, length, sign;
                parse_m_record(object_code, symbol, location, length, sign);
                if (ESTAB.find(symbol) != ESTAB.end())
                {

                    int int_location = stoi(location, nullptr, 16);
                    int int_length = stoi(length, nullptr, 16);
                    int actual_location = (int_location + ESTAB[saved_header].address) - 0x4000;

                    // should apply to m records that use extended format
                    if(int_length == 5)
                    {
                        string t;
                        for (int i = actual_location; i < actual_location + 3; i++)
                        {
                            stringstream ss;
                            ss << hex << (int)mem[i];
                            string s(ss.str());
                            if(s.size() == 1)
                            {
                                s.insert(0, 1, '0');
                            }
                            t += s;
                        }

                        if(t.size() > 6)
                            t = t.substr(0, 6);

                        int modified;
                        if (sign[0] == '+')
                            modified = stoi(t, nullptr, 16) + ESTAB[symbol].address;
                        else
                            modified = stoi(t, nullptr, 16) - ESTAB[symbol].address;
                        stringstream ss;
                        ss << hex << modified;
                        t = ss.str();
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
                    else
                    {
                        string t;
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
                        int modified;
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
                        stringstream ss;
                        ss << hex << modified;
                        t = ss.str();
                        t = insert_zero(t);
                        string temp;
                        for (auto i : t)
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
    print_mem(mem);
}
