#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    camisa,
    casaco,
    calca,
} TipoProduto;

#define NUM_LAVAGENS 3
#define NUM_TESTES 2

typedef struct {
    int id_unico;
    TipoProduto tipo;
    char nome[50];
    float preco;
    int removido;

    union {
        struct {
            int tamanho;
            int espessura;
            char estampa[15];
            float largura_cm;
            float altura_cm;
        } camisa_dados;
    
        struct {
            int impermeavel;
            char cor_veludo[15];
            char cor_dupla_face[15];
        } casaco_dados;
    
        struct {
            char material_jeans[15];
            char estilo_rasgada[15] ;
            int bolsos;
        } calca_dados;
    } dados_especificos;
} Registro;

long calcular_proximo_id(FILE *arquivo) {
    long tamanho_bytes;
    long num_registros;
    
    fseek(arquivo, 0, SEEK_END);
    tamanho_bytes = ftell(arquivo);
    
    if (tamanho_bytes == 0) {
        return 1;
    }
    
    num_registros = tamanho_bytes / sizeof(Registro);
    return num_registros + 1;
}

void adicionar_registro() {
    FILE *arquivo;
    Registro novo_registro;
    int tipo_escolhido;

    arquivo = fopen("dados.bin", "r+b");

    if (arquivo == NULL) {
        arquivo = fopen("dados.bin", "wb");
        if (arquivo == NULL) {
            return;
        }
    }
    
    novo_registro.id_unico = (int)calcular_proximo_id(arquivo);
    novo_registro.removido = 0;

    printf("\n--- Novo Registro (ID #%d) ---\n", novo_registro.id_unico);

    printf("Nome do produto: ");
    scanf("%49s", novo_registro.nome);
    printf("Preco (R$): ");
    scanf("%f", &novo_registro.preco);

    printf("Tipo (0:Camisa, 1:Casaco, 2:Calca): ");
    scanf("%d", &tipo_escolhido);
    
    if (tipo_escolhido < 0 || tipo_escolhido > 2) {
        fclose(arquivo);
        return;
    }
    novo_registro.tipo = (TipoProduto)tipo_escolhido;

    switch (novo_registro.tipo) {
        case camisa:
            printf("Tamanho (int): ");
            scanf("%d", &novo_registro.dados_especificos.camisa_dados.tamanho);
            printf("Espessura (int): ");
            scanf("%d", &novo_registro.dados_especificos.camisa_dados.espessura);
            printf("Estampa (max 14): ");
            scanf("%14s", novo_registro.dados_especificos.camisa_dados.estampa);
            printf("Largura (cm): ");
            scanf("%f", &novo_registro.dados_especificos.camisa_dados.largura_cm);
            printf("Altura (cm): ");
            scanf("%f", &novo_registro.dados_especificos.camisa_dados.altura_cm);
            break;

        case casaco:
            printf("E impermeavel? (1 para Sim, 0 para Nao): ");
            scanf("%d", &novo_registro.dados_especificos.casaco_dados.impermeavel);
            printf("Cor do Veludo: ");
            scanf("%14s", novo_registro.dados_especificos.casaco_dados.cor_veludo);
            printf("Cor Dupla Face: ");
            scanf("%14s", novo_registro.dados_especificos.casaco_dados.cor_dupla_face);
            break;

        case calca:
            printf("Material Jeans: ");
            scanf("%14s", novo_registro.dados_especificos.calca_dados.material_jeans);
            printf("Numero de Bolsos: ");
            scanf("%d", &novo_registro.dados_especificos.calca_dados.bolsos);
            break;
    }

    fseek(arquivo, 0, SEEK_END);
    
    if (fwrite(&novo_registro, sizeof(Registro), 1, arquivo) == 1) {
        printf("\n=> PRODUTO ID #%d ADICIONADO COM SUCESSO.\n", novo_registro.id_unico);
    } else {
        printf("\n=> ERRO: Falha ao escrever o registro no arquivo.\n");
    }

    fclose(arquivo);
}

void exibir_registro(Registro r) {
    const char *impermeavel_str = (r.dados_especificos.casaco_dados.impermeavel == 1) ? "Sim" : "Nao";
    
    printf("ID: %d | Nome: %s | Preco: %.2f | Tipo: ", r.id_unico, r.nome, r.preco);
    
    switch (r.tipo) {
        case camisa:
            printf("Camisa (Tam: %d, Estampa: %s, L/A: %.2f/%.2f)\n", 
                   r.dados_especificos.camisa_dados.tamanho, r.dados_especificos.camisa_dados.estampa, 
                   r.dados_especificos.camisa_dados.largura_cm, r.dados_especificos.camisa_dados.altura_cm);
            break;
        case casaco:
            printf("Casaco (Imperm: %s, Veludo: %s)\n", impermeavel_str, r.dados_especificos.casaco_dados.cor_veludo);
            break;
        case calca:
            printf("Calca (Bolsos: %d, Material: %s)\n", r.dados_especificos.calca_dados.bolsos, r.dados_especificos.calca_dados.material_jeans);
            break;
    }
}

void listar_todos_registros() {
    FILE *arquivo = fopen("dados.bin", "rb");
    Registro registro_lido;
    long num_registros;
    
    if (arquivo == NULL) {
        printf("\nArquivo de dados nao encontrado ou vazio.\n");
        return;
    }

    fseek(arquivo, 0, SEEK_END);
    num_registros = ftell(arquivo) / sizeof(Registro);
    fseek(arquivo, 0, SEEK_SET);

    printf("\n--- LISTA DE REGISTROS ATIVOS (%ld total) ---\n", num_registros);

    for (long i = 0; i < num_registros; i++) {
        if (fread(&registro_lido, sizeof(Registro), 1, arquivo) == 1) {
            if (registro_lido.removido == 0) {
                exibir_registro(registro_lido);
            }
        }
    }

    fclose(arquivo);
}

void limpar_arquivo_dados() {
    char confirmacao[5];
    printf("\nATENCAO! Esta operacao APAGARA TODOS os dados. Deseja continuar (Sim/Nao)? ");
    scanf("%4s", confirmacao);
    
    for(int i = 0; confirmacao[i]; i++){
      confirmacao[i] = tolower(confirmacao[i]);
    }
    
    if (strcmp(confirmacao, "sim") == 0) {
        FILE *arquivo = fopen("dados.bin", "wb");
        if (arquivo == NULL) {
            printf("\nERRO ao tentar limpar o arquivo.\n");
            return;
        }
        fclose(arquivo);
        printf("\n*** BASE DE DADOS LIMPA COM SUCESSO. O proximo ID comecara em 1. ***\n");
    } else {
        printf("\nOperacao de limpeza cancelada.\n");
    }
}

int main() {
    int opcao;

    do {
        printf("\n=== SISTEMA DE ESTOQUE ===\n");
        printf("1. Adicionar Novo Produto\n");
        printf("2. Listar Todos os Produtos\n");
        printf("3. Limpar Base de Dados (Zerar Estoque)\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionar_registro();
                break;
            case 2:
                listar_todos_registros();
                break;
            case 3:
                limpar_arquivo_dados();
                break;
            case 0:
                printf("Saindo do sistema.\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}