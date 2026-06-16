
#include <stdio.h>
#include <stdlib.h>
#include "cria_func.h"

void libera_func (void* func) {free(func);}

void* cria_func (void* f, DescParam params[], int n)
{
	//cabeçalho = Alocacao e inicializacao dos vetores representando os registradores e mexendo na pilha
	unsigned char* codigo = (unsigned char*) malloc(1000);
	unsigned char reg_int[] = {0xbf, 0xbe, 0xba}; // vetor de registradores inteiros edi, esi, edx ...
	unsigned char mais_regs_int[] = {0xb9, 0xba, 0xbb}; // Mais vetor de registradores inteiros caso necessario 
	unsigned char end[] = {0x39, 0x32, 0x13}; // Mais vetor de endereços caso parametros necessitem
	unsigned char pilha[] = {0x55, 0x48, 0x89, 0xe5}; //codigo instruções para a pilha
	unsigned char call_rax[] = {0xff,0xd0}; // instuçao de call *%rax
	unsigned char leave_ret[] = {0xc9, 0xc3}; // instrucao leave e ret
	unsigned long ptr = (unsigned long) f, var_ptr, ptr_par; // para mexer com os parametros   
	int nao_amarrado[] = {0, 0, 0}; // Não ficou com o melhor nome, mas ajuda na lógica
	int i = 0;
	for(i = 0; i < 4; i++) 
		codigo[i] = pilha[i];
	// Testando o caso mais simples de 2 parametros.
	if(n == 2)
	{
		// mov %rdi, %rsi ou mov %edi, %esi
		if(params[0].orig_val != PARAM && params[1].orig_val == PARAM){ //Mantenho o segundo parametro 
			//Se segundo é ponteiro então movq %rdi, %rsi
			if(params[1].tipo_val == PTR_PAR) {
				for(int j = 0; j < 3; i++,j++)
					codigo[i] = pilha[j];
			}
		}
	}
	//Obs: caso o segundo seja inteiro não tratei com %edi e %esi porque %rdi e %rsi aceitam do mesmo jeito pela "expansão" do num de bytes.
	//Fizemos dessa forma como uma forma para otimizar o código e não precisar de mais uma condição e de mais um loop. 
	//Esse comentário é só pra deixar claro que sabemos disso. Não funcionou "por acaso". :)
	//-------------------------------------------------------------------------------------------------------------------------------------------
	//Tratamento dos casos com 3 parâmetros. O trabalho meio que se repete a partir daqui, checo números de parâmetros e trato os casos previstos
	//no enunciado. Depois disso é só fazer o devido mapeamento deles para o assembly e executar as instruções.	
	if(n == 3) // Caso com 3 parametros
	{
		for(int i = 0; i < 3; ++i) {  // testando se é amarrado ou não: passo para a nova função ou para original 
			if(params[i].orig_val == PARAM)  
				nao_amarrado[i] = 1;	
			else
				nao_amarrado[i] = 0;
		}
		// A partir daqui fica praticamente a mesma coisa: checagem de tipos nos parametros e tratamento adequado deles
		// com os registradores correspondentes
		if(nao_amarrado[0] && !nao_amarrado[1] && nao_amarrado[2]) // usando rd's  
		{
			if(params[2].tipo_val == PTR_PAR) 
			codigo[i++] = 0x48; //mov
			codigo[i++] = 0x89; //rdi
			codigo[i++] = 0xf2; //rdx
		}
		else if(!nao_amarrado[0] && nao_amarrado[1] && nao_amarrado[2])
		{
			if(params[2].tipo_val == PTR_PAR) 
			codigo[i++] = 0x48;  
			codigo[i++] = 0x89;  
			codigo[i++] = 0xf2; 

			if(params[1].tipo_val == PTR_PAR) 
			codigo[i++] = 0x48;
			codigo[i++] = 0x89; 
			codigo[i++] = 0xfe; 
		}
		else if(!nao_amarrado[0] && nao_amarrado[1] && !nao_amarrado[2])
		{
			if(params[1].tipo_val == PTR_PAR) 
			codigo[i++] = 0x48;
			codigo[i++] = 0x89;   
			codigo[i++] = 0xfe; 
		}
		else if(!nao_amarrado[0] && !nao_amarrado[1] && nao_amarrado[2])
		{
			if(params[2].tipo_val == PTR_PAR) 
			codigo[i++] = 0x48;
			codigo[i++] = 0x89; 
			codigo[i++] = 0xfa; 
		}
	}
	for(int passou = 0; passou < n; ++passou)
	{
		if(params[passou].tipo_val == INT_PAR)
		{
			// Se a variavel for parametro, nao faco nada
			if(params[passou].orig_val == PARAM) 
				continue; // passou direto ou é amarrado ?
			else if(params[passou].orig_val == FIX)
			{
				// Se a variavel for fixa e do tipo inteiro
				// eu coloco o valor recebido no registrador correspondente
				codigo[i++] = reg_int[passou];
				codigo[i++] = (params[passou].valor.v_int) & 0xff; // chamada
				for (int inc = 1; inc < 4; inc++, i++)
				 	codigo[i] = (params[passou].valor.v_int >> 8*inc) & 0xff; 
			}
			else
			{
				codigo[i++] = 0x49;
				codigo[i++] = mais_regs_int[passou];
				var_ptr = (unsigned long) params[passou].valor.v_ptr;
				codigo[i++] = (var_ptr) & 0xff; // chamada
				for (int inc = 1; inc < 8; inc++, i++){
				 	codigo[i] = (var_ptr >> 8*inc) & 0xff;
				}
				codigo[i++] = 0x41;
				codigo[i++] = 0x8b;
				codigo[i++] = end[passou];
			}
		}
		else if(params[passou].tipo_val == PTR_PAR)
		{
			if(params[passou].orig_val == PARAM) 
				continue; // Vai direto então segue
			else if(params[passou].orig_val == FIX)
			{
				codigo[i++] = 0x48; //mov
				codigo[i++] = reg_int[passou];
				ptr_par = (unsigned long) params[passou].valor.v_ptr;
				codigo[i++] = ptr_par & 0xff; // chamada
				for (int inc = 1; inc < 8; inc++, i++){
				 	codigo[i] = (ptr_par >> 8*inc) & 0xff;
				}
			}
			else
			{
				// ponteiro de ponteiro amarrando o parametro
				// Usando o vetor end 
				codigo[i++] = 0x49; 
				codigo[i++] = mais_regs_int[passou];
				ptr_par = (unsigned long) params[passou].valor.v_ptr;
				// // inicio de mexida
				codigo[i++] = ptr_par & 0xff; // chamada
				for (int inc = 1; inc < 8; inc++, i++){
				 	codigo[i] = (ptr_par >> 8*inc) & 0xff;
				}
				codigo[i++] = 0x49;
				codigo[i++] = 0x8b;
				codigo[i++] = end[passou];
			}
		}
	}
	codigo[i++] = 0x48; //mov
	codigo[i++] = 0xb8; //eax
	codigo[i++] = (ptr) & 0xff; // call
	for (int inc = 1; inc < 8; inc++, i++){
		codigo[i] = (ptr >> 8*inc) & 0xff;
	}
	// Parte Final: chama o rax pela convenção e faz leave ret retornando o vetor codigo!  
	codigo[i++] = call_rax[0]; //0xff;
	codigo[i++] = call_rax[1]; //0xd0;
	codigo[i++] = leave_ret[0]; //leave
	codigo[i++] = leave_ret[1]; //ret
	return codigo;
}

