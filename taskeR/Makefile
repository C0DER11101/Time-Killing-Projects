files=main.o pqAct.o

task: $(files)
	gcc $(files) -o task

main.o: main.c functions.h
	gcc -c main.c

pqAct.o: pqAct.c pq.h typedefs.h
	gcc -c pqAct.c

.PHONY: clean

clean:
	-rm $(files) task
