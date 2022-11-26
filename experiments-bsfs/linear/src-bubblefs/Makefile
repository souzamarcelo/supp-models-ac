FLAGS=-Wall -O3 -std=c++0x

all:
	g++ ${FLAGS} bubble.cpp -o bubble
	
executeTest: clean all
	./bubble
	
grindTest: clean all
	valgrind ./bubble
	
detailedGrind: clean
	g++ -std=c++0x bubble.cpp -o bubble
	valgrind --leak-check=full --track-origins=yes ./bubble
	
clean:
	rm -f bubble
