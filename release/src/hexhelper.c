#include "hexhelper.h"

void stringn2hexstring(char* input, char* output, size_t length)
{
    int loop;
    int i;

    i = 0;
    loop = 0;

    for (i = 0; i < length; i++) {
        sprintf((char*)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}
void string2hexstring(char* input, char* output)
{
    int loop;
    int i;

    i = 0;
    loop = 0;

    while (input[loop] != '\0') {
        sprintf((char*)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}
