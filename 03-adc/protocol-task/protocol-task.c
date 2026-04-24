#include "protocol-task.h"
#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"


static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* device_api){
    api = device_api;
    
    if (api != 0){
        while (api[commands_count].command_name != 0){
            commands_count ++;
        }
    } 
}

void protocol_task_handle(char* command_string){
    
    if (!command_string)
    {    
        return;
    }

    // логика обработки полученной строки. Делим ее на команду и аргументы:
    const char* command_name = command_string;
    const char* command_args = NULL;

    char* space_symbol = strchr(command_string, ' ');

    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        command_args = "";
    }

    printf("command: '%s', arguments: %s\n", command_name, command_args, "\n");

  
// в цикле проходим по массиву команд `api` и ищем совпадение имени команды;

    for (int i = 0; i < commands_count; i++)
    {
        if (strcmp(command_name, api[i].command_name) == 0){
            api[i].command_callback(command_args);
            return;
        }
        
        else { 
            continue;
        }
    }
    printf("ERROR: no command with this name found \n");
    return;
}



void mem_prot(uint32_t addr){
    uint32_t value = *(uint32_t*)addr;
    printf("Value at 0x%08X: 0x%08X\n", addr, value);
}


void wmem_prot(uint32_t addr, uint32_t value){
    *(uint32_t*)addr = value;
    printf("Written 0x%08X to address 0x%08X\n", value, addr);
}