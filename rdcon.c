/* https://archives.miloush.net/michkap/archive/2005/12/15/504092.html */
#include <windows.h>
#include <wincon.h>
#include <stdio.h>

#define CNTL    0x0100      /* Control flag, or'ed in       */
#define META    0x0200      /* Meta flag, or'ed in          */
#define CTLX    0x0400      /* ^X flag, or'ed in            */
#define FUNC    0x0800      /* for function keys, if any    */
#define SHFT    0x1000      /* function keys + Shift        */
#define ALT     0x2000      /* function keys + Alternate    */
#define ED      0x8000      /* flag: cmd changes the file   */

int main()
{
  puts("ReadConsoleInput test");
  puts("Ctrl-D to quit.\n");

  HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
  int n = 0;
  int k, vk, ch, uc, sc, ck;
  SetConsoleMode(in, 0);
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

    k = 0;
    if (ir[i].EventType & KEY_EVENT) {
      if (ir[i].Event.KeyEvent.bKeyDown) {
        vk = ir[i].Event.KeyEvent.wVirtualKeyCode;
//      ch = ir.Event.KeyEvent.uChar.AsciiChar;
        sc = ir[i].Event.KeyEvent.wVirtualScanCode;
        ck = ir[i].Event.KeyEvent.dwControlKeyState;
        uc = ir[i].Event.KeyEvent.uChar.UnicodeChar;
        if (uc == 0) {
          switch (ck) {
            case SHIFT_PRESSED: break;
            case ENHANCED_KEY:
            default:  k = FUNC | sc;
          };
        } else {
          k = uc;
        }
      }
    }
    if (k) printf("k = %d\n\n", k);

        if (!ker.bKeyDown && ker.uChar.AsciiChar == ('D' - '@'))
          return 0;
      }
    }
  }
  return 0;
}

#ifdef nonono
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
