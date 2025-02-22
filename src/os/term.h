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
    int cx, cy;
    int prx, pry;

    int screenRows;
    int screenCols;

    char statusMsg[80];
    uint32 msgLen;
    time_t statusMsgTime;

    bool32 alive;
    StringData filename;
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
void TermProcessKeypress(Arena *arena, TermSettings *ts);
char *TermPrompt(Arena *arena, TermSettings *ts, char *prompt, void (*callback)(char *, int));

void TermSetStatusMessage(TermSettings *ts, const char *fmt, ...);
void TermDrawMessageBar(Arena *arena, TermSettings *ts, TermRenderBuf *rb);

#endif // TERM_H
