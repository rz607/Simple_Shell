

simpleshell: src/simpleshell.c src/utility.c src/simpleshell.h
	mkdir bin
	gcc -Wall src/simpleshell.c src/utility.c -o bin/simpleshell

clean: 
	rm -f bin/simpleshell
