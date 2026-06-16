#include<stdio.h>
#include<string.h>
#include<math.h>
#include "cria_func.h"

static int mem(const void *s1, const void *s2, size_t n) {
  return memcmp(s1,s2,n);
}

/*
 * Valores usados para teste
 */
#define INTEIRO1 0x01020304
#define INTEIRO2 0x05060708
#define INTEIRO3 0xf1f2f3f4

static char str1[] = "* primeira * ";
static char str2[] = "quero saber tudo";
static char str3[] = "quero saber nada";
static char str4[] = "quero saber tudo sobre tudo";

/*
 * Descritor de parâmetros
 */
static DescParam params[4];

/* Tipos das funções */
typedef int (*func_ptr) ();
typedef int (*func_ptr_i) (int i);
typedef int (*func_ptr_p) (char *p);
typedef int (*func_ptr_i_i) (int i1, int i2);
typedef int (*func_ptr_i_p) (int i, char *p);

typedef int (*func_ptr_p_i) (void *c, size_t n);

/*
 * Dump do codigo
 */
void dump (void *p, int n) {
  unsigned char *p1 = (unsigned char *) p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}

/*
 * Funções para teste de passagem de parâmetros
 */

/*
 * Um parâmetro inteiro
 */
int uminteiro(int i) {
  printf("   => esperado = 0x%08x recebido = 0x%08x ", INTEIRO1, i);
  return (i == INTEIRO1);
}

/*
 * Um parâmetro pointer
 */
int umpointer(char *p) {
  printf("   => esperado = %s recebido = %s ", str1, p);
  return (p == str1);
}

/*
 * Três parâmetros inteiros
 */
int tresinteiros(int i1, int i2, int i3) {
  printf("   => primeiro esperado = 0x%08x recebido = 0x%08x ", INTEIRO1, i1);
  printf("\n   => segundo  esperado = 0x%08x recebido = 0x%08x ", INTEIRO2, i2);
  printf("\n   => terceiro  esperado = 0x%08x recebido = 0x%08x ", INTEIRO3, i3);
  return ((i1 == INTEIRO1) && (i2 == INTEIRO2) && (i3 == INTEIRO3));
}

/*
 * Três parâmetros de tipos diferentes
 */
int tres(int i1, char *p, int i2) {
  printf("   => primeiro esperado = 0x%08x recebido = 0x%08x ", INTEIRO1, i1);
  printf("\n   => segundo esperado = %s recebido = %s ", str1, p);
  printf("\n   => terceiro esperado = 0x%08x recebido = 0x%08x ", INTEIRO2, i2);
  return ((i1 == INTEIRO1) && (i2 == INTEIRO2) && (p == str1) );
}


/*
 * Bateria de testes
 */
int main (int argc, char **argv) {
  func_ptr     f_void;
  func_ptr_i   f_int;
  func_ptr_p   f_pointer;
  func_ptr_i_i f_int_int;
  func_ptr_i_p f_int_p;
  func_ptr_p_i f_p_i;

  int res;
  int ntestes, nok;
  int ntot, tok;

  float nota = 0.0;
  float nota_parcial = 0.0;
 
  int int1 = INTEIRO1;
  int int2 = INTEIRO2;
  char *pointer1 = str1;

  int com_ptr = 1;
  int com_ind = 1;

  if (argc > 1) {
    int i;
    for (i = 1; i < argc; i++) {
      if (strcmp("NOPTR", argv[i]) == 0)
        com_ptr = 0;
      if (strcmp("NOIND", argv[i]) == 0)
        com_ind = 0;
    }
  }

  /* Inicializa contagem */

  /* Testa variações com tipos de parâmetros */
  printf("\nTestes com diferentes tipos de parâmetro");
  printf("\n----------------------------------------\n");

  ntot = 0;
  tok = 0;

  ntestes = 0;
  nok = 0;

  ++ntestes;
  printf("\n inteiro (parametro)\n");
  params[0].tipo_val = INT_PAR;
  params[0].orig_val = PARAM;

  f_int = (func_ptr_i)cria_func(uminteiro, params, 1);
  if (f_int(int1)) {
    printf(" -> OK\n");
    nota_parcial += 0.5;
    ++nok;
  }
  else
    printf(" -> NÂO OK\n");
  libera_func(f_int);

  ++ntestes;
  printf("\n inteiro (fixo)\n");
  params[0].tipo_val = INT_PAR;
  params[0].orig_val = FIX;
  params[0].valor.v_int = int1;

  f_void = (func_ptr)cria_func(uminteiro, params, 1);
  if (f_void()) {
    printf(" -> OK\n");
    nota_parcial += 0.5;
    ++nok;
  }
  else
    printf(" -> NÂO OK\n");
  libera_func(f_void);

  ++ntestes;
  if (com_ind) {
    printf("\n inteiro (indireto)\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = IND;
    params[0].valor.v_ptr = &int1;
  
    f_void = (func_ptr)cria_func(uminteiro, params, 1);
    if (f_void()) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_void);
  }

  ++ntestes;
  if (com_ptr) {
    printf("\n pointer (parametro)\n");
    params[0].tipo_val = PTR_PAR;
    params[0].orig_val = PARAM;

    f_pointer = (func_ptr_p)cria_func(umpointer, params, 1);
    if (f_pointer(pointer1)) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_pointer);
  }

  ++ntestes;
  if (com_ptr) {
    printf("\n pointer (fixo)\n");
    params[0].tipo_val = PTR_PAR;
    params[0].orig_val = FIX;
    params[0].valor.v_ptr = pointer1;

    f_void = (func_ptr)cria_func(umpointer, params, 1);
    if (f_void()) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_void);
  }

  ++ntestes;
  if (com_ptr && com_ind) {
      printf("\n pointer (indireto)\n");
      params[0].tipo_val = PTR_PAR;
      params[0].orig_val = IND;
      params[0].valor.v_ptr = &pointer1;
  
      f_void = (func_ptr_p)cria_func(umpointer, params, 1);
      if (f_void()) {
        printf(" -> OK\n");
        nota_parcial += 0.5;
        ++nok;
      }
      else
        printf(" -> NÂO OK\n");
      libera_func(f_void);
  }

  printf("\n** Corretos = %d (%d) -> nota parcial %.2f  **\n", 
           nok, ntestes, nota_parcial);
  ntot += ntestes;
  tok += nok;
  nota += nota_parcial;


  /* Testa variações com parâmetros inteiros */
  printf("\nTestes com parâmetros inteiros");
  printf("\n------------------------------\n");

  nota_parcial = 0;
  ntestes = 0;
  nok = 0;

  ++ntestes;
  printf("\n primeiro e segundo parametros, terceiro fixo\n");
  params[0].tipo_val = INT_PAR;
  params[0].orig_val = PARAM;;
  params[1].tipo_val = INT_PAR;
  params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR;
  params[2].orig_val = FIX;
  params[2].valor.v_int = INTEIRO3;

  f_int_int = (func_ptr_i_i)cria_func(tresinteiros, params, 3);
  if (f_int_int(INTEIRO1, INTEIRO2)) {
    printf(" -> OK\n");
    nota_parcial += 0.5;
    ++nok;
  }
  else
    printf(" -> NÂO OK\n");
  libera_func(f_int_int);

  ++ntestes;
  printf("\n primeiro fixo, segundo e terceiro parametros\n");
  params[0].tipo_val = INT_PAR;
  params[0].orig_val = FIX;
  params[0].valor.v_int = INTEIRO1;
  params[1].tipo_val = INT_PAR;
  params[1].orig_val = PARAM;
  params[2].tipo_val = INT_PAR;
  params[2].orig_val = PARAM;

  f_int_int = (func_ptr_i_i)cria_func(tresinteiros, params, 3);
  if (f_int_int(INTEIRO2, INTEIRO3)) {
    printf(" -> OK\n");
    nota_parcial += 0.5;
    ++nok;
  }
  else
    printf(" -> NÂO OK\n");
  libera_func(f_int_int);

  ++ntestes;
  printf("\n primeiro parametro, segundo fixo e terceiro parametro\n");
  params[0].tipo_val = INT_PAR;
  params[0].orig_val = PARAM;
  params[1].tipo_val = INT_PAR;
  params[1].orig_val = FIX;
  params[1].valor.v_int = INTEIRO2;
  params[2].tipo_val = INT_PAR;
  params[2].orig_val = PARAM;

  f_int_int = (func_ptr_i_i)cria_func(tresinteiros, params, 3);
  if (f_int_int(INTEIRO1, INTEIRO3)) {
    printf(" -> OK\n");
    nota_parcial += 0.5;
    ++nok;
  }
  else
    printf(" -> NÂO OK\n");
  libera_func(f_int_int);

  ++ntestes;
  if (com_ind) {
    printf("\n primeiro parametro, segundo indireto, terceiro fixo\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = PARAM;
    params[1].tipo_val = INT_PAR;
    params[1].orig_val = IND;
    params[1].valor.v_ptr = &int2;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = FIX;
    params[2].valor.v_int = INTEIRO3;

    f_int = (func_ptr_i)cria_func(tresinteiros, params, 3);
    if (f_int(INTEIRO1)) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_int_int);
  }

  printf("\n** Corretos = %d (%d) -> nota parcial %.2f  **\n", 
           nok, ntestes, nota_parcial);
  ntot += ntestes;
  tok += nok;
  nota += nota_parcial;

  /* Testa variações com três parâmetros */
  printf("\nTeste com três parâmetros");
  printf("\n---------------------------\n");

  ntestes = 0;
  nok = 0;
  nota_parcial = 0;

  ++ntestes;
  if (com_ptr) {
    printf("\n int fixo, pointer fixo, int parametro\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = FIX;
    params[0].valor.v_int = INTEIRO1;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = FIX;
    params[1].valor.v_ptr = str1;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = PARAM;

    f_int = (func_ptr_i)cria_func(tres, params, 3);
    if (f_int(INTEIRO2)) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_int);
  }

  ++ntestes;
  if (com_ind && com_ptr) {
    printf("\n int ind, pointer param, int fixo\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = IND;
    params[0].valor.v_ptr = &int1;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = PARAM;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = FIX;
    params[2].valor.v_int = INTEIRO2;

    f_pointer = (func_ptr_p)cria_func(tres, params, 3);
    if (f_pointer(str1)) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_pointer);
  }

  ++ntestes;
  if (com_ptr) {
    printf("\n int fixo, pointer fixo, int indireto\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = FIX;
    params[0].valor.v_int = INTEIRO1;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = FIX;
    params[1].valor.v_ptr = str1;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = IND;
    params[2].valor.v_ptr = &int2;

    f_void = (func_ptr)cria_func(tres, params, 3);
    if (f_void()) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_int);
  }

  ++ntestes;
  if (com_ind && com_ptr) {
    printf("\n int param, pointer param, int fixo\n");
    params[0].tipo_val = INT_PAR;
    params[0].orig_val = PARAM;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = PARAM;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = FIX;
    params[2].valor.v_int = INTEIRO2;

    f_int_p = (func_ptr_i_p)cria_func(tres, params, 3);
    if (f_int_p(int1, str1)) {
      printf(" -> OK\n");
      nota_parcial += 0.5;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_pointer);
  }


  printf("\n** Corretos = %d (%d) -> nota parcial %.2f  **\n", 
           nok, ntestes, nota_parcial);
  ntot += ntestes;
  tok += nok;
  nota += nota_parcial;

  /* Testa o exemplo do enunciado */
  printf("\nExemplo do enunciado (memcmp)");
  printf("\n-----------------------------\n");

  ntestes = 0;
  nok = 0;
  nota_parcial = 0;

  ++ntestes;
  if (com_ptr) {
    printf("\n teste verdadeiro\n");
    params[0].tipo_val = PTR_PAR;
    params[0].orig_val = FIX;
    params[0].valor.v_ptr = str4;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = PARAM;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = PARAM;

    f_p_i = (func_ptr_p_i)cria_func(mem, params,3);
    res = f_p_i(str2, strlen(str2));
    printf("'%s' eh prefixo de '%s'? %s\n", str2, str4, res?"NAO":"SIM");
    if (res == 0) {
      printf(" -> OK\n");
      nota_parcial += 1.0;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_p_i);
  }

  ++ntestes;
  if (com_ptr) {
    printf("\n teste falso\n");
    params[0].tipo_val = PTR_PAR;
    params[0].orig_val = FIX;
    params[0].valor.v_ptr = str4;
    params[1].tipo_val = PTR_PAR;
    params[1].orig_val = PARAM;
    params[2].tipo_val = INT_PAR;
    params[2].orig_val = PARAM;
  
    f_p_i = (func_ptr_p_i)cria_func(mem, params, 3);
    res = f_p_i(str3, strlen(str3));
    printf("'%s' eh prefixo de '%s'? %s\n", str3, str4, res?"NAO":"SIM");
    if (res) {
      printf(" -> OK\n");
      nota_parcial += 1.0;
      ++nok;
    }
    else
      printf(" -> NÂO OK\n");
    libera_func(f_p_i);
  } 
  printf("\n** Corretos = %d (%d) -> nota parcial %.2f  **\n", 
           nok, ntestes, nota_parcial);
  ntot += ntestes;
  tok += nok;
  nota += nota_parcial;

  printf("\n*****************************************************\n");
  printf("  Total corretos = %d (%d) -> nota recomendada %.2f",tok, ntot, nota);
  printf("\n*****************************************************\n");
  return 0;
}
