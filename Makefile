all:
	g++ -std=c++0x -g ./serverM.cpp -o ./serverM
	g++ -std=c++0x -g ./serverA.cpp -o ./serverA
	g++ -std=c++0x -g ./serverB.cpp -o ./serverB
	g++ -std=c++0x -g ./client.cpp -o ./client

clean:
	rm -f ./serverM
	rm -f ./serverA
	rm -f ./serverB
	rm -f ./client