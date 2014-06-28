all : server.out client.out
.PHONY : all

server.out: libstubs.a ece454_server_app ece454_test_functions
	gcc ece454_server_app ece454_test_functions -L. -lstubs -lstdc++ -o server.out

client.out: libstubs.a ece454_client_app
	gcc ece454_client_app -L. -lstubs -lstdc++ -o client.out

libstubs.a:
	$(MAKE) libstubs.a

clean:
	$(MAKE) clean
	rm -rf *.out *~
