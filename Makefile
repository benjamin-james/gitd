CFLAGS =-Wall -pedantic -g -O2
all: gitd

gitd: gitd.c
	cc $(CFLAGS) gitd.c -o gitd -lSDL2
install: gitd
	cp gitd /usr/local/bin
clean:
	rm gitd
