#include "globals.h"
#include "string.h"

int set_var(char*name, unsigned char* value, unsigned int size)
{
    VAR_TABLE[VAR_COUNT].name = name;
    VAR_TABLE[VAR_COUNT].val = value;
    VAR_TABLE[VAR_COUNT].size = size;
    VAR_COUNT++;
    return 0;
}
int get_var(char*name, unsigned char** value)
{
    unsigned int size;
    for (int i = 0; i < VAR_COUNT; i++) {
        if (strcmp(VAR_TABLE[i].name, name) == 0) {
            *value = VAR_TABLE[i].val;
            size = VAR_TABLE[i].size;
            return size;
        }
    }
    return -1;
}
int mod_var(char*name, unsigned char* value, unsigned int size)
{
    struct var new;
    new.name = name;
    new.val = value;
    new.size = size;

    for (int i = 0; i < VAR_COUNT; i++) {
        if (strcmp(VAR_TABLE[i].name, name) == 0) {
            VAR_TABLE[i] = new;
            return 0;
        }
    }
    return -1;
}