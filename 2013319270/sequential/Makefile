CFLAGS = -O0
OBJECTS = game_of_life_seq.o
test_seq : $(OBJECTS)
	g++ $(CFLAGS) -o $@ $^

game_of_life_seq.o : game_of_life_seq.cpp
	g++ $(CFLAGS) -c $<

clean:
	rm -rf test_seq $(OBJECTS)
