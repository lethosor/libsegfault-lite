#include <cxxabi.h>
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
    char** stack_trace = backtrace_symbols(stack, backtrace_length);
    for (size_t i = 0; i < backtrace_length; i++) {
        std::string line = stack_trace[i];
        size_t start = line.find(" 0x"),
            end, length;
        if (start != std::string::npos)
            start = line.find(" ", start + 1) + 1;
        if (start != std::string::npos) {
            end = line.find(" ", start);
            if (end != std::string::npos) {
                length = end - start;
                int err = 1;
                char* demangled = abi::__cxa_demangle(line.substr(start, length).c_str(), NULL, NULL, &err);
                if (!err) {
                    line.replace(start, length, std::string(demangled));
                }
                free(demangled);
            }
        }
        fprintf(stderr, "%s\n", line.c_str());
    }
    fflush(stderr);
    free(stack_trace);
    // Trigger default handler
    raise(sig_id);
}

void libsegfault_lite_init (std::set<int> &signals, int depth) {
    max_depth = depth;
    for (auto it = signals.begin(); it != signals.end(); it++) {
        signal(*it, libsegfault_lite_handler);
    }
}
