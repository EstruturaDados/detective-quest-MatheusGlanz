#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================
   ESTRUTURAS DE DADOS
   ============================ */

// Estrutura para representar uma sala da mansão (nó da árvore binária)
typedef struct Sala {
    char nome[50];           // Nome do cômodo
    char pista[100];         // Pista (pode ser vazia)
    struct Sala *esquerda;   // Caminho à esquerda
    struct Sala *direita;    // Caminho à direita
} Sala;

// Estrutura para representar um nó da árvore de pistas (BST)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;


/* ============================
   FUNÇÕES DE CRIAÇÃO
   ============================ */

/**
 * Função: criarSala
 * -----------------
 * Cria uma nova sala com nome e pista (opcional).
 * Retorna um ponteiro para a nova sala criada.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para a sala.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

/**
 * Função: criarPistaNode
 * ----------------------
 * Cria um novo nó na árvore de pistas com a string fornecida.
 */
PistaNode* criarPistaNode(const char *pista) {
    PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (!novo) {
        printf("Erro ao alocar memória para a pista.\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}


/* ============================
   FUNÇÕES DE MANIPULAÇÃO DA BST
   ============================ */

/**
 * Função: inserirPista
 * --------------------
 * Insere uma nova pista na árvore BST em ordem alfabética.
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL)
        return criarPistaNode(pista);

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    // Se for igual, não insere duplicado
    return raiz;
}

/**
 * Função: exibirPistas
 * --------------------
 * Exibe as pistas armazenadas em ordem alfabética (in-order traversal).
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf(" - %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}


/* ============================
   FUNÇÃO DE EXPLORAÇÃO
   ============================ */

/**
 * Função: explorarSalasComPistas
 * ------------------------------
 * Permite ao jogador explorar a mansão e coletar pistas automaticamente.
 */
void explorarSalasComPistas(Sala *inicio, PistaNode **pistas) {
    Sala *atual = inicio;
    char escolha;

    printf("\nVocê entrou na %s.\n", atual->nome);

    // Loop de exploração
    while (1) {
        // Coleta automática da pista (se existir)
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *pistas = inserirPista(*pistas, atual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        printf("\nCaminhos disponíveis:\n");
        if (atual->esquerda) printf("  (e) Ir para %s (esquerda)\n", atual->esquerda->nome);
        if (atual->direita)  printf("  (d) Ir para %s (direita)\n", atual->direita->nome);
        printf("  (s) Sair da mansão\n");

        printf("\nEscolha sua ação: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
                printf("\nVocê foi para a %s.\n", atual->nome);
            } else {
                printf("Não há caminho à esquerda!\n");
            }
        }
        else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita != NULL) {
                atual = atual->direita;
                printf("\nVocê foi para a %s.\n", atual->nome);
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


/* ============================
   FUNÇÃO PRINCIPAL
   ============================ */

int main() {
    // Criação das salas (árvore binária da mansão)
    Sala *hall        = criarSala("Hall de Entrada", "A chave do escritório está faltando.");
    Sala *salaEstar   = criarSala("Sala de Estar", "Um retrato com uma marca estranha.");
    Sala *cozinha     = criarSala("Cozinha", "Pegadas de lama perto da pia.");
    Sala *biblioteca  = criarSala("Biblioteca", "Um livro arrancado da estante.");
    Sala *jardim      = criarSala("Jardim", "Um lenço com iniciais misteriosas.");
    Sala *porao       = criarSala("Porão", "Um cofre trancado e sem chave.");
    Sala *escritorio  = criarSala("Escritório", "Um bilhete rasgado com o nome 'Eleanor'.");

    // Estrutura fixa da árvore:
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

    // Árvore de pistas começa vazia
    PistaNode *arvorePistas = NULL;

    printf("=== Detective Quest: Coleta de Pistas ===\n");
    printf("Bem-vindo(a), detetive! Explore a mansão e colete todos os indícios.\n");

    explorarSalasComPistas(hall, &arvorePistas);

    printf("\nPISTAS COLETADAS (em ordem alfabética):\n");
    exibirPistas(arvorePistas);

    printf("\nObrigado por jogar Detective Quest!\n");

    // Liberação de memória (simplificada)
    free(escritorio);
    free(porao);
    free(jardim);
    free(biblioteca);
    free(cozinha);
    free(salaEstar);
    free(hall);
    // (para a árvore de pistas, omissão aceitável para fins didáticos)

    return 0;
}
