#ifdef nonono

 gcc rdcon.c -o rdcon.exe

#include <windows.h>
#include <stdio.h>

#define MAX_SIZE 256

int main() {
    WCHAR input[2] = { '\0', '\0' };
    DWORD count;
    DWORD opts;
    HANDLE buf = GetStdHandle(STD_INPUT_HANDLE);
    
    GetConsoleMode(buf, &opts);
    SetConsoleMode(buf, 0);

    do {
        ReadConsoleW(buf, input, 2, &count, NULL);
        wprintf(L"%s\n", input);
        printf("%d %d\n", count, input[0]);
    } while (input[0] != '$');

    SetConsoleMode(buf, opts);
}
#endif

/* https://archives.miloush.net/michkap/archive/2005/12/15/504092.html */
#include <windows.h>
#include <wincon.h>
#include <stdio.h>
 
int main()
{
  puts("ReadConsoleInput test");
  puts("Ctrl-D to quit.\n");

  HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
  int n = 0;
  puts("#    UC     ch u/d  VK   SC  State\n\n");
  for (;;) {
    INPUT_RECORD ir[1];
    DWORD cEventsRead;
    if (! ReadConsoleInputW(in, ir, 1, &cEventsRead)) {
      puts("ReadConsoleInput failed!");
      return 0;
    }
    for (DWORD i = 0; i < cEventsRead; ++i, ++n) {
      if (ir[i].EventType & KEY_EVENT) {
        KEY_EVENT_RECORD ker = ir[i].Event.KeyEvent;
        printf("%3d: U+%04x %02x %s %04x %04x %04x\n",
            n,
            (WORD)ker.uChar.UnicodeChar,
            (WORD)ker.uChar.AsciiChar,
            ker.bKeyDown ? "down" : " up ",
            ker.wVirtualKeyCode,
            ker.wVirtualScanCode,
            ker.dwControlKeyState);

        if (!ker.bKeyDown && ker.uChar.AsciiChar == ('D' - '@'))
          return 0;
      }
    }
  }
  return 0;
}
