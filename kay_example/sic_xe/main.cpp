#include "assembler.h"

using namespace std;

int main(int argc, char** argv)
{
    Assembler a;
    if(argc != 2)
    {
        cout << "Try again. How to run:\nassemble filename.as\n";
        exit(1);
    }
    a.run(argv[1]);
}
