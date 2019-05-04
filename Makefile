make:
	gcc -c *.c
	gcc -o Vash *.o

clean:
	rm *.o
