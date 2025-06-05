#include <stdio.h>    // Para entrada e saída (printf, scanf)
#include <stdlib.h>   // Para alocação de memória (malloc, free)
#include <string.h>   // Para manipulação de strings (strcpy, strcmp)
#include <stdbool.h>  // Para usar tipos booleanos (true/false)

// Definições e Estruturas

#define MAX_USUARIOS 100 // Número máximo de usuários na rede
#define NOME_MAX 50      // Tamanho máximo do nome do usuário

// Estrutura para representar um usuário
typedef struct Usuario {
    int id;          // ID único do usuário (índice no array de usuários)
    char nome[NOME_MAX]; // Nome do usuário
} Usuario;

// Estrutura para um nó na lista de adjacência (representa uma conexão de amizade)
typedef struct NoAdj {
    int id_amigo;       // ID do usuário amigo
    struct NoAdj* prox; // Ponteiro para o próximo amigo na lista
} NoAdj;

// Estrutura principal do Grafo (nossa rede social)
typedef struct Grafo {
    NoAdj* adj[MAX_USUARIOS];   // Array de listas de adjacência (um para cada usuário)
    Usuario usuarios[MAX_USUARIOS]; // Array para armazenar os dados dos usuários
    int num_usuarios;           // Contador de usuários atualmente no grafo
} Grafo;

// Funções Auxiliares 

// Inicializa o grafo, definindo tudo como vazio
void inicializarGrafo(Grafo* g) {
    g->num_usuarios = 0; // Começa com nenhum usuário
    for (int i = 0; i < MAX_USUARIOS; i++) {
        g->adj[i] = NULL; // Todas as listas de adjacência iniciam vazias
        g->usuarios[i].id = -1; // Marca usuários como não existentes
    }
}

// Cria um novo nó para a lista de adjacência
NoAdj* criarNoAdj(int id_amigo) {
    NoAdj* novoNo = (NoAdj*)malloc(sizeof(NoAdj)); // Aloca memória para o novo nó
    if (novoNo == NULL) {
        printf("Erro de alocacao de memoria para NoAdj.\n");
        exit(EXIT_FAILURE); // Sai do programa se houver erro
    }
    novoNo->id_amigo = id_amigo; // Define o ID do amigo
    novoNo->prox = NULL;         // O próximo é nulo por enquanto
    return novoNo;
}

// Libera a memória de todas as listas de adjacência do grafo
void liberarGrafo(Grafo* g) {
    for (int i = 0; i < g->num_usuarios; i++) {
        NoAdj* atual = g->adj[i];
        while (atual != NULL) {
            NoAdj* temp = atual;
            atual = atual->prox;
            free(temp); // Libera cada nó da lista
        }
        g->adj[i] = NULL;
    }
}

// Encontra o ID de um usuário pelo nome
int obterIdUsuarioPorNome(Grafo* g, const char* nome) {
    for (int i = 0; i < g->num_usuarios; i++) {
        if (g->usuarios[i].id != -1 && strcmp(g->usuarios[i].nome, nome) == 0) {
            return g->usuarios[i].id; // Retorna o ID se encontrar
        }
    }
    return -1; // Retorna -1 se não encontrar o usuário
}

// Funções do Grafo (Nossa Rede Social) 

// Adiciona um novo usuário ao grafo
void adicionarUsuario(Grafo* g, const char* nome) {
    if (g->num_usuarios >= MAX_USUARIOS) {
        printf("Limite de usuarios atingido.\n");
        return;
    }
    if (obterIdUsuarioPorNome(g, nome) != -1) {
        printf("Usuario '%s' ja existe.\n", nome);
        return;
    }

    // Encontra o próximo ID disponível
    int novo_id = g->num_usuarios;
    g->usuarios[novo_id].id = novo_id;       // Atribui o ID
    strcpy(g->usuarios[novo_id].nome, nome); // Copia o nome
    g->adj[novo_id] = NULL;                  // Inicializa a lista de adjacência vazia
    g->num_usuarios++;                       // Incrementa o contador de usuários
    printf("Usuario '%s' adicionado com sucesso! (ID: %d)\n", nome, novo_id);
}

// Cria uma conexão (amizade) entre dois usuários
void criarConexao(Grafo* g, int id1, int id2) {
    // Verifica se os IDs são válidos
    if (id1 < 0 || id1 >= g->num_usuarios || g->usuarios[id1].id == -1 ||
        id2 < 0 || id2 >= g->num_usuarios || g->usuarios[id2].id == -1) {
        printf("IDs de usuario invalidos para criar conexao.\n");
        return;
    }
    if (id1 == id2) {
        printf("Um usuario nao pode ser amigo de si mesmo.\n");
        return;
    }

    // Adiciona id2 na lista de adjacência de id1
    NoAdj* novoNo1 = criarNoAdj(id2);
    novoNo1->prox = g->adj[id1]; // Coloca o novo nó no início da lista
    g->adj[id1] = novoNo1;

    // Adiciona id1 na lista de adjacência de id2 (amizade é mútua)
    NoAdj* novoNo2 = criarNoAdj(id1);
    novoNo2->prox = g->adj[id2];
    g->adj[id2] = novoNo2;

    printf("Conexao entre '%s' e '%s' criada com sucesso!\n", g->usuarios[id1].nome, g->usuarios[id2].nome);
}

// Exibe os amigos diretos de um usuário
void visualizarAmizades(Grafo* g, int id_usuario) {
    if (id_usuario < 0 || id_usuario >= g->num_usuarios || g->usuarios[id_usuario].id == -1) {
        printf("Usuario nao encontrado.\n");
        return;
    }

    printf("Amigos de '%s':\n", g->usuarios[id_usuario].nome);
    NoAdj* atual = g->adj[id_usuario];
    if (atual == NULL) {
        printf("  Nenhum amigo.\n");
        return;
    }
    while (atual != NULL) {
        printf("  - %s (ID: %d)\n", g->usuarios[atual->id_amigo].nome, atual->id_amigo);
        atual = atual->prox;
    }
}

// Algoritmos de Busca

// Implementação da Busca em Largura (BFS)
void bfs(Grafo* g, int inicio_id) {
    if (inicio_id < 0 || inicio_id >= g->num_usuarios || g->usuarios[inicio_id].id == -1) {
        printf("Usuario de inicio nao encontrado para BFS.\n");
        return;
    }

    // Array para controlar usuários visitados
    bool visitado[MAX_USUARIOS];
    for (int i = 0; i < g->num_usuarios; i++) {
        visitado[i] = false; // Ninguém foi visitado ainda
    }

    // Fila para o BFS (usando um array simples como fila circular ou com ponteiros)
    // Para simplificar, vamos usar um array e controlar indices (enqueue/dequeue)
    int fila[MAX_USUARIOS];
    int frente = 0; // Inicio da fila
    int tras = 0;   // Fim da fila

    // Adiciona o nó inicial na fila e marca como visitado
    fila[tras++] = inicio_id;
    visitado[inicio_id] = true;

    printf("\n--- Busca em Largura (BFS) a partir de '%s' ---\n", g->usuarios[inicio_id].nome);

    // Enquanto a fila não estiver vazia
    while (frente < tras) {
        int u_id = fila[frente++]; // Pega o primeiro da fila
        printf("Visitando: %s (ID: %d)\n", g->usuarios[u_id].nome, u_id);

        // Percorre os amigos do usuário atual
        NoAdj* atual = g->adj[u_id];
        while (atual != NULL) {
            int v_id = atual->id_amigo;
            // Se o amigo não foi visitado, marca como visitado e adiciona na fila
            if (!visitado[v_id]) {
                visitado[v_id] = true;
                fila[tras++] = v_id;
            }
            atual = atual->prox;
        }
    }
    printf("--- Fim do BFS ---\n");
}

// Função auxiliar recursiva para a Busca em Profundidade (DFS)
void dfs_recursivo(Grafo* g, int u_id, bool visitado[]) {
    visitado[u_id] = true; // Marca o usuário atual como visitado
    printf("Visitando: %s (ID: %d)\n", g->usuarios[u_id].nome, u_id);

    // Percorre os amigos do usuário atual
    NoAdj* atual = g->adj[u_id];
    while (atual != NULL) {
        int v_id = atual->id_amigo;
        // Se o amigo não foi visitado, chama o DFS para ele
        if (!visitado[v_id]) {
            dfs_recursivo(g, v_id, visitado);
        }
        atual = atual->prox;
    }
}

// Implementação da Busca em Profundidade (DFS)
void dfs(Grafo* g, int inicio_id) {
    if (inicio_id < 0 || inicio_id >= g->num_usuarios || g->usuarios[inicio_id].id == -1) {
        printf("Usuario de inicio nao encontrado para DFS.\n");
        return;
    }

    // Array para controlar usuários visitados
    bool visitado[MAX_USUARIOS];
    for (int i = 0; i < g->num_usuarios; i++) {
        visitado[i] = false; // Ninguém foi visitado ainda
    }

    printf("\n--- Busca em Profundidade (DFS) a partir de '%s' ---\n", g->usuarios[inicio_id].nome);
    dfs_recursivo(g, inicio_id, visitado); // Chama a função recursiva
    printf("--- Fim do DFS ---\n");
}

// --- Funcionalidades da Rede Social ---

// Sugere amigos baseando-se em amigos de amigos (conexões de segundo grau)
void sugerirAmigos(Grafo* g, int id_usuario) {
    if (id_usuario < 0 || id_usuario >= g->num_usuarios || g->usuarios[id_usuario].id == -1) {
        printf("Usuario nao encontrado para sugestao de amigos.\n");
        return;
    }

    // Array para marcar visitados no BFS e calcular distância
    int distancia[MAX_USUARIOS];
    bool visitado[MAX_USUARIOS];
    for (int i = 0; i < g->num_usuarios; i++) {
        distancia[i] = -1; // Distância desconhecida
        visitado[i] = false; // Não visitado
    }

    // Fila para o BFS
    int fila[MAX_USUARIOS];
    int frente = 0;
    int tras = 0;

    // Começa com o usuário principal
    fila[tras++] = id_usuario;
    visitado[id_usuario] = true;
    distancia[id_usuario] = 0; // Distância zero para ele mesmo

    // Executa um BFS para calcular distâncias
    while (frente < tras) {
        int u_id = fila[frente++];
        NoAdj* atual = g->adj[u_id];
        while (atual != NULL) {
            int v_id = atual->id_amigo;
            if (!visitado[v_id]) {
                visitado[v_id] = true;
                distancia[v_id] = distancia[u_id] + 1; // Distância do amigo
                fila[tras++] = v_id;
            }
            atual = atual->prox;
        }
    }

    printf("\n--- Sugestoes de Amigos para '%s' ---\n", g->usuarios[id_usuario].nome);
    bool encontrou_sugestao = false;

    // Percorre todos os usuários para encontrar sugestões
    for (int i = 0; i < g->num_usuarios; i++) {
        // Se o usuário está a distância 2 (amigo de amigo) e não é o próprio usuário
        if (distancia[i] == 2) {
            printf("  - %s (ID: %d)\n", g->usuarios[i].nome, i);
            encontrou_sugestao = true;
        }
    }

    if (!encontrou_sugestao) {
        printf("  Nenhuma sugestao de amigo encontrada (conexao de 2o grau).\n");
    }
    printf("-------------------------------------------\n");
}


// Função Principal (Main) 

int main() {
    Grafo minhaRede;
    inicializarGrafo(&minhaRede); // Inicializa a rede social

    int opcao;
    char nome[NOME_MAX];
    char nome1[NOME_MAX], nome2[NOME_MAX];
    int id_usuario;

    do {
        printf("\n--- Menu da Rede Social --- (Total de usuarios: %d)\n", minhaRede.num_usuarios);
        printf("1. Adicionar Usuario\n");
        printf("2. Criar Conexao (Amizade)\n");
        printf("3. Visualizar Amizades\n");
        printf("4. Buscar em Largura (BFS)\n");
        printf("5. Buscar em Profundidade (DFS)\n");
        printf("6. Sugerir Amigos\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // Consome o '\n' deixado pelo scanf

        switch (opcao) {
            case 1:
                printf("Digite o nome do novo usuario: ");
                fgets(nome, NOME_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0; // Remove o '\n' do final
                adicionarUsuario(&minhaRede, nome);
                break;
            case 2:
                printf("Digite o nome do primeiro usuario: ");
                fgets(nome1, NOME_MAX, stdin);
                nome1[strcspn(nome1, "\n")] = 0;
                printf("Digite o nome do segundo usuario: ");
                fgets(nome2, NOME_MAX, stdin);
                nome2[strcspn(nome2, "\n")] = 0;

                int id1 = obterIdUsuarioPorNome(&minhaRede, nome1);
                int id2 = obterIdUsuarioPorNome(&minhaRede, nome2);

                if (id1 != -1 && id2 != -1) {
                    criarConexao(&minhaRede, id1, id2);
                } else {
                    printf("Um ou ambos os usuarios nao foram encontrados.\n");
                }
                break;
            case 3:
                printf("Digite o nome do usuario para visualizar amizades: ");
                fgets(nome, NOME_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_usuario = obterIdUsuarioPorNome(&minhaRede, nome);
                visualizarAmizades(&minhaRede, id_usuario);
                break;
            case 4:
                printf("Digite o nome do usuario de inicio para BFS: ");
                fgets(nome, NOME_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_usuario = obterIdUsuarioPorNome(&minhaRede, nome);
                bfs(&minhaRede, id_usuario);
                break;
            case 5:
                printf("Digite o nome do usuario de inicio para DFS: ");
                fgets(nome, NOME_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_usuario = obterIdUsuarioPorNome(&minhaRede, nome);
                dfs(&minhaRede, id_usuario);
                break;
            case 6:
                printf("Digite o nome do usuario para sugestao de amigos: ");
                fgets(nome, NOME_MAX, stdin);
                nome[strcspn(nome, "\n")] = 0;
                id_usuario = obterIdUsuarioPorNome(&minhaRede, nome);
                sugerirAmigos(&minhaRede, id_usuario);
                break;
            case 0:
                printf("Saindo da rede social. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                break;
        }
    } while (opcao != 0);

    liberarGrafo(&minhaRede); // Libera a memória alocada antes de sair
    return 0;
}