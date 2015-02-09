#include <stdlib.h>
void(*func_ptr)(void) = NULL;
int main() {
    func_ptr();
    return 0;
}
