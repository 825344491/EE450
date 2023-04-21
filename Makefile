all:
	g++ -std=c++0x -fdiagnostics-color=always -g ./serverM.cpp -o ./serverM
	g++ -std=c++0x -fdiagnostics-color=always -g ./serverA.cpp -o ./serverA
	g++ -std=c++0x -fdiagnostics-color=always -g ./serverB.cpp -o ./serverB
	g++ -std=c++0x -fdiagnostics-color=always -g ./client.cpp -o ./client

clean:
	rm -f ./serverM
	rm -f ./serverA
	rm -f ./serverB
	rm -f ./client