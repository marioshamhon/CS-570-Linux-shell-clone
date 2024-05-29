#include <stdio.h>
#include "getword.h" //so we can use getword.c
#include <string.h> //so we can use strcmp
#include <unistd.h> //so we can use execvp
#include <stdlib.h> // so we can use abs
#include <errno.h> // so we can use perror
#include <sys/types.h> // so we can use fork
#include <sys/wait.h> // so we can use wait
#include <sys/stat.h> // so we can use open
#include <fcntl.h> // so we can use open
#include <signal.h> // so we can use signal handler
#include <libgen.h>  //so we can use basename
#include "CHK.h" //so we can use the macro in the loop that reaps zombie children


#define MAXITEM 100 /* max number of words per line */
#define MAXCHARACTERS 25400 /* max number of characters per line (254 characters per word * 100 word per line)  */
