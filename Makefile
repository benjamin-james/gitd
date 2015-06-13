CFLAGS = -Wall -ansi -pedantic -g -O2 -D_POSIX_SOURCE
all: gitd

gitd: gitd.c
	cc $(CFLAGS) gitd.c -o gitd
install: gitd
	cp gitd /usr/local/bin
clean:
	rm gitd
