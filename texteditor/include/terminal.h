#ifndef EDITOR_TERMINAL_H
#define EDITOR_TERMINAL_H

#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <aio.h>
#include <sys/select.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define CLEAR_SCREEN() write(STDOUT_FILENO, "\x1b[2J", 4)
#define CURSOR_RESET() write(STDOUT_FILENO, "\x1b[H", 4)

void die(const char *s);
struct termios terminalEnableRawMode();
void terminalDisableRawMode(struct termios* original);
int terminalGetCursorPos(int* row, int* col);
int terminalGetTermDim(int* rows, int* cols);
int terminalIsKeyHit();


#endif // EDITOR_TERMINAL_H
