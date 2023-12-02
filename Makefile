all: app
clean:
	rm app
app: 4.c
	gcc 4.c -o app
