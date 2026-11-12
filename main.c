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
    puts("  stash <descricao>     guarda o staging inteiro no topo da pilha");
    puts("  stash pop             tira o stash do topo e devolve os arquivos ao staging");
    puts("  stash list            mostra os stashes do topo para a base");
    puts("  stash drop            descarta o stash do topo sem aplicar");
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

/* Mostra o prompt e le uma linha digitada.
 * Devolve 1 se leu algo e 0 se a entrada acabou (Ctrl+D / Ctrl+Z). */
static int ler_comando(char *entrada, int tamanho) {
    printf("minigit> ");
    fflush(stdout);
    if (fgets(entrada, tamanho, stdin) == NULL) {
        return 0;
    }
    entrada[strcspn(entrada, "\r\n")] = '\0';
    return 1;
}

int main(void) {
    Lista *historico = lista_criar();   /* commits */
    Fila  *staging   = fila_criar();    /* arquivos adicionados */
    pilha  stash;                       /* stashes guardados (vetor) */
    pil_inicializar(&stash);
    int proximo_id = 1;
    int proximo_stash = 1;

    char entrada[MAX_ENTRADA];
    int continuar = 1;
    puts("mini-git em C -- digite 'ajuda' para ver os comandos.");

    while (continuar && ler_comando(entrada, sizeof entrada)) {
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
            printf("Stash (pilha): %d entrada(s)\n", pil_tamanho(stash));

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
            if (sub && strcmp(sub, "pop") == 0) {         /* pop */
                if (pil_vazia(stash)) { puts("erro: stash vazio, nada para recuperar"); continue; }
                if (!fila_vazia(staging)) { puts("erro: o staging precisa estar vazio"); continue; }
                Stash topo;
                pil_topo(stash, &topo);
                /* Reabre os arquivos antes de mexer na pilha: se algo falhar,
                 * o stash e mantido (como o git stash pop faz no conflito). */
                Fila *nova = fila_criar();
                int falhou = 0;
                for (int i = 0; i < topo.n_arquivos; i++) {
                    Arquivo *a = arquivo_ler(topo.arquivos[i]);
                    if (!a) {
                        printf("erro: nao consegui reabrir '%s'; o stash foi mantido\n", topo.arquivos[i]);
                        falhou = 1;
                        break;
                    }
                    fila_enfileirar(nova, a);
                }
                if (falhou) { fila_liberar(nova); continue; }
                pil_pop(&stash, &topo);
                fila_liberar(staging);
                staging = nova;
                printf("stash %d \"%s\" recuperado: %d arquivo(s) no staging (%d na pilha)\n",
                       topo.id, topo.descricao, topo.n_arquivos, pil_tamanho(stash));

            } else if (sub && strcmp(sub, "list") == 0) { /* list: so le */
                if (pil_vazia(stash)) { puts("stash vazio, nada para listar"); continue; }
                for (int i = pil_tamanho(stash) - 1; i >= 0; i--) {
                    const Stash *st = &stash.itens[i];
                    printf("stash@{%d}: stash %d \"%s\" (", pil_tamanho(stash) - 1 - i, st->id, st->descricao);
                    for (int j = 0; j < st->n_arquivos; j++)
                        printf("%s%s", j ? ", " : "", st->arquivos[j]);
                    puts(")");
                }

            } else if (sub && strcmp(sub, "drop") == 0) { /* drop: pop sem aplicar */
                Stash descartado;
                if (!pil_pop(&stash, &descartado)) { puts("erro: stash vazio, nada para descartar"); continue; }
                printf("stash %d \"%s\" descartado (%d na pilha)\n",
                       descartado.id, descartado.descricao, pil_tamanho(stash));

            } else {                                      /* push: stash <descricao> */
                char *resto = sub ? strtok(NULL, "") : NULL;
                if (sub && resto) sub[strlen(sub)] = ' ';   /* recompoe a descricao */
                char *desc = sub ? limpar_mensagem(sub) : NULL;
                if (!desc || *desc == '\0') { puts("uso: stash <descricao> | stash pop | stash list | stash drop"); continue; }
                if (fila_vazia(staging)) { puts("erro: nada para guardar: staging vazio"); continue; }
                if (pil_cheia(stash)) { printf("erro: stash cheio (maximo de %d)\n", PIL_MAX); continue; }
                Stash novo = {0};
                if (strlen(desc) >= sizeof novo.descricao) {
                    printf("erro: descricao com mais de %zu caracteres\n", sizeof novo.descricao - 1);
                    continue;
                }
                if (fila_tamanho(staging) > ARQ_MAX) {
                    printf("erro: um stash guarda no maximo %d arquivos\n", ARQ_MAX);
                    continue;
                }
                /* Tira os arquivos da fila; se algum nome nao couber, devolve tudo. */
                Arquivo *tmp[ARQ_MAX];
                int n = fila_tamanho(staging), cabe = 1;
                for (int i = 0; i < n; i++) tmp[i] = fila_desenfileirar(staging);
                for (int i = 0; i < n; i++)
                    if (strlen(tmp[i]->nome) >= sizeof novo.arquivos[0]) cabe = 0;
                if (!cabe) {
                    for (int i = 0; i < n; i++) fila_enfileirar(staging, tmp[i]);
                    printf("erro: nome de arquivo com %zu caracteres ou mais\n", sizeof novo.arquivos[0]);
                    continue;
                }
                for (int i = 0; i < n; i++) {
                    strcpy(novo.arquivos[i], tmp[i]->nome);
                    arquivo_liberar(tmp[i]);
                }
                novo.n_arquivos = n;
                novo.id = proximo_stash++;
                strcpy(novo.descricao, desc);
                pil_push(&stash, novo);                   /* staging ja ficou vazio */
                printf("stash %d guardado (%d na pilha)\n", novo.id, pil_tamanho(stash));
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
            continuar = 0;
        } else {
            printf("comando desconhecido: '%s' (digite 'ajuda')\n", cmd);
        }
    }

    lista_liberar(historico);
    fila_liberar(staging);
    return 0;
}