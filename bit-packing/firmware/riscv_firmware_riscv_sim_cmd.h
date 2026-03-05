#pragma once
#include <stdint.h>
#include <stdio.h>
#include "riscv_sim_codes.h"

#define STR1(x) #x
#define STR(x) STR1(x)

#define RISCV_SIM_CMD_REG(rd,funct) {                           \
    volatile uint32_t rd_ = (uint32_t) rd;    \
    asm volatile (                                              \
        "add a0, %0, x0\t\n"      \
        ".insn r " STR(RISCV_SIM_CMD_OPCODE) ", 0, " STR(funct) \
        ", a0, x0, x0"          \
        :                                                       \
        : "r" (rd_) : "a0");                                     \
  }

#define RISCV_SIM_CMD_MEM(rd,funct) {                        \
    volatile uint32_t rd_ = (uint32_t) rd;  \
    asm volatile (                                              \
        "addi a0, %0,0\t\n"      \
        ".insn r " STR(RISCV_SIM_CMD_OPCODE) ", 0, " STR(funct) \
        ", a0, x0, x0"          \
        :                                                       \
        : "r" (rd_) : "a0");                                     \
  }

#define RISCV_SIM_CMD_INST(funct) { \
  asm volatile (".insn r " STR(RISCV_SIM_CMD_OPCODE) ", 0, " STR(funct) ", x0, x0, x0 \n\t"); \
  }

#define riscv_sim_show_registers     RISCV_SIM_CMD_INST(RISCV_SIM_SHOW_REGISTERS_FUNCT)
#define riscv_sim_show_vregisters    RISCV_SIM_CMD_INST(RISCV_SIM_SHOW_VREGISTERS_FUNCT)
#define riscv_sim_show_cycles        RISCV_SIM_CMD_INST(RISCV_SIM_SHOW_CYCLES_FUNCT)
#define riscv_sim_reset_hpc          RISCV_SIM_CMD_INST(RISCV_SIM_RESET_HPC_FUNCT)
#define riscv_sim_show_hpc           RISCV_SIM_CMD_INST(RISCV_SIM_SHOW_HPC_FUNCT)
#define riscv_sim_print_int(rd)      RISCV_SIM_CMD_REG(rd, RISCV_SIM_PRINT_INT_FUNCT)
#define riscv_sim_print_str(rd)      RISCV_SIM_CMD_REG(rd, RISCV_SIM_PRINT_STR_FUNCT)
#define riscv_sim_tracing_toggle     RISCV_SIM_CMD_INST(RISCV_TRACING_TRIGGER_TOGGLE)

