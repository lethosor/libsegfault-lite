#include <csignal>
#include <cstdio>
#include <set>
#include <string>

namespace libsighandler {
    const char version[] = "1.1";
    void init_handler (std::set<int> &signals, int depth);
    std::string signal_name (int sig, std::string defval = "");
    int signal_id (std::string name, int defval = -1);
    void info();
}
