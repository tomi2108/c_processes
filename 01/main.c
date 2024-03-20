#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int id = fork();
  int n;
  if (id == 0) {
    n = 1;
  } else {
    n = 6;
    wait(&id);
  }

  for (int i = n; i < n + 5; i++) {
    printf("%d ", i);
  }

  return 0;
}