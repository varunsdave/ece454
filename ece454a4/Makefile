OBJECTS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJECTS2 := $(patsubst %.c, simplified_rpc/%.o, $(wildcard simplified_rpc/*.c))

all: fs_client

fs_client: fs_client.o fs_dummy.o libstubs.a

libstubs.a: simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o
	ar r libstubs.a simplified_rpc/server_stub.o simplified_rpc/client_stub.o simplified_rpc/helper.o simplified_rpc/mybind.o

$(OBJECTS): %.o: %.c ece454_fs.h
	gcc -c $< -o $@

$(OBJECTS2): simplified_rpc/%.o: simplified_rpc/%.c
	gcc -c $< -o $@

clean:
	rm -rf *.o core *.a fs_client simplified_rpc/*.a simplified_rpc/*.o
