#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void handle_sigusr1(int sig) {
  printf("\n (HINT): Remember that multiplication is repetitive addition\n");
}

int main(int argc, char *argv[]) {
  int pid = fork();

  if (pid == 0) {
    sleep(5);
    kill(getppid(), SIGUSR1);
    return 0;
  }

  struct sigaction sa;
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = &handle_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);

  int answer;
  printf("What is 3 * 5?\n");
  scanf("%d", &answer);
  if (answer == 15) {
    printf("Right!");
  } else {
    printf("Wrong!");
  }
  kill(pid, SIGKILL);
  wait(NULL);

  return 0;
}