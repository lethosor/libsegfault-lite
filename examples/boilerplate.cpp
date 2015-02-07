#include "boilerplate.h"

void print(const char *text) {
    printf("text: %s\n", text);
};

int main() {
    printf("func_ptr = %p\n", func_ptr);
    a();
    return 0;
}

void a() { b(); }
void b() { c(); }
