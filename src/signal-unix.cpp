#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>

#include "libsegfault-lite.h"

static int max_depth;
volatile static std::sig_atomic_t in_handler = 0;

void libsegfault_lite_handler (int sig_id) {
    if (in_handler) {
        raise(sig_id);
        return;
    }
    in_handler = 1;
    // Restore default handler
    signal(sig_id, SIG_DFL);
    fprintf(stderr, "\nSignal %i (%s):\n", sig_id, signal_name(sig_id).c_str());
    void * stack[max_depth];
    size_t backtrace_length = backtrace(stack, max_depth);
    backtrace_symbols_fd(stack, backtrace_length, STDERR_FILENO);
    // Trigger default handler
    raise(sig_id);
}

void libsegfault_lite_init (std::set<int> &signals, int depth) {
    max_depth = depth;
    for (auto it = signals.begin(); it != signals.end(); it++) {
        signal(*it, libsegfault_lite_handler);
    }
}
