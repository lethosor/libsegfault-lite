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
    #undef map
    char* raw_signals = getenv("LSFL_SIGNALS");
    std::set<int> signals;
    if (raw_signals) {

    }
    else {
        signals.insert(SIGSEGV);
    }
    libsegfault_lite_init(signals);
}

#ifndef constructor_support
int unused = con();
#endif