CC=gcc

shell: shell.o shell_display.o read_input.o execution.o changedir.o
	$(CC) shell.o shell_display.o read_input.o execution.o changedir.o -lm -o shell
	
shell.o: shell.c read_input.h shell_display.h
	$(CC) -c shell.c

changedir.o: changedir.c changedir.h
	$(CC) -c changedir.c

shell_display.o: shell_display.c shell_display.h
	$(CC) -c shell_display.c

execution.o: execution.c execution.h
	$(CC) -c execution.c -lm

read_input.o: read_input.c read_input.h execution.h changedir.h
	$(CC) -c read_input.c

clean:
	-rm *.o