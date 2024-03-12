O=-O2
CFLAGS=$(O) -Wall -I ed25519

src=ed25519/keypair.c ed25519/sign.c \
	ed25519/ge.c ed25519/fe.c ed25519/sc_muladd.c ed25519/sc_reduce.c \
	ed25519/sha512/hash.c ed25519/sha512/blocks/blocks.c ed25519/verify32/verify.c
objects=$(src:.c=.o)
objects-wasm=$(src:.c=.wasm.o)

outdir=build
$(shell if [ ! -d $(outdir) ]; then mkdir $(outdir); fi)

# Build targets
ed25519=$(outdir)/ed25519_edsrp.a
ed25519_wasm=$(outdir)/ed25519_edsrp.wasm
all=$(ed25519) $(ed25519_wasm)

$(ed25519): $(objects)
	$(AR) rcs $@ $^

prepare:
	if [ ! -d ed25519 ]; then $(SHELL) prepare.sh; fi
.PHONY: prepare

clean:
	rm -rf $(objects) $(objects-wasm) $(all) ed25519
.PHONY: clean
