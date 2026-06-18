/* Nome_do_Aluno1 Matricula Turma */
/* Nome_do_Aluno2 Matricula Turma */

#include "cria_func.h"

/*
 * Gerador dinamico de funcoes (INF1018 - 2026.1).
 *
 * cria_func gera, em tempo de execucao, o codigo de maquina x86-64 de uma
 * "nova versao" da funcao f. A nova funcao coloca nos registradores os valores
 * descritos em params (parametros repassados, fixos ou indiretos), chama f por
 * meio de um call indireto e retorna preservando o valor de retorno de f.
 *
 * Convencao System V AMD64: os tres primeiros argumentos inteiros/ponteiros sao
 * passados em %rdi, %rsi e %rdx (nesta ordem). Como o numero maximo de
 * parametros e 3, apenas esses registradores sao necessarios.
 */

/* numero dos registradores de argumento %rdi, %rsi, %rdx na codificacao
 * usada pelos campos reg/rm dos bytes opcode (B8+rd) e ModRM. */
static const unsigned char regnum[3] = {7, 6, 2};

static int emite_byte(unsigned char codigo[], int pos, unsigned char b) {
    codigo[pos] = b;
    return pos + 1;
}

/* grava um imediato de 32 bits em little-endian */
static int emite_imm32(unsigned char codigo[], int pos, int valor) {
    unsigned int v = (unsigned int) valor;
    int k;
    for (k = 0; k < 4; k++)
        codigo[pos++] = (v >> (8 * k)) & 0xff;
    return pos;
}

/* grava um imediato de 64 bits em little-endian */
static int emite_imm64(unsigned char codigo[], int pos, unsigned long valor) {
    int k;
    for (k = 0; k < 8; k++)
        codigo[pos++] = (valor >> (8 * k)) & 0xff;
    return pos;
}

void cria_func(void* f, DescParam params[], int n, unsigned char codigo[]) {
    int pos = 0;
    int i;
    int idx_param[3]; /* para cada posicao, o indice do PARAM (ou -1) */
    int j;

    /* Prologo: push %rbp ; mov %rsp,%rbp
     * Na entrada da funcao gerada %rsp = ...8 (mod 16); apos o push de %rbp a
     * pilha fica alinhada em 16 bytes. Como nao empilhamos mais nada, a pilha
     * permanece alinhada na hora do call, o que e necessario para chamar
     * funcoes de biblioteca (ex.: memcmp). */
    pos = emite_byte(codigo, pos, 0x55);       /* push %rbp        */
    pos = emite_byte(codigo, pos, 0x48);       /* mov  %rsp, %rbp  */
    pos = emite_byte(codigo, pos, 0x89);
    pos = emite_byte(codigo, pos, 0xe5);

    /*
     * 1a fase - parametros nao amarrados (PARAM).
     *
     * O j-esimo parametro PARAM chega na funcao gerada no registrador de
     * argumento de indice j (%rdi, %rsi, %rdx) e deve ser colocado no
     * registrador correspondente a sua posicao i na chamada de f. Como o numero
     * de PARAMs ate a posicao i nunca passa de i+1, vale sempre i >= j, ou seja,
     * o destino esta a direita (ou no mesmo lugar) da origem. Processando as
     * posicoes de tras para frente (i decrescente), nenhuma origem ainda
     * necessaria e sobrescrita.
     */
    j = 0;
    for (i = 0; i < n; i++) {
        if (params[i].orig_val == PARAM)
            idx_param[i] = j++;
        else
            idx_param[i] = -1;
    }
    for (i = n - 1; i >= 0; i--) {
        int src, dst;
        if (idx_param[i] < 0)
            continue;
        src = idx_param[i];
        dst = i;
        if (src == dst)
            continue; /* movq para o mesmo registrador: desnecessario */
        /* movq %r(src), %r(dst) : 48 89 ModRM(mod=11, reg=src, rm=dst) */
        pos = emite_byte(codigo, pos, 0x48);
        pos = emite_byte(codigo, pos, 0x89);
        pos = emite_byte(codigo, pos, 0xc0 | (regnum[src] << 3) | regnum[dst]);
    }

    /*
     * 2a fase - parametros amarrados (FIX e IND).
     *
     * Feita apos a 1a fase: os registradores de origem dos PARAMs ja foram
     * consumidos, entao podemos sobrescrever qualquer registrador destino.
     */
    for (i = 0; i < n; i++) {
        if (params[i].orig_val == PARAM)
            continue;

        if (params[i].orig_val == FIX) {
            if (params[i].tipo_val == INT_PAR) {
                /* movl $imm32, %e(reg) : (B8+rd) imm32 */
                pos = emite_byte(codigo, pos, 0xb8 | regnum[i]);
                pos = emite_imm32(codigo, pos, params[i].valor.v_int);
            } else {
                /* movabs $imm64, %r(reg) : 48 (B8+rd) imm64 */
                pos = emite_byte(codigo, pos, 0x48);
                pos = emite_byte(codigo, pos, 0xb8 | regnum[i]);
                pos = emite_imm64(codigo, pos, (unsigned long) params[i].valor.v_ptr);
            }
        } else { /* IND: o valor amarrado e o conteudo corrente da variavel */
            /* carrega o endereco da variavel no proprio registrador destino:
             * movabs $endereco, %r(reg) : 48 (B8+rd) imm64 */
            pos = emite_byte(codigo, pos, 0x48);
            pos = emite_byte(codigo, pos, 0xb8 | regnum[i]);
            pos = emite_imm64(codigo, pos, (unsigned long) params[i].valor.v_ptr);
            if (params[i].tipo_val == INT_PAR) {
                /* movl (%r(reg)), %e(reg) : 8B ModRM(mod=00, reg=rm=reg) */
                pos = emite_byte(codigo, pos, 0x8b);
                pos = emite_byte(codigo, pos, (regnum[i] << 3) | regnum[i]);
            } else {
                /* movq (%r(reg)), %r(reg) : 48 8B ModRM(mod=00, reg=rm=reg) */
                pos = emite_byte(codigo, pos, 0x48);
                pos = emite_byte(codigo, pos, 0x8b);
                pos = emite_byte(codigo, pos, (regnum[i] << 3) | regnum[i]);
            }
        }
    }

    /* Chamada da funcao original: movabs $f, %rax ; call *%rax */
    pos = emite_byte(codigo, pos, 0x48);
    pos = emite_byte(codigo, pos, 0xb8);
    pos = emite_imm64(codigo, pos, (unsigned long) f);
    pos = emite_byte(codigo, pos, 0xff);
    pos = emite_byte(codigo, pos, 0xd0);

    /* Epilogo: leave ; ret (preserva o valor de retorno de f em %rax) */
    pos = emite_byte(codigo, pos, 0xc9);
    pos = emite_byte(codigo, pos, 0xc3);
}
