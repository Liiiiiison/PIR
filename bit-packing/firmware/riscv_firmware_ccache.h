#pragma once

#include <stdint.h>

#define STR1(x) #x
#define STR(x) STR1(x)

#define CCACHE_OPCODE 0b0001011
#define CSET_FUNCT3 0b000
#define CNOT_FUNCT3 0b001
#define CCPY_FUNCT3 0b010
#define COR_FUNCT3  0b011
#define CXOR_FUNCT3 0b100
#define CAND_FUNCT3 0b101

#define CCACHE_GEN_INST(funct3,rd,rs1,rs2,size) \
	(CCACHE_OPCODE         | \
	rd     << (7)          | \
	funct3 << (7+5)        | \
	rs1    << (7+5+3)      | \
	rs2    << (7+5+3+5)    | \
	size   << (7+5+3+5+5))

#define CCACHE_GEN_INLINE_ASSEMBLY_D(funct3,rd,size) {\
	asm volatile("move x10,%[_rd];" \
	".word" STR(CCACHE_GEN_INST(funct3, 10, 0, 0, size)) "\t\n" \
	:: [_rd] "r" (rd): "memory","x10");}

#define CCACHE_GEN_INLINE_ASSEMBLY_DS(funct3,rd,rs1,size) { \
	asm volatile("move x10,%[_rd];" \
	             "move x11,%[_rs1];" \
	".word" STR(CCACHE_GEN_INST(funct3, 10, 11, 0, size)) "\t\n" \
	:: [_rd] "r" (rd), [_rs1] "r" (rs1): "memory","x10","x11"); }
#define CCACHE_GEN_INLINE_ASSEMBLY_DSS(funct3,rd,rs1,rs2,size) { \
	asm volatile("move x10,%[_rd];" \
	             "move x11,%[_rs1];" \
	             "move x12,%[_rs2];" \
	".word" STR(CCACHE_GEN_INST(funct3, 10, 11, 12, size)) "\t\n" \
	:: [_rd] "r" (rd), [_rs1] "r" (rs1), [_rs2] "r" (rs2): "memory","x10","x11","x12"); }

#define CSET(rd, imm, size)      CCACHE_GEN_INLINE_ASSEMBLY_DS(CSET_FUNCT3, rd, imm, size)
#define CNOT(rd, rs1, size)      CCACHE_GEN_INLINE_ASSEMBLY_DS(CNOT_FUNCT3, rd, rs1, size)
#define CCPY(rd, rs1, size)      CCACHE_GEN_INLINE_ASSEMBLY_DS(CCPY_FUNCT3, rd, rs1, size)

#define COR(rd, rs1, rs2, size)  CCACHE_GEN_INLINE_ASSEMBLY_DSS(COR_FUNCT3 , rd, rs1, rs2, size)
#define CXOR(rd, rs1, rs2, size) CCACHE_GEN_INLINE_ASSEMBLY_DSS(CXOR_FUNCT3, rd, rs1, rs2, size)
#define CAND(rd, rs1, rs2, size) CCACHE_GEN_INLINE_ASSEMBLY_DSS(CAND_FUNCT3, rd, rs1, rs2, size)

