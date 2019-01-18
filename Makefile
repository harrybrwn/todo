todo: todo.o command.o
	gcc todo.o command.o -o todo.exe
	del *.gch

todo.o: todo.c
	gcc -c todo.c

command.o: command.c command.h
	gcc -c command.c
