#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h>

/* TAD Pilha (LIFO) em vetor -- usada como stash.
 * Cada item e um Stash: numero, descricao e os nomes dos arquivos que
 * estavam no staging. Segue o padrao do codigo base do professor:
 * struct visivel no .h e funcoes que alteram a pilha recebem ponteiro.
 * Todas as operacoes custam O(1). */

#ifndef PIL_MAX
#define PIL_MAX 5   /* quantos stashes cabem na pilha */
#endif
#ifndef ARQ_MAX
#define ARQ_MAX 5   /* quantos arquivos cabem em um stash */
#endif

typedef struct {
    int  id;                      /* numero do stash: 1, 2, 3... */
    char descricao[50];           /* ex.: "tela de login pela metade" */
    char arquivos[ARQ_MAX][30];   /* arquivos que estavam no staging */
    int  n_arquivos;              /* quantos arquivos foram guardados */
} Stash;

typedef struct {
    Stash itens[PIL_MAX];
    int   topo;                   /* indice do topo; -1 = pilha vazia */
} pilha;

void pil_inicializar(pilha *p);          /* deixa a pilha vazia (topo = -1) */
bool pil_push(pilha *p, Stash val);      /* empilha; false se cheia */
bool pil_pop(pilha *p, Stash *val);      /* desempilha; false se vazia */
bool pil_topo(pilha p, Stash *val);      /* consulta o topo sem remover */
bool pil_vazia(pilha p);
bool pil_cheia(pilha p);
int  pil_tamanho(pilha p);

#endif
