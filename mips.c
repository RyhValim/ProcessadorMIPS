#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TAMANHO STR INSTRUCOES   // EXEMPLOS:
#define TAM_INSTR 20        // 'add $t0, $t1, $t2' ou 'li $t0, 40' ou 'lw $t0, 8($t1)' ou 'sw $t0, 12($t1)'.
#define INSTR_J 35           // j puloCondicional ou j puloIncondicional (quantia pode variar).

#define QTD_REGS 32

void printRegistradores(char *nomeRegs[], int *valorRegs){
    printf("REGST |  ID  | VAL\n");
    for(int i = 0; i < QTD_REGS; i++){
        printf("%-5s |  %2d  |  %d\n", nomeRegs[i], i, valorRegs[i]);
    }
}

char acharFormato(char *instrucao){
    // FORMATO R
    if (strncmp(instrucao, "add", 3) == 0 || strncmp(instrucao, "sub", 3) == 0 || 
        strncmp(instrucao, "and", 3) == 0 || strncmp(instrucao, "div", 3) == 0 || 
        strncmp(instrucao, "mul", 3) == 0) {
        return 'R';
    }
    
    // FORMATO I
    else if (strncmp(instrucao, "lw", 2) == 0 || strncmp(instrucao, "sw", 2) == 0 ||
        strncmp(instrucao, "li", 2) == 0) {
        return 'I';
    }
    
    // FORMATO J
    else if (strncmp(instrucao, "j", 1) == 0) {
        return 'J';
    }

    // CASO NÃO SEJA NENHUM DOS FORMATOS
    return 'X';
}

void printFormato(int opcode, int rs, int rt, int rd, int shamt, int funct, int imediato, int endereco){
    printf("Formato: R\n");
    printf("OPCODE: %d\n", opcode);
    printf("RS: %d\n", rs);
    printf("RT: %d\n", rt);
    printf("RD: %d\n", rd);
    printf("SHAMT: %d\n", shamt);
    printf("FUNCT: %d\n", funct);
    printf("IMEDIATO: %d\n", imediato);
    printf("ENDERECO: %d\n\n", endereco);
}

int main(){

    // Alocando memória para os registradores e já escrevendo 0 em todos.
    int *valorRegs = (int *)calloc(QTD_REGS, sizeof(int));

    int i, j;
    for(i = 0, j = 0; i < QTD_REGS; i++, j+=3){
        valorRegs[i] = j;
    }

    if(!valorRegs){
        printf("Erro ao alocar memória para os registradores.\n");
        return 1;
    }

    // Vetor com o nome dos registradores.
    char *nomeRegs[] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0",
        "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1",
        "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0",
        "$k1", "$gp", "$sp", "$fp", "$ra", "$h1", "$lo"
    };

    // Instrução para leitura.
    char instrucao[TAM_INSTR] = "add $0, $9, $10";
    //char instrucao[TAM_INSTR] = "sub $2, $14, $22";
    //char instrucao[TAM_INSTR] = "and $21, $6, $18";
    //char instrucao[TAM_INSTR] = "div $17, $31, $5";
    //char instrucao[TAM_INSTR] = "mul $25, $24, $23";
    //char instrucao[TAM_INSTR] = "lw $0, 8($6)";
    //char instrucao[TAM_INSTR] = "sw $12, 16($20)";
    //char instrucao[TAM_INSTR] = "li $11, 32";
    //char instrucao[INSTR_J] = "j label";

    // Checa qual é o formato, se R, I ou J.
    char formato = acharFormato(instrucao);

    // Separando valores no sscanf();
    printf("Instrucao: %s\n", instrucao);

    // Valores das variáveis dos formatos estruturais:
    int opcode, rs, rt, rd, shamt = 0, funct = 0, imediato = 0, endereco = 100;

    // Formato da instrução == R.
    if(formato == 'R'){
        sscanf(instrucao, "%s $%d, $%d, $%d", instrucao, &rs, &rt, &rd);

        printf("\ninstrucao: %s\n", instrucao);
        printf("rs: %d - %lld\n", rs, sizeof(rs));
        printf("rt: %d - %lld\n", rt, sizeof(rs));
        printf("rd: %d - %lld\n\n", rd, sizeof(rs));

        if(strncmp(instrucao, "add", 3) == 0){ funct = 32; }
        if(strncmp(instrucao, "sub", 3) == 0){ funct = 34; }
        if(strncmp(instrucao, "and", 3) == 0){ funct = 36; }
        if(strncmp(instrucao, "div", 3) == 0){ funct = 26; }
        if(strncmp(instrucao, "mul", 3) == 0){ funct = 24; }

        printFormato(0, rs, rt, rd, shamt, funct, 0, 0);
    }
    
    // Formato da instrução == I.
    else if(formato == 'I' && strncmp(instrucao, "li", 2) == 0){
        sscanf(instrucao, "%s $%d, %d($%d)", instrucao, &rs, &rt, &imediato);
        printFormato(8, 0, rt, 0, shamt, funct, imediato, 0);
    }
    else if(formato == 'I' && strncmp(instrucao, "lw", 2) == 0){
        sscanf(instrucao, "%s $%d, %d($%d)", instrucao, &rs, &rt, &imediato);
        printFormato(35, rs, rt, 0, shamt, funct, imediato, 0);
    }
    else if(formato == 'I' && strncmp(instrucao, "sw", 2) == 0){
        sscanf(instrucao, "%s $%d, %d($%d)", instrucao, &rs, &rt, &imediato);
        printFormato(43, rs, rt, 0, shamt, funct, imediato, 0);
    }
    
    // Formato da instrução == J.
    else if(formato == 'J'){
        opcode = 2;
        endereco = 100;
        printFormato(opcode, 0, 0, 0, shamt, funct, imediato, endereco);
    }

    // Formato inválido;
    else{
        printf("Formato inválido! Tente novamente.\n");
    }

    // Printando os registradores.
    printRegistradores(nomeRegs, valorRegs);
}