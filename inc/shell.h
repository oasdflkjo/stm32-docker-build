#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

#define SHELL_MAX_CMD_LEN 32
#define SHELL_PROMPT "boot> "

void Shell_Init(void);
void Shell_Process(void);

#endif // SHELL_H 