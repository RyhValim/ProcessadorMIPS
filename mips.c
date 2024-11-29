#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define QTD_REGISTRADORES 34
#define QTD_INSTRUCOES 9
#define TAM_FUNCT 5
#define TAM_INSTRUCAO 30

int pc = 0;

// Mapeamento de opcodes e functs
typedef struct {
    char nome[8];
    int opcode;
    int funct;
} Instrucao;

const Instrucao instrucoes[QTD_INSTRUCOES] = {
    {"add", 0, 32}, {"sub", 0, 34}, {"mul", 0, 24}, {"div", 0, 26},
    {"and", 0, 36}, {"lw", 35, -1}, {"sw", 43, -1}, {"li", -1, -1}, {"j", 2, -1}
};

void tabelaRegs(const int *regs, const char (*nomeRegs)[8]) {
    printf("\n== TABELA REGISTRADORES ==\n");
    printf("ID  |   NOME  | VALOR\n");

    for (int i = 0; i < QTD_REGISTRADORES; i++) {
        if (i < QTD_REGISTRADORES - 2) {
            printf("%-4d|   %-5s |   %-4d\n", i, nomeRegs[i], regs[i]);
        } else {
            printf("    |   %-5s |   %-4d\n", nomeRegs[i], regs[i]);
        }
    }
}

int acharFunct(const char *instrucao, const Instrucao *instrucoes, char *funct, int *opcode, int *functCode) {
    char instrucaoLower[TAM_INSTRUCAO];
    strncpy(instrucaoLower, instrucao, TAM_INSTRUCAO);
    instrucaoLower[TAM_INSTRUCAO - 1] = '\0';

    for (int i = 0; instrucaoLower[i]; i++) {
        instrucaoLower[i] = tolower(instrucaoLower[i]);
    }

    for (int i = 0; i < QTD_INSTRUCOES; i++) {
        if (strstr(instrucaoLower, instrucoes[i].nome) != NULL) {
            strcpy(funct, instrucoes[i].nome);
            *opcode = instrucoes[i].opcode;
            *functCode = instrucoes[i].funct;
            return 1;
        }
    }
    return 0;
}

void imprimirFormatoInstrucao(int opcode, const char *rs, const char *rt, const char *rd, int shamt, int functCode, const char *funct, int offset, int enderecoJump) {
    printf("\n== FORMATO DA INSTRUCAO ==\n");
    printf("OPCODE: %d\n", opcode);
    if (opcode == 2) { // Formato J
        printf("ENDERECO: %d\n", enderecoJump);
    } else {
        printf("RS:     %s\nRT:     %s\nRD:     %s\n", rs, rt, rd);
        printf("SHAMT:  %d\n", shamt);
        printf("OFFSET: %d\n", offset);
    }
    printf("FUNCT:  %d (%s)\n", functCode, funct);
    printf("PC:     %d\n", pc);
}

int main() {
    int *regs = (int *)calloc(QTD_REGISTRADORES, sizeof(int));
    if (!regs) {
        perror("Falha ao alocar memória");
        return 1;
    }

    const char nomeRegs[QTD_REGISTRADORES][8] = {
        "$0 ", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0",
        "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1",
        "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0",
        "$k1", "$gp", "$sp", "$fp", "$ra", "$hi", "$lo"
    };

    printf("As instrucoes disponiveis sao: ");
    for (int i = 0; i < QTD_INSTRUCOES; i++) {
        printf("'%s'%s", instrucoes[i].nome, i < QTD_INSTRUCOES - 1 ? ", " : "\n");
    }

    char instrucao[TAM_INSTRUCAO], funct[TAM_FUNCT] = "";
    int opcode, functCode;

    while (1) {
        printf("\nEscreva a sua instrucao do MIPS: ");
        fgets(instrucao, TAM_INSTRUCAO, stdin);
        instrucao[strcspn(instrucao, "\n")] = '\0';

        if (!acharFunct(instrucao, instrucoes, funct, &opcode, &functCode)) {
            printf("Instrucao invalida. Tente novamente.\n");
        } else {
            break;
        }
    }

    int shamt = 0, offset = 0, enderecoJump = 0;
    char rs[4] = "", rt[4] = "", rd[4] = "";
    int rsIndex = -1, rtIndex = -1, rdIndex = -1;

    if (opcode == 0) { // Formato R
        sscanf(instrucao, "%*s %[^,], %[^,], %s", rd, rs, rt);
    } else if (opcode == 35 || opcode == 43) { // Formato I (lw/sw)
        sscanf(instrucao, "%*s %[^,], %d(%[^)])", rt, &offset, rs);
    } else if (opcode == 2) { // Formato J (j)
        sscanf(instrucao, "%*s %d", &enderecoJump);
    }

    imprimirFormatoInstrucao(opcode, rs, rt, rd, shamt, functCode, funct, offset, enderecoJump);
    tabelaRegs(regs, nomeRegs);
    free(regs);

    return 0;
}
