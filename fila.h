#ifndef FILA_H
#define FILA_H

#include "arquivo.h"

/* TAD Fila (FIFO) de arquivos -- usada como staging area.
 * Implementada como lista encadeada com ponteiros para inicio e fim,
 * assim enfileirar e desenfileirar custam O(1). */
typedef struct Fila Fila;

Fila    *fila_criar(void);
void     fila_enfileirar(Fila *f, Arquivo *a);   /* insere no fim    */
Arquivo *fila_desenfileirar(Fila *f);            /* remove do inicio */
Arquivo *fila_frente(const Fila *f);             /* consulta sem remover */
int      fila_vazia(const Fila *f);
int      fila_tamanho(const Fila *f);
void     fila_imprimir(const Fila *f);
void     fila_liberar(Fila *f);                  /* libera tambem os arquivos */

#endif
