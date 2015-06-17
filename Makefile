CFLAGS =-Wall -pedantic -g -O2
all: gitd

gitd: gitd.c
	cc $(CFLAGS) gitd.c -o gitd
install: gitd
	cp gitd /usr/bin
	test -d /usr/share/gitd || mkdir /usr/share/gitd
	test -d /etc/gitd || mkdir /etc/gitd
	cp default.conf /etc/gitd/gitd.conf
	cp gitd.8 /usr/share/man/man8/gitd.8
	gzip /usr/share/man/man8/gitd.8
clean:
	rm gitd
