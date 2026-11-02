#ifndef PILHA_H
#define PILHA_H

#include "fila.h"

/* TAD Pilha (LIFO) -- usada como stash.
 * Cada elemento e um staging inteiro (uma Fila) que foi guardado.
 * Implementada como lista encadeada: push e pop mexem so no topo, O(1). */
typedef struct Pilha Pilha;

Pilha *pilha_criar(void);
void   pilha_empilhar(Pilha *p, Fila *f);   /* push */
Fila  *pilha_desempilhar(Pilha *p);         /* pop  */
Fila  *pilha_topo(const Pilha *p);          /* peek */
int    pilha_vazia(const Pilha *p);
int    pilha_tamanho(const Pilha *p);
void   pilha_liberar(Pilha *p);             /* libera tambem as filas */

#endif
