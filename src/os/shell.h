#ifndef SHELL_H
#define SHELL_H

#include "base/base.h"

#define SHELL_LINE_BUFSIZE 1024
#define SHELL_DELIM "\t\r\n\a "

char *ShellReadLine(Arena *arena, char *prompt);

char **ShellParseLine(char *line);
StringList ShellParseStringList(Arena *arena, char *line);

void ShellExecLine(char **args);
void ShellExecStringList(Arena *arena, StringList args);

#endif // SHELL_H
