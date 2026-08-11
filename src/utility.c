#include "simpleshell.h"

// cmd_cd is used to  change current directory, or print it if no argument given/ no directory to change to
// also updates the PWD environment variable
void cmd_cd(char **args)
{
    char cwd[MAX_BUFFER];

    if (args[1] == NULL) {
        // if there is no argument given, just print the current directory
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            fprintf(stdout, "%s\n", cwd);
        else
            perror("simpleshell: cd");
    } 
    else {
        // try to change into the given directory
        if (chdir(args[1]) != 0)
            perror("simpleshell: cd");
        else {
            // update PWD so other processes see the right value
            if (getcwd(cwd, sizeof(cwd)) != NULL)
                setenv("PWD", cwd, 1);
            else
                perror("simpleshell: cd");
        }
    }
}

// cmd_clr function is used to  clear the terminal screen
void cmd_clr(void)
{
    system("clear");
}

// cmd_dir function lists directory contents with ls -al
void cmd_dir(char **args)
{
    char command[MAX_BUFFER];

    // if user supplied a argument after dir argument use that as the directory to list, else just use '.' (current directory)
    const char *target = (args[1] != NULL) ? args[1] : ".";

    snprintf(command, sizeof(command), "ls -al %s", target); //store the full command string into command var


    //if the supplied directory is invalid, then an error is raised, if not the system command runs without raaisning an error.
    if (system(command) == -1)
        perror("simpleshell: dir");
}

// cmd_environ function is used to print all environment strings
void cmd_environ(void)
{
    int i = 0;
    //iterate through array of enviroment variables and print each on a new line
    while (environ[i] != NULL)
        fprintf(stdout, "%s\n", environ[i++]);
}

// cmd_echo function is used to display arguments, multiple spaces collapsed to one by tokeniser
void cmd_echo(char **args)
{
    int i = 1;
    while (args[i] != NULL) {
        if (i > 1) fputc(' ', stdout);
        fputs(args[i], stdout);
        i++;
    }
    fputc('\n', stdout);
}

// cmd_help is used to show the user manual (readme.txt) through the more filter
void cmd_help(void)
{
    if (system("more manual/readme.txt") == -1)
        perror("simpleshell: help");
}

// function used to pause shell operation until user presses Enter
void cmd_pause(void)
{
    int c;
    fputs("Press Enter to continue...", stdout);
    fflush(stdout);
    while ((c = getchar()) != '\n' && c != EOF);
}





//stage 2

// function walks throught the args array and looks for <, >, >>, & and it takes note of themn
// afterwards these tokens are stripped 
void parse_redir(char **args, char **infile, char **outfile, int *append, int *background)
{
    int i = 0; //used to scan every token
    int j = 0; //used to track regukar args
    
    while (args[i] != NULL) {

        // input redirection token found
        if (!strcmp(args[i], "<")){

            *infile = args[i+1];
            i += 2;  // skip '< and filename

        }


        //output redir (append) token found
        else if (!strcmp(args[i], ">>")){

            *outfile = args[i + 1];

            // signal to open  file 
            *append  = 1;
            i += 2;    //skip '>>' and filename

        }

        // output redir (truncate) token foung
        else if (!strcmp(args[i], ">")) {    
            
            *outfile = args[i + 1];

            // signal to overwrite  file 
            *append  = 0;
            i += 2;  // skip >> and the filename
 
        } 

        else if (!strcmp(args[i], "&")) {     // background execution token found
            *background = 1;
            i++;
 
        } 
        
        
        //regular agruments
        else {

            // keep normal arguments 
            args[j++] = args[i++];             
        }
    }

    //place NULL at the new en of array
    args[j] = NULL;   
    }

//function handes i/o redirection and background execution
void run_ext(char **args, char *infile, char *outfile,int append, int background)
{
    pid_t pid;
    int status;

    //set parent env var to the Shell that currently holds environment
    setenv("parent", getenv("shell"), 1);

    //fork called
    switch (pid = fork()) {

        //fork fails
        case -1:
            perror("simpleshell: fork");
            break;

        //inside child process
        case 0:

            //check if parse_redir founf '<' token
            if (infile != NULL) {

                //replace stdin with given file which is opened in read mode
                if (freopen(infile, "r", stdin) == NULL) {
                    
                    //if file cant be opended print an error and exit the child process
                    fprintf(stderr, "simpleshell: cannot open '%s'\n", infile);
                    exit(1);
                }

            }
                
            //check if parse_redir found '>' of '>>' token
            if (outfile != NULL) {

                //replace stdout with given file opened in either append or write mode which is decided by append flag
                if (freopen(outfile, append ? "a" : "w", stdout) == NULL) {

                    //if file cannot be opened print error and exit child process 
                    fprintf(stderr, "simpleshell: cannot open '%s'\n", outfile);
                    exit(1);
                }
            }

            //replaces child process with requested program
            execvp(args[0], args);

            //lines below only reached on failure
            fprintf(stderr, "simpleshell: command '%s' not found!\n", args[0]);
            exit(1);

        //block and wait for child to finish
        default:

            //if there is no background flag parent waits for child to finish
             if (!background){
                waitpid(pid, &status, 0);
            }

            //if background flag is set skip waiting and notify user
            else {
                fprintf(stdout, "Running in background...\n");
            }
            break;
        

    }
}


//redirect stdout to a file (for internal commands)
FILE *redir_stdout(char *outfile, int append)
{
    //if no outfile was given return null
    if (outfile == NULL)
        return NULL;

    //save real stdout so it is able to be restored later
    FILE *saved = fdopen(dup(STDOUT_FILENO), "w");

    //if saving stdout failed print error and bail
    if (saved == NULL) {
        perror("simpleshell: redirect_stdout");
        return NULL;
    }
    
    //replace stdout with given file opened in either append or write mode which is decided by append flag
    if (freopen(outfile, append ? "a" : "w", stdout) == NULL) {

        //if file cant be opened print error and bail
        fprintf(stderr, "simpleshell: cannot open '%s'\n", outfile);
        return NULL;
    }

    return saved;
}

//restore stdout back to terminal
void restr_stdout(FILE *saved)
{
    //if no redirection happened there is nothing to restore so return
    if (saved == NULL)
        return;

    fflush(stdout);  //flush buffered output to file before switching
    dup2(fileno(saved), STDOUT_FILENO); //copies saved back to STDOUT_FILENO, reconecting terminal
    fclose(saved); //closes save
}


