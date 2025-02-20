#ifndef TERM_H
#define TERM_H
#include <termios.h>

void TermDie(const char *);

void TermEnableRawMode(struct termios *);
void TermDisableRawMode(struct termios *);

#endif // TERM_H
