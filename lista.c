#include <stdlib.h>
#include "lista.h"

typedef struct NoLista {
    Commit         *commit;
    struct NoLista *prox;
} NoLista;

struct Lista {
    NoLista *inicio;
    NoLista *fim;
    int      tamanho;
};

Lista *lista_criar(void) {
    Lista *l = malloc(sizeof *l);
    l->inicio = l->fim = NULL;
    l->tamanho = 0;
    return l;
}

void lista_inserir_fim(Lista *l, Commit *c) {
    NoLista *no = malloc(sizeof *no);
    no->commit = c;
    no->prox = NULL;
    if (l->fim) l->fim->prox = no;
    else        l->inicio = no;
    l->fim = no;
    l->tamanho++;
}

Commit *lista_buscar_por_id(const Lista *l, int id) {
    for (NoLista *no = l->inicio; no; no = no->prox)
        if (no->commit->id == id) return no->commit;
    return NULL;
}

Commit *lista_ultimo(const Lista *l)  { return l->fim ? l->fim->commit : NULL; }
int     lista_tamanho(const Lista *l) { return l->tamanho; }

void lista_percorrer(const Lista *l, void (*visitar)(const Commit *c)) {
    for (NoLista *no = l->inicio; no; no = no->prox)
        visitar(no->commit);
}

void lista_liberar(Lista *l) {
    if (!l) return;
    NoLista *no = l->inicio;
    while (no) {
        NoLista *prox = no->prox;
        commit_liberar(no->commit);
        free(no);
        no = prox;
    }
    free(l);
}
