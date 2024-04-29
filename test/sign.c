#include <stdio.h>
#include <openssl/rand.h>
#include "base64.h"
#include "ed25519.h"

int randbytes(unsigned char *msg, const size_t msg_len) {
	return RAND_priv_bytes(msg, msg_len) != 1;
}

int main() {
	printf("Generating random message\n");
	const size_t message_len = 32;
	unsigned char message[message_len];
	if (randbytes(message, message_len) != 0) {
		fprintf(stderr, "Failed to generate random message\n");
		return 1;
	}

	printf("Generating ed25519 keypair\n");
	const size_t seed_len = 32;
	unsigned char seed[seed_len];
	if (randbytes(seed, seed_len) != 0) {
		fprintf(stderr, "Failed to generate random keypair seed\n");
		return 1;
	}

	const size_t pubkey_len = 32;
	unsigned char pubkey[pubkey_len];
	const size_t privkey_len = 64;
	unsigned char privkey[privkey_len];
	if (ed25519_keypair(seed, pubkey, privkey) != 0) {
		fprintf(stderr, "Failed to derive ed25519 keypair from seed (ed25519_keypair)\n");
		return 1;
	}

	printf("Signing message\n");
	const size_t sm_len = message_len + 64;
	unsigned char sm[sm_len];
	if (ed25519_sign(sm, message, message_len, privkey) != 0) {
		fprintf(stderr, "Failed to sign message (ed25519_sign)\n");
		return 1;
	}

	// Encode signed message and pubkey, then write to disk
	const size_t enc_sm_len = b64_encoded_len(sm_len);
	char enc_sm[enc_sm_len + 1];
	enc_sm[enc_sm_len] = '\0'; // Perform null termination for safe use in fprintf
	b64_encode(sm, sm_len, enc_sm, enc_sm_len);

	const size_t enc_pubkey_len = b64_encoded_len(pubkey_len);
	char enc_pubkey[enc_pubkey_len + 1];
	enc_pubkey[enc_pubkey_len] = '\0';
	b64_encode(pubkey, pubkey_len, enc_pubkey, enc_pubkey_len);

	FILE *f = fopen("test/message.txt", "w");
	fprintf(f, "%s\n%s", enc_sm, enc_pubkey);
	fclose(f);

	return 0;
}
