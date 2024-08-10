#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/parser.h"

char* encode_string(char* string) {
    // find all special characters
    // donc alloc, use the old memory
    // replace them with their ascii code

    char* encoded = calloc(strlen(string) + 1, sizeof(char));
    int index = 0;

    for (int i = 0; string[i]; i++) {
        if (string[i] == '\\') {
            i++;
            switch (string[i]) {
                case 'n':
                    encoded[index++] = '\n';
                    break;
                case 't':
                    encoded[index++] = '\t';
                    break;
                case 'r':
                    encoded[index++] = '\r';
                    break;
                case '0':
                    encoded[index++] = '\0';
                    break;
                case '\\':
                    encoded[index++] = '\\';
                    break;
                case '"':
                    encoded[index++] = '"';
                    break;
                default:
                    fprintf(stderr, "Unknown escape sequence: \\%c\n", string[i]);
                    free(encoded);
                    return NULL;
            }
        }
        else {
            encoded[index++] = string[i];
        }
    }

    // zeroes string
    memset(string,0,index+1);
    // copy encoded to string
    memcpy(string, encoded, index);
    free(encoded);
    return string;
}

int* encode_literals(char** litterals, char** encoded_literals) {
    *encoded_literals = calloc(256, sizeof(char));
    int array_memory = 256;
    int index = 0;

    int* addresses = calloc(MAX_LITTERALS, sizeof(int));

    int i;
    for (i = 0; litterals[i]; i++) {
        if (litterals[i][0] == '"') {
            printf("Found string litteral\n");
            int len = strlen(litterals[i]) - 1; // Subtract  for the quotation marks
            if (index + len + 1 > array_memory) {
                array_memory += 256;
                *encoded_literals = realloc(*encoded_literals, array_memory * sizeof(char));
            }
            int addr = index;
            printf("%d : ",i);
            printf("Address: %d\n", addr);
            printf("Litteral: %s\n", litterals[i] + 1);
            printf("Length: %d\n", len);

            litterals[i] = encode_string(litterals[i] + 1);

            memcpy(*encoded_literals + index, litterals[i], len); // Copy the string without the quotation marks
            index += len; // Update the index
            addresses[i] = addr; // Store the address
        }
        else {
            printf("Found numeric litteral\n");
            // litteral is numeric
            int addr = index;
            printf("%d : ",i);
            printf("Address: %d\n", addr);
            printf("Litteral: %s\n", litterals[i]);
            // Convert the string to an integer
            int value = atoi(litterals[i]);
            memcpy(*encoded_literals + index, &value, sizeof(int)); // Copy the integer
            index += sizeof(int); // Update the index
            addresses[i] = addr; // Store the address
        }
    }

    int final_length = index;
    addresses[i] = final_length; // Store the final length

    return addresses;
}

