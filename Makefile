CFLAGS=-Wall -g

clean:
	rm -f raptor
	rm -rf `find . -name "*.dSYM" -print`
