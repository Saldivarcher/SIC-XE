#ifndef LINKER_H
#define LINKER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
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

class Linker
{
private:
    char mem[10000];
    ifstream infile;
    map<string, estab_data> ESTAB;
    list<string> drec_holder, t_rec;
    int CSLTH, PROGADDR, CSADDR, EXECADDR, start_addr;
    string header, record, whole_record, cs, object_code, saved_header;

    char get_hex(string);
    string insert_zero(string);

    void parse_m_record(string &, string &, string &, string &);
    void memory_object(string &, int);
    void update_mem(string, int);

    void modified_obj_code(int &, string, string, string);

    void read_line();
    void parse_record();
    void parse_t_record();
    void update_estab();
    void print_estab();
    void print_mem();

    void pass1();
    void pass2();

public:
    Linker();
    void link(char *);
};


#endif