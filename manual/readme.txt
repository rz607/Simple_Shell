--------------------------------------------------------------------------------
                         SIMPLESHELL USER MANUAL
--------------------------------------------------------------------------------


                            STARTING THE SHELL
                            ------------------

To start the shell in interactive mode (read user input/stdin)

    ./simpleshell

To start the shell in batch mode (reads commands from a file, then exits) simply type:

    ./simpleshell <batchfile>

In batch mode, each line in the  <batchfile> is treated as a command.
The shell exits automatically when the end of the file is reached/ no commands are left to process.

                        
                        THE PROGRAM ENVIRONMENT
                        -----------------------

When simpleshell starts it sets the following environment variable:

    shell= <path to the simpleshell>

This records where the shell is located. You can see it by
running the environ command.

When an external command is run, simpleshell also sets:

    parent= <path to the simpleshell>

This tells any program launched by the shell which shell started it.



                            INTERNAL COMMANDS
                            -----------------

cd <directory>
    Change the current working directory to <directory>. 
    If no directory is specified the current directory is printed instead.


    Examples:
        cd /home/user       (change to /home/user)

clr
    clear the terminal screen.

    Example:
        clr

dir <directory>
    List the contents of <directory> in long format.
    If no directory is given, the contents of the current directory are listed instead.

    Examples:
        dir                 (list current directory)
        dir /tmp            (list /tmp)

environ
    Print all environment variables currently set in the shell, one per line in the format NAME=value.

    Example:
        environ

echo <text ...>
    Print the given  text to the screen followed by a newline.
    Multiple spaces or tabs between words may be reduced to a single space.

    Examples:
        echo Hello World

help
    Display this user manual. Press the space bar to advance a page and press q to quit.

    Example:
        help

pause
    Pause the shell until you press the Enter key to continue.

    Example:
        pause

quit
    Exit the shell and return to the parent process.

    Example:
        quit

                                EXTERNAL COMMANDS
                                ----------------

Any command that is not one of the interanl commands listed above is treated as an external command.
The shell forks a child process and uses execvp to run the program, searching the PATH automatically.

    Examples:

        ls -al
        grep hello hello.txt
        wc -l essay.txt

                                I/O REDIRECTION
                                ---------------

Simpleshell allows you to redirect input and output.

Input redirection '<' from file instead of keyboard.

    syntax: command < file
    example: cat < notes.txt

Output redirection '>' or '>>' writes output to a file.
'>' creates a file to write to if it doesnt exist or overwrites it if it doesnt.
'>>' writes the output at the end of the content already in the file

    syntax: command >> file
    example: echo hello > hello.txt
    example: echo 7 >> numbers.txt

                                BACKGROUND EXECUTION
                                --------------------

By default all commands run in the foreground - the shell waits for the program 
to finish before showing the prompt again. Adding '&' at the end of a command runs it in the 
background meaning that the shell returns to the prompt without waiting for the program to finish. 

    syntax: command [args] [redirection] &
    example: sleep 10 &




                                THINGS TO NOTE
                                --------------
Commands are case-sensitive: "cd" works, "CD" doesnt.
Arguments are separated by spaces or tabs.
In a batchfile, there is only  one command per line; blank lines are ignored.
Background execution only works for external commands.
Note that all redirection symbols must be seperated from each other.



