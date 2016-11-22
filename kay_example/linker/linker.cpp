#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <list>
#include <iomanip>

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
        cs = header;
    }
    else
        header = "";
}

void parse_record(string record, list<string> &v)
{
    record = record.substr(1);
    string temp;
    for (auto i : record)
    {
        temp += i;
        if(temp.size() == 6){
            v.push_back(temp);
            temp.clear();
        }
    }
}

void parse_t_record(string record, list<string> &v, int &start_addr)
{
    // getting the address from the T record
    start_addr = stoi(record.substr(1, 6), nullptr, 16);
    record = record.substr(9);
    string temp;
    for (auto i : record)
        temp += i;
    v.push_back(temp);
}

void parse_m_record(string record)
{
    return;
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
         << setw(7) << "CS "
         << setw(12) << "Address "
         << setw(5) << "Length " << endl;
    for (auto i : ESTAB)
    {
        cout << i.first << setw(10) << i.second.control_section
             << hex << setw(5) << i.second.address
             << setw(5) << i.second.length << endl;
    }
    
    // Pass 2
    CSADDR = PROGADDR;
    int EXECADDR = PROGADDR;
    int start_addr = 0;
    int test = 0;

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
        if(record[0] != 'E')
        {
            if (record[0] == 'T')
            {
                parse_t_record(object_code, t_rec, start_addr);
                // this is where you link!!
                for (auto i : t_rec)
                {
                    for (auto j : i)
                    {
                        mem[(ESTAB[saved_header].address + start_addr++) - 0x4000] = j;
                    }
                }
            }
            if(record[0] == 'M')
            {
                parse_m_record(object_code);
            }
            t_rec.clear();
        }
    }

    cout << endl;
    for (int i = 0; i < 0x150; i++)
    {
        if(i == 0)
            cout << i + 0x4000 << " ";
        if(i % 16 == 0 and i != 0)
            cout << endl << i + 0x4000 << " ";
        cout << mem[i];
    }
    cout << endl;
}
