#include "./interpreter.h"
#include "./shell.h"
#include "./stdio.h"
#include "mod/other_utils/str_utils.h"

char command_buffer[50];
int command_buffer_index = 0;

void echo()
{
    int i = 6;
    while(command_buffer[i] != '"')
    {
        if(i >= 50)
        {
            break;
        }
        char temp[2] = {command_buffer[i], '\0'};
        kprintf(temp);
        ++i;
    }
    kprintf("\n");
}

void clear()
{
    terminal_clear();
}

void help()
{
    kprintf("Commands Available:\n");
    kprintf("echo - Echo a string to stdout\n");
    kprintf("clear - Clear the terminal output\n");
    kprintf("help - Display list of available commands\n");
}

void execute_command()
{
    //command search
    if(strcmp(command_buffer, "echo", 4))
    {
        echo();
    }
    else if(strcmp(command_buffer, "clear", 5))
    {
        clear();
    }
    else if(strcmp(command_buffer, "help", 4))
    {
        help();
    }
    else
    {
        kprintf("Command not found.\n");
    }
    command_buffer_index = 0;
    kprintf("root > ");
    current_io_state = stdin_command;
}
