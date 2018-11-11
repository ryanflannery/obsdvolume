# build flags
CFLAGS  += -c -std=c89 -Wall -Wextra -Werror -O2
LDFLAGS +=

.c.o:
	$(CC) $(CFLAGS) $<

volume: volume.o
	$(CC) -o $@ $(LDFLAGS) volume.o

clean:
	rm -f volume.o volume
