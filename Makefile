CFLAGS =-Wall -pedantic -g -O2
all: gitd

gitd: gitd.c
	cc $(CFLAGS) gitd.c -o gitd
install: gitd
	cp gitd /usr/local/bin
	test -d /usr/share/gitd || mkdir /usr/share/gitd
	test -d /etc/gitd || mkdir /etc/gitd
	cp gitd.conf /etc/gitd/gitd.conf
	cp gitd.man /usr/local/man/man1/gitd.1
	gzip /usr/local/man/man1/gitd.1
clean:
	rm gitd
