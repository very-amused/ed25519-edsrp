#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "base64.h"
#include "ed25519.h"

void cleanup(char **enc_sm, char **enc_pubkey) {
	free(*enc_sm);
	*enc_sm = NULL;
	free(*enc_pubkey);
	*enc_pubkey = NULL;
}

int main() {
	const size_t pubkey_len = 32;
	char *enc_sm = NULL;
	size_t enc_sm_len = 0;
	char *enc_pubkey = NULL;
	size_t enc_pubkey_len = 0;

	static const char message_path[] = "test/message.txt";
	FILE *f = fopen(message_path, "r");
	assert(f != NULL);
	getline(&enc_sm, &enc_sm_len, f);
	getline(&enc_pubkey, &enc_pubkey_len, f);
	fclose(f);

	// Trim message and pubkey strings
	do {
		enc_sm_len--;
	} while (enc_sm[enc_sm_len] != '\n' && enc_sm_len > 0);
	enc_sm[enc_sm_len] = '\0';

	if (enc_pubkey_len <= b64_encoded_len(pubkey_len)) {
		fprintf(stderr, "bad pubkey length\n");
		cleanup(&enc_sm, &enc_pubkey);
		return 1;
	}
	enc_pubkey_len = b64_encoded_len(pubkey_len);
	enc_pubkey[enc_pubkey_len] = '\0';

	// Decode message and pubkey
	unsigned char pubkey[pubkey_len];
	const size_t sm_len = b64_decoded_len(enc_sm, enc_sm_len);
	unsigned char sm[sm_len];
	assert(b64_decode(enc_sm, enc_sm_len, sm, sm_len) == 0);
	assert(b64_decode(enc_pubkey, enc_pubkey_len, pubkey, pubkey_len) == 0);
	if (ed25519_verify(sm, sm_len, pubkey) != 0) {
		fprintf(stderr, "Failed to verify message (ed25519_verify)\n");
		cleanup(&enc_sm, &enc_pubkey);
		return 1;
	}
	printf("\x1b[32m%s\x1b[0m\n", "Message signature is OK");

	cleanup(&enc_sm, &enc_pubkey);

	return 0;
}
