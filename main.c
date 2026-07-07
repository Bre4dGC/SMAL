#include <stdio.h>
#include <string.h>

#include "smal.h"

#define VERSION "1.4.2"

int main(int argc, char** argv)
{
    if(argc < 2) return fprintf(stderr, "Use \"help\" to get list of commands\n");

    if(strcmp(argv[1], "version") == 0){
        printf("SMAL is a simple programming language designed for educational purposes.\n");
        printf("Current version: %s\n", VERSION);
        printf("Latest update: 2026-07-07\n");
        printf("Update message: Implementation of array operations\n");
    }
    else if(strcmp(argv[1], "help") == 0){
        printf("List of available commands:\n");
        printf("    version\tGet information about the language\n");
        printf("    run <f>\tExecute .smal file\n");
        printf("    repl   \tStart interactive interpreter\n");
    }
    else if(strcmp(argv[1], "run") == 0){
        if(argc < 3) return fprintf(stderr, "Provide path to the .smal file\n");
        execute(argv[2]);
    }
    else if(strcmp(argv[1], "repl") == 0){
        repl();
    }
    return 0;
}
