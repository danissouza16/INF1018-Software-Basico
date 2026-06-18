#include <stdio.h>
#include <string.h>
#include "cria_func.h"

/*
 * Bateria de testes para cria_func.
 *
 * Compilar com:
 *   gcc -Wall -Wa,--execstack -o teste cria_func.c teste.c
 *
 * A opcao --execstack e necessaria porque o vetor "codigo" fica na pilha
 * (variavel local) e nele e gravado o codigo de maquina que sera executado.
 */

static int mem(const void *s1, const void *s2, size_t n) {
  return memcmp(s1, s2, n);
}

#define INTEIRO1 0x01020304
#define INTEIRO2 0x05060708
#define INTEIRO3 0x090a0b0c

static char str1[] = "* primeira * ";
static char str2[] = "quero saber tudo";
static char str3[] = "quero saber nada";
static char str4[] = "quero saber tudo sobre tudo";

static DescParam params[4];

/* Tipos das funcoes geradas */
typedef int (*func_ptr) ();
typedef int (*func_ptr_i) (int i);
typedef int (*func_ptr_p) (char *p);
typedef int (*func_ptr_i_i) (int i1, int i2);
typedef int (*func_ptr_i_p) (int i, char *p);
typedef int (*func_ptr_p_i) (void *c, size_t n);

/* Funcao trivial do enunciado: produto de dois inteiros */
int mult(int x, int y) {
  return x * y;
}

/* Funcoes auxiliares para checar a passagem de parametros */
int uminteiro(int i) {
  return (i == INTEIRO1);
}

int umpointer(char *p) {
  return (p == str1);
}

int tresinteiros(int i1, int i2, int i3) {
  return ((i1 == INTEIRO1) && (i2 == INTEIRO2) && (i3 == INTEIRO3));
}

int tres(int i1, char *p, int i2) {
  return ((i1 == INTEIRO1) && (i2 == INTEIRO2) && (p == str1));
}

static int ntot = 0, tok = 0;

static void checa(const char *nome, int ok) {
  ntot++;
  if (ok) {
    tok++;
    printf("  [OK]   %s\n", nome);
  } else {
    printf("  [FALHA] %s\n", nome);
  }
}

int main(void) {
  unsigned char codigo[500];
  func_ptr     f_void;
  func_ptr_i   f_int;
  func_ptr_p   f_pointer;
  func_ptr_i_i f_int_int;
  func_ptr_i_p f_int_p;
  func_ptr_p_i f_p_i;

  int int1 = INTEIRO1;
  int int2 = INTEIRO2;
  char *pointer1 = str1;

  /* ----------------------------------------------------------------- */
  printf("\nExemplo do enunciado: mult amarrando o 2o parametro a 10\n");
  printf("-------------------------------------------------------\n");
  {
    int i, ok = 1;
    params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
    params[1].tipo_val = INT_PAR; params[1].orig_val = FIX;
    params[1].valor.v_int = 10;
    cria_func(mult, params, 2, codigo);
    f_int = (func_ptr_i) codigo;
    for (i = 1; i <= 10; i++) {
      int r = f_int(i);
      printf("   f_mult(%d) = %d\n", i, r);
      if (r != i * 10) ok = 0;
    }
    checa("mult(x,10) = x*10 para x=1..10", ok);
  }

  /* mesma coisa, mas amarrando o 1o parametro a uma variavel (IND) */
  {
    int i, ok = 1;
    params[0].tipo_val = INT_PAR; params[0].orig_val = IND;
    params[0].valor.v_ptr = &i;
    params[1].tipo_val = INT_PAR; params[1].orig_val = FIX;
    params[1].valor.v_int = 10;
    cria_func(mult, params, 2, codigo);
    f_void = (func_ptr) codigo;
    for (i = 1; i <= 10; i++) {
      int r = f_void();
      if (r != i * 10) ok = 0;
    }
    checa("mult(i,10) com i indireto, sem argumentos", ok);
  }

  /* ----------------------------------------------------------------- */
  printf("\nTestes com diferentes tipos de parametro\n");
  printf("----------------------------------------\n");

  params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
  cria_func(uminteiro, params, 1, codigo);
  f_int = (func_ptr_i) codigo;
  checa("inteiro (parametro)", f_int(int1));

  params[0].tipo_val = INT_PAR; params[0].orig_val = FIX;
  params[0].valor.v_int = int1;
  cria_func(uminteiro, params, 1, codigo);
  f_void = (func_ptr) codigo;
  checa("inteiro (fixo)", f_void());

  params[0].tipo_val = INT_PAR; params[0].orig_val = IND;
  params[0].valor.v_ptr = &int1;
  cria_func(uminteiro, params, 1, codigo);
  f_void = (func_ptr) codigo;
  checa("inteiro (indireto)", f_void());

  params[0].tipo_val = PTR_PAR; params[0].orig_val = PARAM;
  cria_func(umpointer, params, 1, codigo);
  f_pointer = (func_ptr_p) codigo;
  checa("pointer (parametro)", f_pointer(pointer1));

  params[0].tipo_val = PTR_PAR; params[0].orig_val = FIX;
  params[0].valor.v_ptr = pointer1;
  cria_func(umpointer, params, 1, codigo);
  f_void = (func_ptr) codigo;
  checa("pointer (fixo)", f_void());

  params[0].tipo_val = PTR_PAR; params[0].orig_val = IND;
  params[0].valor.v_ptr = &pointer1;
  cria_func(umpointer, params, 1, codigo);
  f_void = (func_ptr) codigo;
  checa("pointer (indireto)", f_void());

  /* ----------------------------------------------------------------- */
  printf("\nTestes com parametros inteiros (3 parametros)\n");
  printf("---------------------------------------------\n");

  params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
  params[1].tipo_val = INT_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = FIX;
  params[2].valor.v_int = INTEIRO3;
  cria_func(tresinteiros, params, 3, codigo);
  f_int_int = (func_ptr_i_i) codigo;
  checa("p0 param, p1 param, p2 fixo", f_int_int(INTEIRO1, INTEIRO2));

  params[0].tipo_val = INT_PAR; params[0].orig_val = FIX;
  params[0].valor.v_int = INTEIRO1;
  params[1].tipo_val = INT_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = PARAM;
  cria_func(tresinteiros, params, 3, codigo);
  f_int_int = (func_ptr_i_i) codigo;
  checa("p0 fixo, p1 param, p2 param", f_int_int(INTEIRO2, INTEIRO3));

  params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
  params[1].tipo_val = INT_PAR; params[1].orig_val = FIX;
  params[1].valor.v_int = INTEIRO2;
  params[2].tipo_val = INT_PAR; params[2].orig_val = PARAM;
  cria_func(tresinteiros, params, 3, codigo);
  f_int_int = (func_ptr_i_i) codigo;
  checa("p0 param, p1 fixo, p2 param", f_int_int(INTEIRO1, INTEIRO3));

  params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
  params[1].tipo_val = INT_PAR; params[1].orig_val = IND;
  params[1].valor.v_ptr = &int2;
  params[2].tipo_val = INT_PAR; params[2].orig_val = FIX;
  params[2].valor.v_int = INTEIRO3;
  cria_func(tresinteiros, params, 3, codigo);
  f_int = (func_ptr_i) codigo;
  checa("p0 param, p1 indireto, p2 fixo", f_int(INTEIRO1));

  /* ----------------------------------------------------------------- */
  printf("\nTestes com tres parametros de tipos diferentes\n");
  printf("----------------------------------------------\n");

  params[0].tipo_val = INT_PAR; params[0].orig_val = FIX;
  params[0].valor.v_int = INTEIRO1;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = FIX;
  params[1].valor.v_ptr = str1;
  params[2].tipo_val = INT_PAR; params[2].orig_val = PARAM;
  cria_func(tres, params, 3, codigo);
  f_int = (func_ptr_i) codigo;
  checa("int fixo, ptr fixo, int param", f_int(INTEIRO2));

  params[0].tipo_val = INT_PAR; params[0].orig_val = IND;
  params[0].valor.v_ptr = &int1;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = FIX;
  params[2].valor.v_int = INTEIRO2;
  cria_func(tres, params, 3, codigo);
  f_pointer = (func_ptr_p) codigo;
  checa("int indireto, ptr param, int fixo", f_pointer(str1));

  params[0].tipo_val = INT_PAR; params[0].orig_val = FIX;
  params[0].valor.v_int = INTEIRO1;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = FIX;
  params[1].valor.v_ptr = str1;
  params[2].tipo_val = INT_PAR; params[2].orig_val = IND;
  params[2].valor.v_ptr = &int2;
  cria_func(tres, params, 3, codigo);
  f_void = (func_ptr) codigo;
  checa("int fixo, ptr fixo, int indireto", f_void());

  params[0].tipo_val = INT_PAR; params[0].orig_val = PARAM;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = FIX;
  params[2].valor.v_int = INTEIRO2;
  cria_func(tres, params, 3, codigo);
  f_int_p = (func_ptr_i_p) codigo;
  checa("int param, ptr param, int fixo", f_int_p(int1, str1));

  /* ----------------------------------------------------------------- */
  printf("\nExemplo do enunciado: memcmp (mesmo prefixo)\n");
  printf("--------------------------------------------\n");

  params[0].tipo_val = PTR_PAR; params[0].orig_val = FIX;
  params[0].valor.v_ptr = str4;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = PARAM;
  cria_func(mem, params, 3, codigo);
  f_p_i = (func_ptr_p_i) codigo;
  {
    int res = f_p_i(str2, strlen(str2));
    printf("   '%s' eh prefixo de '%s'? %s\n", str2, str4, res ? "NAO" : "SIM");
    checa("memcmp: prefixo verdadeiro", res == 0);
  }

  params[0].tipo_val = PTR_PAR; params[0].orig_val = FIX;
  params[0].valor.v_ptr = str4;
  params[1].tipo_val = PTR_PAR; params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR; params[2].orig_val = PARAM;
  cria_func(mem, params, 3, codigo);
  f_p_i = (func_ptr_p_i) codigo;
  {
    int res = f_p_i(str3, strlen(str3));
    printf("   '%s' eh prefixo de '%s'? %s\n", str3, str4, res ? "NAO" : "SIM");
    checa("memcmp: prefixo falso", res != 0);
  }

  printf("\n*****************************************************\n");
  printf("  Total corretos = %d de %d\n", tok, ntot);
  printf("*****************************************************\n");

  return (tok == ntot) ? 0 : 1;
}
