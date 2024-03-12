# Build options
O=-O2
CFLAGS=$(O) -Wall -I ed25519 -flto
WASM_CFLAGS=$(CFLAGS) --no-entry
WASM_EXPORTED_FUNCTIONS=_malloc,_free,_ed25519_keypair,_ed25519_sign
WASM_LDFLAGS=-s EXPORTED_FUNCTIONS=$(WASM_EXPORTED_FUNCTIONS)


# Source and object files
src=ed25519/keypair.c ed25519/sign.c \
	ed25519/ge.c ed25519/fe.c ed25519/sc_muladd.c ed25519/sc_reduce.c \
	ed25519/sha512/hash.c ed25519/sha512/blocks/blocks.c ed25519/verify32/verify.c
objects=$(src:.c=.o)
objects-wasm=$(src:.c=.wasm.o)

# Ensure output dir exists
outdir=build
$(shell if [ ! -d $(outdir) ]; then mkdir $(outdir); fi)

# Build targets
ed25519=$(outdir)/ed25519_edsrp.a
ed25519_wasm=$(outdir)/ed25519_edsrp.wasm
ed25519_all=$(ed25519) $(ed25519_wasm)

# Prefix helper for testing
prefix=sed -e 's/^/\x1b[1m[$@]\x1b[0m /'

all: $(ed25519_all)
.PHONY: all

$(ed25519): $(objects)
	$(AR) rcs $@ $^

$(ed25519_wasm): $(objects-wasm)
	emcc -o $@ $^ $(WASM_LDFLAGS) $(WASM_CFLAGS)

ed25519/%.o: ed25519/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

ed25519/%.wasm.o: ed25519/%.c
	emcc -c -o $@ $< $(WASM_CFLAGS)

prepare:
	if [ ! -d ed25519 ]; then $(SHELL) prepare.sh; fi
.PHONY: prepare

test: test-wasm .WAIT test-c
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

clean:
	rm -rf $(objects) $(objects-wasm) $(ed25519_all) ed25519
	cd test; $(MAKE) clean
.PHONY: clean
