#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd[2];
  int pipe_result = pipe(fd);
  if (pipe_result == -1) {
    printf("Failed to open pipe\n");
    return 1;
  }

  int ping_pid = fork();

  if (ping_pid == -1) {
    printf("Failed to fork ping process\n");
    return 1;
  }

  if (ping_pid == 0) {
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);

    int err = execlp("ping", "ping", "-c", "5", "google.com", NULL);
    if (err == -1) {
      printf("Failed to execute ping process\n ");
      return 2;
    }
  }

  int grep_pid = fork();
  if (grep_pid == -1) {
    printf("Failed to fork grep process\n");
    return 1;
  }

  if (grep_pid == 0) {
    close(fd[1]);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    int log_file = open("ping.log", O_WRONLY | O_CREAT | O_APPEND, 0777);
    dup2(log_file, STDOUT_FILENO);

    int err = execlp("grep", "grep", "bytes.", NULL);
    if (err == -1) {
      printf("Failed to execute grep program\n ");
      return 2;
    }
  }
  close(fd[0]);
  close(fd[1]);

  int ping_process;
  int grep_process;
  waitpid(ping_pid, &ping_process, 0);
  waitpid(grep_pid, &grep_process, 0);

  if (!(WIFEXITED(ping_process))) {
    printf("Ping process was terminated unexpectedly\n");
    return 4;
  };

  int ping_status = WEXITSTATUS(ping_process);
  if (ping_status != 0) {
    printf("Ping failed\n");
    return 3;
  }

  if (!(WIFEXITED(grep_process))) {
    printf("Grep process was terminated unexpectedly\n");
    return 4;
  };

  int grep_status = WEXITSTATUS(grep_process);
  if (grep_status != 0) {
    printf("Grep failed\n");
    return 3;
  }

  printf("Success\n");
  return 0;
}