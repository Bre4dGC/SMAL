#include <stdio.h>
#include <string.h>

#include "smalc.h"

#define VERSION "2.1"

int main(int argc, char** argv)
{
    if(argc < 2) return fprintf(stderr, "Use \"help\" to get list of commands\n");

    if(strcmp(argv[1], "version") == 0){
        printf("SMAL is a simple programming language designed for educational purposes.\n");
        printf("Current version: %s\n", VERSION);
        printf("Latest update: 2026-06-29\n");
        printf("Update message: Implement full functionality.\n");
    }
    else if(strcmp(argv[1], "help") == 0){
        printf("List of available commands:\n");
        printf("    version\tGet information about the language\n");
        printf("    run <f>\tExecute .smal file\n");
    }
    else if(strcmp(argv[1], "run") == 0){
        if(argc < 3) return fprintf(stderr, "Provide path to the .smal file\n");
        execute(argv[2]);
    }
    return 0;
}
