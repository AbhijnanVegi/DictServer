S=server
C=client

CFLAGS= -g -lpthread

S_Files= \
	$S/main.cpp\
	$S/server.cpp\
	$S/handler.cpp

C_Files = \
	$C/client.cpp\
	$C/main.cpp


all: server client

server: $(S_Files)
	g++ $(S_Files) $(CFLAGS) -o srvr

client: $(C_Files)
	g++ $(C_Files) $(CFLAGS) -o clnt