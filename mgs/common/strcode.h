#pragma once

inline
unsigned int strcode(const char* string) {
	unsigned char c;
	unsigned char* p = (unsigned char*)string;
	unsigned int id, mask = 0x00FFFFFF;

	for (id = 0; c = *p; p++) {
		id = ((id >> 19) | (id << 5));
		id += c;
		id &= mask;
	}

	return (!id) ? 1 : id;
}

inline
unsigned int strcode32(const char* string) {
	unsigned int c;
	signed   int n = 0;
	unsigned int id = 0;

	while ((c = *string++)) {
		id += ((id << (c & 0x0F)) | ((id >> 3) + (c << (n & 0x0F)) + c));
		n++;
	}
	return id;
}