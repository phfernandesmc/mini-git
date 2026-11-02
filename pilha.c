#include <stdlib.h>
#include "pilha.h"

typedef struct NoPilha {
    Fila           *staging;
    struct NoPilha *abaixo;
} NoPilha;

struct Pilha {
    NoPilha *topo;
    int      tamanho;
};

Pilha *pilha_criar(void) {
    Pilha *p = malloc(sizeof *p);
    p->topo = NULL;
    p->tamanho = 0;
    return p;
}

void pilha_empilhar(Pilha *p, Fila *f) {
    NoPilha *no = malloc(sizeof *no);
    no->staging = f;
    no->abaixo = p->topo;   /* o novo no aponta para o antigo topo */
    p->topo = no;
    p->tamanho++;
}

Fila *pilha_desempilhar(Pilha *p) {
    if (!p->topo) return NULL;
    NoPilha *no = p->topo;
    Fila *f = no->staging;
    p->topo = no->abaixo;
    free(no);
    p->tamanho--;
    return f;
}

Fila *pilha_topo(const Pilha *p)    { return p->topo ? p->topo->staging : NULL; }
int   pilha_vazia(const Pilha *p)   { return p->tamanho == 0; }
int   pilha_tamanho(const Pilha *p) { return p->tamanho; }

void pilha_liberar(Pilha *p) {
    if (!p) return;
    while (!pilha_vazia(p)) fila_liberar(pilha_desempilhar(p));
    free(p);
}
