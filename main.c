/**+-------------------------------------------------------------+
  |          UNIFAL – Universidade Federal de Alfenas.          |
  | BACHARELADO EM CIENCIA DA COMPUTACAO.                       |
  | Trabalho..: SIMULACAO DE SISTEMA DE ARQUIVOS FAT            |
  | Disciplina: Estrutura de Dados I                            |
  | Professor.: Luiz Eduardo da Silva                           |
  | Aluno(s)..: Glauco Amaral Geraldino                         |
  |             Gabriel Marinello Moura Leite                   |
  |             Vitor Muniz                                     |
  | Data......: 27/07/2017                                      |
  +-------------------------------------------------------------+**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define TAM_GRANULO 3
#define TAM_MEMORIA 30
#define TRUE 1
#define FALSE 0

typedef struct noSet * ptnoSet;

typedef struct noSet {
    int inicio, fim;
    ptnoSet prox;
} noSet;


typedef struct noArq * ptnoArq;

typedef struct noArq {
    char nome[13];
    int caracteres;
    ptnoSet setores;
    ptnoArq prox;
} noArq;


typedef char memoria[TAM_MEMORIA][TAM_GRANULO];

void mostraSetores(ptnoSet S, char *n) {
    printf("%s = [", n);
    while (S) {
        printf("(%d,%d)", S->inicio, S->fim);
        S = S->prox;
        if (S) printf(",");
    }
    printf("]\n");

}

void mostraArquivos(ptnoArq A) {
    printf("Arquivos:\n");
    while (A) {
        printf("  %12s, %2d caracter(es).  ", A->nome, A->caracteres);
        mostraSetores(A->setores, "Setores");
        A = A->prox;
    }
    printf("\n");

}

void mostraMemoria(memoria Memo) {
    int i, j;
    for (i = 0; i < TAM_MEMORIA; i++) {
        printf("%3d:[", i);
        for (j = 0; j < TAM_GRANULO - 1; j++)
            printf("%c,", Memo[i][j]);
        printf("%c]", Memo[i][TAM_GRANULO - 1]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }

}

void inicia(ptnoSet *Area, ptnoArq *Arq, memoria Memo) {
    int i, j;
    * Area = (ptnoSet) malloc(sizeof (noSet));
    (*Area)->inicio = 0;
    (*Area)->fim = TAM_MEMORIA - 1;
    (*Area)->prox = NULL;
    * Arq = NULL;
    for (i = 0; i < TAM_MEMORIA; i++)
        for (j = 0; j < TAM_GRANULO; j++)
            Memo[i][j] = ' ';

}

/*---------------------------------------------
 * Implementar as rotinas para simulacao da FAT
 *---------------------------------------------*/
void gravar(ptnoSet *Area, ptnoArq *Arq, memoria Memo, char *c, char *cnome) {
    int qtdSetoresNec, qtdSetoresDisp = 0;
    int qtdChar = strlen(c);
    int i = 0, j = 0; //Variaveis Controladoras dos for

    if (qtdChar % TAM_GRANULO)
        qtdSetoresNec = ((int) qtdChar / TAM_GRANULO) + 1;
    else
        qtdSetoresNec = qtdChar / TAM_GRANULO;

    ptnoSet ContaArea = (*Area); //Utilizado parar contar a quantidade de setores disponiveis na area
    ptnoArq iguais = (*Arq); //Utilizado para achar se há algum arquivo com nome igual a ser gravado

    //Verificacao para ver se há alguma arquivo com mesmo nome
    while (iguais) {
        if (!strcmp(cnome, iguais->nome)) {
            puts("Nao foi possivel, arquivo ja existente!");
            return;
        }
        iguais = iguais->prox;
    }
    //Contagem de Setores Disponiveis
    while (ContaArea) {
        qtdSetoresDisp += (ContaArea->fim - ContaArea-> inicio) + 1;
        ContaArea = ContaArea->prox;
    }

    if (qtdSetoresNec > qtdSetoresDisp)
        puts("Setores Cheios!");
    else {
        //Se caiu aqui é pq da pra gravar!
        ptnoSet ultimo = NULL; //Utilizado para fazer a ligacao entre os setores que o arquivo utiliza
        int CharPorChar = 0; //Utilizado para correr caracter a caracter do texto na gravacao


        ptnoArq auxArq = (ptnoArq) malloc(sizeof (noArq));
        strcpy(auxArq -> nome, cnome);
        auxArq -> caracteres = qtdChar;
        auxArq -> setores = NULL;
        auxArq -> prox = NULL;

        ptnoArq ant = NULL, atual = *Arq;


        while (atual && strcmp(cnome, atual->nome) > 0) {
            ant = atual;
            atual = atual->prox;
        }

        if (!ant) {//Primeira Insercao ou Primeira Posicao
            auxArq -> prox = *Arq;
            *Arq = auxArq;
        } else { //Demais insercoes 
            auxArq -> prox = ant->prox; //arquivo aponta para o atual
            ant -> prox = auxArq; //anterior aponta para o arquivo
        }

        while (qtdSetoresNec) {

            if (((*Area)->fim - (*Area)->inicio + 1) > qtdSetoresNec) {
                ptnoSet auxNo = (ptnoSet) malloc(sizeof (noSet));
                auxNo->inicio = (*Area) -> inicio;
                auxNo->fim = (*Area)->inicio + (qtdSetoresNec - 1);
                auxNo->prox = NULL;

                //Ligacao Entre os Nós dos setores utilizados pelo Arquivo
                if (!(auxArq -> setores)) {//Primeiro nó dos setores no arquivo
                    auxArq->setores = auxNo;
                    ultimo = auxNo;
                } else {
                    ultimo -> prox = auxNo;
                    ultimo = auxNo;
                }

                //Gravacao Na Memoria.
                for (i = (auxNo->inicio); i <= (auxNo -> fim); i++) {
                    for (j = 0; j < TAM_GRANULO && qtdChar; j++) {
                        Memo[i][j] = *(c + CharPorChar);
                        CharPorChar++;
                        qtdChar--;
                    }
                }
                //Mudanca Aritmetica no Nó dos setores que foram utilizados
                (*Area)->inicio += qtdSetoresNec;
                qtdSetoresNec = 0; //Zero, pois ou consigo gravar direto no primeiro nó da Area ou é a ultima iteracao da gravacao
            } else {

                ptnoSet aux = (ptnoSet) malloc(sizeof (noSet));
                aux->inicio = (*Area)->inicio;
                aux->fim = (*Area)->fim;
                aux->prox = NULL;

                //Ligacao Entre os Nós dos setores utilizados pelo Arquivo
                if (!(auxArq->setores)) {//Primeira no dos setores no arquivo
                    auxArq->setores = aux;
                    ultimo = aux;
                } else {
                    ultimo->prox = aux;
                    ultimo = aux;
                }

                //Gravacao Na Memoria.
                for (i = aux->inicio; i < (aux->fim + 1); i++) {
                    for (j = 0; j < TAM_GRANULO && qtdChar; j++) {
                        Memo[i][j] = *(c + CharPorChar);
                        qtdChar--;
                        CharPorChar++;
                    }
                }
                //Aqui Ocorre a diminuicao de setores e desalocamento do setor no Area pelo fato do Arquivo Utilizar
                qtdSetoresNec -= (aux->fim - aux->inicio) + 1;
                ptnoSet desaloca = (*Area);
                (*Area) = (*Area)->prox;
                free(desaloca);
            }
        }
    }
}

void deletar(ptnoSet *Area, ptnoArq *Arq, memoria Memo, char *cnome) {
    int i = 0, j = 0; //Utilizado para o controle do for
    ptnoArq ant = NULL, atual = *Arq;
    while (atual && strcmp(cnome, atual->nome)) {
        ant = atual;
        atual = atual->prox;
    }
    if (!atual)
        puts("Arquivo inexistente.");
    else {
        //Se caiu aqui é porque possuo um arquivo
        puts("Arquivo Existente Excluido com Sucesso!");

        //Refazendo a Ligacao entre os Arquivos
        if (!ant)
            *Arq = atual->prox;
        else
            ant -> prox = atual -> prox;

        ptnoSet ContadorSetores = atual->setores; //

        while (atual->setores) {
            //Apagar o Arquivo da Memoria
            for (i = ContadorSetores->inicio; i < ContadorSetores->fim + 1; i++) {
                for (j = 0; j < TAM_GRANULO; j++) {
                    Memo[i][j] = ' ';
                }
            }
            ptnoSet AreaAnt = NULL, AreaAtual = *Area;
            //Procurar o local na Area Disponivel onde devolverei os setores utilizados
            while (AreaAtual && (ContadorSetores->inicio > AreaAtual->inicio)) {
                AreaAnt = AreaAtual;
                AreaAtual = AreaAtual -> prox;
            }

            atual->setores = ContadorSetores->prox; //*Arq (no caso, 'atual') comeca a apontar para o setor proximo do que vou deletar
            //Caso eu vá devolver para primeira posicao de Area
            //Aqui eu nao desaloco, apenas "levo" o setores utilizados pelo arquivo para o area disponivel
            if (!AreaAnt) {
                ContadorSetores -> prox = AreaAtual;
                *Area = ContadorSetores;
            } else {
                ContadorSetores->prox = AreaAnt -> prox;
                AreaAnt -> prox = ContadorSetores;
            }
            ContadorSetores = atual -> setores;
        }
        free(atual); //Liberar o Arquivo dos setores de Arquivos
    }

    //Juntar os nós Consecutivos no Setores Livres
    ptnoSet JuntaAnt = (*Area), JuntaAtual = (*Area)->prox;
    while (JuntaAtual) {
        if (JuntaAnt->fim == JuntaAtual->inicio - 1) {
            ptnoSet aux = JuntaAtual;
            JuntaAnt->fim = JuntaAtual->fim;
            JuntaAtual = JuntaAtual->prox;
            JuntaAnt->prox = JuntaAtual;
            free(aux);
        } else {
            JuntaAnt->prox = JuntaAtual;
            JuntaAnt = JuntaAtual;
            JuntaAtual = JuntaAtual->prox;
        }
    }

}

void defrag(ptnoArq *Arq, ptnoSet *Area, memoria Memo) {
    int i, j;
    ptnoArq Aux = malloc(sizeof (*Aux));
    Aux->setores = NULL;
    Aux->prox = NULL;
    ptnoArq Primeiro = Aux;
    ptnoArq Anterior = Aux;


    //Zerando area para retornar ao seu estado inicial
    ptnoSet AreaAux = (*Area)->prox;
    while (AreaAux) {
        ptnoSet delete = AreaAux;
        AreaAux = AreaAux->prox;
        free(delete);
    }

    (*Area)->inicio = 0;
    (*Area)->fim = TAM_MEMORIA - 1;
    (*Area)->prox = NULL;

    //Movendo os arquivos para a estrutura auxiliar
    while ((*Arq)) {
        ptnoArq delete = (*Arq);
        int QtdSetores = 0, ContadorSetores = 0;

        //Copiando Arq pra Aux
        strcpy(Aux->nome, (*Arq)->nome);
        Aux->caracteres = (*Arq)->caracteres;
        QtdSetores = ((*Arq)->caracteres) % TAM_GRANULO ? (((*Arq)->caracteres) / TAM_GRANULO) : (((*Arq)->caracteres) / TAM_GRANULO) - 1;
        Aux->setores = malloc(sizeof (*(Aux->setores)));
        Aux->setores->inicio = (*Area)->inicio;
        Aux->setores->fim = QtdSetores + Aux->setores->inicio;
        //Fim da copia de arq para aux;
        char CharAux[Aux->caracteres];

        ptnoSet CopiaMemoria = (*Arq)->setores;

        int ContadorVetor = 0;
        while (CopiaMemoria) {
            for (i = CopiaMemoria->inicio; i <= CopiaMemoria->fim; i++) {
                for (j = 0; j < TAM_GRANULO; j++)
                    CharAux[ContadorVetor++] = Memo[i][j];
            }
            CopiaMemoria = CopiaMemoria->prox;
        }

        (*Area)->inicio += QtdSetores + 1;

        //Alocando a proxima estrutura para Arq auxiliar
        Anterior = Aux;
        Aux = Aux->prox;
        Aux = malloc(sizeof (*Aux));
        Aux->setores = NULL;
        Aux->prox = NULL;
        Anterior->prox = Aux;

        //Passando para frente e desalocando a estrutura antiga
        (*Arq) = (*Arq)->prox;
        free(delete);

        for (i = Anterior->setores->inicio; i <= Anterior->setores->fim; i++) {
            for (j = 0; j < TAM_GRANULO && ((i * TAM_GRANULO) + j) < Anterior->caracteres; j++) {
                Memo[i][j] = CharAux[(i * TAM_GRANULO) + j];
            }
        }
    }

    for (i = (*Area)->inicio; i < TAM_MEMORIA - 1; i++) {
        for (j = 0; j < TAM_GRANULO; j++) {
            Memo[i][j] = ' ';
        }
    }

    Anterior->prox = NULL;
    free(Aux);
    (*Arq) = Primeiro;

}

void apresentar(ptnoArq Arq, memoria Memo, char *cnome) {
    ptnoArq ArquivoApresent = Arq;
    int i = 0, j = 0;

    while (ArquivoApresent) {
        if (!strcmp(cnome, ArquivoApresent->nome))
            break;
        ArquivoApresent = ArquivoApresent->prox;
    }
    if (!ArquivoApresent)
        puts("Arquivo não Encontrado");
    else {
        puts("Arquivo Encontrado \n");
        int qtdCaracteres = ArquivoApresent->caracteres;
        ptnoSet SetoresApresent = ArquivoApresent->setores;
        printf("Setores | Conteudo\n");
        printf("--------+---------\n");
        while (SetoresApresent) {
            printf("( %d, %d ) | ", SetoresApresent->inicio, SetoresApresent->fim);
            for (i = SetoresApresent->inicio; i < (SetoresApresent->fim + 1); i++) {
                for (j = 0; j < TAM_GRANULO && qtdCaracteres; j++) {
                    printf("%c", Memo[i][j]);
                    qtdCaracteres--;
                }
            }
            printf("\n");
            SetoresApresent = SetoresApresent->prox;
        }

    }


}

void ajuda() {
    printf("\nCOMANDOS\n");
    printf("--------\n");
    printf("G <arquivo.txt> <texto><ENTER>\n");
    printf(" -Grava o <arquivo.txt> e conteúdo <texto> no disco\n");
    printf("D <arquivo.txt>\n");
    printf(" -Deleta o <arquivo.txt> do disco\n");
    printf("A <arquivo.txt>\n");
    printf(" -Apresenta o conteudo do <arquivo.txt>\n");
    printf("M\n");
    printf(" -Mostra as estruturas utilizadas\n");
    printf("H\n");
    printf(" -Apresenta essa lista de comandos\n");
    printf("F\n");
    printf(" -Fim da simulacao\n");

}

/*------------------------------------------
 * CORPO PRINCIPAL DO PROGRAMA
 *------------------------------------------*/
int main(void) {
    ptnoSet Area, set;
    ptnoArq Arq, ant; //Arq aponta para inicio dos Arquivos
    memoria Memo;
    char com[3];
    char nome[13]; //Nome do Arquivo
    char texto[TAM_MEMORIA * TAM_GRANULO]; //Texto que irei passar para funcao

    inicia(&Area, &Arq, Memo);

    do {
        printf("\n=> ");
        scanf("%3s", com);
        com[0] = toupper(com[0]);
        switch (com[0]) {
            case 'G':
                scanf("%s %s", nome, texto);
                printf("nome = %s\n", nome);
                printf("texto = %s\n", texto);
                gravar(&Area, &Arq, Memo, texto, nome);
                /*
                 * Implementar as chamadas das funcoes pra GRAVAR arquivo
                 */
                break;
            case 'D':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                deletar(&Area, &Arq, Memo, nome);
                /*
                 * Implementar as chamadas das funcoes pra DELETAR arquivo
                 */
                break;
            case 'A':
                scanf("%s", nome);
                printf("nome = %s\n", nome);
                apresentar(Arq, Memo, nome);

                /*
                 * Implementar as chamadas das funcoes pra APRESENTAR arquivo
                 */
                break;
            case 'M':
                mostraSetores(Area, "Area");
                mostraArquivos(Arq);
                printf("Memoria:\n");
                mostraMemoria(Memo);
                break;
            case 'H':
                ajuda();
                break;
            case 'E':
                defrag(&Arq, &Area, Memo);
                break;
        }
    } while (com[0] != 'F');
    printf("\nFim da Execucao\n\n");
    return (EXIT_SUCCESS);

}