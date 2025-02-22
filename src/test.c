
#include "base/base.h"
#include "os/os.h"

int main(void)
{
    Arena arena = {0};

    TermSettings ts = {0};
    TermSettingsInit(&ts);

    TermEnableRawMode(&ts.orig_termios);

    TermSetStatusMessage(&ts, "HELP: C-Q = Quit");
    while (ts.alive)
    {
        TermRender(&arena, &ts);
        TermProcessKeypress(&arena, &ts);
    }

    TermDisableRawMode(&ts.orig_termios);
    ArenaFree(&arena);
    return 0;
}
