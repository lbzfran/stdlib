#ifndef TERM_H
#define TERM_H
#include <termios.h>
#include <time.h>

typedef struct TermRenderBuf {
    char *b;
    memory_index length;
    memory_index capacity;
} TermRenderBuf;

typedef struct TermSettings {
    uint32 cx, cy;

    uint32 screenRows;
    uint32 screenCols;

    char statusMsg[80];
    uint32 msgLen;
    time_t statusMsgTime;

    bool32 alive;
    struct termios orig_termios;
} TermSettings;


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

#define CTRL_KEY(k) ((k) & 0x1f)

void TermDie(const char *);
void TermSettingsInit(TermSettings *ts);

void TermEnableRawMode(struct termios *);
void TermDisableRawMode(struct termios *);

void TermRenderAppend(Arena *arena, TermRenderBuf *rb, char *c, memory_index len);
void TermRender(Arena *arena, TermSettings *ts);

int TermReadKey(void);
void TermProcessKeypress(TermSettings *ts);

void TermSetStatusMessage(TermSettings *ts, const char *fmt, ...);
void TermDrawMessageBar(Arena *arena, TermSettings *ts, TermRenderBuf *rb);

#endif // TERM_H
