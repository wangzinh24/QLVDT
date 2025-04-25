#include "utils.h"
#include <locale.h>
#include <windows.h>
void setupConsole() {
    setlocale(LC_ALL, "en_US.utf8");
    SetConsoleOutputCP(CP_UTF8);
}
