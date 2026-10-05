#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

void exit_handler() {
    printf("[atexit] процесс %d завершается\n", getpid());
}

void signal_handler(int sig) {
    printf("[сигнал] получен сигнал %d: %s\n", sig, strsignal(sig));
}

int main() {
    atexit(exit_handler);

    signal(SIGINT, signal_handler);

    struct sigaction sa = {};
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, nullptr);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("[потомок] PID=%d PPID=%d\n", getpid(), getppid());
        sleep(10);
        printf("[потомок] завершаюсь с кодом 42\n");
        exit(42);
    }

    printf("[родитель] PID=%d PPID=%d, PID потомка=%d\n", getpid(), getppid(), pid);

    int status = 0;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("[родитель] потомок завершился с кодом %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[родитель] потомок убит сигналом %d\n", WTERMSIG(status));
    }

    return 0;
}
