#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TAM_INSTR 30
#define TAM_MEMORIA 1024
#define QTD_REGS 34

int pc = 100;

void printRegistradores(char *nomeRegs[], int *valorRegs){
    printf("===== Tabela de Registradores =====\n");
    printf("REGISTR  |  CODIGO  |  VALOR\n");

    for(int i = 0; i < QTD_REGS; i++){
        if(i < QTD_REGS-2){
            printf("%-7s  |    %2d     |  %3d\n", nomeRegs[i], i, valorRegs[i]);
        }
        else{
            printf("%-7s  |           |  %3d\n", nomeRegs[i], valorRegs[i]);
        }
    }
}

char acharFormato(char *instrucao){
    if ((strncmp(instrucao, "add", 3) == 0 && (instrucao[3] == ' ')) ||
        (strncmp(instrucao, "sub", 3) == 0 && (instrucao[3] == ' ')) || 
        (strncmp(instrucao, "and", 3) == 0 && (instrucao[3] == ' ')) ||
        (strncmp(instrucao, "div", 3) == 0 && (instrucao[3] == ' ')) || 
        (strncmp(instrucao, "mul", 3) == 0 && (instrucao[3] == ' '))) {
        return 'R';
    }
    
    else if ((strncmp(instrucao, "lw", 2) == 0 && (instrucao[2] == ' ')) ||
             (strncmp(instrucao, "sw", 2) == 0 && (instrucao[2] == ' ')) ||
             (strncmp(instrucao, "li", 2) == 0 && (instrucao[2] == ' '))) {
        return 'I';
    }
    
    else if ((strncmp(instrucao, "j", 1) == 0) && (instrucao[1] == ' ')){
        return 'J';
    }

    return 'X';
}

void printFormato(int opcode, int rs, int rt, int rd, int shamt, int funct, int imediato, int offset, int endereco){
    printf("OPCODE:      %2d\n", opcode);
    printf("RS:          %2d\n", rs);
    printf("RT:          %2d\n", rt);
    printf("RD:          %2d\n", rd);
    printf("SHAMT:       %2d\n", shamt);
    printf("FUNCT:       %2d\n\n", funct);
    
    printf("IMEDIATO:    %2d\n", imediato);
    printf("OFFSET:      %2d\n", offset);
    
    if ((opcode != 0) || (opcode != 8)){
        printf("ENDERECO:    %2d\n\n", endereco);
    }
    else{
        printf("ENDERECO:    -1\n\n");
    }
}

void embaralharRegs(int *regs){
    srand(time(NULL));

    int i;
    for(i = 1; i < QTD_REGS-2; i++){
        regs[i] = rand() % 5;
    }
}

int main(){

    int *valorRegs = (int*)calloc(QTD_REGS, sizeof(int));
    if(!valorRegs){
        printf("Erro ao alocar memória para os registradores.\n");
        return 1;
    }

    int *memoria = (int*) calloc(TAM_MEMORIA, sizeof(int));
    if(!memoria){
        printf("Erro ao alocar espaco de memoria para armazenamento.\n");
        return 1;
    }

    for (int i = 0; i < TAM_MEMORIA; i++) {
        memoria[i] = i * 4;
    }

    char *nomeRegs[] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0",
        "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1",
        "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0",
        "$k1", "$gp", "$sp", "$fp", "$ra", "$hi", "$lo"
    };

    int opcode = 0, rs = -1, rt = -1, rd = -1, shamt = -1, funct = -1;
    int imediato, offset = -1, endereco;

    char formatoR[4], formatoI[3], formatoJ;

    printf("===== Todos os dados iniciais =====\n");
    printf("PC = %d\n\n", pc);
    printFormato(0, 0, 0, 0, 0, 0, 0, 0, 0);
    printRegistradores(nomeRegs, valorRegs);

    char instrucao[TAM_INSTR];

    embaralharRegs(valorRegs);
    
    printf("\n======== Dados embaralhados =======\n");
    printf("PC = %d\n\n", pc); 
    pc+=4;
    printFormato(0, 0, 0, 0, 0, 0, 0, 0, 0);
    printRegistradores(nomeRegs, valorRegs);

    printf("===================================\n");
    printf("\nInsira a instrucao Assembly do MIPS: ");
    scanf("%30[^\n]s", instrucao);
    getchar();

    char formato = acharFormato(instrucao);

    switch(formato){
        case 'R':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);
            sscanf(instrucao, "%s $%d, $%d, $%d", formatoR, &rd, &rs, &rt);
            if (strncmp(formatoR, "add", 3) == 0) {
                funct = 32;
                valorRegs[rd] = valorRegs[rs] + valorRegs[rt];
            }
            else if (strncmp(formatoR, "sub", 3) == 0) {
                funct = 34;
                valorRegs[rd] = valorRegs[rs] - valorRegs[rt];
            }
            else if (strncmp(formatoR, "and", 3) == 0) {
                funct = 36;
                valorRegs[rd] = valorRegs[rs] & valorRegs[rt];
            }
            else if (strncmp(formatoR, "div", 3) == 0) {
                funct = 26;
                valorRegs[32] = valorRegs[rs] / valorRegs[rt];  // Quociente vai para lo
                valorRegs[33] = valorRegs[rs] % valorRegs[rt];  // Resto vai para hi
                valorRegs[rd] = valorRegs[32];                  // mflo
            }
            else if (strncmp(formatoR, "mul", 3) == 0) {
                funct = 24;
                valorRegs[32] = valorRegs[rs] * valorRegs[rt];
                valorRegs[33] = 0;
                valorRegs[rd] = valorRegs[32];
            }

            formatoR[4] = '\n';

            printf("============ Formato R ============\n");
            printFormato(opcode, rs, rt, rd, shamt, funct, -1, offset, endereco);

            printf("PC = %d\n\n", pc);
            break;

        case 'I':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);

            if (strncmp(instrucao, "li", 2) == 0){
                sscanf(instrucao, "%s $%d, %d", formatoI, &rt, &imediato);

                valorRegs[rt] = imediato;

                printf("==== Formato I ====\n");
                printFormato(8, rs, rt, rd, shamt, funct, imediato, offset, -1);

                printf("PC = %d\n\n", pc);
            }
            else if (strncmp(instrucao, "lw", 2) == 0) {
                sscanf(instrucao, "%s $%d, %d($%d)", formatoI, &rt, &imediato, &rs);

                endereco = valorRegs[rs] + imediato;

                if (endereco % 4 != 0) {
                    printf("Erro: Endereço não alinhado para LW: %d\n", endereco);
                } 
                else if (endereco >= 0 && endereco < TAM_MEMORIA) {
                    valorRegs[rt] = memoria[endereco / 4];
                } 
                else {
                    printf("Erro: Endereço fora do limite de memória para LW.\n");
                }

                printf("==== Formato I ====\n");
                printFormato(35, rs, rt, rd, -1, -1, imediato, imediato, endereco);

                printf("PC = %d\n\n", pc);
            }

            else if (strncmp(instrucao, "sw", 2) == 0) {
                sscanf(instrucao, "%s $%d, %d($%d)", formatoI, &rt, &imediato, &rs);
                
                endereco = valorRegs[rs] + imediato;

                if (endereco % 4 != 0) {
                    printf("Erro: Endereço não alinhado para SW: %d\n", endereco);
                } else if (endereco >= 0 && endereco < TAM_MEMORIA) {
                    memoria[endereco / 4] = valorRegs[rt];
                } else {
                    printf("Erro: Endereço fora do limite de memória para SW.\n");
                }

                printf("==== Formato I ====\n");
                printFormato(43, rs, rt, rd, -1, -1, imediato, imediato, endereco);

                printf("PC = %d\n\n", pc);
            }

            formatoI[2] = '\0';
            break;

        case 'J':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);
            sscanf(instrucao, "%c %d", &formatoJ, &endereco);

            printf("==== Formato J ====\n");            
            printFormato(2, -1, -1, -1, -1, -1, -1, -1, endereco);
            
            pc = endereco;
            printf("PC = %d\n\n", pc);
            break;

        default:
            printf("Formato invalido! Finalizando o programa.\n");
            break;
    }

    if(formato != 'J'){ pc += 4; }

    if(formato != 'X'){
        printRegistradores(nomeRegs, valorRegs);
    }

    free(valorRegs);

    return 0;
}