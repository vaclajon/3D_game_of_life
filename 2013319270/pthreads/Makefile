CFLAGS = -O0
OBJECTS = game_of_life_pthreads.o 
test_pthreads : $(OBJECTS)
	g++ $(CFLAGS) -o $@ $^ -lpthread
game_of_life_pthreads.o : game_of_life_pthreads.cpp
	g++ $(CFLAGS) -c $< -lpthread

clean:
	rm -rf test_pthreads $(OBJECTS)
