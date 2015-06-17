CFLAGS =-Wall -pedantic -g -O2
all: gitd

gitd: gitd.c
	cc $(CFLAGS) gitd.c -o gitd
install: gitd
	cp gitd /usr/local/bin
	test -d /usr/share/gitd || mkdir /usr/share/gitd
	test -d /etc/gitd || mkdir /etc/gitd
	cp gitd.conf /etc/gitd/gitd.conf	
clean:
	rm gitd
