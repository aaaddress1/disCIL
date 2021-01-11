// disCIL by aaaddress1@chroot.org
// .NET Disassembler Written In C/C++
#include <string>
#include <stdio.h>
#include <sstream>
using namespace std;
#pragma warning(disable : 4996)

/* Poping */
/* 1 bit */
#define Pop0    1
/* 2 bits */
#define Pop1    2
/* 3 bits */
#define PopI    8
/* 1 bit */
#define PopI8   64
/* 1 bit */
#define Pop8    128
/* 1 bit */
#define PopR4   256
/* 1 bit */
#define PopR8   512
/* 1 bit */
#define PopRef  1024
/* 1 bit */
#define VarPop  2048

/* Pushing */
#define Push0   1
#define PushI   2
#define PushI8  4
#define PushR4  8
#define PushR8  16
#define PushRef 32
#define VarPush 64
#define Push1   128

enum {
	InlineBrTarget = 0,
	InlineField = 1,
	InlineI = 2,
	InlineI8 = 3,
	InlineMethod = 4,
	InlineNone = 5,
	InlineR = 6,
	InlineSig = 7,
	InlineString = 8,
	InlineSwitch = 9,
	InlineTok = 10,
	InlineType = 11,
	InlineVar = 12,
	ShortInlineBrTarget = 13,
	ShortInlineI = 14,
	ShortInlineR = 15,
	ShortInlineVar = 16,
};

#define OPDEF(a,b,c,d,e,f,g,h,i,j) \
	{ b, c, d, e, g, h, i },

typedef struct {
	const char* name;
	int   pop, push;
	int   argument;
	int   bytes;
	unsigned char  o1, o2;
} opcode_t;

static opcode_t opcodes[] = {
#include "opcode.def"
};


size_t disCIL(char* ptrToBytecode, string &sz_Opcode, string &sz_Args) {
	sz_Opcode = opcodes[*ptrToBytecode].name;
	auto insType = opcodes[*ptrToBytecode].argument;
	size_t insLength = 1, tokenId = 0;
	stringstream tmp_szArgs;

	switch (insType) {
	case InlineNone: break;
	case InlineType:
	case InlineField:
	case InlineMethod:
	case InlineTok:
	case InlineSig:
		tokenId = *(uint32_t*)(ptrToBytecode + insLength);
		tmp_szArgs << string("signature-0x") << hex << tokenId;
		insLength += 4;
		break;
	case InlineString:
		tokenId = *(uint32_t*)(ptrToBytecode + insLength);
		tmp_szArgs << string("#US_0x") << hex << tokenId;
		insLength += 4;
		break;
	case InlineVar:
		tmp_szArgs << *(uint16_t*)(ptrToBytecode + insLength);
		insLength += 2;
		break;
	case ShortInlineVar:
		tmp_szArgs << *(uint8_t*)(ptrToBytecode + insLength);
		insLength++;
		break;
	case InlineBrTarget:
		insLength += 4;
		break;
	case ShortInlineBrTarget:
		tmp_szArgs << *(uint8_t*)(ptrToBytecode + insLength);
		insLength++;
		break;
	case InlineSwitch: {
		insLength += 4;
		break;
	}
	case InlineR: {
		insLength += 8;
		break;
	}
	case ShortInlineR: {
		insLength += 4;
		break;
	}
	case InlineI:
		tmp_szArgs << "0x" << hex << *(uint32_t*)(ptrToBytecode + insLength);
		insLength += 4;
		break;
	case ShortInlineI:
		tmp_szArgs << *(ptrToBytecode + insLength);
		insLength++;
		break;
	case InlineI8:
		insLength += 8;
		break;
	default:
		throw;
	}
	sz_Args = string(tmp_szArgs.str());
	return insLength;
}

int main(void) {
	char cil_Bytecode[] = {
		0x00, 0x15, 0x0A, 0x06, 0x20, 0xEF, 0xBE, 0xAD, 0xDE, 0x58, 0x0A, 0x06, 0x20, 0xFE, 0xCA, 0xFE,
		0xCA, 0x61, 0x0A, 0x12, 0x00, 0x28, 0x03, 0x00, 0x00, 0x0A, 0x72, 0x01, 0x00, 0x00, 0x70, 0x28,
		0x04, 0x00, 0x00, 0x0A, 0x26, 0x2A
	};

	string op = "", args = "";
	for (size_t indx = 0, offset = 0; indx < sizeof(cil_Bytecode); indx += offset) {
		offset = disCIL(&cil_Bytecode[indx], op, args);
		printf("%.4x - %s %s\n", indx, op.c_str(), args.c_str());
	}
	return 0;

}