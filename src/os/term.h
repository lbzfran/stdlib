#ifndef TERM_H
#define TERM_H

#include "base/base.h"

#include <time.h>

#if defined(OS_WINDOWS)
#include <Windows.h>
struct termios {
    DWORD InMode;
    DWORD OutMode;
};
#else
# include <termios.h>
#endif

// NOTE(liam): the full buffer gets written by TermRender.
typedef struct TermRenderBuf {
    char *b;
    memory_index length;
    memory_index capacity;
} TermRenderBuf;

typedef struct TermBuf {
    char *b;
} TermBuf;

#define TERM_HISTORY_SIZE 2048

typedef struct TermKeyHistory {
    int buf[2048];
} TermKeyHistory;

typedef struct TermSettings {
    int cx, cy;
    int prx, pry; // prompt pos

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

// NOTE(liam): misc functions
void TermDie(const char *);
void TermSettingsInit(TermSettings *ts);
void TermEnableRawMode(struct termios *);
void TermDisableRawMode(struct termios *);

// NOTE(liam): render functions
void TermRenderAppend(Arena *arena, TermRenderBuf *rb, char *c, memory_index len);
void TermRender(Arena *arena, TermSettings *ts);
void TermSetStatusMessage(TermSettings *ts, const char *fmt, ...);
void TermDrawStatusBar(Arena *arena, TermSettings *ts, TermRenderBuf *rb);
void TermDrawMessageBar(Arena *arena, TermSettings *ts, TermRenderBuf *rb);

// NOTE(liam): input functions
int TermReadKey(void);
void TermProcessKeypress(Arena *arena, TermSettings *ts);

// TODO(liam): interact with shell execution
char *TermPrompt(Arena *arena, TermSettings *ts, char *prompt, void (*callback)(char *, int));

// NOTE(liam): ...

#endif // TERM_H
