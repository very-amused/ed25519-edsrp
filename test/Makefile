CFLAGS=-Wall -I ../include -I ./include
LDFLAGS=-lssl -lcrypto

# Prefer clang if available
ifneq (,$(shell which clang))
override CC=clang
endif

# Source files
src-sign=sign.c
src-verify=verify.c
lib-b64=base64.c

slib=../build/edsrp_ed25519.a

targets=sign_c verify_c

# Build targets
all: $(targets)
.PHONY: all

sign_c: $(src-sign) $(lib-b64) $(slib)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

verify_c: $(src-verify) $(lib-b64) $(slib)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(targets)
.PHONY: clean
