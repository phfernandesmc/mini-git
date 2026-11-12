#include <stddef.h>
#include "pilha.h"

void pil_inicializar(pilha *p) {
    p->topo = -1;
}

bool pil_push(pilha *p, Stash val) {
    if (pil_cheia(*p))                 /* overflow: nao cabe */
        return false;
    p->topo++;                         /* o topo sobe uma posicao */
    p->itens[p->topo] = val;           /* e o item novo fica nela */
    return true;
}

bool pil_pop(pilha *p, Stash *val) {
    if (pil_vazia(*p) || val == NULL)  /* underflow: nada para tirar */
        return false;
    *val = p->itens[p->topo];          /* devolve o item do topo */
    p->topo--;                         /* e o topo desce uma posicao */
    return true;
}

bool pil_topo(pilha p, Stash *val) {
    if (pil_vazia(p) || val == NULL)
        return false;
    *val = p.itens[p.topo];
    return true;
}

bool pil_vazia(pilha p)  { return p.topo == -1; }
bool pil_cheia(pilha p)  { return p.topo == PIL_MAX - 1; }
int  pil_tamanho(pilha p) { return p.topo + 1; }
