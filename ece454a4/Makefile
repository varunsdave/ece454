OBJECTS := fs_server.o fs_client.o
OBJECTS2 := simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o

all: client server

client: fs_client.o client-api.a libstubs.a
	gcc -g fs_client.o -L. -lclient-api -o fs_client

server: libstubs.a fs_server.o
	gcc -g fs_server.o -L. -lstubs -o fsServer

libstubs.a: simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o
	ar r libstubs.a simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o

client-api.a: client_api.o ece454_fs.h
	ar r libclient-api.a client_api.o simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o

client_api.o: client_api.c libstubs.a
	gcc -c -g client_api.c -L. -lstubs -o client_api.o

$(OBJECTS): %.o: %.c ece454_fs.h
	gcc -c -g $< -o $@

$(OBJECTS2): simplified_rpc/%.o: simplified_rpc/%.c simplified_rpc/ece454rpc_types.h
	gcc -c -g $< -o $@

clean:
	rm -rf *.o core *.a fs_client simplified_rpc/*.a simplified_rpc/*.o fsServer
