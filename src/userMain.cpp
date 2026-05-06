#include "../h/userMain.hpp"
#include "../h/syscall_c.hpp"
#include "../h/print.hpp"

void userMain()
{
    putc('a');
    putc('\n');
    printStringA("Broj: ");
    printIntA(3);
    putc('\n');
}
