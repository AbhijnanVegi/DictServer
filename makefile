S=server
C=client

CFLAGS= -g -lpthread

S_Files= \
	$S/main.cpp\
	$S/server.cpp\
	$S/handler.cpp

server: $(S_Files)
	g++ $(S_Files) $(CFLAGS) -o srvr