#include <map>
#include "libsegfault-lite.h"

std::map<int, std::string> *signal_names;
std::map<std::string, int> *signal_ids;

std::string signal_name (int sig, std::string defval /* = "" */) {
    auto it = signal_names->find(sig);
    if (it != signal_names->end())
        return it->second;
    return defval;
}

int signal_id (std::string name, int defval /* = -1 */) {
    auto it = signal_ids->find(name);
    if (it != signal_ids->end())
        return it->second;
    return defval;
}


#ifdef __GXX_ABI_VERSION
#define constructor_support
static void con() __attribute__((constructor));
#endif

static void con() {
    signal_names = new std::map<int, std::string>;
    signal_ids = new std::map<std::string, int>;
    #define map(name) \
        signal_names->insert(std::pair<int, std::string>(name, #name)); \
        signal_ids->insert(std::pair<std::string, int>(#name, name))
    map(SIGABRT);
    map(SIGFPE);
    map(SIGILL);
    map(SIGINT);
    map(SIGSEGV);
    map(SIGTERM);
    #ifdef SIGBUS
        map(SIGBUS);
    #endif
    #ifdef SIGHUP
        map(SIGHUP);
    #endif
    #ifdef SIGKILL
        map(SIGKILL);
    #endif
    #ifdef SIGQUIT
        map(SIGQUIT);
    #endif
    #undef map
    char* raw_signals = getenv("LSFL_SIGNALS");
    std::set<int> signals;
    if (raw_signals) {
        std::string tmp = raw_signals;
        for (size_t i = 0; i < tmp.size(); i++)
            tmp[i] = toupper(tmp[i]);
        size_t start = 0, end;
        while (start < tmp.size()) {
            end = tmp.find(' ', start);
            if (end == std::string::npos)
                end = tmp.size();
            std::string name = tmp.substr(start, end - start);
            if (name == "ALL") {
                for (auto it = signal_ids->begin(); it != signal_ids->end(); ++it)
                    signals.insert(it->second);
            }
            else {
                int sig = signal_id(name, -1);
                if (sig == -1)
                    sig = signal_id(std::string("SIG") + name, -1);
                if (sig == -1)
                    fprintf(stderr, "libsegfault-lite: Unrecognized signal: %s\n", name.c_str());
                else
                    signals.insert(sig);
            }
            start = end + 1;
        }
    }
    else {
        signals.insert(SIGSEGV);
    }
    char* raw_depth = getenv("LSFL_DEPTH");
    int depth = 20;
    if (raw_depth) {
        depth = atoi(raw_depth);
        if (depth < 2) {
            fprintf(stderr, "Invalid depth: %s (must be at least 2)\n", raw_depth);
            depth = 20;
        }
    }
    libsegfault_lite_init(signals, depth);
}

#ifndef constructor_support
int unused = (con(), 0);
#endif
