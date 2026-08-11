//including necessary headers 
#ifndef SIMPLESHELL_H
#define SIMPLESHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//stage 2 necessary libraries
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER  1024        // define max line buffer
#define MAX_ARGS    64          // define max  args
#define SEPARATORS  " \t\n"     // token separators

extern char **environ; //live enviroment array

// internal command prototypes  (in utility.c)
void cmd_cd(char **args);
void cmd_clr(void);
void cmd_dir(char **args);
void cmd_environ(void);
void cmd_echo(char **args);
void cmd_help(void);
void cmd_pause(void);


//stage 2 command prototypes
void parse_redir(char **args, char **infile, char **outfile, int *append, int *background);
void run_ext(char **args, char *infile, char *outfile,int append, int background);
FILE *redir_stdout(char *outfile, int append);
void  restr_stdout(FILE *saved);

#endif


