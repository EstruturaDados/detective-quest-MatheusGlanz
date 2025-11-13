/* bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estrutura que representa uma sala na mansão (nó da árvore)
typedef struct Sala {
    char nome[50];               // Nome da sala
    struct Sala *esquerda;       // Caminho à esquerda
    struct Sala *direita;        // Caminho à direita
} Sala;

/**
 * Função: criarSala
 * -----------------
 * Cria dinamicamente uma nova sala com o nome informado.
 * Retorna um ponteiro para a nova sala.
 */
Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro ao alocar memória para a sala '%s'.\n", nome);
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/**
 * Função: explorarSalas
 * ---------------------
 * Permite ao jogador explorar o mapa da mansão interativamente.
 * O jogador escolhe seguir para a esquerda (e) ou direita (d) até
 * alcançar uma sala sem saídas.
 */
void explorarSalas(Sala *atual) {
    char escolha;

    printf("\nVocê entrou na %s.\n", atual->nome);

    // Enquanto a sala atual tiver caminhos, o jogador pode escolher
    while (1) {
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Você chegou ao fim do caminho na %s.\n", atual->nome);
            break;
        }

        printf("\nCaminhos disponíveis:\n");
        if (atual->esquerda != NULL)
            printf("  (e) Ir para %s à esquerda\n", atual->esquerda->nome);
        if (atual->direita != NULL)
            printf("  (d) Ir para %s à direita\n", atual->direita->nome);
        printf("  (s) Sair da exploração\n");

        printf("\nEscolha sua ação: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
                printf("\nVocê se moveu para a %s.\n", atual->nome);
            } else {
                printf("Não há caminho à esquerda!\n");
            }
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL) {
                atual = atual->direita;
                printf("\nVocê se moveu para a %s.\n", atual->nome);
            } else {
                printf("Não há caminho à direita!\n");
            }
        }
        else if (escolha == 's' || escolha == 'S') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            break;
        }
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/**
 * Função: main
 * ------------
 * Monta o mapa fixo da mansão e inicia a exploração.
 */
int main() {
    // Criação automática da estrutura da mansão (árvore binária)
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *porao = criarSala("Porão");
    Sala *escritorio = criarSala("Escritório");

    // Estrutura da árvore:
    //           Hall
    //          /    \
    //   Sala Estar   Cozinha
    //     /   \          \
    //  Biblioteca Jardim   Porão
    //                       \
    //                     Escritório

    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->direita = porao;
    porao->direita = escritorio;

    printf("=== Detective Quest ===\n");
    printf("Bem-vindo(a) à mansão misteriosa!\n");
    printf("Você começará sua exploração a partir do Hall de Entrada.\n");

    explorarSalas(hall);

    printf("\nObrigado por jogar Detective Quest!\n");

    // Liberação de memória
    free(escritorio);
    free(porao);
    free(jardim);
    free(biblioteca);
    free(cozinha);
    free(salaEstar);
    free(hall);

    return 0;
}

