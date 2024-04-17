CFLAGS=-Wall -I ../include -I ./include
LDFLAGS=-lssl -lcrypto

# Source files
src=base64.c sign.c

slib=../build/edsrp_ed25519.a

# Build targets
test=sign_c

$(test): $(src) $(slib)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(test)
.PHONY: clean
