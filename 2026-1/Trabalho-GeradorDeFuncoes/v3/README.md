# Gerador Dinâmico de Funções — versão final (v3)

Trabalho G2 de INF1018 (Software Básico) — **2026.1**.

Esta é a versão final e consolidada do trabalho **"Gerador Dinâmico de Funções"**,
que implementa a função `cria_func` exatamente como descrito no enunciado de 2026.1.

## Por que uma v3?

O diretório possuía duas versões anteriores, ambas inadequadas para o enunciado de 2026.1:

- **`v1`** — É de fato o trabalho `cria_func` (Gerador Dinâmico de Funções), mas usa uma
  **assinatura antiga** (`void* cria_func(void* f, DescParam params[], int n)` com `malloc`
  e `libera_func`) e tem uma implementação **incompleta/com bugs** (vários casos de 2 e 3
  parâmetros não são tratados corretamente).
- **`v2`** — É um **trabalho diferente** (`geracod`, da disciplina de 2015.1): um gerador de
  código para a linguagem "SB", gerando código **IA-32 (32 bits)**. Não corresponde ao
  enunciado de 2026.1.

A `v3` segue **estritamente** o enunciado atual:

```c
void cria_func (void* f, DescParam params[], int n, unsigned char codigo[]);
```

- O chamador fornece o vetor `codigo[]` onde o código gerado é gravado (sem `malloc`).
- Geração de código de máquina **x86-64** (System V AMD64).
- Suporta de 1 a 3 parâmetros, dos tipos `INT_PAR`/`PTR_PAR`, com origens `PARAM`/`FIX`/`IND`.

## Arquivos

| Arquivo         | Descrição                                                        |
|-----------------|------------------------------------------------------------------|
| `cria_func.h`   | Definições do enunciado (`TipoValor`, `OrigemValor`, `DescParam`).|
| `cria_func.c`   | Implementação de `cria_func` (arquivo a ser entregue).            |
| `teste.c`       | Bateria de testes (função `main`), incluindo os exemplos do enunciado. |
| `relatorio.txt` | Relatório com a descrição e os testes realizados.                |

## Como compilar e executar

```sh
gcc -Wall -Wa,--execstack -o teste cria_func.c teste.c
./teste
```

A opção `-Wa,--execstack` é obrigatória: o vetor `codigo` fica na pilha (variável local)
e nele é gravado o código de máquina que será executado.

## Entrega

Conforme o enunciado, devem ser entregues no EaD apenas `cria_func.c` e `relatorio.txt`.
Lembre-se de preencher os nomes/matrículas/turmas no cabeçalho de `cria_func.c` e em
`relatorio.txt`.
