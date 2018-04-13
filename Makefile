CFLAGS=-Wall -g

clean:
	rm -f statserve
	rm -rf `find . -name "*.dSYM" -print`
