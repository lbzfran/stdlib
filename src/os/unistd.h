#ifndef _UNISTD_H
#define _UNISTD_H

/* This is intended as a drop-in replacement for unistd.h on Windows.
 * Please add functionality as needed.
 * https://stackoverflow.com/a/826027/1202830
 */

#ifndef OS_WINDOWS
# error "use <unistd.h> instead."
#endif

#include <stdlib.h>
#include <io.h>
#include <getopt.h> /* getopt at: https://gist.github.com/ashelly/7776712 */
#include <process.h> /* for getpid() and the exec..() family */
#include <direct.h> /* for _getcwd() and _chdir() */
#include <Windows.h>

#define srandom srand
#define random rand

/* Values for the second argument to access.
   These may be OR'd together.  */
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#define access _access
#define dup2 _dup2
#define execve _execve
#define ftruncate _chsize
#define unlink _unlink
#define fileno _fileno
#define getcwd _getcwd
#define chdir _chdir
#define isatty _isatty
#define lseek _lseek
/* read, write, and close are NOT being #defined here, because while there are file handle specific versions for Windows, they probably don't work for sockets. You need to look at your app and consider whether to call e.g. closesocket(). */

#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2



DWORD WinFileNumToHandle(uint8 fileno);
memory_index WinWrite_(uint8 fileno, char *c, memory_index size);
memory_index WinRead_(uint8 fileno, char *buf, memory_index size);

#define write WinWrite_
#define read WinRead_

#endif // UNISTD_WIN32_H