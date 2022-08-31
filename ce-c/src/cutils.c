#include "stdlib.h"
#include "string.h"

char* substr(const char *src, int m, int n)
{
    // get the length of the destination string
    int len = n - m;
 
    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    // extracts characters between m'th and n'th index from source string
    // and copy them into the destination string
    for (int i = m; i < n; i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    // null-terminate the destination string
    *dest = '\0';
 
    // return the destination string
    return dest - len;
}

int str2hex(const char *hexStr,unsigned char *output,unsigned int *outputLen) 
{
    size_t len = strlen(hexStr);
    if (len % 2 != 0) {
        return -1;
    }
    size_t finalLen = len / 2;
    *outputLen = finalLen;
    for (size_t inIdx = 0, outIdx = 0; outIdx < finalLen; inIdx += 2, outIdx++) {
        if ((hexStr[inIdx] - 48) <= 9 && (hexStr[inIdx + 1] - 48) <= 9) {
        goto convert;
        } else {
        if ((hexStr[inIdx] - 65) <= 5 && (hexStr[inIdx + 1] - 65) <= 5) {
            goto convert;
        } else {
            *outputLen = 0;
            return -1;
        }
        }
    convert:
        output[outIdx] =
            (hexStr[inIdx] % 32 + 9) % 25 * 16 + (hexStr[inIdx + 1] % 32 + 9) % 25;
    }
    output[finalLen] = '\0';
    return 0;
}