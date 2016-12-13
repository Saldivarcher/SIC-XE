#include "linker.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "You need to have an argument, which should be a sic_xe object file\n";
        exit(1);
    }

    Linker l;
    l.link(argv[1]);
}
