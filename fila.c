/*//
//  fila_lista.c
//  Lista
//
//  Created by Bernardo Ruga on 6/5/16.
//  Copyright © 2016 Bernardo Ruga. All rights reserved.
//

/* estrutura da fila */

#include "fila.h"
#include <stdio.h>
#include <stdlib.h>


Fila* fila_cria (void)
{
    Fila* f = (Fila *)malloc(sizeof(Fila));
    if (f==NULL)
    {
        printf("erro");
        return NULL;
    }

    
    f->ini = NULL;
    f->fim = NULL;
    
    return f;
}

void fila_insere (Fila* f, int m)
{
    //############################### NESSA AQUI QUE TA DANDO RUIM, VER PQ
    NoLista *novo = (NoLista *)malloc(sizeof(NoLista));

    novo->info = m; /* armazena informação */
    novo->prox = NULL; /* novo nó passa a ser o último */

    if (f->fim != NULL) /* verifica se lista não estava vazia */
        f->fim->prox = novo;

    else /* fila estava vazia */
        f->ini = novo;

    f->fim = novo; /* fila aponta para novo elemento */

}

int fila_retira (Fila* f)
{
    NoLista * t;
    int v;
    if (fila_vazia(f)){
        //printf("Fila vazia.\n");
        return -1;
    }
        
        t = f->ini;
        v = t->info;
        f->ini = t->prox;
        
        if (f->ini == NULL) /* verifica se fila ficou vazia */
            f->fim = NULL;
        free(t);
        return v;
}

int fila_vazia (Fila* f)
{
    if(f->fim==0)
        return 1;
    return 0;
}

void fila_libera (Fila* f)
{
    NoLista *q = f->ini;
    while (q!=NULL)
    {
        NoLista *t = q->prox;
        free(q);
        q = t;
    }
    free(f);
}
    