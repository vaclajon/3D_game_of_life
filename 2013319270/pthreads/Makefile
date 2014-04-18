CFLAGS = -O0
OBJECTS = game_of_life_pthreads.o
test_pthreads : $(OBJECTS)
	gcc $(CFLAGS) -o $@ $^

game_of_life_pthreads.o : game_of_life_pthreads.c
	gcc $(CFLAGS) -c $<

clean:
	rm -rf test_pthreads $(OBJECTS)
