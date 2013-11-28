#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "process.h"
#include <fstream>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ext/stdio_filebuf.h>

#define	PARENT_READ	readpipe[0]
#define	CHILD_WRITE	readpipe[1]
#define CHILD_READ	writepipe[0]
#define PARENT_WRITE	writepipe[1]

#define BUFFER_SIZE 256

using namespace std;

Process::Process(const char *path, char *const argv[],
                 bool redirectStderrToStdout) {
  int writepipe[2], readpipe[2];

  if (pipe(writepipe) == -1) {
    throw (const char *)strerror(errno);
  }

  if (pipe(readpipe) == -1) {
    close(writepipe[0]);
    close(writepipe[1]);
    throw (const char *)strerror(errno);
  }

  cpid = fork();
  if (cpid == -1) {
    close(writepipe[0]);
    close(writepipe[1]);
    close(readpipe[0]);
    close(readpipe[1]);
    throw (const char *)strerror(errno);
  }

  if (cpid == 0) {
    // child process
    close(PARENT_WRITE);
    close(PARENT_READ);
    dup2(CHILD_READ,  0);
    close(CHILD_READ);
    dup2(CHILD_WRITE, 1);
    if (redirectStderrToStdout) {
      dup2(CHILD_WRITE, 2);
    }
    close(CHILD_WRITE);

    execv(path, argv);
    perror("execv");
    exit(255);
  }

  close(CHILD_READ);
  close(CHILD_WRITE);

  int status;
  waitpid(cpid, &status, WNOHANG);
  if (WIFEXITED(status) && WEXITSTATUS(status) == 255)
  {
    throw (const char *)"program not found";
  }

  processOut = fdopen(PARENT_READ, "r");
  processIn = fdopen(PARENT_WRITE, "w");
}

Process::~Process() {
  int status;
  waitpid(cpid, &status, WNOHANG);
  if (!WIFEXITED(status) && !WIFSIGNALED(status))
  {
    kill(cpid, SIGHUP);
    waitpid(cpid, &status, WNOHANG);
    int nseconds = 10;
    while (nseconds > 0 && !WIFEXITED(status) && !WIFSIGNALED(status)) {
      sleep(1);
      waitpid(cpid, &status, WNOHANG);
      --nseconds;
    }
    kill(cpid, SIGKILL);
    waitpid(cpid, &status, WNOHANG);
  }
  fclose(processOut);
  fclose(processIn);
}

int Process::getline(std::string &str) {
  ssize_t nread;
  char *line = NULL;
  size_t len = 0;

  str.clear();

  nread = ::getline(&line, &len, processOut);
  ssize_t idx = nread-1;
  while (idx > 0 && (line[idx] == '\r' || line[idx] == '\n') ) {
    line[idx] = '\0';
    --idx;
  }

  if (nread > 0)
    str = line;

  if (line) {
    free(line);
  }

  return nread;
}

int Process::wait() {
  int status;

  waitpid(cpid, &status, 0);

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    return -WTERMSIG(status);
  } else {
    return -255;
  }
}
