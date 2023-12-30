CFLAGS=-Wall -Werror -g

TARGETS = readFirewall checkPacket

all: $(TARGETS)

%.o: %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(TARGETS) *.o
