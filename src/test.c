
#include "base/base.h"
#include "os/os.h"

int main(void)
{
    Arena arena = {0};
    TermSettings ts = {0};
    TermSettingsInit(&ts);

    TermEnableRawMode(&ts.orig_termios);

    TermSetStatusMessage(&ts, "HELP: C-S = save | C-Q = quit | C-F = find");
    while (ts.alive)
    {
        TermRender(&arena, &ts);
        TermProcessKeypress(&ts);
    }

    TermDisableRawMode(&ts.orig_termios);
    ArenaFree(&arena);
    return 0;
}
