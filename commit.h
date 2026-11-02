#ifndef COMMIT_H
#define COMMIT_H

#include <time.h>
#include "arquivo.h"
#include "fila.h"

/* Um commit guarda um "snapshot": a versao de TODOS os arquivos
 * versionados naquele momento (como no git de verdade). */
typedef struct {
    int       id;
    char     *mensagem;
    time_t    data;
    Arquivo **arquivos;
    int       n_arquivos;
} Commit;

/* Cria o commit copiando o snapshot do anterior e aplicando, em ordem,
 * tudo o que estiver no staging (esvazia a fila). */
Commit        *commit_criar(int id, const char *mensagem,
                            const Commit *anterior, Fila *staging);
const Arquivo *commit_buscar_arquivo(const Commit *c, const char *nome);
void           commit_liberar(Commit *c);

#endif
