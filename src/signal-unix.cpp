#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "libsegfault-lite.h"

static struct termios* orig_termios = NULL;

static void** stack;
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
    if (!orig_termios || tcsetattr(STDERR_FILENO, TCSAFLUSH, orig_termios) < 0)
        fprintf(stderr, "Could not reset stderr\n");
    fprintf(stderr, "\nSignal %i (%s):\n", sig_id, signal_name(sig_id).c_str());
    size_t backtrace_length = backtrace(stack, max_depth);
    if (!getenv("LSFL_DEMANGLE")) {
        backtrace_symbols_fd(stack, backtrace_length, STDERR_FILENO);
    }
    else {
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
        free(stack_trace);
    }
    fflush(stderr);
    // Trigger default handler
    raise(sig_id);
}

void libsegfault_lite_init (std::set<int> &signals, int depth) {
    max_depth = depth;
    stack = (void**)malloc(sizeof(void*) * max_depth);
    orig_termios = (struct termios*)calloc(1, sizeof(struct termios));
    tcgetattr(STDERR_FILENO, orig_termios);
    for (auto it = signals.begin(); it != signals.end(); it++) {
        signal(*it, libsegfault_lite_handler);
    }
}
