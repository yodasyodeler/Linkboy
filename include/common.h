#pragma once
#include <stdint.h>

struct regset {
	struct {
		union {
			struct {
				uint8_t f;
				uint8_t a;
			};
			uint16_t af;
		};
	};
	struct {
		union {
			struct {
				uint8_t c;
				uint8_t b;
			};
			uint16_t bc;
		};
	};
	struct {
		union {
			struct {
				uint8_t e;
				uint8_t d;
			};
			uint16_t de;
		};
	};
	struct {
		union {
			struct {
				uint8_t l;
				uint8_t h;
			};
			uint16_t hl;
		};
	};
	uint16_t sp;
	uint16_t pc;
};

struct debug {
	regset oldReg;
	regset reg;
	uint32_t opcodeIndex;
};

bool convertString(int& num, const char* buf, const int base);

int lb_strlen(const char* str);

void lb_strcpy(char* str1, const char* str2);

int lb_strncmp(const char* str1, const char* str2, int n);

void lb_strncat(char* str1, const char* str2, int n);