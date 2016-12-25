# C-shell
An attempt to implement the C shell with all it's features

## BUILTIN COMMANDS

cd and echo, both work exactly the same like the normal terminal one. That means, even using "" or '' is supported. I have not included escape characters and made different flags ( as it was mentioned in the news forum ).
For example, 
```
cd asd/"saf
> asd"  
```
(Reading user input again)
This will go to `asd/safasd`
OR 
```
echo sad      asd "as
>   asd"
```
(Reading user input again)
This will output `sad asd as  asd`

## SIGNAL HANDLING AND PIPING/REDIRECTION

All the Basic Signal Handling is done

- [x] Ctrl-D ( to exit the shell )
- [x] Ctrl-Z ( to push a process to background )
- [x] Ctrl-C ( to give an interrupt )

## FILES

I have created four dependency files

- changedir.c : This file basically should be renamed builtin.c because it has all the builtin commands (echo, cd, pwd). There are three functions which implements the behaviour of the commands.

- shell_display.c : This file contains the code to display the username, hostname and the current working directory.

- read_input.c : As the name of the file conveys, it is used read input, parse them (tokenise them) and give it to other builtin commands or execute another process

- execute.c : This file contains pinfo (BONUS II) and executeCommand (BONUS I included) which can create a fg/bg process.

- shell.c : This is the main file.

## COMPILING AND EXECUTION

I have made a makefile which compiles each of the dependencies I have created (.c and .h files which support the main program - shell.c). The makefile creates an object file after compilation and in the end compiles all of them with the main program shell.o (because it has the main function). It creates an executable file by the name "shell"

To start the shell, just execute the "shell" executable file which is created after you use the make command (./shell)
