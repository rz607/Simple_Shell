#include "simpleshell.h"

int main(int argc, char **argv)
{
    char  buf[MAX_BUFFER];          // buffer
    char  prompt[MAX_BUFFER];       // shell prompt
    char *args[MAX_ARGS];           // pointers to argument strings
    char **arg;                     // will be used for working pointer through args
    FILE *input = stdin;            // set the default input to stdin (keyboard)
    char  resolved[MAX_BUFFER];     // for resolving shell path

    char *infile; //input file name 
    char *outfile; // output file name
    int append; // stores number, 1 for >>, 0 for >
    int background; // will be 1 if '&' was at end of line
    FILE *saved; //saves stdout for internal redirection


    // set SHELL= environment variable to full path of this executable
    if (realpath(argv[0], resolved) != NULL)
        setenv("shell", resolved, 1);
    else
        setenv("shell", argv[0], 1);


    /*if there are more than one arguments when running ./simpleshell (meaning there is a file to read input from),
     then take the input from that file */ 
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            //if the file entered could not be opened then return an error message
            fprintf(stderr, "simpleshell: could not open file named %s!\n", argv[1]);
            exit(1);
        }
    }

    // keep reading input until "quit" command or end of file (if a batchfile is provided)
    while (!feof(input)) {

        // show prompt in interactive mode
        if (input == stdin) {
            //usually show prompt as 'cwd >'
            if (getcwd(prompt, sizeof(prompt)) != NULL)
                fprintf(stdout, "%s > ", prompt);
            // if not prompt will just be 'simpleshell >' 
            else
                fputs("simpleshell > ", stdout);
            fflush(stdout);
        }

        if (fgets(buf, MAX_BUFFER, input)) {    // read a line

            // tokenize the input into args array
            arg = args;
            *arg++ = strtok(buf, SEPARATORS);   
            while ((*arg++ = strtok(NULL, SEPARATORS)));
            // last entry will be NULL

            // check if there is a first argument
            if (args[0]) {                      

                //reset redirection state for every new command
                infile = outfile = NULL;
                append = background = 0;

                //call parse_redir function in utility.c
                parse_redir(args, &infile, &outfile, &append, &background);

                // check if argument matches one of the below commands

                //if argument entered == 'quit'
                if (!strcmp(args[0], "quit"))   
                    // break out of while loop
                    break;                      

                //if first argument == 'cd'
                if (!strcmp(args[0], "cd")) {   
                    //call the cmd_cd function in ultility.c.
                    cmd_cd(args);
                    continue;
                }


                //if the argument entered is the same as 'clr'
                if (!strcmp(args[0], "clr")) {  
                    //call the cmd_clr command in ultility.c
                    cmd_clr();
                    continue;
                }

                //if the first argument is dir
                if (!strcmp(args[0], "dir")) { 

                    //redirect stdout if specified
                    saved = redir_stdout(outfile, append);

                    //call the cmd_dir function in ultility.c
                    cmd_dir(args);
                    restr_stdout(saved);
                    continue;
                }

                //if the argument entered is the same as 'environ'
                if (!strcmp(args[0], "environ")) { 

                    //redirect stdout if specified
                    saved = redir_stdout(outfile, append);

                    //call the cmd_environ funtction in ultility.c
                    cmd_environ();
                    restr_stdout(saved); //restore stdout to terminal
                    continue;
                }

                //if the argument entered is the same as 'echo'
                if (!strcmp(args[0], "echo")) { 

                    //redirect stdout if specified
                    saved = redir_stdout(outfile, append);

                    //call the cmd_echo function from utility.c
                    cmd_echo(args);
                    restr_stdout(saved);
                    continue;
                }

                // if the argument entered is the same as 'help' 
                if (!strcmp(args[0], "help")) {

                    //redirect stdout if specified
                    saved = redir_stdout(outfile, append);

                    //call the cmd_help function in utility.c
                    cmd_help();
                    restr_stdout(saved);
                    continue;
                }

                //if the argument entered is the same as 'pause'.
                if (!strcmp(args[0], "pause")) {
                    //call the pause cmd_pause function in utility.c
                    cmd_pause();
                    continue;
                }

                //if the command doesnt match any of the above call run_ext function
                run_ext(args, infile, outfile, append, background);

            }
        }
    }

    // close batchfile if it was opened
    if (input != stdin)
        fclose(input);      

    return 0;
}

