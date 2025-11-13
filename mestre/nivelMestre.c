/* bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =========================
   DEFINIÇÕES E ESTRUTURAS
   ========================= */

/* Estrutura que representa uma sala (nó da árvore binária) */
typedef struct Sala {
    char nome[64];           // identificador da sala
    char pista[200];         // pista associada (pode ser string vazia)
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST que armazena pistas coletadas em ordem alfabética */
typedef struct PistaNode {
    char *pista;                // string alocada dinamicamente
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/* Nó para cadeias na tabela hash (encadeamento separado) */
typedef struct HashNode {
    char *chave;           // pista (chave)
    char *suspeito;        // suspeito associado à pista (valor)
    struct HashNode *next;
} HashNode;

/* Tabela hash simples */
#define HASH_SIZE 101  // tamanho razoável para poucos elementos
HashNode *hashTable[HASH_SIZE];

/* =========================
   FUNÇÃO: criarSala
   -----------------
   Cria dinamicamente uma sala com nome e pista.
   Retorna ponteiro para Sala alocada.
   ========================= */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: malloc criarSala\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, sizeof(s->nome)-1);
    s->nome[sizeof(s->nome)-1] = '\0';
    strncpy(s->pista, pista, sizeof(s->pista)-1);
    s->pista[sizeof(s->pista)-1] = '\0';
    s->esquerda = s->direita = NULL;
    return s;
}

/* =========================
   FUNÇÕES BST (pistas)
   ========================= */

/* Cria nó de pista */
PistaNode* criarPistaBST(const char *pista) {
    PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
    if (!n) { fprintf(stderr,"Erro: malloc criarPistaBST\n"); exit(EXIT_FAILURE); }
    n->pista = strdup(pista);
    n->esq = n->dir = NULL;
    return n;
}

/* inserirPista - insere uma pista na BST (sem duplicatas) */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (!raiz) return criarPistaBST(pista);
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0) raiz->dir = inserirPista(raiz->dir, pista);
    else {
        // já existe a pista — não inserir duplicada
    }
    return raiz;
}

/* Percorre em ordem e imprime pistas coletadas */
void exibirPistasInOrder(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistasInOrder(raiz->dir);
}

/* Percorre em ordem e conta quantas pistas apontam para 'suspeitoAcusado' */
int contarPistasParaSuspeito(PistaNode *raiz, const char *suspeitoAcusado) {
    if (!raiz) return 0;
    int count = 0;
    // verificar esquerda
    count += contarPistasParaSuspeito(raiz->esq, suspeitoAcusado);
    // verificar o nó atual: procurar suspeito na tabela hash
    // função encontrarSuspeito será usada (declaração abaixo)
    extern char* encontrarSuspeito(const char *pista); // forward
    char *s = encontrarSuspeito(raiz->pista);
    if (s != NULL) {
        // comparar nomes ignorando maiúsculas/minúsculas
        char a[100], b[100];
        strncpy(a, s, sizeof(a)-1); a[sizeof(a)-1]='\0';
        strncpy(b, suspeitoAcusado, sizeof(b)-1); b[sizeof(b)-1]='\0';
        // tolower
        for (int i=0; a[i]; i++) a[i] = tolower((unsigned char)a[i]);
        for (int i=0; b[i]; i++) b[i] = tolower((unsigned char)b[i]);
        if (strcmp(a,b) == 0) count++;
    }
    // direita
    count += contarPistasParaSuspeito(raiz->dir, suspeitoAcusado);
    return count;
}

/* Libera memória da BST de pistas */
void liberarPistasBST(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistasBST(raiz->esq);
    liberarPistasBST(raiz->dir);
    free(raiz->pista);
    free(raiz);
}

/* =========================
   FUNÇÕES TABELA HASH
   ========================= */

/* djb2 hash function (strings) */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

/* inserirNaHash - insere associação pista -> suspeito */
void inserirNaHash(const char *pista, const char *suspeito) {
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashNode *node = (HashNode*) malloc(sizeof(HashNode));
    if (!node) { fprintf(stderr,"Erro: malloc inserirNaHash\n"); exit(EXIT_FAILURE); }
    node->chave = strdup(pista);
    node->suspeito = strdup(suspeito);
    node->next = hashTable[h];
    hashTable[h] = node;
}

/* encontrarSuspeito - retorna o suspeito associado a uma pista (ou NULL se não existir) */
char* encontrarSuspeito(const char *pista) {
    unsigned long h = hash_djb2(pista) % HASH_SIZE;
    HashNode *cur = hashTable[h];
    while (cur) {
        if (strcmp(cur->chave, pista) == 0) return cur->suspeito;
        cur = cur->next;
    }
    return NULL;
}

/* Libera toda a tabela hash */
void liberarHash() {
    for (int i = 0; i < HASH_SIZE; ++i) {
        HashNode *cur = hashTable[i];
        while (cur) {
            HashNode *tmp = cur;
            cur = cur->next;
            free(tmp->chave);
            free(tmp->suspeito);
            free(tmp);
        }
        hashTable[i] = NULL;
    }
}

/* =========================
   FUNÇÃO: explorarSalas
   ----------------------
   Navega pela árvore de salas, coleta pistas automaticamente
   e insere na BST de pistas.
   ========================= */
void explorarSalas(Sala *inicio, PistaNode **arvorePistas) {
    Sala *atual = inicio;
    char escolha;

    printf("\nVocê entrou na '%s'.\n", atual->nome);

    while (1) {
        // Exibir e coletar pista, se houver
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: \"%s\"\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
            // mostrar para qual suspeito essa pista aponta (consulta na hash)
            char *s = encontrarSuspeito(atual->pista);
            if (s) printf("   (Essa pista indica: %s)\n", s);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        // Mostrar opções
        printf("\nCaminhos disponíveis a partir de '%s':\n", atual->nome);
        if (atual->esquerda) printf("  (e) Ir para '%s' (esquerda)\n", atual->esquerda->nome);
        if (atual->direita)  printf("  (d) Ir para '%s' (direita)\n", atual->direita->nome);
        printf("  (s) Sair da exploração\n");

        printf("\nEscolha sua ação: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda) {
                atual = atual->esquerda;
                printf("\nVocê foi para '%s'.\n", atual->nome);
            } else {
                printf("Não há caminho à esquerda!\n");
            }
        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita) {
                atual = atual->direita;
                printf("\nVocê foi para '%s'.\n", atual->nome);
            } else {
                printf("Não há caminho à direita!\n");
            }
        } else if (escolha == 's' || escolha == 'S') {
            printf("\nVocê encerrou a exploração.\n");
            break;
        } else {
            printf("Opção inválida — tente novamente.\n");
        }
    }
}

/* =========================
   FUNÇÃO: verificarSuspeitoFinal
   -------------------------------
   Ao final da exploração, solicita a acusação do jogador e
   verifica se existem ao menos 2 pistas que apontam para o acusado.
   ========================= */
void verificarSuspeitoFinal(PistaNode *arvorePistas) {
    char acusado[100];
    // consumir newline pendente no buffer antes de fgets
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    printf("\n=== Fase de Acusação ===\n");
    if (!arvorePistas) {
        printf("Você não coletou pistas — não há evidências suficientes.\n");
        return;
    }

    printf("Pistas coletadas:\n");
    exibirPistasInOrder(arvorePistas);

    printf("\nDigite o nome do suspeito que deseja acusar: ");
    if (!fgets(acusado, sizeof(acusado), stdin)) {
        printf("Entrada inválida.\n");
        return;
    }
    // remover newline
    acusado[strcspn(acusado, "\n")] = '\0';
    if (strlen(acusado) == 0) {
        printf("Nenhum nome fornecido.\n");
        return;
    }

    int contador = contarPistasParaSuspeito(arvorePistas, acusado);
    printf("\n%s recebeu %d pista(s) que o ligam ao crime.\n", acusado, contador);

    if (contador >= 2) {
        printf("\nAcusação válida: existem evidências suficientes para acusar %s.\n", acusado);
        printf("O caso prossegue para o interrogatório e possível condenação.\n");
    } else {
        printf("\nAcusação fraca: não há pistas suficientes para responsabilizar %s.\n", acusado);
        printf("Recomenda-se continuar investigando.\n");
    }
}

/* =========================
   FUNÇÃO: liberarSalas (libera árvore de salas - pós-ordem)
   ========================= */
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

/* =========================
   FUNÇÃO: main
   ========================= */
int main(void) {
    /* Inicializar tabela hash nula */
    for (int i=0;i<HASH_SIZE;i++) hashTable[i] = NULL;

    /* Criar mapa fixo da mansão (árvore binária) com pistas estáticas */
    // Exemplo de mapa:
    //              Hall
    //             /    \
    //        SalaEstar  Cozinha
    //        /    \         \
    //  Biblioteca Jardim    Porão
    //                         \
    //                      Escritório

    Sala *hall = criarSala("Hall de Entrada", "A chave do escritório está faltando.");
    Sala *salaEstar = criarSala("Sala de Estar", "Um retrato com uma mancha vermelha.");
    Sala *cozinha = criarSala("Cozinha", "Pegadas de lama perto da janela.");
    Sala *biblioteca = criarSala("Biblioteca", "Página rasgada mencionando \"Eleanor\".");
    Sala *jardim = criarSala("Jardim", "Um lenço com as iniciais 'M.R.'");
    Sala *porao = criarSala("Porão", "Marcas de ferramentas próximas ao cofre.");
    Sala *escritorio = criarSala("Escritório", "Um bilhete com a assinatura 'Marta R.'");

    // montar conexões
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->direita = porao;
    porao->direita = escritorio;

    /* Criar tabela hash que associa cada pista a um suspeito */
    // Observação: aqui associamos as strings exatas das pistas criadas acima
    inserirNaHash("A chave do escritório está faltando.", "Eleanor");
    inserirNaHash("Um retrato com uma mancha vermelha.", "Carlos");
    inserirNaHash("Pegadas de lama perto da janela.", "Marta R.");
    inserirNaHash("Página rasgada mencionando \"Eleanor\".", "Eleanor");
    inserirNaHash("Um lenço com as iniciais 'M.R.'", "Marta R.");
    inserirNaHash("Marcas de ferramentas próximas ao cofre.", "Carlos");
    inserirNaHash("Um bilhete com a assinatura 'Marta R.'", "Marta R.");

    /* Árvore BST de pistas coletadas começa vazia */
    PistaNode *arvorePistas = NULL;

    /* Mensagem inicial */
    printf("=== Detective Quest: Julgamento Final ===\n");
    printf("Você é o detetive. Explore a mansão, colete pistas e acuse um suspeito.\n");
    printf("Controles: 'e' = esquerda, 'd' = direita, 's' = sair\n");

    /* Exploração interativa */
    explorarSalas(hall, &arvorePistas);

    /* Fase de acusação: listar pistas e pedir o acusado */
    verificarSuspeitoFinal(arvorePistas);

    /* Limpeza de memória */
    liberarPistasBST(arvorePistas);
    liberarHash();
    liberarSalas(hall);

    printf("\nObrigado por jogar Detective Quest! Até a próxima investigação.\n");

    return 0;
}
