#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int pid = fork();

  if (pid == -1) {
    printf("Failed to fork process\n");
    return 1;
  }

  if (pid == 0) {
    int ping_results = open("ping.log", O_WRONLY | O_CREAT | O_APPEND, 0777);
    dup2(ping_results, STDOUT_FILENO);
    int err = execlp("ping", "ping", "-c", "5", "google.com", NULL);
    if (err == -1) {
      printf("Failed to execute ping program\n ");
      return 2;
    }
  }
  int ping_process;
  wait(&ping_process);

  if (WIFEXITED(ping_process)) {
    int ping_status = WEXITSTATUS(ping_process);
    if (ping_status == 0) {
      printf("Success!\n");
      return 0;
    } else {
      printf("Ping failed!\n");
      return 3;
    }
  }
  printf("Ping process was terminated unexpectedly!\n");

  return 4;
}