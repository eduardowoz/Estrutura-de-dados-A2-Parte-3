#include <stdio.h>    // Para entrada e saída 
#include <stdlib.h>   // Para alocação de memória 
#include <string.h>   // Para manipulação de strings 
#include <stdbool.h>  // Para usar tipos booleanos 

// --- Definições e Estruturas ---

#define MAX_CIDADES 100 // Número máximo de cidades no mapa
#define NOME_CIDADE_MAX 50 // Tamanho máximo do nome da cidade
#define INFINITO 99999 // Um valor grande para representar distâncias inatingíveis

// Estrutura para representar uma cidade
typedef struct Cidade {
    int id;                // ID único da cidade (índice no array de cidades)
    char nome[NOME_CIDADE_MAX]; // Nome da cidade
} Cidade;

// Estrutura para um nó na lista de adjacência (representa uma rota)
typedef struct NoRota {
    int id_destino;       // ID da cidade de destino desta rota
    int custo;            // Custo/distância da rota (peso da aresta)
    struct NoRota* prox;  // Ponteiro para a próxima rota na lista
} NoRota;

// Estrutura principal do Grafo (nosso mapa de cidades e rotas)
typedef struct Grafo {
    NoRota* adj[MAX_CIDADES];     // Array de listas de adjacência (um para cada cidade)
    Cidade cidades[MAX_CIDADES]; // Array para armazenar os dados das cidades
    int num_cidades;           // Contador de cidades atualmente no grafo
} Grafo;

//Funções Auxiliares

// Inicializa o grafo, definindo tudo como vazio
void inicializarGrafo(Grafo* g) {
    g->num_cidades = 0; // Começa com nenhuma cidade
    for (int i = 0; i < MAX_CIDADES; i++) {
        g->adj[i] = NULL; // Todas as listas de adjacência iniciam vazias
        g->cidades[i].id = -1; // Marca cidades como não existentes
    }
}

// Cria um novo nó para a lista de adjacência (uma nova rota)
NoRota* criarNoRota(int id_destino, int custo) {
    NoRota* novoNo = (NoRota*)malloc(sizeof(NoRota)); // Aloca memória para o novo nó
    if (novoNo == NULL) {
        printf("Erro de alocacao de memoria para NoRota.\n");
        exit(EXIT_FAILURE); // Sai do programa se houver erro
    }
    novoNo->id_destino = id_destino; // Define o ID da cidade de destino
    novoNo->custo = custo;           // Define o custo da rota
    novoNo->prox = NULL;             // O próximo é nulo por enquanto
    return novoNo;
}

// Libera a memória de todas as listas de adjacência do grafo
void liberarGrafo(Grafo* g) {
    for (int i = 0; i < g->num_cidades; i++) {
        NoRota* atual = g->adj[i];
        while (atual != NULL) {
            NoRota* temp = atual;
            atual = atual->prox;
            free(temp); // Libera cada nó da lista
        }
        g->adj[i] = NULL;
    }
}

// Encontra o ID de uma cidade pelo nome
int obterIdCidadePorNome(Grafo* g, const char* nome) {
    for (int i = 0; i < g->num_cidades; i++) {
        if (g->cidades[i].id != -1 && strcmp(g->cidades[i].nome, nome) == 0) {
            return g->cidades[i].id; // Retorna o ID se encontrar
        }
    }
    return -1; // Retorna -1 se não encontrar a cidade
}

// Funções de Gerenciamento do Grafo

// Adiciona uma nova cidade ao grafo
void adicionarCidade(Grafo* g, const char* nome) {
    if (g->num_cidades >= MAX_CIDADES) {
        printf("Limite de cidades atingido.\n");
        return;
    }
    if (obterIdCidadePorNome(g, nome) != -1) {
        printf("Cidade '%s' ja existe.\n", nome);
        return;
    }

    // Encontra o próximo ID disponível
    int novo_id = g->num_cidades;
    g->cidades[novo_id].id = novo_id;       // Atribui o ID
    strcpy(g->cidades[novo_id].nome, nome); // Copia o nome
    g->adj[novo_id] = NULL;                  // Inicializa a lista de adjacência vazia
    g->num_cidades++;                       // Incrementa o contador de cidades
    printf("Cidade '%s' adicionada com sucesso! (ID: %d)\n", nome, novo_id);
}

// Cria uma rota (conexão ponderada) entre duas cidades
// Assume que a rota é de mão dupla (grafo não direcionado)
void criarRota(Grafo* g, int id_origem, int id_destino, int custo) {
    // Verifica se os IDs são válidos e se o custo é positivo
    if (id_origem < 0 || id_origem >= g->num_cidades || g->cidades[id_origem].id == -1 ||
        id_destino < 0 || id_destino >= g->num_cidades || g->cidades[id_destino].id == -1) {
        printf("IDs de cidades invalidos para criar rota.\n");
        return;
    }
    if (id_origem == id_destino) {
        printf("Uma rota nao pode conectar a mesma cidade a si mesma.\n");
        return;
    }
    if (custo <= 0) {
        printf("O custo da rota deve ser positivo.\n");
        return;
    }

    // Adiciona a rota de origem para destino
    NoRota* novoNo1 = criarNoRota(id_destino, custo);
    novoNo1->prox = g->adj[id_origem]; // Coloca o novo nó no início da lista
    g->adj[id_origem] = novoNo1;

    // Adiciona a rota de destino para origem (se for de mão dupla)
    NoRota* novoNo2 = criarNoRota(id_origem, custo);
    novoNo2->prox = g->adj[id_destino];
    g->adj[id_destino] = novoNo2;

    printf("Rota entre '%s' e '%s' (Custo: %d) criada com sucesso!\n",
           g->cidades[id_origem].nome, g->cidades[id_destino].nome, custo);
}

// Exibe as rotas que partem de uma cidade específica
void visualizarRotas(Grafo* g, int id_cidade) {
    if (id_cidade < 0 || id_cidade >= g->num_cidades || g->cidades[id_cidade].id == -1) {
        printf("Cidade nao encontrada.\n");
        return;
    }

    printf("Rotas partindo de '%s':\n", g->cidades[id_cidade].nome);
    NoRota* atual = g->adj[id_cidade];
    if (atual == NULL) {
        printf("  Nenhuma rota cadastrada.\n");
        return;
    }
    while (atual != NULL) {
        printf("  - Para %s (ID: %d), Custo: %d\n", g->cidades[atual->id_destino].nome, atual->id_destino, atual->custo);
        atual = atual->prox;
    }
}

// Algoritmo de Dijkstra

// Implementação do algoritmo de Dijkstra para encontrar o menor caminho
void dijkstra(Grafo* g, int id_inicio) {
    if (id_inicio < 0 || id_inicio >= g->num_cidades || g->cidades[id_inicio].id == -1) {
        printf("Cidade de inicio nao encontrada para Dijkstra.\n");
        return;
    }

    int dist[MAX_CIDADES];   // Array para armazenar as menores distâncias do início
    int pai[MAX_CIDADES];    // Array para reconstruir o caminho (quem "chegou" em quem)
    bool visitado[MAX_CIDADES]; // Array para marcar cidades já processadas

    // Inicializa distâncias, pais e visitados
    for (int i = 0; i < g->num_cidades; i++) {
        dist[i] = INFINITO;   // Todas as distâncias começam como infinito
        pai[i] = -1;          // Nenhum pai definido ainda
        visitado[i] = false;  // Nenhuma cidade visitada
    }

    dist[id_inicio] = 0; // A distância da cidade inicial para ela mesma é 0

    // Loop principal de Dijkstra: processa todas as cidades
    for (int count = 0; count < g->num_cidades - 1; count++) {
        int u = -1; // Cidade com a menor distância atual ainda não visitada

        // Encontra a cidade 'u' com a menor distância que ainda não foi visitada
        for (int v = 0; v < g->num_cidades; v++) {
            if (!visitado[v] && (u == -1 || dist[v] < dist[u])) {
                u = v; // Atualiza 'u' se encontrar uma distância menor
            }
        }

        // Se 'u' for infinito, significa que as cidades restantes não são alcançáveis
        if (dist[u] == INFINITO) break;

        visitado[u] = true; // Marca 'u' como visitado

        // Percorre os vizinhos de 'u' para relaxar as arestas (atualizar distâncias)
        NoRota* atual = g->adj[u];
        while (atual != NULL) {
            int v = atual->id_destino;
            int custo_aresta = atual->custo;

            // Se 'v' não foi visitado e o novo caminho via 'u' é mais curto
            if (!visitado[v] && dist[u] + custo_aresta < dist[v]) {
                dist[v] = dist[u] + custo_aresta; // Atualiza a distância de 'v'
                pai[v] = u; // Define 'u' como pai de 'v' no menor caminho
            }
            atual = atual->prox;
        }
    }

    // Exibe os resultados
    printf("\n--- Menores Caminhos a partir de '%s' (Dijkstra) ---\n", g->cidades[id_inicio].nome);
    for (int i = 0; i < g->num_cidades; i++) {
        if (i == id_inicio) continue; // Pula a cidade de início

        printf("  Para '%s': ", g->cidades[i].nome);
        if (dist[i] == INFINITO) {
            printf("Inatingivel.\n");
        } else {
            printf("Custo total: %d. Caminho: ", dist[i]);
            // Reconstrói e exibe o caminho
            int caminho[MAX_CIDADES]; // Armazena o caminho invertido
            int k = 0;
            int atual_caminho = i;
            while (atual_caminho != -1) {
                caminho[k++] = atual_caminho;
                atual_caminho = pai[atual_caminho];
            }
            // Imprime o caminho na ordem correta
            for (int j = k - 1; j >= 0; j--) {
                printf("%s", g->cidades[caminho[j]].nome);
                if (j > 0) printf(" -> ");
            }
            printf("\n");
        }
    }
    printf("--------------------------------------------------\n");
}


// Função Principal (Main)

int main() {
    Grafo meuMapa;
    inicializarGrafo(&meuMapa); // Inicializa o mapa de cidades

    int opcao;
    char nome[NOME_CIDADE_MAX];
    char nome_origem[NOME_CIDADE_MAX], nome_destino[NOME_CIDADE_MAX];
    int id_cidade, custo_rota;
    int id_origem, id_destino;

    do {
        printf("\n--- Menu do Sistema de Rotas --- (Cidades cadastradas: %d)\n", meuMapa.num_cidades);
        printf("1. Adicionar Cidade\n");
        printf("2. Criar Rota\n");
        printf("3. Visualizar Rotas de uma Cidade\n");
        printf("4. Calcular Menor Caminho (Dijkstra)\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Consome o '\n' deixado pelo scanf

        switch (opcao) {
            case 1:
                printf("Digite o nome da nova cidade: ");
                fgets(nome, NOME_CIDADE_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0; // Remove o '\n' do final
                adicionarCidade(&meuMapa, nome);
                break;
            case 2:
                printf("Digite o nome da cidade de origem: ");
                fgets(nome_origem, NOME_CIDADE_MAX, stdin);
                nome_origem[strcspn(nome_origem, "\n")] = 0;
                printf("Digite o nome da cidade de destino: ");
                fgets(nome_destino, NOME_CIDADE_MAX, stdin);
                nome_destino[strcspn(nome_destino, "\n")] = 0;
                printf("Digite o custo da rota (ex: distancia em km): ");
                scanf("%d", &custo_rota);
                getchar(); // Consome o '\n'

                id_origem = obterIdCidadePorNome(&meuMapa, nome_origem);
                id_destino = obterIdCidadePorNome(&meuMapa, nome_destino);

                if (id_origem != -1 && id_destino != -1) {
                    criarRota(&meuMapa, id_origem, id_destino, custo_rota);
                } else {
                    printf("Uma ou ambas as cidades nao foram encontradas.\n");
                }
                break;
            case 3:
                printf("Digite o nome da cidade para visualizar as rotas: ");
                fgets(nome, NOME_CIDADE_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_cidade = obterIdCidadePorNome(&meuMapa, nome);
                visualizarRotas(&meuMapa, id_cidade);
                break;
            case 4:
                printf("Digite o nome da cidade de inicio para o calculo do menor caminho: ");
                fgets(nome, NOME_CIDADE_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_cidade = obterIdCidadePorNome(&meuMapa, nome);
                dijkstra(&meuMapa, id_cidade);
                break;
            case 0:
                printf("Saindo do sistema de rotas. Boa viagem!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    liberarGrafo(&meuMapa); // Libera a memória alocada antes de sair
    return 0;
}