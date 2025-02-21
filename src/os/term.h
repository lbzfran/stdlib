#ifndef TERM_H
#define TERM_H
#include <termios.h>

typedef struct TermRenderBuf {
    uint8 *b;
    memory_index length;
    memory_index capacity;
} TermRenderBuf;

enum termKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    CTRL_ARROW_LEFT,
    CTRL_ARROW_RIGHT,
    CTRL_ARROW_UP,
    CTRL_ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,
};

void TermDie(const char *);

void TermEnableRawMode(struct termios *);
void TermDisableRawMode(struct termios *);

void TermRenderAppend(Arena *arena, TermRenderBuf *rb, uint8 *c, memory_index len);
void TermRender(Arena *arena);

#endif // TERM_H
