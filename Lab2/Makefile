all:httpServer

CFLAGS = -O2 -pthread -lpthread
SRC = main.cpp 

httpServer: $(SRC)
	g++ $(CFLAGS) $(SRC) -o http-server
	
clean:
	-rm -f httpserver
