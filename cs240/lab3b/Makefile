
all: printargs convert

convert: convert.c
	gcc -g -o convert convert.c

printargs: printargs.c
	gcc -g -o printargs printargs.c

org: convert
	cp convert convert.org


clean:
	rm -f printargs convert *out
