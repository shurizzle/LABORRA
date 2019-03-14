all: lib/cpu.cmd lib/mem.cmd

lib/cpu.cmd: src/cpu.c
	$(CC) -Wall -Werror -pedantic -std=c11 -Os src/cpu.c -o lib/cpu.cmd

lib/mem.cmd: src/mem.c
	$(CC) -Wall -Werror -pedantic -std=c11 -Os src/mem.c -o lib/mem.cmd

clean:
	rm -f lib/cpu.cmd lib/mem.cmd

.PHONY: all clean
