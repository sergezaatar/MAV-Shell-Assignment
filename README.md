Shell program, similar to burne shell (bash), c-shell (csh), or korn shell (ksh). 
It accepts commands, forks a child process and execute commands. The shell, like csh or bash, runs and accepts
commands until the user exits the shell.

Compiles on omega with:
gcc -Wall msh.c -o msh —std=c99