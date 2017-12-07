/*//
//  fila_lista.h
//  Lista
//
//  Created by Bernardo Ruga on 6/5/16.
//  Copyright © 2016 Bernardo Ruga. All rights reserved.
//

/* nó da lista para armazenar valores reais */
struct noLista
{
    int info;
    struct noLista* prox;
    
};
typedef struct noLista NoLista;

struct fila
{
    NoLista * ini;
    NoLista * fim;
    
};

typedef struct fila Fila;

Fila* fila_cria (void);

void fila_insere (Fila* f, int m);

int fila_retira (Fila* f);

int fila_vazia (Fila* f);

void fila_libera (Fila* f);
