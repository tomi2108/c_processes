#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void generate_process(int index, int ***fd, int n) {}

int main(int argc, char *argv[]) {
  printf("Input number of processes:\n");
  int number_of_processes;
  scanf("%d", &number_of_processes);

  int pids[number_of_processes];
  int fd[number_of_processes + 1][2];
  for (int i = 0; i < number_of_processes + 1; i++) {
    pipe(fd[i]);
  }

  for (int process_index = 0; process_index < number_of_processes;
       process_index++) {
    int pid = fork();
    int next_process_index = process_index + 1;

    if (pid == 0) {
      for (int i = 0; i < number_of_processes; i++) {
        if (i != process_index) {
          close(fd[i][0]);
        }
        if (i != next_process_index) {
          close(fd[i][1]);
        }
      }
      int x;
      read(fd[process_index][0], &x, sizeof(int));
      x += 5;
      write(fd[next_process_index][1], &x, sizeof(int));
      return 0;
    }
    pids[process_index] = pid;
  }

  for (int i = 0; i < number_of_processes; i++) {
    if (i != 0) {
      close(fd[i][1]);
    }
    if (i != number_of_processes) {
      close(fd[i][0]);
    }
  }

  int x = 0;
  write(fd[0][1], &x, sizeof(int));
  read(fd[number_of_processes][0], &x, sizeof(int));

  for (int i = 0; i < number_of_processes; i++) {
    waitpid(pids[i], NULL, 0);
  }
  printf("Result is %d", x);

  return 0;
}