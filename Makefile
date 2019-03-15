all: lib/cpu.cmd lib/mem.cmd lib/netif.cmd

lib/netif.cmd: src/netif.c
	$(CC) -Wall -Werror -pedantic -std=c11 -framework CoreFoundation -framework SystemConfiguration -framework CoreWLAN -Os src/netif.c -o lib/netif.cmd

lib/cpu.cmd: src/cpu.c
	$(CC) -Wall -Werror -pedantic -std=c11 -Os src/cpu.c -o lib/cpu.cmd

lib/mem.cmd: src/mem.c
	$(CC) -Wall -Werror -pedantic -std=c11 -Os src/mem.c -o lib/mem.cmd

clean:
	rm -f lib/cpu.cmd lib/mem.cmd lib/netif.cmd

.PHONY: all clean
