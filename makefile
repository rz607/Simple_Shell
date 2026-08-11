

simpleshell: src/simpleshell.c src/utility.c src/simpleshell.h
	gcc -Wall src/simpleshell.c src/utility.c -o bin/simpleshell

clean: 
	rm -f bin/simpleshell
