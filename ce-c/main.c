#include "stdio.h"
#include "stdlib.h"

#include "globals.h"
#include "main.h"
#include "structs.h"
#include "cutils.h"
#include <string.h>



int main(int argc, char *argv[])
{

    init();
    

    struct scope* scopes;
    char* code;
    unsigned int scope_count = parser(argv[1],&scopes,&code);

    for (int i = 0; i < scope_count+1; i++)
    {
        //printf("printing scope %d\n", i);
        for (int j = 0; j < scopes[i].c_lines; j++)
        {
            // parse line
            if (scopes[i].lines[j].start == scopes[i].lines[j].end) {
                //printf("empty line\n");
                continue;
                }

                char *line = substr(code, scopes[i].lines[j].start, scopes[i].lines[j].end);


            if (line[0] == '$')
            {
                printf("detected variable\n");
                // name = line before '='
                // value = line after '='
                char* name = substr(line, 1, strchr(line, '=')-line);
                char* value = substr(line, strchr(line, '=')-line+1, strlen(line));
                printf("name: %s\n", name);
                printf("value: %s\n", value);
                set_var(name, (unsigned char*)value, strlen(value));
                continue;
            }
            else {
                printf("detected function\n");
                // name = line before '('
                // args = line between '(' and ')'
                // code = line after ')'
                char* name = substr(line, 0, strchr(line, '(')-line);
                char* args = substr(line, strchr(line, '(')-line+1, strchr(line, ')')-line);
                char* code = substr(line, strchr(line, ')')-line+1, strlen(line));
                printf("name: %s\n", name);
                printf("args: %s\n", args);
                printf("code: %s\n", code);
                // split args by ','
                char** argv = (char**)calloc(128, sizeof(char*));
                unsigned int argc = 0;
                for (int i = 0; i < strlen(args); i++)
                {
                    if (args[i] == ',')
                    {
                        argc++;
                        continue;
                    }
                    argv[argc] = (char*)realloc(argv[argc], sizeof(char) * (strlen(argv[argc]) + 1));
                    argv[argc][strlen(argv[argc])] = args[i];
                }
                argc++;
                // get the addresses of the args
                unsigned char** argaddr = (unsigned char**)calloc(argc, sizeof(unsigned char*));
                for (int i = 0; i < argc; i++)
                {
                    unsigned char *addr;
                    get_var(argv[i], &addr);
                    argaddr[i] = addr;
                }
                // create the function
                def_function(name, scopes[i], argv, argc, argaddr, code);
            }
        }

    }
    return 0;
}
