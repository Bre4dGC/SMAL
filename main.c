#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "smalc.h"

#define VERSION "1.0"

int main(int argc, char** argv)
{
    if(argc < 2) return fprintf(stderr, "Use \"help\" to get list of commands\n");

    if(strcmp(argv[1], "version") == 0){
        printf("SMAL is a simple programming language designed for educational purposes.\n");
        printf("Current version: %s\n", VERSION);
        printf("Latest update: 2026-06-26\n");
        printf("Update message: Implement basic SMAL interpreter functionality.\n");
    }
    else if(strcmp(argv[1], "help") == 0){
        printf("List of available commands:\n");
        printf("    version\tGet information about the language\n");
        printf("    run <f>\tExecute .smal file\n");
    }
    else if(strcmp(argv[1], "run") == 0){
        if(argc < 3) return fprintf(stderr, "Provide path to the .smal file\n");

        FILE* file = fopen(argv[2], "r");
        if(file == NULL) return fprintf(stderr, "Could not open file: %s\n", argv[2]);

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* buffer = malloc(length + 1);
        if(buffer == NULL) return fprintf(stderr, "Memory allocation failed\n");

        if(fread(buffer, 1, length, file) != length) return fprintf(stderr, "Could not read file: %s\n", argv[2]);
        buffer[length] = '\0';

        execute(buffer);

        free(buffer);
        fclose(file);
    }
    return 0;
}