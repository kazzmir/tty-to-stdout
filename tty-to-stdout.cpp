#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef MACOS
#include <util.h>
#elif LINUX
#include <pty.h>
#endif

/* Forces a child process to behave by capturing the child process's stdout even if the child
 * only wants to write to a tty.
 * Opens a psuedo-tty, runs a child process using that tty, then writes the tty output to stdout
 */
int main(int argc, char ** argv){
    int master = 0;
    int pid = forkpty(&master, NULL, NULL, NULL);
    if (pid == 0){
        execvp(argv[1], &argv[1]);
    } else {
        if (pid == -1){
            perror("forkpty");
            return 1;
        }
        while (true){
            char buffer[1024];
            setlinebuf(stdout);
            int c = read(master, buffer, sizeof(buffer));
            if (c == 0){
                break;
            }
            fwrite(buffer, c, 1, stdout);
        }
    }

    return 0;
}

/* this would work if the program just sent its output to stdout without checking that stdout is a tty */
int main2(int argc, char ** argv){
    setlinebuf(stdout);
    // setvbuf(stdout, NULL, _IONBF, 0);
    char ** child_argv = new char*[argc];
    for (int i = 1; i < argc; i++){
        child_argv[i - 1] = argv[i];
    }
    child_argv[argc - 1] = NULL;
    execvp(child_argv[0], child_argv);
    return 0;
}
