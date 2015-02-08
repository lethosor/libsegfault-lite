#include <csignal>
#include <cstdio>
#include <set>
#include <string>

const char libsegfault_lite_version[] = "1.1";

void libsegfault_lite_init (std::set<int> &signals, int depth);
std::string signal_name (int sig, std::string defval = "");
int signal_id (std::string name, int defval = -1);

void libsegfault_lite_info();
