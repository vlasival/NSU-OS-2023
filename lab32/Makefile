all: server client
server: server.c
	gcc server.c -o server -lsocket -lnsl
client: client.c
	gcc client.c -o client -lsocket -lnsl
clean:
	rm server client
