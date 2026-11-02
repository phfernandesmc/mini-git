#ifndef LISTA_H
#define LISTA_H

#include "commit.h"

/* TAD Lista de commits -- o historico do repositorio.
 * Lista encadeada com ponteiro para o fim, para inserir em O(1).
 * Buscar por id percorre a lista: O(n). */
typedef struct Lista Lista;

Lista  *lista_criar(void);
void    lista_inserir_fim(Lista *l, Commit *c);
Commit *lista_buscar_por_id(const Lista *l, int id);
Commit *lista_ultimo(const Lista *l);    /* o HEAD */
int     lista_tamanho(const Lista *l);
void    lista_percorrer(const Lista *l, void (*visitar)(const Commit *c));
void    lista_liberar(Lista *l);         /* libera tambem os commits */

#endif
