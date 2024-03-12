#pragma once
#include <stddef.h>

// Get the exact # of chars needed to encode len bytes as base64
const size_t b64_encoded_len(const size_t len);
// Get the exact # of chars needed to encode len bytes as base64 without padding
const size_t b64_nopadding_encoded_len(const size_t len);
// Get the exact # of bytes needed to decode encoded_len bytes of base64
const size_t b64_decoded_len(const char *encoded, const size_t encoded_len);

// Encode binary data as to a base64 string.
// Use b64_encoded_len() to get length for result buffer.
// No null termination is performed.
int b64_encode(const unsigned char *data, const size_t data_len,
		char *result, const size_t result_len);
// Encode binary data as to a base64 string.
// Use b64_nopadding_encoded_len() to get length for result buffer.
// No null termination is performed.
int b64_nopadding_encode(const unsigned char *data, const size_t data_len,
		char *result, const size_t result_len);
// Decode a base64 string to binary data.
// Use b64_decoded_len() to get length for the result buffer.
// No null termination is performed.
int b64_decode(const char *encoded, const size_t encoded_len,
		unsigned char *result, const size_t result_len);
