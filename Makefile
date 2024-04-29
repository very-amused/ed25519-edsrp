# Build options
O=-O3
override CFLAGS=$(O) -Wall -I ed25519
WASM_CFLAGS=$(CFLAGS) --no-entry
WASM_EXPORTED_FUNCTIONS=_malloc,_free,_ed25519_keypair,_ed25519_sign,_ed25519_verify
WASM_LDFLAGS=-s EXPORTED_FUNCTIONS=$(WASM_EXPORTED_FUNCTIONS)

# Prefer building with clang if available
ifneq (,$(shell which clang))
override CC=clang
endif

# Installation vars
ifndef PREFIX
PREFIX=/usr/local
endif

# Source and object files
src=ed25519/keypair.c ed25519/sign.c ed25519/open.c \
	ed25519/ge.c ed25519/fe.c ed25519/sc_muladd.c ed25519/sc_reduce.c \
	ed25519/sha512/hash.c ed25519/sha512/blocks/blocks.c ed25519/verify32/verify.c
objects=$(src:.c=.o)
objects-pic=$(src:.c=.pic.o)
objects-wasm=$(src:.c=.wasm.o)

# Ensure output dir exists
outdir=build
$(shell if [ ! -d $(outdir) ]; then mkdir $(outdir); fi)

# Build targets
ed25519=$(outdir)/edsrp_ed25519.a
ed25519_so=$(outdir)/edsrp_ed25519.so
ed25519_wasm=$(outdir)/edsrp_ed25519.wasm
ed25519_all=$(ed25519) $(ed25519_wasm)

# Prefix helper for testing
prefix=sed -e 's/^/\x1b[1m[$@]\x1b[0m /'

release: ed25519 so
.PHONY: release

all: ed25519 wasm so
.PHONY: all

ed25519: $(ed25519)
.PHONY: ed25519

so: $(ed25519_so)
.PHONY: ed25519_so

wasm: $(ed25519_wasm)
.PHONY: wasm

$(ed25519): $(objects)
	$(AR) rcs $@ $^

$(ed25519_so): $(objects-pic)
	$(CC) -shared -o $@ $^

$(ed25519_wasm): CC=emcc
$(ed25519_wasm): $(objects-wasm)
	$(CC) -o $@ $^ $(WASM_LDFLAGS) $(WASM_CFLAGS)

install: $(ed25519) $(ed25519_so) include/ed25519.h
	install -d $(DESTDIR)$(PREFIX)/lib
	install -m644 $(ed25519) $(DESTDIR)$(PREFIX)/lib/libedsrp_ed25519.a
	install -m644 $(ed25519_so) $(DESTDIR)$(PREFIX)/lib/libedsrp_ed25519.so
	install -d $(DESTDIR)$(PREFIX)/include
	install -m644 include/ed25519.h $(DESTDIR)$(PREFIX)/include/edsrp_ed25519.h
.PHONY: install

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/libedsrp_ed25519.a
	rm -f $(DESTDIR)$(PREFIX)/lib/libedsrp_ed25519.so
	rm -f $(DESTDIR)$(PREFIX)/include/edsrp_ed25519.h
.PHONY: uninstall

ed25519/%.o: ed25519/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

ed25519/%.pic.o: CFLAGS += -fPIC
ed25519/%.pic.o: ed25519/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

ed25519/%.wasm.o: CC=emcc
ed25519/%.wasm.o: ed25519/%.c
	$(CC) -c -o $@ $< $(WASM_CFLAGS)

prepare:
	if [ ! -d ed25519 ]; then $(SHELL) prepare.sh; fi
.PHONY: prepare

test: all .WAIT test-wasm .WAIT test-c .WAIT test-c-verify
.PHONY: test

test-wasm:
	node test/sign.mjs | $(prefix)
	go run ./test/verify | $(prefix)
.PHONY: test-wasm

test-c:
	@cd test; $(MAKE) | $(prefix)
	./test/sign_c | $(prefix)
	go run ./test/verify | $(prefix)
.PHONY: test-c

test-c-verify:
	@cd test; $(MAKE) | $(prefix)
	./test/sign_c | $(prefix)
	./test/verify_c | $(prefix)
.PHONY: test-c-verify

clean:
	rm -rf $(objects) $(objects-wasm) $(ed25519_all) ed25519 build
	cd test; $(MAKE) clean
.PHONY: clean
