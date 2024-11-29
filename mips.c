#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_INSTR 30       // 'add $t0, $t1, $t2', 'li $t0, 40', 'lw $t0, 8($t1)', j puloCondicional.
#define TAM_MEMORIA 1024    // valor total de memória para o programa.
#define QTD_REGS 34         // quantidade total de registradores

// PC começa em 0
int pc = 0;

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
    // FORMATO R
    if ((strncmp(instrucao, "add", 3) == 0 && (instrucao[3] == ' ')) ||
        (strncmp(instrucao, "sub", 3) == 0 && (instrucao[3] == ' ')) || 
        (strncmp(instrucao, "and", 3) == 0 && (instrucao[3] == ' ')) ||
        (strncmp(instrucao, "div", 3) == 0 && (instrucao[3] == ' ')) || 
        (strncmp(instrucao, "mul", 3) == 0 && (instrucao[3] == ' '))) {
        return 'R';
    }
    
    // FORMATO I
    else if ((strncmp(instrucao, "lw", 2) == 0 && (instrucao[2] == ' ')) ||
             (strncmp(instrucao, "sw", 2) == 0 && (instrucao[2] == ' ')) ||
             (strncmp(instrucao, "li", 2) == 0 && (instrucao[2] == ' '))) {
        return 'I';
    }
    
    // FORMATO J
    else if ((strncmp(instrucao, "j", 1) == 0) && (instrucao[1] == ' ')){
        return 'J';
    }

    // CASO NÃO SEJA NENHUM DOS FORMATOS
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
    
    if ((opcode == 0) || (opcode == 8)){ endereco = -1; }
    printf("ENDERECO:    %2d\n\n", endereco);
}

int main(){

    // Alocando memória para os registradores e já escrevendo 0 em todos.
    int *valorRegs = (int*)calloc(QTD_REGS, sizeof(int));
    if(!valorRegs){
        printf("Erro ao alocar memória para os registradores.\n");
        return 1;
    }
    // Alocando espaço de memória para as operações com LW e SW.
    int *memoria = (int*) calloc(TAM_MEMORIA, sizeof(int));
    if(!memoria){
        printf("Erro ao alocar espaco de memoria para armazenamento.\n");
        return 1;
    }

    // Inicializando o valorRegs com múltiplos de 2 (somente de exemplo)
    int i, j;
    for(i = 0, j = 2; i < QTD_REGS; i++, j+=2){
        valorRegs[i] = j;
    }

    // Inicializando a memória com múltiplos de 4 (tamanho das palavras)
    for (int i = 0; i < TAM_MEMORIA; i++) {
        memoria[i] = i * 4;
    }

    // Vetor com o nome dos registradores para o print na tabela.
    char *nomeRegs[] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0",
        "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1",
        "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0",
        "$k1", "$gp", "$sp", "$fp", "$ra", "$hi", "$lo"
    };

    // Instruções hardcoded para leitura.
    //char instrucao[TAM_INSTR] = "add $2, $1, $0";
    //char instrucao[TAM_INSTR] = "sub $3, $4, $5";
    //char instrucao[TAM_INSTR] = "and $6, $8, $7";
    //char instrucao[TAM_INSTR] = "div $10, $11, $9";
    //char instrucao[TAM_INSTR] = "mul $14, $13, $12";
    //char instrucao[TAM_INSTR] = "lw $16, 20($15)";
    //char instrucao[TAM_INSTR] = "sw $17, 1000($18)";
    //char instrucao[TAM_INSTR] = "li $20, 19";
    //char instrucao[TAM_INSTR] = "j LOOP";

    // Pegando a instrução do Assembly do MIPS do usuário.
    char instrucao[TAM_INSTR];

    printf("Insira a instrucao Assembly do MIPS: ");
    scanf("%30[^\n]s", instrucao);
    getchar();

    // Checa qual é o formato, se R, I ou J.
    char formato = acharFormato(instrucao);

    // Valores das variáveis dos formatos estruturais:
    int opcode = 0, rs = -1, rt = -1, rd = -1, shamt = -1, funct = -1;
    int imediato, offset = -1, endereco;

    // opcode: código do tipo da operação.
    // rs: registrador fonte, primeiro operando ou base do endereço.
    // rt: registrador alvo, segundo operando ou registrador de destino.
    // rs: registrador destino, registrador que irá receber algum resultado.
    // shamt: quantidade de deslocamento (só serve para operações de shift).
    // funct: código da função (ex.: add = 32, sub = 34, mul = 24...).
    // imediato: valor constante que é usado diretamente em uma instrução.
    // offset: valor de deslocamento, campo 'imediato' em funções de acesso a memória
    // endereco = RS (endereço base) + IMEDIATO.

    // Formatos da instrução, na prática, por exemplo: "add", "lw", "j"
    char formatoR[4], formatoI[3], formatoJ;

    // Depois de definir o formato da instrução (R, I ou J), executa as operações.
    switch(formato){
        // FORMATO R:
        case 'R':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);
            sscanf(instrucao, "%s $%d, $%d, $%d", formatoR, &rd, &rs, &rt);
            if (strncmp(formatoR, "add", 3) == 0) {
                // Atribuição do novo valor de funct.
                funct = 32;

                // Execução da operação:
                valorRegs[rd] = valorRegs[rs] + valorRegs[rt];
            }
            else if (strncmp(formatoR, "sub", 3) == 0) {
                // Atribuição do novo valor de funct.
                funct = 34;

                // Execução da operação: 
                valorRegs[rd] = valorRegs[rs] - valorRegs[rt];
            }
            else if (strncmp(formatoR, "and", 3) == 0) {
                // Atribuição do novo valor de funct.
                funct = 36;

                // Execução da operação:
                if((valorRegs[rs] == 0) || valorRegs[rt] == 0){
                    valorRegs[rd] = 0;
                }
                else{
                    // Operação bit a bit para o 'and'.
                    valorRegs[rd] = valorRegs[rs] & valorRegs[rt];
                }
            }
            else if (strncmp(formatoR, "div", 3) == 0) {
                // Atribuição do novo valor de funct.
                funct = 26;

                // Execução da operação:
                valorRegs[32] = valorRegs[rs] / valorRegs[rt];  // Quociente vai para lo
                valorRegs[33] = valorRegs[rs] % valorRegs[rt];  // Resto vai para hi
                valorRegs[rd] = valorRegs[32];                  // mflo
            }
            else if (strncmp(formatoR, "mul", 3) == 0) {
                // Atribuição do novo valor de funct.
                funct = 24;

                // Execução da operação considerando que o resultado da multiplicação caberá em 32 bits.
                valorRegs[32] = valorRegs[rs] * valorRegs[rt];
                valorRegs[33] = 0;
                valorRegs[rd] = valorRegs[32];
            }

            formatoR[4] = '\n';

            // Printando a tabela de registradores atualizada.
            printf("============ Formato R ============\n");
            printFormato(opcode, rs, rt, rd, shamt, funct, -1, offset, endereco);

            printf("PC = %d\n\n", pc);
            break;

        // FORMATO I:
        case 'I':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);
            // Para a instrução LI:
            if (strncmp(instrucao, "li", 2) == 0){
                sscanf(instrucao, "%s $%d, %d", formatoI, &rt, &imediato);

                // Execução da operação:
                valorRegs[rt] = imediato;

                // Leva o OPCODE 8 pois quando é <= 16 bits e quando acontece isso o montador utiliza a instrução "addi" para realizar a execução.

                // Printando a tabela de registradores atualizada.
                printf("==== Formato I ====\n");
                printFormato(8, rs, rt, rd, shamt, funct, imediato, offset, -1);

                printf("PC = %d\n\n", pc);
            }
            
            // A reserva da memória para LW e SW é feita com 2x o tamanho de offset + imediato (que equivale ao deslocamento total que a instrução vai faer na memória)

            // Para a instrução LW:
            else if (strncmp(instrucao, "lw", 2) == 0) {
                sscanf(instrucao, "%s $%d, %d($%d)", formatoI, &rt, &imediato, &rs);

                // Execução da operação:
                endereco = valorRegs[rs] + imediato;

                // Garantindo que o endereço de memória seja compatível com o tamanho de memória disponível.
                if (endereco % 4 != 0) {
                    printf("Erro: Endereço não alinhado para LW: %d\n", endereco);
                } 
                else if (endereco >= 0 && endereco < TAM_MEMORIA) {
                    // RT = memória na posição 'endereco / 4'. Dividindo em 4, por que cada palavra tem 4 bytes, então precisa alinhar.
                    valorRegs[rt] = memoria[endereco / 4];
                } 
                else {
                    printf("Erro: Endereço fora do limite de memória para LW.\n");
                }

                // Printando a tabela de registradores atualizada.
                printf("==== Formato I ====\n");
                printFormato(35, rs, rt, rd, -1, -1, imediato, imediato, endereco);

                printf("PC = %d\n\n", pc);
            }

            // Para a instrução SW:
            else if (strncmp(instrucao, "sw", 2) == 0) {
                sscanf(instrucao, "%s $%d, %d($%d)", formatoI, &rt, &imediato, &rs);
                
                // Execução da operação:
                endereco = valorRegs[rs] + imediato;

                // Garantindo que o endereço de memória seja compatível com o tamanho de memória disponível.
                if (endereco % 4 != 0) {
                    printf("Erro: Endereço não alinhado para SW: %d\n", endereco);
                } else if (endereco >= 0 && endereco < TAM_MEMORIA) {
                    // memoria na posição 'endereço / 4' = RT. Dividindo em 4, por que cada palavra tem 4 bytes, então precisa alinhar.
                    memoria[endereco / 4] = valorRegs[rt];
                } else {
                    printf("Erro: Endereço fora do limite de memória para SW.\n");
                }

                // Printando a tabela de registradores atualizada.
                printf("==== Formato I ====\n");
                printFormato(43, rs, rt, rd, -1, -1, imediato, imediato, endereco);

                printf("PC = %d\n\n", pc);
            }

            formatoI[2] = '\0';
            break;

        // FORMATO J:
        case 'J':
            printf("\n>> Analisando a Instrucao: %s\n\n", instrucao);
            sscanf(instrucao, "%c %d", &formatoJ, &endereco);

            // Printando a tabela de registradores atualizada.
            printf("==== Formato J ====\n");            
            printFormato(2, -1, -1, -1, -1, -1, -1, -1, endereco);
            
            pc = endereco;
            printf("PC = %d\n\n", pc);
            break;

        // Caso formato == 'X'
        default:
            printf("Formato invalido! Finalizando o programa.\n");
            break;
    }

    if(formato!= 'J'){ pc += 4; }

    if(formato != 'X'){
        printRegistradores(nomeRegs, valorRegs);
    }

    free(valorRegs);

    return 0;
}