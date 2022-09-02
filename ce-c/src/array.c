#include "stdlib.h"
#include "stdio.h"

#include "../include/globals.h"

int arrchr(const byte *src, byte c,unsigned int size)
{
    for (int i = 0; i < size; i++)
    {
        if (*(src + i) == c)
        {
            return i;
        }
    }
    return -1;
}

byte* subarr(const byte* src,unsigned int start ,unsigned int end)
{
    // get the length of the destination string
    int len = end - start;
 
    // allocate (len + 1) chars for destination (+1 for extra null character)
    byte* dest = (byte*)malloc(sizeof(char) * (len));
 
    // extracts characters between m'th and n'th index from source string
    // and copy them into the destination string
    for (int i = start; i < end; i++)
    {
        *dest = *(src + i);
        dest++;
    }
    // addterminating null character
    *dest = '\0';
 
    // return the destination string
    return dest - len;
}

int arrcmp(const byte* src, const byte* dest,unsigned int size)
{
    for (int i = 0; i < size; i++)
    {
        if (*(src + i) != *(dest + i))
        {
            return 0;
        }
    }
    return 1;
}