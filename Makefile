all: shut-term-keys

shut-term-keys: shut-term-keys.c
	gcc -std=gnu99 -O2 -fvisibility=hidden -Wall -Wextra -pedantic shut-term-keys.c -o shut-term-keys

clean:
	rm -f shut-term-keys shut-term-keys.o
