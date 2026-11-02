#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

typedef struct NoFila {
    Arquivo       *arquivo;
    struct NoFila *prox;
} NoFila;

struct Fila {
    NoFila *inicio;   /* de onde sai     */
    NoFila *fim;      /* por onde entra  */
    int     tamanho;
};

Fila *fila_criar(void) {
    Fila *f = malloc(sizeof *f);
    f->inicio = f->fim = NULL;
    f->tamanho = 0;
    return f;
}

void fila_enfileirar(Fila *f, Arquivo *a) {
    NoFila *no = malloc(sizeof *no);
    no->arquivo = a;
    no->prox = NULL;
    if (f->fim) f->fim->prox = no;   /* liga depois do ultimo */
    else        f->inicio = no;      /* fila estava vazia     */
    f->fim = no;
    f->tamanho++;
}

Arquivo *fila_desenfileirar(Fila *f) {
    if (!f->inicio) return NULL;
    NoFila *no = f->inicio;
    Arquivo *a = no->arquivo;
    f->inicio = no->prox;
    if (!f->inicio) f->fim = NULL;   /* ficou vazia */
    free(no);
    f->tamanho--;
    return a;
}

Arquivo *fila_frente(const Fila *f) { return f->inicio ? f->inicio->arquivo : NULL; }
int      fila_vazia(const Fila *f)   { return f->tamanho == 0; }
int      fila_tamanho(const Fila *f) { return f->tamanho; }

void fila_imprimir(const Fila *f) {
    if (fila_vazia(f)) { puts("  (vazio)"); return; }
    int pos = 1;
    for (NoFila *no = f->inicio; no; no = no->prox)
        printf("  %d. %s (%d linhas)\n", pos++, no->arquivo->nome, no->arquivo->n);
}

void fila_liberar(Fila *f) {
    if (!f) return;
    while (!fila_vazia(f)) arquivo_liberar(fila_desenfileirar(f));
    free(f);
}
