default: my_ls
my_ls: my_ls.o
	gcc -ggdb my_ls.c -o my_ls
my_ls.o: my_ls.c
	gcc -c -ggdb my_ls.c

clean:
	rm -rf *o *out my_ls 
