/* very-amused/b64 - Copyright (c) 2024 Keith Scroggs */
#include <stdint.h>
#include <stddef.h>
#include "base64.h"

static const char STD_ENCODING[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char PADDING = '=';

const size_t b64_encoded_len(const size_t len) {
	return ((len / 3) * 4) + (len % 3 > 0 ? 4 : 0);
}

const size_t b64_nopadding_encoded_len(const size_t len) {
	return ((len / 3) * 4) + (len % 3 > 0 ? (len % 3) + 1 : 0);
}

static inline size_t b64_decoded_maxlen(const size_t encoded_len) {
	return ((encoded_len / 4) * 3) + // Each full group of 4 encoded -> 3 bytes
		(encoded_len % 4 > 0 ? encoded_len % 4 - 1 : 0); // Add trailing bytes
}

// Reduce encoded_len to exclude end padding characters
static void b64_trim_padding(const char *encoded, size_t *encoded_len) {
	for (size_t i = *encoded_len; i >= *encoded_len - 2; i--) {
		if (encoded[i] == PADDING) {
			(*encoded_len)--;
		}
	}
}

const size_t b64_decoded_len(const char *encoded, const size_t encoded_len) {
	size_t decoded_len = b64_decoded_maxlen(encoded_len);
	b64_trim_padding(encoded, &decoded_len);
	return decoded_len;
}

// Encode up to 3 bytes from src into dst
static void encode_group(const unsigned char *data, const uint8_t data_len, char *result) {
	// Byte 1
	result[0] = STD_ENCODING[data[0] >> 2];

	// Byte 2
	if (data_len == 1) {
		result[1] = STD_ENCODING[data[0] << 4 & 0x30];
		return;
	}
	result[1] = STD_ENCODING[(data[0] << 4 & 0x30) | data[1] >> 4];

	// Byte 3
	if (data_len == 2) {
		result[2] = STD_ENCODING[data[1] << 2 & 0x3C];
		return;
	}
	result[2] = STD_ENCODING[(data[1] << 2 & 0x3C) | data[2] >> 6];

	// Byte 4
	result[3] = STD_ENCODING[data[2] & 0x3F];
}

// Base64 ASCII to index.
// Decode a single character of base64 to 6 bits [0, 63].
// Returns 1 if an invalid input character is supplied
static int _atoi(const char encoded, unsigned char *index);

// Decode up to 4 base64 characters to result
static int decode_group(const char *encoded, unsigned char *result, const uint8_t result_len) {
	unsigned char indexes[result_len + 1];
	for (uint8_t i = 0; i < result_len + 1; i++) {
		if (_atoi(encoded[i], &indexes[i]) != 0) {
			return 1;
		}
	}

	// Byte 1
	result[0] = indexes[0] << 2 | indexes[1] >> 4;
	if (result_len == 1) {
		return 0;
	}

	// Byte 2
	result[1] = indexes[1] << 4 | indexes[2] >> 2;
	if (result_len == 2) {
		return 0;
	}

	// Byte 3
	result[2] = indexes[2] << 6 | indexes[3];
	return 0;
}

static int _atoi(const char encoded, unsigned char *index) {
		if (encoded >= '0' && encoded <= '9') {
			// 0-9
			*index = encoded + 4;
		} else if (encoded >= 'A' && encoded <= 'Z') {
			// A-Z
			*index = encoded - 65;
		} else if (encoded >= 'a' && encoded <= 'z') {
			// a-z
			*index = encoded - 71;
		} else if (encoded == '+') {
			// +
			*index = 62;
		} else if (encoded == '/') {
			// /
			*index = 63;
		} else {
			// Invalid character
			return 1;
		}
		return 0;
}

int b64_encode(const unsigned char *data, const size_t data_len, char *result, const size_t result_len) {
	if (result_len != b64_encoded_len(data_len)) {
		return 1;
	}

	size_t data_offset = 0, // Offset from beinning of data
				result_offset = 0; // Offset from beginning of result
	// Encode each full group of 3 bytes -> 4 chars
	for (; data_offset < data_len - 2; data_offset += 3, result_offset += 4) {
		encode_group(data + data_offset, 3, result + result_offset);
	}

	// Encode trailing bytes and add padding
	switch (data_len % 3) {
	case 1:
		encode_group(data + (data_len - 1), 1, result + (result_len - 4));
		result[result_len - 2] = PADDING;
		result[result_len - 1] = PADDING;
		break;

	case 2:
		encode_group(data + (data_len - 2), 2, result + (result_len - 4));
		result[result_len - 1] = PADDING;
	}

	return 0;
}

int b64_nopadding_encode(const unsigned char *data, const size_t data_len, char *result, const size_t result_len) {
	if (result_len != b64_nopadding_encoded_len(data_len)) {
		return 1;
	}

	size_t data_offset = 0, // Offset from beinning of data
				result_offset = 0; // Offset from beginning of result
	// Encode each full group of 3 bytes -> 4 chars
	for (; data_offset < data_len - 2; data_offset += 3, result_offset += 4) {
		encode_group(data + data_offset, 3, result + result_offset);
	}

	// Encode trailing bytes
	switch (data_len % 3) {
	case 1:
		encode_group(data + (data_len - 1), 1, result + (result_len - 2));
		break;

	case 2:
		encode_group(data + (data_len - 2), 2, result + (result_len - 3));
	}

	return 0;
}

int b64_decode(const char *encoded, size_t encoded_len, unsigned char *result, const size_t result_len) {
	if (result_len != b64_decoded_len(encoded, encoded_len)) {
		return 1;
	}
	// Trim padding from end by reducing encoded_len
	b64_trim_padding(encoded, &encoded_len);

	size_t encoded_offset = 0, // Offset from beinning of encoded data
				result_offset = 0; // Offset from beginning of result
	// Decode each full group of 4 chars -> 3 bytes
	for (; encoded_offset < encoded_len - 3; encoded_offset += 4, result_offset += 3) {
		if (decode_group(encoded + encoded_offset, result + result_offset, 3) != 0) {
			return 1;
		}
	}

	// Decode trailing chars
	switch (encoded_len % 4) {
	case 2:
		if (decode_group(encoded + (encoded_len - 2), result + (result_len - 1), 1) != 0) {
			return 1;
		}
		break;
	
	case 3:
		if (decode_group(encoded + (encoded_len - 3), result + (result_len - 2), 2) != 0) {
			return 1;
		}
	}

	return 0;
}
