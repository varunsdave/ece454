objects := $(patsubst %.c,%.o,$(wildcard *.c))

all: client server

client: libstubs.a myclient.o
	gcc myclient.o -L. -lstubs -o client

server: libstubs.a myserver.o
	gcc myserver.o -L. -lstubs -o server

libstubs.a: server_stub.o client_stub.o mybind.o
	ar r libstubs.a server_stub.o client_stub.o mybind.o

$(objects): %.o: %.c ece454rpc_types.h
	gcc -c $< -o $@

clean:
	rm -rf client server *.o core *.a *.out
