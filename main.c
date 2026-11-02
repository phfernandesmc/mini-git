#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fila.h"
#include "pilha.h"
#include "lista.h"
#include "commit.h"
#include "diff.h"

#define MAX_ENTRADA 1024

static void imprimir_commit(const Commit *c) {
    char data[32];
    strftime(data, sizeof data, "%d/%m/%Y %H:%M:%S", localtime(&c->data));
    printf("commit %d  [%s]  %s  (%d arquivo%s)\n",
           c->id, data, c->mensagem, c->n_arquivos, c->n_arquivos == 1 ? "" : "s");
}

static void ajuda(void) {
    puts("Comandos:");
    puts("  add <arquivo>         le o arquivo do disco e coloca no staging (fila)");
    puts("  status                mostra o staging e o tamanho do stash");
    puts("  commit <mensagem>     esvazia o staging e cria um commit (lista)");
    puts("  log                   mostra o historico de commits");
    puts("  stash                 guarda o staging atual na pilha");
    puts("  stash pop             recupera o ultimo staging guardado");
    puts("  diff <id1> <id2>      compara dois commits linha a linha (LCS)");
    puts("  ajuda                 mostra esta lista");
    puts("  sair                  encerra o programa");
}

/* Remove espacos iniciais e aspas em volta: "minha msg" -> minha msg */
static char *limpar_mensagem(char *s) {
    while (*s == ' ') s++;
    size_t tam = strlen(s);
    if (tam >= 2 && s[0] == '"' && s[tam - 1] == '"') {
        s[tam - 1] = '\0';
        s++;
    }
    return s;
}

int main(void) {
    Lista *historico = lista_criar();   /* commits */
    Fila  *staging   = fila_criar();    /* arquivos adicionados */
    Pilha *stash     = pilha_criar();   /* stagings guardados */
    int proximo_id = 1;

    char entrada[MAX_ENTRADA];
    puts("mini-git em C -- digite 'ajuda' para ver os comandos.");

    for (;;) {
        printf("minigit> ");
        fflush(stdout);
        if (!fgets(entrada, sizeof entrada, stdin)) break;
        entrada[strcspn(entrada, "\r\n")] = '\0';

        char *cmd = strtok(entrada, " ");
        if (!cmd) continue;

        if (strcmp(cmd, "add") == 0) {
            char *nome = strtok(NULL, " ");
            if (!nome) { puts("uso: add <arquivo>"); continue; }
            Arquivo *a = arquivo_ler(nome);
            if (!a) { printf("erro: nao consegui abrir '%s'\n", nome); continue; }
            fila_enfileirar(staging, a);
            printf("'%s' adicionado ao staging (%d linhas)\n", nome, a->n);

        } else if (strcmp(cmd, "status") == 0) {
            puts("Staging (fila):");
            fila_imprimir(staging);
            printf("Stash (pilha): %d entrada(s)\n", pilha_tamanho(stash));

        } else if (strcmp(cmd, "commit") == 0) {
            char *resto = strtok(NULL, "");
            char *msg = resto ? limpar_mensagem(resto) : NULL;
            if (!msg || *msg == '\0') { puts("uso: commit <mensagem>"); continue; }
            if (fila_vazia(staging)) { puts("nada para commitar: staging vazio"); continue; }
            Commit *c = commit_criar(proximo_id++, msg, lista_ultimo(historico), staging);
            lista_inserir_fim(historico, c);
            printf("commit %d criado: %s\n", c->id, c->mensagem);

        } else if (strcmp(cmd, "log") == 0) {
            if (lista_tamanho(historico) == 0) puts("nenhum commit ainda");
            else lista_percorrer(historico, imprimir_commit);

        } else if (strcmp(cmd, "stash") == 0) {
            char *sub = strtok(NULL, " ");
            if (!sub) {                                   /* push */
                if (fila_vazia(staging)) { puts("nada para guardar: staging vazio"); continue; }
                pilha_empilhar(stash, staging);
                staging = fila_criar();
                printf("staging guardado no stash (%d no total)\n", pilha_tamanho(stash));
            } else if (strcmp(sub, "pop") == 0) {         /* pop */
                if (pilha_vazia(stash)) { puts("stash vazio"); continue; }
                if (!fila_vazia(staging)) {
                    puts("erro: o staging precisa estar vazio para o stash pop");
                    continue;
                }
                fila_liberar(staging);
                staging = pilha_desempilhar(stash);
                printf("stash recuperado: %d arquivo(s) de volta no staging\n",
                       fila_tamanho(staging));
            } else {
                puts("uso: stash | stash pop");
            }

        } else if (strcmp(cmd, "diff") == 0) {
            char *s1 = strtok(NULL, " ");
            char *s2 = strtok(NULL, " ");
            if (!s1 || !s2) { puts("uso: diff <id1> <id2>"); continue; }
            Commit *c1 = lista_buscar_por_id(historico, atoi(s1));
            Commit *c2 = lista_buscar_por_id(historico, atoi(s2));
            if (!c1 || !c2) { puts("commit nao encontrado"); continue; }
            diff_commits(c1, c2);

        } else if (strcmp(cmd, "ajuda") == 0) {
            ajuda();
        } else if (strcmp(cmd, "sair") == 0) {
            break;
        } else {
            printf("comando desconhecido: '%s' (digite 'ajuda')\n", cmd);
        }
    }

    lista_liberar(historico);
    fila_liberar(staging);
    pilha_liberar(stash);
    return 0;
}
