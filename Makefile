all:
	g++ -c -Ofast attacks.cpp
	g++ -c -Ofast board.cpp
	g++ -c -Ofast main.cpp
	g++ -Ofast main.o board.o attacks.o -o chess
# -oFast
