#ifndef DIFF_H
#define DIFF_H

#include "arquivo.h"
#include "commit.h"

/* Diff linha a linha usando LCS (programacao dinamica).
 * antigo ou novo podem ser NULL (arquivo inexistente = vazio).
 * Tempo e espaco: O(m * n). */
void diff_arquivos(const Arquivo *antigo, const Arquivo *novo);

/* Compara todos os arquivos de dois commits. */
void diff_commits(const Commit *a, const Commit *b);

#endif
