
#include "base/base.h"
#include "os/os.h"

#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "gap.h"

#define WEISS_DISPLAY_DIRT_COUNTER 1

#undef AssertBreak
#define AssertBreak(c) { fprintf(stderr, "%s:%d: failed assertion '%s'\n.", __FILE__, __LINE__, #c); TermDisableRawMode(); exit(1); }

enum editorKey {
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

typedef struct erow {
    int idx;
    int size;
    int rsize;
    char *chars;
    char *render;
    unsigned char *hl;
    int hl_open_comment;
} erow;

struct editorConfig {
    int cx, cy;
    int rx; // actual x of gap buffer.
    int px;
    int rowoff;
    int coloff;

    int screenRows; // total rows available
    int screenCols;
    int numRows;
    erow *row;

    int dirty;
    int mode;
    StringData filename;
    char statusMsg[80];
    time_t statusMsgTime;
    struct termios orig_termios;
    bool32 alive;
};

// TODO(liam): do

static struct editorConfig E;

#define CTRL_KEY(k) ((k) & 0x1f)
int editorReadKey(void);
int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow *row, int rx);
void editorRefreshScreen(Arena *arena);
void editorUpdateRow(erow *row);

/*** append buf start ***/

struct abuf {
    char *b;
    uint32 length;
    uint32 capacity;
};

#define ABUF_INIT {NULL, 0}

void abAppend(Arena *arena, struct abuf *ab, char *s, uint32 len)
{
    if (ab->length + len > ab->capacity)
    {
        uint32 newCap = Max(ab->length + len, ab->capacity * 2);
        char *new = PushArray(arena, char, newCap);
        for (uint32 i = 0; i < ab->length; i++)
        {
            *(new + i) = *(ab->b + i);
        }
        ab->b = new;
    }

    /*MemoryCopy(&ab->b[ab->length], s, len);*/
    for (uint32 i = 0; i < len; i++)
    {
        *(ab->b + ab->length + i) = *(s + i);
    }
    ab->length += len;
}

/*** append buf end ***/

void editorDrawRows(Arena *arena, struct abuf *ab)
{
    int y;
    for (y = 0; y < E.screenRows; y++)
    {
        int filerow = y + E.rowoff;
        if (filerow >= E.numRows) {
            if (E.numRows == 0 && y == E.screenRows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                        "weiss editor -- version %s", "1.0");
                if (welcomelen > E.screenCols)
                {
                    welcomelen = E.screenCols;
                }

                int padding = (E.screenCols - welcomelen) / 2;
                if (padding)
                {
                    abAppend(arena, ab, "~", 1);
                    padding--;
                }
                while (padding--) abAppend(arena, ab, " ", 1);

                abAppend(arena, ab, welcome, welcomelen);
            }
            else
            {
                /*write(STDOUT_FILENO, "~", 1);*/
                abAppend(arena, ab, "~", 1);
            }
        }
        else
        {
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0) { len = 0; }
            if (len > E.screenCols) { len = E.screenCols; }
        }

        // NOTE(liam): Erase inline.
        abAppend(arena, ab, "\x1b[K", 3);
        /*write(STDOUT_FILENO, "\r\n", 2);*/
        abAppend(arena, ab, "\r\n", 2);
    }
}

void editorSetStatusMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.statusMsg, sizeof(E.statusMsg), fmt, ap);
    va_end(ap);
    E.statusMsgTime = time(NULL);
}

int getScreenCenter(void)
{
    int center = E.cy - E.screenRows / 2;
    if (center < 0)
    {
        center = 0;
    }
    else if (center >= E.numRows - E.screenRows)
    {
        center = E.numRows - E.screenRows;
    }
    return center;
}

char *editorPrompt(Arena *arena, char *prompt, void (*callback)(char *, int))
{
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1)
    {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen(arena);

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
        {
            if (buflen != 0) { buf[--buflen] = '\0'; }
        }
        else if (c == '\x1b')
        {
            editorSetStatusMessage("");
            if (callback) { callback(buf, c); }
            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buflen != 0)
            {
                editorSetStatusMessage("");
                if (callback) { callback(buf, c); }
                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buflen == bufsize - 1)
            {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }

        if (callback) { callback(buf, c); }
    }
}

void editorFindCallback(char *query, int key)
{
    static int last_match = -1;
    static int direction = 1;

    static int saved_hl_line;
    static char *saved_hl = NULL;

    if (saved_hl)
    {
        memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize);
        free(saved_hl);
        saved_hl = NULL;
    }

    if (key == '\r' || key == '\x1b')
    {
        last_match = -1;
        direction = 1;
        return;
    }
    else if (key == ARROW_RIGHT || key == ARROW_DOWN)
    {
        direction = 1;
    }
    else if (key == ARROW_LEFT || key == ARROW_UP)
    {
        direction = -1;
    }
    else
    {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1) { direction = 1; }
    int current = last_match;
    int i;
    for (i = 0; i < E.numRows; i++)
    {
        current += direction;
        if (current == -1) { current = E.numRows - 1; }
        else if (current == E.numRows) { current = 0; }

        erow *row = &E.row[current];
        char *match = strstr(row->render, query);
        if (match)
        {
            last_match = current;
            E.cy = current;
            E.cx = editorRowRxToCx(row, match - row->render);
            E.rowoff = getScreenCenter();

            saved_hl_line = current;
            saved_hl = malloc(row->rsize);
            memcpy(saved_hl, row->hl, row->rsize);

            /*memset(&row->hl[match - row->render], HL_MATCH, strlen(query));*/
            break;
        }
    }
}

void editorFind(Arena *arena)
{
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;

    char *query = editorPrompt(arena, "Search: %s (ESC/Arrows/Enter)", editorFindCallback);
    if (query)
    {
        free(query);
    }
    else
    {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
    }
}

int editorRowCxToRx(erow *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
        {
            rx += (4 - 1) - (rx % 4);
        }
        rx++;
    }
    return rx;
}

int editorRowRxToCx(erow *row, int rx)
{
    int cur_rx = 0;
    int cx;

    for (cx = 0; cx < row->size; cx++)
    {
        if (row->chars[cx] == '\t')
        {
            cur_rx += (4 - 1) - (cur_rx % 4);
        }
        cur_rx++;

        if (cur_rx > rx) { return cx; }
    }
    return cx;
}

void editorIndentDown() {
    if (E.cy >= E.numRows) return;
    erow *row = &E.row[E.cy];
    if (row->size == 0) return;  // Nothing to unindent.

    int removeCount = 0;
    if (1) {
        // Count up to WEISS_TAB_STOP leading spaces.
        for (int i = 0; i < 4 && i < row->size; i++) {
            if (row->chars[i] == ' ') {
                removeCount++;
            } else {
                break;
            }
        }
    } else {
        // If not using spaces, remove a tab if present.
        if (row->chars[0] == '\t')
            removeCount = 1;
    }
    if (removeCount == 0) return;  // No indent found.

    // Remove the indent by shifting the rest of the row left.
    memmove(row->chars, row->chars + removeCount, row->size - removeCount + 1); // include null terminator
    row->size -= removeCount;
    editorUpdateRow(row);

    // Adjust the cursor: reduce E.cx by removeCount, ensuring it doesn't go negative.
    if (E.cx >= removeCount) {
        E.cx -= removeCount;
    } else {
        E.cx = 0;
    }
    E.dirty++;
}

void editorIndentUp() {
    if (E.cy >= E.numRows) return;
    erow *row = &E.row[E.cy];

    // Determine indent string and length.
    int indentSize = 4;  // Number of spaces if using spaces.
    char indentStr[4 + 1];
    if (1) {
        memset(indentStr, ' ', indentSize);
    } else {
        // If not using spaces, just insert a tab.
        indentStr[0] = '\t';
        indentSize = 1;
    }
    indentStr[indentSize] = '\0';

    // Reallocate to make room for the indent.
    row->chars = realloc(row->chars, row->size + indentSize + 1);
    // Shift existing characters to the right.
    memmove(row->chars + indentSize, row->chars, row->size + 1); // include null terminator
    // Copy the indent string into the beginning.
    memcpy(row->chars, indentStr, indentSize);
    row->size += indentSize;
    editorUpdateRow(row);

    // Adjust the cursor: if not at the beginning, shift it right.
    E.cx += indentSize;
    E.dirty++;
}

void editorUpdateRow(erow *row)
{
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t') { tabs++; }
    }

    free(row->render);
    row->render = malloc(row->size + tabs * (4 - 1) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = '%';
            while (idx % 4 != 0) { row->render[idx++] = ' '; }
        }
        else
        {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;

    /*editorUpdateSyntax(row);*/
}

void editorInsertRow(int at, char *s, size_t len)
{
    if (at < 0 || at > E.numRows) { return; }

    E.row = realloc(E.row, sizeof(erow) * (E.numRows + 1));
    memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numRows - at));
    for (int j = at + 1; j <= E.numRows; j++) { E.row[j].idx++; }

    E.row[at].idx = at;

    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    E.row[at].hl = NULL;
    E.row[at].hl_open_comment = 0;
    editorUpdateRow(&E.row[at]);

    E.numRows++;
    E.dirty++;
}

void editorMoveCursorParagraphUp()
{
    if (E.cy <= 0) return;  // Already at the top.

    int target = E.cy;
    // Search upward for a blank row.
    for (int i = E.cy - 1; i >= 0; i--) {
        if (E.row[i].size == 0) {
            // Found a blank line. Now, jump to the first non-blank row before it.
            for (int j = i - 1; j >= 0; j--) {
                if (E.row[j].size != 0) {
                    target = j;
                    break;
                }
            }
            // If no non-blank row exists before the blank line, stay at the blank.
            if (target == E.cy) target = i;
            break;
        }
        // If no blank line is encountered, simply go to the top.
        target = i;
    }

    E.cy = target;
    int rowlen = (E.cy < E.numRows) ? E.row[E.cy].size : 0;
    if (E.cx > rowlen) E.cx = rowlen;
}



// Jump to the next paragraph: search downward for a blank line and then to the
// first non-blank row after that blank.
void editorMoveCursorParagraphDown()
{
    if (E.cy >= E.numRows - 1) return;  // Already at the bottom.

    int target = E.cy;
    // Search downward for a blank row.
    for (int i = E.cy + 1; i < E.numRows; i++) {
        if (E.row[i].size == 0) {
            // Found a blank line. Now, jump to the first non-blank row after it.
            for (int j = i + 1; j < E.numRows; j++) {
                if (E.row[j].size != 0) {
                    target = j;
                    break;
                }
            }
            // If no non-blank row exists after the blank line, stay at the blank.
            if (target == E.cy) target = i;
            break;
        }
        // If no blank line is encountered, simply go to the bottom.
        target = i;
    }

    E.cy = target;
    int rowlen = (E.cy < E.numRows) ? E.row[E.cy].size : 0;
    if (E.cx > rowlen) E.cx = rowlen;
}

void editorFreeRow(erow *row)
{
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void editorDelRow(int at)
{
    if (at < 0 || at >= E.numRows) { return; }
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numRows - at - 1));
    for (int j = at; j < E.numRows - 1; j++) { E.row[j].idx--; }
    E.numRows--;
    /*E.dirty++;*/
}

void editorRowInsertChar(erow *row, int at, int c)
{
    if (at < 0 || at > row->size) { at = row->size; }
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDelChar(erow *row, int at)
{
    if (at < 0 || at >= row->size) { return; }
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(row);
    E.dirty++;
}

void editorScroll()
{
    E.rx = 0;
    if (E.cy < E.numRows)
    {
        E.rx = E.cx;
    }

    if (E.cy < E.rowoff)
    {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenRows)
    {
        E.rowoff = E.cy - E.screenRows + 1;
    }
    if (E.rx < E.coloff)
    {
        E.coloff = E.cx;
    }
    if (E.rx >= E.coloff + E.screenCols)
    {
        E.coloff = E.cx - E.screenCols + 1;
    }
}

void editorDrawStatusBar(Arena *arena, struct abuf *ab)
{
    abAppend(arena, ab, "\x1b[7m", 4);
    char status[80], rstatus[80], dirtstatus[6];

    int dirtlen = snprintf(dirtstatus, sizeof(dirtstatus), "[%d]", E.dirty < 999 ? E.dirty : 999);

    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                       E.filename.size ? (char *)StringLiteral(E.filename) : "[.]", E.numRows,
                       WEISS_DISPLAY_DIRT_COUNTER ? (E.dirty && dirtlen ? dirtstatus : "") :
                       (E.dirty ? "[+]" : ""));
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d:%d",
                        E.cy + 1, E.cx + 1);
    if (len > E.screenCols) { len = E.screenCols; }
    abAppend(arena, ab, status, len);
    while (len < E.screenCols)
    {
        if (E.screenCols - len == rlen)
        {
            abAppend(arena, ab, rstatus, rlen);
            break;
        }
        else
        {
            abAppend(arena, ab, " ", 1);
            len++;
        }
    }
    abAppend(arena, ab, "\x1b[m", 3);
    abAppend(arena, ab, "\r\n", 2);
}

void editorDrawMessageBar(Arena *arena, struct abuf *ab)
{
    abAppend(arena, ab, "\x1b[K", 3);
    int msglen = strlen(E.statusMsg);
    if (msglen > E.screenCols) { msglen = E.screenCols; }
    if (msglen && time(NULL) - E.statusMsgTime < 5)
    {
        abAppend(arena, ab, E.statusMsg, msglen);
    }
}

void editorRefreshScreen(Arena *arena)
{
    editorScroll();
    ArenaTemp tmp = ArenaScratchCreate(arena);
    struct abuf ab = ABUF_INIT;

    /*write(STDOUT_FILENO, "\x1b[2J", 4);*/
    /*write(STDOUT_FILENO, "\x1b[H", 3);*/
    /*abAppend(arena, &ab, "\x1b[2J", 4);*/
    /*abAppend(arena, &ab, "\x1b[H", 3);*/
    abAppend(arena, &ab, "\x1b[?25l", 6);
    /*abAppend(&ab, "\x1b[2J", 4);*/

    /*abAppend(arena, &ab, (gb.buf + gb.right), gb.capacity);*/
    editorDrawRows(arena, &ab);
    editorDrawStatusBar(arena, &ab);
    editorDrawMessageBar(arena, &ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                              (E.rx - E.coloff) + 1);
    abAppend(arena, &ab, buf, StringLength((uint8 *)buf));
    abAppend(arena, &ab, "\x1b[?25h", 6);
    /*abAppend(arena, &ab, "\x1b[H", 3);*/

    write(STDOUT_FILENO, ab.b, ab.length);
    ArenaScratchFree(tmp);
}

int getCursorPosition(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    {
        return -1;
    }

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
        {
            break;
        }
        if (buf[i] == 'R')
        {
            break;
        }
        i++;
    }

    if (buf[0] != '\x1b' || buf[1] != '[')
    {
        return -1;
    }
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
    {
        return -1;
    }

    return 0;
}

void editorMoveCursor(int key)
{
    erow *row = (E.cy >= E.numRows) ? NULL : &E.row[E.cy];

    switch (key)
    {
        case CTRL_ARROW_UP:
            {
                editorMoveCursorParagraphUp();
            } break;
        case CTRL_ARROW_DOWN:
            {
                editorMoveCursorParagraphDown();
            } break;
        case CTRL_ARROW_LEFT:
            {
                if (E.cy >= E.numRows) { break; }
                row = &E.row[E.cy];
                if (E.cx == 0)
                {
                    if (E.cy > 0)
                    {
                        E.cy--;
                        row = &E.row[E.cy];
                        E.cx = row->size;
                    }
                    break;
                }

                int pos = E.cx - 1;
                while (pos > 0 && (row->chars[pos] == ' ' || row->chars[pos] == '\t'))
                { pos--; }

                while (pos > 0 && (row->chars[pos] != ' ' && row->chars[pos] != '\t'))
                { pos--; }

                if (row->chars[pos] == ' ' || row->chars[pos] == '\t') { pos++; }

                /*if (pos < 0)*/
                /*{*/
                /*    E.cx = 0;*/
                /*    break;*/
                /*}*/
                /**/
                /*int wordStart = pos;*/
                /*while (wordStart >= 0 && (row->chars[wordStart] != ' ' && row->chars[wordStart] != '\t'))*/
                /*{ wordStart--; }*/
                /**/
                /*// Now, to jump to the end of the word, iterate forward from wordStart.*/
                /*int wordEnd = wordStart + 1;*/
                /*while (wordEnd < row->size && (row->chars[wordEnd] != ' ' && row->chars[wordEnd] != '\t'))*/
                /*{ wordEnd++; }*/
                /**/
                /*// Set the cursor to the end of the word (i.e. just past the last character).*/
                /*E.cx = wordEnd;*/
                E.cx = pos;
                E.px = E.cx;
            } break;
        case CTRL_ARROW_RIGHT:
            {
                if (E.cy >= E.numRows) { break; }
                row = &E.row[E.cy];

                if (E.cx >= row->size)
                {
                    if (E.cy < E.numRows - 1)
                    {
                        E.cy++;
                        E.cx = 0;
                    }
                    break;
                }
                int pos = E.cx;

                while (pos < row->size && (row->chars[pos] != ' ' && row->chars[pos] != '\t'))
                { pos++; }
                // Skip whitespace until the next word.
                while (pos < row->size && (row->chars[pos] == ' ' || row->chars[pos] == '\t'))
                { pos++; }
                E.cx = pos;
                E.px = E.cx;
            } break;
        case ARROW_LEFT:
            {
                if (E.cx != 0)
                {
                    E.cx--;
                }
                else if (E.cy > 0)
                {
                    E.cy--;
                    E.cx = E.row[E.cy].size;
                }
                E.px = E.cx;
            } break;
        case ARROW_RIGHT:
            {
                if (row && E.cx < row->size)
                {
                    E.cx++;
                }
                else if (row && E.cx == row->size)
                {
                    E.cy++;
                    E.cx = 0;
                }
                E.px = E.cx;
            } break;
        case ARROW_UP:
            {
                if (E.cy > 0)
                {
                    E.cy--;
                    row = &E.row[E.cy];
                    E.cx = (E.px > row->size ? row->size : E.px);
                }
            } break;
        case ARROW_DOWN:
            {
                if (E.cy < E.numRows - 1)
                {
                    E.cy++;
                    row = &E.row[E.cy];
                    E.cx = (E.px > row->size ? row->size : E.px);
                }
            } break;
    }
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
        {
            return getCursorPosition(rows, cols);
        }
        editorReadKey();
        return -1;
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void editorOpen(Arena *arena, char *filename)
{
    // TODO(liam): fix this part
    E.filename = StringDataNew((uint8 *)filename);


    FILE *fp = fopen(filename, "r");
    /*GapLoad(arena, &E.buf, filename);*/

    /*while (GapMoveLeft(&E.buf));*/

    /*editorSelectSyntaxHighlight();*/

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                    line[linelen - 1] == '\r'))
        { linelen--; }
        editorInsertRow(E.numRows, line, linelen);
    }
    E.dirty = 0;

    free(line);
    fclose(fp);
}

void editorInsertChar(int c)
{
    if (E.cy == E.numRows)
    {
        editorInsertRow(E.numRows, "", 0);
    }
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;
}

char *editorRowsToString(int *buflen)
{
    int totlen = 0;
    int j;
    for (j = 0; j < E.numRows; j++)
    {
        totlen += E.row[j].size + 1;
    }
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < E.numRows; j++)
    {
        memcpy(p, E.row[j].chars, E.row[j].size);
        p += E.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}


void editorDelChar()
{
    if (E.cy == E.numRows) { return; }
    if (E.cx == 0 && E.cy == 0) { return; }

    erow *row = &E.row[E.cy];
    if (E.cx > 0)
    {
        editorRowDelChar(row, E.cx - 1);
        E.cx--;
    }
    else if (1)// NOTE(liam): implicitly E.cx == 0
    {
        E.cx = E.row[E.cy - 1].size;
        editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
        editorDelRow(E.cy);
        E.cy--;
    }
}

void editorInsertNewline()
{
    if (E.cx == 0)
    {
        editorInsertRow(E.cy, "", 0);
    }
    else
    {
        erow *row = &E.row[E.cy];
        editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }
    E.cy++;
    E.cx = 0;
}

int editorReadKey(void)
{
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
        {
            TermDie("read");
        }
    }

    if (c == '\x1b')
    {
        char seq[6] = {0};

        if (read(STDIN_FILENO, &seq[0], 1) != 1) { return '\x1b'; }
        if (read(STDIN_FILENO, &seq[1], 1) != 1) { return '\x1b'; }

        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) { return '\x1b'; }

                if (seq[2] == ';')
                {
                    // TODO(liam): arrow case here; check gpt
                    if (read(STDIN_FILENO, &seq[3], 1) != 1) return '\x1b';
                    // Read the final letter that indicates the arrow direction.
                    if (read(STDIN_FILENO, &seq[4], 1) != 1) return '\x1b';

                    if (seq[3] == '5') { // Modifier 5 means Ctrl.
                        switch (seq[4]) {
                            case 'A': return CTRL_ARROW_UP;
                            case 'B': return CTRL_ARROW_DOWN;
                            case 'C': return CTRL_ARROW_RIGHT;
                            case 'D': return CTRL_ARROW_LEFT;
                        }
                    }
                }
                else if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch(seq[1])
            {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }

        return '\x1b';
    }
    else
    {
        return c;
    }
}

void editorProcessKeypress(Arena *arena)
{
    static int quitTimes = 2;
    static int resetTimes = 2;
    int c = editorReadKey();

    switch (c)
    {
        case '\r':
        {
            /*GapInsert(arena, &E.buf, '\r');*/
            /*GapInsert(arena, &E.buf, '\n');*/
            E.cy = 0;
            E.cx = 0;
            editorInsertNewline();
        } break;
        case CTRL_KEY('q'):
        {
            if (E.dirty && quitTimes > 0)
            {
                editorSetStatusMessage("UNSAVED CHANGES: "
                    "Press C-Q %d more times to quit.", quitTimes);
                quitTimes--;
                return;
            }
            E.alive = false;
        } break;

        case CTRL_KEY('s'):
        {
            /*editorSave();*/
        } break;

        case CTRL_KEY('l'):
        {
            /*E.rowoff = getScreenCenter();*/
        } break;

        case CTRL_KEY('r'):
        {
            // NOTE(liam): reopens current file without saving.
            if (E.dirty && resetTimes > 0)
            {
                editorSetStatusMessage("UNSAVED CHANGES: "
                        "Press C-R %d more times to reset file.", resetTimes);
                resetTimes--;
                return;
            }
            /*editorReload();*/
        } break;

        case CTRL_KEY('a'):
        case CTRL_KEY('c'):
        case CTRL_KEY('v'):
        case CTRL_KEY('z'):
        {
            editorSetStatusMessage("not implemented!");
        } break;

        case CTRL_KEY('j'):
        {
            /*editorRowAppendToPrev();*/
        } break;
        case HOME_KEY:
        {
            E.cx = 0;
        } break;
        case END_KEY:
        {
            if (E.cy < E.numRows)
            {
                E.cx = E.row[E.cy].size;
            }
        } break;

        case CTRL_KEY('n'):
        {
            editorIndentDown();
        } break;
        case CTRL_KEY('p'):
        {
            editorIndentUp();
        } break;

        case CTRL_KEY('f'):
        {
            editorFind(arena);
        } break;

        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
        {
            if (c == DEL_KEY) { editorMoveCursor(ARROW_RIGHT); }
            editorDelChar();
        } break;

        case PAGE_UP:
        case PAGE_DOWN:
        {
            if (c == PAGE_UP)
            {
                E.cy = E.rowoff;
            }
            else if (c == PAGE_DOWN)
            {
                E.cy = E.rowoff + E.screenRows - 1;
                if (E.cy > E.numRows) { E.cy = E.numRows; }
            }

            int times = E.screenRows;
            while (times--)
            {
                editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
            }
        } break;

        case CTRL_ARROW_UP:
        case CTRL_ARROW_DOWN:
        case CTRL_ARROW_LEFT:
        case CTRL_ARROW_RIGHT:
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        {
            editorMoveCursor(c);
        } break;

        /*case CTRL_KEY('l'):*/
        case '\x1b':
        {
        } break;

        case '\t':
        {
            if (1)
            {
                for (unsigned int i = 0; i < 4; i++)
                {
                    editorInsertChar(' ');
                    /*GapInsert(arena, &E.buf, ' ');*/
                }
                break;
            }
            __attribute__ ((fallthrough));
        }
        default:
        {
            /*GapMove(&E.buf, E.cx);*/
            /*GapInsert(arena, &E.buf, c);*/
            editorInsertChar(c);
        } break;
    }

    quitTimes = 2;
    resetTimes = 2;
}

void editorInit()
{
    /*E.filename = NULL;*/
    E.alive = true;

    if (getWindowSize(&E.screenRows, &E.screenCols) == -1)
    {
        TermDie("getWindowSize");
    }
    E.screenRows -= 2;
}

int main(int argc, char **argv)
{
    Arena arena = {0};
    TermEnableRawMode(&E.orig_termios);
    editorInit();
    if (argc >= 2)
    {
        editorOpen(&arena, argv[1]);
    }

    while (E.alive)
    {
        editorRefreshScreen(&arena);
        editorProcessKeypress(&arena);
    }

    TermDisableRawMode(&E.orig_termios);
    ArenaFree(&arena);
    return 0;
}
