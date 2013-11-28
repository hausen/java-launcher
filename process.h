#ifndef PROCESS_H
#define PROCESS_H
#include <sys/types.h>
#include <string>

class Process {
public:
  Process(const char *path, char *const argv[],
          bool redirectStderrToStdout = true);
  ~Process();
  int getline(std::string& str);
  int wait();

private:
  pid_t cpid;
  FILE *processIn, *processOut;
};

#endif
