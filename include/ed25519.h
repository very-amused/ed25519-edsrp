int ed25519_keypair(const unsigned char *seed, unsigned char *pk, unsigned char *sk);
int ed25519_sign(unsigned char *sm, const unsigned char *m, unsigned long mlen, const unsigned char *sk);
int ed25519_verify(unsigned char *sm, const unsigned long long smlen, const unsigned char *pk);
