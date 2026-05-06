
#include "../h/print.hpp"

uint64 lockPrintA = 0;

#define LOCK() while(copy_and_swap(lockPrintA, 0, 1)) thread_dispatch()
#define UNLOCK() while(copy_and_swap(lockPrintA, 1, 0))

void printStringA(char const *string)
{
    LOCK();
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
    UNLOCK();
}

char* getStringA(char *buf, int max) {
    LOCK();
    int i, cc;
    char c;

    for(i=0; i+1 < max; ){
        cc = getc();
        if(cc < 1)
            break;
        c = cc;
        buf[i++] = c;
        if(c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';

    UNLOCK();
    return buf;
}

int stringToIntA(const char *s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

char digitsA[] = "0123456789ABCDEF";

void printIntA(int xx, int base, int sgn)
{
    LOCK();
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0){
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do{
        buf[i++] = digitsA[x % base];
    }while((x /= base) != 0);
    if(neg)
        buf[i++] = '-';

    while(--i >= 0)
        putc(buf[i]);

    UNLOCK();
}