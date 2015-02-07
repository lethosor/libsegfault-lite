#include <cstdio>

void a();
void b();
void c();

void print(const char *text) {
    printf("text: %s\n", text);
};
void (*func_ptr)(const char*) = NULL;

int main() {
    a();
    return 0;
}

void a() { b(); }
void b() { c(); }
