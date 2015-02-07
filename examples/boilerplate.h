#include <cstdio>

void a();
void b();
void c();

void print(const char *text);
static void (*func_ptr)(const char*) = NULL;

int main();
