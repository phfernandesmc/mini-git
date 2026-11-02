#include <stdlib.h>
#include <string.h>
#include "commit.h"

static int indice_do_arquivo(const Commit *c, const char *nome) {
    for (int i = 0; i < c->n_arquivos; i++)
        if (strcmp(c->arquivos[i]->nome, nome) == 0) return i;
    return -1;
}

Commit *commit_criar(int id, const char *mensagem,
                     const Commit *anterior, Fila *staging) {
    Commit *c = malloc(sizeof *c);
    c->id = id;
    c->mensagem = duplicar_texto(mensagem);
    c->data = time(NULL);

    /* No maximo: arquivos do commit anterior + todos do staging. */
    int capacidade = (anterior ? anterior->n_arquivos : 0) + fila_tamanho(staging);
    c->arquivos = malloc((size_t)(capacidade > 0 ? capacidade : 1) * sizeof(Arquivo *));
    c->n_arquivos = 0;

    /* 1) Parte do snapshot anterior. */
    if (anterior)
        for (int i = 0; i < anterior->n_arquivos; i++)
            c->arquivos[c->n_arquivos++] = arquivo_copiar(anterior->arquivos[i]);

    /* 2) Esvazia a fila na ordem FIFO: arquivo que ja existe e substituido,
     *    arquivo novo e acrescentado. Se o mesmo arquivo foi adicionado
     *    duas vezes, vale a versao mais recente (a ultima a sair da fila). */
    while (!fila_vazia(staging)) {
        Arquivo *novo = fila_desenfileirar(staging);
        int pos = indice_do_arquivo(c, novo->nome);
        if (pos >= 0) {
            arquivo_liberar(c->arquivos[pos]);
            c->arquivos[pos] = novo;
        } else {
            c->arquivos[c->n_arquivos++] = novo;
        }
    }
    return c;
}

const Arquivo *commit_buscar_arquivo(const Commit *c, const char *nome) {
    int pos = indice_do_arquivo(c, nome);
    return pos >= 0 ? c->arquivos[pos] : NULL;
}

void commit_liberar(Commit *c) {
    if (!c) return;
    for (int i = 0; i < c->n_arquivos; i++) arquivo_liberar(c->arquivos[i]);
    free(c->arquivos);
    free(c->mensagem);
    free(c);
}
