#ifndef ARQUIVO_H
#define ARQUIVO_H

/* Um arquivo de texto guardado em memoria, linha por linha. */
typedef struct {
    char  *nome;     /* nome/caminho do arquivo          */
    char **linhas;   /* vetor de linhas (sem o '\n')     */
    int    n;        /* quantidade de linhas             */
} Arquivo;

char    *duplicar_texto(const char *s);
Arquivo *arquivo_ler(const char *caminho);   /* devolve NULL se nao conseguir abrir */
Arquivo *arquivo_copiar(const Arquivo *a);
void     arquivo_liberar(Arquivo *a);

#endif
