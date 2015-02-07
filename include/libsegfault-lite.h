#include <csignal>
#include <cstdio>
#include <set>
#include <string>

void libsegfault_lite_init (std::set<int> &signals, int depth);
std::string signal_name (int sig, std::string defval = "");
int signal_id (std::string name, int defval = -1);
