#pragma once

namespace Utilities
{
	typedef struct SBit8 {
		unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1;
	} Bit8;

	typedef struct SBit16 {
		unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1, b8 : 1, b9 : 1, b10 : 1, b11 : 1, b12 : 1, b13 : 1, b14 : 1, b15 : 1;
	} Bit16;

	typedef struct SBit32 {
		unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1, b8 : 1, b9 : 1, b10 : 1, b11 : 1, b12 : 1, b13 : 1, b14 : 1, b15 : 1,
			b16 : 1, b17 : 1, b18 : 1, b19 : 1, b20 : 1, b21 : 1, b22 : 1, b23 : 1, b24 : 1, b25 : 1, b26 : 1, b27 : 1, b28 : 1, b29 : 1, b30 : 1, b31 : 1;
	} Bit32;

	typedef struct SBit64 {
		unsigned b0 : 1, b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1, b8 : 1, b9 : 1, b10 : 1, b11 : 1, b12 : 1, b13 : 1, b14 : 1, b15 : 1,
			b16 : 1, b17 : 1, b18 : 1, b19 : 1, b20 : 1, b21 : 1, b22 : 1, b23 : 1, b24 : 1, b25 : 1, b26 : 1, b27 : 1, b28 : 1, b29 : 1, b30 : 1, b31 : 1,
			b32 : 1, b33 : 1, b34 : 1, b35 : 1, b36 : 1, b37 : 1, b38 : 1, b39 : 1, b40 : 1, b41 : 1, b42 : 1, b43 : 1, b44 : 1, b45 : 1, b46 : 1, b47 : 1,
			b48 : 1, b49 : 1, b50 : 1, b51 : 1, b52 : 1, b53 :  1, b54 : 1, b55 : 1, b56 : 1, b57 : 1, b58 : 1, b59 : 1, b60 : 1, b61 : 1, b62 : 1, b63 : 1;
	} Bit64;

	typedef struct SByte16 {
		UINT8 byte0;
		UINT8 byte1;
	} Byte16;

	typedef struct SByte32 {
		UINT8 byte0;
		UINT8 byte1;
		UINT8 byte2;
		UINT8 byte3;
	} Byte32;

	typedef struct SByte64 {
		UINT8 byte0;
		UINT8 byte1;
		UINT8 byte2;
		UINT8 byte3;
		UINT8 byte4;
		UINT8 byte5;
		UINT8 byte6;
		UINT8 byte7;
		UINT8 byte8;
	} Byte64;

	typedef struct SWord32 {
		UINT16 word0;
		UINT16 word1;
	} Word32;

	typedef struct SWord64 {
		UINT16 word0;
		UINT16 word1;
		UINT16 word2;
		UINT16 word3;
	} Word64;

	typedef struct SDWord64 {
		UINT32 dword0;
		UINT32 dword1;
	} DWord64;

	union UINT8_UT {
		Bit8 bit;
		UINT8 byte;
	};

	union UINT16_UT {
		Bit16 bit;
		SByte16 byte;
		UINT16 word;
	};

	union UINT32_UT {
		Bit32 bit;
		Byte32 byte;
		Word32 word;
		UINT32 dword;
		FLOAT real32;
	};

	union UINT64_UT {
		Bit64 bit;
		Byte64 byte;
		Word64 word;
		DWord64 dword;
		UINT64 uint64;
		double real64;
	};
}
