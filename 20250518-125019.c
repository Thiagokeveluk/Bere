#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Produto
{
    int Codigo;
    char nome[100];
    char categoria[30];
    float preco;
    int estoque;
    int estoque_min;
    struct Produto *proximo;
} Produto;

typedef struct itemCarrinho
{
    Produto *produto;
    int quantidade;
    struct itemCarrinho *proximo;
} itemCarrinho;

typedef struct
{
    itemCarrinho *inicio;
    float total;
} Carrinho;

typedef struct Cliente
{
    int codigo;
    char nome_completo[51];
    char nome_social[51];
    char cpf[15];
    char email[51];
    char rua_numero[100];
    char bairro[51];
    char celular[21];
    Carrinho *carrinho;
    struct Cliente *proximo; // ponteiro para o próximo cliente
} Cliente;

// Prototipação das funções
int abrir_menu_principal();
void exibir_opcoes_menu_principal();
void exibir_clientes_cadastrados(Cliente *cliente);
void cadastros();
void cadastrar_novo_cliente();
void cadastrar_novo_produto();
void atualiza_estoque_abertura();
void fechar_caixa();
float calcula_valor_produtos();
int exibir_opcoes();
float abrir_menu_categoria();
void efetuar_pagamentos();
void efetuar_pagamento_dinheiro(Cliente *cliente);
void efetuar_pagamento_cartao(Cliente *cliente);
void exibir_venda_produto_categoria();
void listar_cliente();
void liberar_lista_clientes();
void iniciar_venda();
void exibir_produtos();
void menu_venda();
void Nova_Venda();
void exibir_carrinho(Carrinho *carrinho);
void adicionarAoCarrinho(Carrinho *carrinho, Produto *produto, int quantidade);
void abertura_caixa();
void gerar_relatorio_caixa();


int clientes_atendidos=0;
int caixa_aberto = 0; // 0= FECHADO, 1=ABERTO.
float abrir_caixa = 0.0;
float saldo_inicial=0.0;
float saldo_caixa=0.0;
float saldo_inicial_caixa=0.0;
float faturamento_total=0.0;
Produto *buscar_produto_por_codigo(int Codigo);
Cliente *inicio_cliente = NULL;
Produto *inicio_produto = NULL;
Cliente *selecionar_cliente();

void abertura_caixa()
{
    float valor_inicial;
    int resultado;
    
    do
    {
        printf("____________________________________________________________________");
        printf("\n|Digite valor inicial para abrir o caixa. Valor mínimo (R$300,00)\n|-->");
        resultado = scanf("%f", &valor_inicial);
        printf("--------------------------------------------------------------------\n\n");

        if(resultado != 1){
            printf("Entrada inválida, insira apenas números!\n");
            while(getchar() != '\n') // limpa o buffet
            continue;
            }
        
        if (valor_inicial < 0)
        {
            printf("\n=================================================");
            printf("\n|Valor não pode ser Negativo!!! Tente novamente!\n\n");
        }
        else if (valor_inicial < 300)
        {
            printf("\nValor insuficiente! É necessário um valor no mínimo R$300,00 para abrir o caixa\n");
        }
    } while (valor_inicial < 300);
    
    saldo_inicial_caixa=valor_inicial;
    saldo_caixa = valor_inicial;
    caixa_aberto = 1;
    printf("Caixa aberto com sucesso com R$%.2f\n", saldo_caixa);
}
int abrir_menu_principal()
{
    int opcao = 0;
    do
    {
        exibir_opcoes_menu_principal();
        printf("\nDigite a opção desejada de [1] a [5]\n-->");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            printf("\n\n\t=================================================");
            printf("\n\t    --> Opção de |CADASTRO| selecionado! <--");
            printf("\n\t=================================================\n\n");
            cadastros();
            break;

        case 2:
            // NOVA VENDA
            printf("\n\n\t=================================================");
            printf("\n\t      --> Opção |VENDAS| selecionado! <--");
            printf("\n\t=================================================\n\n");
            menu_venda();
            break;
            
        case 3:
            printf("\n\n\t=================================================");
            printf("\n\t   --> Opcão |FECHAR CAIXA| Selecionada <--");
            printf("\n\t=================================================\n\n"); 
            fechar_caixa();
            break;
            
        case 4:
            printf("\n\n\t=================================================");
            printf("\n\t    --> Opção de |RELATÓRIOS| Disponível! <--");
            printf("\n\t=================================================\n\n");
            break;

        case 5:
           printf("\n\n\n\t============================================");
            printf("\n\t[✓][✓] Saindo do Sistema... Até mais! [✓][✓]");
           printf("\n\t============================================\n\n");
            break;

        default:
            printf("\n\n===============================================");
            printf("\n\t[×][×] Opção Inválida! Tente novamente. [×][×]\n\n\n");
            printf("\n\n===============================================");
            break;
        }
    } while (opcao != 5); // Corrigi para sair na opção 7 (não era 4!)

    return 0;
}

void exibir_opcoes_menu_principal()
{
    printf("          \t\t  _____________ \n");
    printf("     \t\t   _________|LOJA|_________ \n");
    printf("  \t\t _____________| DA |______________\n");
    printf(" \t\t_______________|BERE|__________________\n");
    printf("\t\t_______________________________________\n");
    printf("\t\t| [1] -> Cadastro                     |\n");
    printf("\t\t| [2] -> Vendas                       |\n");
    printf("\t\t| [3] -> Fechar Caixa                 |\n");
    printf("\t\t| [4] -> Relatório                    |\n");
    printf("\t\t| [5] -> Sair                         |\n");
    printf("\t\t_______________________________________\n\n");

    printf("\t\t             ___________________\n");
    printf("\t\t _____________SEJA BEM VINDO(A)________\n");
    printf("\t\t             __________________\n\n");
}

void exibir_clientes_cadastrados(Cliente *cliente)
{
    Cliente *cursor = cliente;

    if (cursor != NULL)
    {
        printf("|%-5s|%-25s|%-15s|%-12s|%-20s|%-25s |%-15s|%-12s|\n", "Cod", "Nome", "Nome Social", "CPF", "Email", "Rua N°", "Bairro", "Celular");
        while (cursor != NULL)
        {
            printf("|%-5d|%-25s|%-15s|%-12s|%-20s|%-25s|%-15s|%-12s|\n", cursor->codigo, cursor->nome_completo, cursor->nome_social, cursor->cpf, cursor->email, cursor->rua_numero, cursor->bairro, cursor->celular);
            cursor = cursor->proximo;
        }
    }
    else
    {
        printf("\033[1mNenhum Cliente Cadastrado\033[0m\n");
    }
}

void cadastros()
{
    int opcao_cadastro = 0;
    char escolha;

    do
    {
        printf("\n\t\t______________MENU DE CADASTROS___________\n");
        printf("\t\t| [1] -> CADASTRO CLIENTES                |\n");
        printf("\t\t| [2] -> CADASTRO PRODUTOS                |\n");
        printf("\t\t| [3] -> VOLTAR PARA MENU                 |\n");
        printf("\t\t|_________________________________________|\n");

        printf("Digite uma opção\n-->");
        scanf("%d", &opcao_cadastro);

        switch (opcao_cadastro)
        {
        case 1:
            exibir_clientes_cadastrados(inicio_cliente);
            printf("Deseja Cadastrar Novo Cliente? S/n\n");
            getchar();
            scanf("%c", &escolha);
            if (escolha == 'S' || escolha == 's')
            {
                printf("\n\nCadastrando um novo Cliente.... process..\n\n");
                cadastrar_novo_cliente();
            }
            break;

        case 2:
            printf("\nCadastrando novo Produto....\n");
            cadastrar_novo_produto();
            break;

        case 3:
            printf("\n\tVoltando para menu principal...\n\n\n");
            break;

        default:
            printf("\n Opção inválida! Tente novamente\n");
            break;
        }
    } while (opcao_cadastro != 3);
}

// ============================C L I E N T E==============================//
void cadastrar_novo_cliente()
{
    Cliente *novo_cliente = (Cliente *)malloc(sizeof(Cliente)); // Aloca memória pra novo cliente
    static int contador_codigo = 1;

    Cliente *cursor = inicio_cliente;

    if (cursor != NULL)
    {
        while (cursor->proximo != NULL)
        {
            cursor = cursor->proximo;
        }
    }

    if (novo_cliente == NULL)
    {
        printf("Erro ao alocar memória para novo cliente!\n");
        return;
    }

    novo_cliente->codigo = contador_codigo++;
    novo_cliente->carrinho = NULL;

    printf("\n__________CADASTRO DE NOVO CLIENTE______________\n");
    printf("|Digite nome completo do cliente:\n|-->");
    getchar(); // Limpa buffer
    fgets(novo_cliente->nome_completo, sizeof(novo_cliente->nome_completo), stdin);
    novo_cliente->nome_completo[strcspn(novo_cliente->nome_completo, "\n")] = '\0'; // Remove \n

    printf("|Digite nome social:\n|-->");
    fgets(novo_cliente->nome_social, sizeof(novo_cliente->nome_social), stdin);
    novo_cliente->nome_social[strcspn(novo_cliente->nome_social, "\n")] = '\0';

    printf("|Digite CPF do cliente:\n|-->");
    fgets(novo_cliente->cpf, sizeof(novo_cliente->cpf), stdin);
    novo_cliente->cpf[strcspn(novo_cliente->cpf, "\n")] = '\0';

    printf("|Digite E-mail do cliente:\n|-->");
    fgets(novo_cliente->email, sizeof(novo_cliente->email), stdin);
    novo_cliente->email[strcspn(novo_cliente->email, "\n")] = '\0';

    printf("| Digite Nome da rua e o N°:\n|-->");
    fgets(novo_cliente->rua_numero, sizeof(novo_cliente->rua_numero), stdin);
    novo_cliente->rua_numero[strcspn(novo_cliente->rua_numero, "\n")] = '\0';

    printf("| Digite Nome do Bairro:\n|-->");
    fgets(novo_cliente->bairro, sizeof(novo_cliente->bairro), stdin);
    novo_cliente->bairro[strcspn(novo_cliente->bairro, "\n")] = '\0';

    printf("| Digite número de Celular :\n|-->");
    fgets(novo_cliente->celular, sizeof(novo_cliente->celular), stdin);
    novo_cliente->celular[strcspn(novo_cliente->celular, "\n")] = '\0';

    novo_cliente->proximo = NULL;

    printf("|_____________________________________________\n\n");

    if (inicio_cliente == NULL)
    {
        inicio_cliente = novo_cliente;
    }
    else
    {
        cursor->proximo = novo_cliente;
    }

    // novo_cliente->proximo = inicio_cliente; // Insere na cabeça da lista
    // inicio_cliente = novo_cliente;

    printf("\t\t_______________________________");
    printf("\n\t\t|Cliente cadastrado com Sucesso!\n\t\t|");
    printf("\n\t\t| Nome Completo: %s\n", novo_cliente->nome_completo);
    printf("\t\t| Nome Social: %s\n", novo_cliente->nome_social);
    printf("\t\t| CPF: %s\n", novo_cliente->cpf);
    printf("\t\t| E-mail: %s\n", novo_cliente->email);
    printf("\t\t| Nome da Rua e N°: %s\n", novo_cliente->rua_numero);
    printf("\t\t| Bairro: %s\n", novo_cliente->bairro);
    printf("\t\t| Celular: %s\n", novo_cliente->celular);
    printf("\t\t|________________________________________\n\n");
}

Cliente *selecionar_cliente()
{
    int codigo;
    Cliente *atual = inicio_cliente;

    printf("\n Digite o Código do cliente:\n-->");
    scanf("%d", &codigo);

    while (atual != NULL)
    {
        if (atual->codigo == codigo)
        {
            printf("\n---------Cliente encontrado--------");
            printf("\n___________________________");
            printf("\n|Nome:%s", atual->nome_completo);
            printf("\n|CPF:%s", atual->cpf);
            printf("\n________________________________\n\n");
            return atual;
        }
        atual = atual->proximo;
    }

    printf("Cliente com codigo %d não encontrado\n", codigo);

    return NULL;
}

// void listar_cliente()
// {
//     Cliente *atual = inicio_cliente;

//     if (atual == NULL)
//     {
//         printf("Nenhum cliente cadastrado!\n");
//         return;
//     }

//     printf("\nLista de Clientes:\n");
//     while (atual != NULL)
//     {
//         printf("Nome: %s\n", atual->nome_completo);
//         printf(" Nome: %s\n", atual->nome_social);
//         printf("CPF: %s\n", atual->cpf);
//         printf("E-mail: %s\n", atual->email);
//         printf("Rua e Número: %s\n", atual->rua_numero);
//         printf("Bairro:%s\n", atual->bairro);
//         printf("Celular:%s\n", atual->celular);
//         atual = atual->proximo; // Move para o próximo!
//     }
// }

void liberar_lista_clientes()
{
    Cliente *atual = inicio_cliente;
    Cliente *temp;

    while (atual != NULL)
    {
        temp = atual;
        atual = atual->proximo;
        free(temp); // Libera a memória do cliente
    }
}

//=====================F I M - C L I E N T E ===========================//

//=========================P R O D U T O=============================//
void cadastrar_novo_produto()
{
    int selecao;
    Produto *novo = (Produto *)malloc(sizeof(Produto));

    if (novo == NULL)
    {
        printf("Erro ao alocar memória\n");
        return;
    }
    static int contador_codigo_produto = 1;
    novo->Codigo = contador_codigo_produto++; // <- Incrementa automaticamente

    printf("\n___________CADASTRO NOVO PRODUTO_________\n");

    getchar(); // Limpa buffer do scanf anterior
    printf("| Digite o nome do produto:\n|-->");
    fgets(novo->nome, sizeof(novo->nome), stdin);
    novo->nome[strcspn(novo->nome, "\n")] = '\0'; // Remove \n

    do{
        printf("| Selecione a Categoria:\n|  ->1. Alimentos\n|  ->2. Limpeza\n|  ->3. Padaria\n|-->");
        scanf("%d", &selecao);

        switch (selecao)
        {
        case 1:
            strcpy(novo->categoria, "Alimentos");
            break;
        case 2:
            strcpy(novo->categoria, "Limpeza");
            break;
        case 3:
            strcpy(novo->categoria, "Padaria");
            break;
        default:
            printf("->Digite uma opção válida<-\n");
            break;
        }
    }while(selecao < 1 || selecao > 3);

    do
    {
        printf("| Digite o preço do produto:\n|-->");
        scanf("%f", &novo->preco);
        if (novo->preco < 0)
        {
            printf("\n->Preço inválido, Digite um número positivo!!!<-\n");
        }
    } while (novo->preco < 0);

    do
    {
        printf("| Digite a quantidade em estoque:\n|-->");
        scanf("%d", &novo->estoque);
        if (novo->estoque < 0)
        {
            printf("\n->Estoque Insuficiente, Digite um Número válido!<-\n\n");
        }
    } while (novo->estoque < 0);

    do
    {
        printf("| Digite o estoque Minimo:\n|-->");
        scanf("%d", &novo->estoque_min);
        if (novo->estoque_min < 0)
        {
            printf("\n->Estoque Insuficiente, Digite um Número válido!<-\n\n");
        }
    } while (novo->estoque_min < 0);

    printf("|_________________________________________\n");
    novo->proximo = inicio_produto;
    inicio_produto = novo;

    printf("\t\t________________________________");
    printf("\n\t\t|Produto cadastrado com Sucesso!|\n\t\t|\t\t\t\t|\n");

    printf("\t\t| Produto: %s\t\t\t|\n", novo->nome);
    printf("\t\t| Preço R$%.2f\t\t\t|\n", novo->preco);
    printf("\t\t| Estoque: %d unidades\t\t|\n", novo->estoque);
    printf("\t\t|_______________________________|\n\n");
}

//====================== F I M - P R O D U T O ======================//

//========================= C A R R I N H O ========================//

Carrinho *criar_carrinho()
{
    Carrinho *carrinho = (Carrinho *)malloc(sizeof(Carrinho));
    carrinho->inicio = NULL;
    carrinho->total = 0.0;
    return carrinho;
}

void adicionarAoCarrinho(Carrinho *carrinho, Produto *produto, int quantidade)
{
    if (produto == NULL || quantidade <= 0)
        return;

    if (produto->estoque < quantidade)
    {
        printf("Estoque Insuficiente para o produto: %s\n ", produto->nome);
        return;
    }

    itemCarrinho *novo = (itemCarrinho *)malloc(sizeof(itemCarrinho));
    novo->produto = produto;
    novo->quantidade = quantidade;
    novo->proximo = carrinho->inicio;
    carrinho->inicio = novo;

    carrinho->total += produto->preco * quantidade;

    printf("%d x %s Adicionados ao carrinho\n", quantidade, produto->nome);
}

Produto *buscar_produto(char *nome)
{
    Produto *p = inicio_produto;
    while (p != NULL)
    {
        if (strcmp(p->nome, nome) == 0)
        {
            return p;
        }
        p = p->proximo;
    }
    return NULL;
}

void exibir_carrinho(Carrinho *carrinho)
{
    if (carrinho == NULL || carrinho->inicio == NULL)
    {
        printf("\n[CARRINHO VAZIO]\n");
        return;
    }

    printf("\n\n============= CARRINHO DE COMPRAS =============\n");
    itemCarrinho *atual = carrinho->inicio;
    int item = 1;
    while (atual != NULL)
    {
        float subtotal = atual->quantidade * atual->produto->preco;
        printf("[%d] %dx %s | R$ %.2f cada | Subtotal: R$ %.2f\n",
               item, atual->quantidade, atual->produto->nome,
               atual->produto->preco, subtotal);
        atual = atual->proximo;
        item++;
    }
    printf("-----------------------------------------------\n");
    printf("TOTAL: R$ %.2f\n", carrinho->total);
    printf("===============================================\n\n");
}

//================================================

void menu_venda()
{
    int opcaoVenda;

    do
    {
        printf("\n_________MENU DE VENDA_________\n");
        printf("|1-Nova venda\n");
        printf("|2-Retirada de caixa (Sangria)\n");
        printf("|3-Pagamento\n");
        printf("|4 Voltar para menu Principal\n");
        printf("|Escolha uma Opção\n|-->");
        scanf("%d", &opcaoVenda);

        switch (opcaoVenda)
        {
        case 1:
            Nova_Venda();
            break;
        case 2:
            //  retirada_Caixa();
            break;
        case 3:
            efetuar_pagamentos();
            break;
        case 4:
            printf("Retornando ao Menu Principal\n");
            break;
        default:
            printf("Opção Invalida!!!");
            break;
        }
    } while (opcaoVenda != 4);
}

void exibir_produtos()
{
    Produto *cursor = inicio_produto;

    printf("\n _________________PRODUTOS DISPONIVEIS___________________________________");
    printf("\n|%-5s%-25s  %-20s%-9s% -8s\n ", "COD", "Descrição", "Categoria", "Preço", "Estoque");
    printf("________________________________________________________________________\n");

    while(cursor != NULL){
        printf("|%-5d%-25s%-20sRS%-7.2f%-8d\n", cursor->Codigo, cursor->nome, cursor->categoria, cursor->preco, cursor->estoque);

        cursor = cursor->proximo;
    }
}

void Nova_Venda()
{
    Cliente *cliente = selecionar_cliente();
    if (cliente == NULL)
        return;

    if (cliente->carrinho == NULL)
        cliente->carrinho = criar_carrinho();

    int Codigo, quantidade;
    char continuar;

    printf("\n__________Nova venda_________\n");

    do
    {
        exibir_produtos();

        printf("________________________________________________\n");
        printf("\n|Informe o Codigo do produto desejado\n|-->");
        scanf("%d", &Codigo);

        Produto *prod = buscar_produto_por_codigo(Codigo);
        if (!prod)
        {
            printf("\nCodigo Inválido! Tente Novamente.\n");
            continue;
        }
        printf("\n|Informe a quantidade\n|-->");
        scanf("%d", &quantidade);

        adicionarAoCarrinho(cliente->carrinho, prod, quantidade);

        printf("\n________________________________________\n");
        printf("|[✓] Produto adicionado com sucesso!!!\n");
        printf("|Deseja adionar outro item? (S/s) para (sim) (N/n) para  não:");
        printf("\n________________________________________\n");
        getchar(); // para limpar o buffer e o usuario poder digitar sem problemas
        scanf("%c", &continuar);

    } while (continuar == 'S' || continuar == 's');

    // Exibe o resumo final do carrinho
    exibir_carrinho(cliente->carrinho);
}

Produto *buscar_produto_por_codigo(int Codigo)
{
    Produto *p = inicio_produto;
    while (p)
    {
        if (p->Codigo == Codigo)
            return p;
        p = p->proximo;
    }
    return NULL;
}

void iniciar_venda()
{
    Cliente *cliente = selecionar_cliente();
    if (cliente == NULL)
        return;

    Carrinho *carrinho = criar_carrinho();
    char nome_produto[100];
    int quantidade;

    while (1)
    {
        printf("\nDigite o nome do produto (ou 'fim' para finalizar): ");
        getchar(); // limpar buffer
        fgets(nome_produto, sizeof(nome_produto), stdin);
        nome_produto[strcspn(nome_produto, "\n")] = '\0';

        if (strcmp(nome_produto, "fim") == 0)
            break;

        Produto *produto = buscar_produto(nome_produto);
        if (produto == NULL)
        {
            printf("Produto não encontrado!\n");
            continue;
        }

        printf("Digite a quantidade: ");
        scanf("%d", &quantidade);

        adicionarAoCarrinho(carrinho, produto, quantidade);
    }

    printf("\nResumo da compra:\n");
    itemCarrinho *item = carrinho->inicio;
    while (item != NULL)
    {
        printf("- %d x %s = R$ %.2f\n", item->quantidade, item->produto->nome,
               item->quantidade * item->produto->preco);
        item = item->proximo;
    }

    printf("Total a pagar: R$ %.2f\n", carrinho->total);

    // Simples pagamento fictício
    float valor_pago;
    do
    {
        printf("Digite o valor pago: ");
        scanf("%f", &valor_pago);
        if (valor_pago < carrinho->total)
        {
            printf("Valor insuficiente!\n");
        }
    } while (valor_pago < carrinho->total);

    printf("Troco: R$ %.2f\n", valor_pago - carrinho->total);

    // Atualiza estoque
    item = carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Venda finalizada com sucesso!\n");

    // Libera carrinho
    while (carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = carrinho->inicio;
        carrinho->inicio = carrinho->inicio->proximo;
        free(tmp);
    }
    free(carrinho);
}

/*void fechar_caixa(){
    char confirmacao;
    
    if(! caixa_aberto){
        printf("O caixa já está Fechado!!!\n");
        return ;
    }
        
    printf("\t\t=========FECHAMENTO DE CAIXA=======\n");
    printf("\t\t|Saldo Inicial: %.2f\n", saldo_inicial_caixa);
    printf("\t\t|Saldo Final: %.2f\n", saldo_caixa);
    printf("\t\t|Deseja realmente fechar o caixa (S/s) ou (N/n):");
    printf("\n\t\t=================================\n-->");
    getchar();
    scanf("%c", &confirmacao);
    if( confirmacao == 'S' || confirmacao == 's'){
        caixa_aberto=0;
        saldo_inicial=0.0;
        saldo_inicial_caixa=0.0;
        printf("\n\t\t==================================");
        printf("\n\t\t| Caixa fechado com sucesso!! |\n");
        printf("\t\t==========================================");
        
        //encerra o sistema se quiser travar tudo
        exit(0);
    }else{
        printf("\n\n\t\t=============================================");
        printf("\n\t\t|Operacão cancelada.O caixa continua aberto |\n ");  
        printf("\t\t===============================================");
    }
}     */
//================================ P A G A M E N T O ========================================
void efetuar_pagamentos()
{
    Cliente *cliente = selecionar_cliente(); // Seleciona cliente
    if (cliente == NULL || cliente->carrinho == NULL)
    {
        printf("Cliente sem carrinho ou não encontrado.\n");
        return;
    }

    int opcao_pagamento;
    do
    {
        printf("\n======= MENU DE PAGAMENTO =======\n");
        printf("| [1] -> Visualizar Carrinho     |\n");
        printf("| [2] -> Pagar com Dinheiro      |\n");
        printf("| [3] -> Pagar com Cartão        |\n");
        printf("| [4] -> Cancelar                |\n");
        printf("=================================\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao_pagamento);

        switch (opcao_pagamento)
        {
        case 1:
            exibir_carrinho(cliente->carrinho);
            break;

        case 2:
            efetuar_pagamento_dinheiro(cliente);
            return; // Após pagamento, volta ao menu anterior

        case 3:
            efetuar_pagamento_cartao(cliente);
            return;

        case 4:
            printf("Cancelando pagamento.\n");
            break;

        default:
            printf("Opção inválida!\n");
        }
    } while (opcao_pagamento != 4);
}

/*void efetuar_pagamento_dinheiro(Cliente *cliente)
{
    float valor_pago = 0.0;
    float total = cliente->carrinho->total;

    printf("\n--- PAGAMENTO EM DINHEIRO ---\n");
    printf("Total a pagar: R$ %.2f\n", total);

    do
    {
        printf("Digite o valor pago: R$ ");
        scanf("%f", &valor_pago);
        if (valor_pago < total)
        {
            printf("Valor insuficiente! Falta R$ %.2f\n", total - valor_pago);
        }
    } while (valor_pago < total);

    printf("Troco: R$ %.2f\n", valor_pago - total);

    // Atualiza estoque
    itemCarrinho *item = cliente->carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Pagamento realizado com sucesso!\n");

    // Libera carrinho
    while (cliente->carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = cliente->carrinho->inicio;
        cliente->carrinho->inicio = tmp->proximo;
        free(tmp);
    }

    free(cliente->carrinho);
    cliente->carrinho = NULL;
}

void efetuar_pagamento_cartao(Cliente *cliente)
{
    char numero_cartao[20];
    int parcelas;
    float total = cliente->carrinho->total;

    printf("\n--- PAGAMENTO COM CARTÃO ---\n");
    printf("Total a pagar: R$ %.2f\n", total);

    printf("Digite o número do cartão (fictício): ");
    getchar(); // limpar buffer
    fgets(numero_cartao, sizeof(numero_cartao), stdin);
    numero_cartao[strcspn(numero_cartao, "\n")] = '\0';

    do
    {
        printf("Digite o número de parcelas (1 a 12): ");
        scanf("%d", &parcelas);
        if (parcelas < 1 || parcelas > 12)
        {
            printf("Número de parcelas inválido.\n");
        }
    } while (parcelas < 1 || parcelas > 12);

    printf("Pagamento de R$ %.2f em %d x de R$ %.2f\n",
           total, parcelas, total / parcelas);

    // Atualiza estoque
    itemCarrinho *item = cliente->carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Pagamento realizado com sucesso!\n");

    // Libera carrinho
    while (cliente->carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = cliente->carrinho->inicio;
        cliente->carrinho->inicio = tmp->proximo;
        free(tmp);
    }

    free(cliente->carrinho);
    cliente->carrinho = NULL;
}*/


// Adicione essas variáveis globais no início do seu arquivo:
int quantidade_vendas = 0;
float valor_pago_dinheiro = 0.0;
float valor_pago_cartao = 0.0;
float valor_pago_dinheiro_cartao = 0.0;
float valor_ajuste = 0.0;

// Para registrar o faturamento (já existe):
// float faturamento_total = 0.0;

// Altere as funções de pagamento para contabilizar os valores:

void efetuar_pagamento_dinheiro(Cliente *cliente)
{
    float valor_pago = 0.0;
    float total = cliente->carrinho->total;

    printf("\n--- PAGAMENTO EM DINHEIRO ---\n");
    printf("Total a pagar: R$ %.2f\n", total);

    do
    {
        printf("Digite o valor pago: R$ ");
        scanf("%f", &valor_pago);
        if (valor_pago < total)
        {
            printf("Valor insuficiente! Falta R$ %.2f\n", total - valor_pago);
        }
    } while (valor_pago < total);

    printf("Troco: R$ %.2f\n", valor_pago - total);

    // Atualiza estoque
    itemCarrinho *item = cliente->carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Pagamento realizado com sucesso!\n");

    // Atualiza dados do fechamento de caixa
    quantidade_vendas++;
    faturamento_total += total;
    valor_pago_dinheiro += total;

    // Libera carrinho
    while (cliente->carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = cliente->carrinho->inicio;
        cliente->carrinho->inicio = tmp->proximo;
        free(tmp);
    }

    free(cliente->carrinho);
    cliente->carrinho = NULL;
}

void efetuar_pagamento_cartao(Cliente *cliente)
{
    char numero_cartao[20];
    int parcelas;
    float total = cliente->carrinho->total;

    printf("\n--- PAGAMENTO COM CARTÃO ---\n");
    printf("Total a pagar: R$ %.2f\n", total);

    printf("Digite o número do cartão (fictício): ");
    getchar(); // limpar buffer
    fgets(numero_cartao, sizeof(numero_cartao), stdin);
    numero_cartao[strcspn(numero_cartao, "\n")] = '\0';

    do
    {
        printf("Digite o número de parcelas (1 a 12): ");
        scanf("%d", &parcelas);
        if (parcelas < 1 || parcelas > 12)
        {
            printf("Número de parcelas inválido.\n");
        }
    } while (parcelas < 1 || parcelas > 12);

    printf("Pagamento de R$ %.2f em %d x de R$ %.2f\n",
           total, parcelas, total / parcelas);

    // Atualiza estoque
    itemCarrinho *item = cliente->carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Pagamento realizado com sucesso!\n");

    // Atualiza dados do fechamento de caixa
    quantidade_vendas++;
    faturamento_total += total;
    valor_pago_cartao += total;

    // Libera carrinho
    while (cliente->carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = cliente->carrinho->inicio;
        cliente->carrinho->inicio = tmp->proximo;
        free(tmp);
    }

    free(cliente->carrinho);
    cliente->carrinho = NULL;
}

// Exemplo de função para pagamento misto (dinheiro/cartão)
void efetuar_pagamento_dinheiro_cartao(Cliente *cliente)
{
    float total = cliente->carrinho->total;
    float valor_dinheiro = 0.0, valor_cartao = 0.0;

    printf("\n--- PAGAMENTO EM DINHEIRO/CARTÃO ---\n");
    printf("Total a pagar: R$ %.2f\n", total);

    do {
        printf("Digite o valor pago em dinheiro: ");
        scanf("%f", &valor_dinheiro);
        printf("Digite o valor pago em cartão: ");
        scanf("%f", &valor_cartao);
        if (valor_dinheiro + valor_cartao < total) {
            printf("Valor insuficiente! Falta R$ %.2f\n", total - (valor_dinheiro + valor_cartao));
        }
    } while ((valor_dinheiro + valor_cartao) < total);

    printf("Troco: R$ %.2f\n", (valor_dinheiro + valor_cartao) - total);

    // Atualiza estoque
    itemCarrinho *item = cliente->carrinho->inicio;
    while (item != NULL)
    {
        item->produto->estoque -= item->quantidade;
        item = item->proximo;
    }

    printf("Pagamento realizado com sucesso!\n");

    // Atualiza dados do fechamento de caixa
    quantidade_vendas++;
    faturamento_total += total;
    valor_pago_dinheiro_cartao += total;

    // Libera carrinho
    while (cliente->carrinho->inicio != NULL)
    {
        itemCarrinho *tmp = cliente->carrinho->inicio;
        cliente->carrinho->inicio = tmp->proximo;
        free(tmp);
    }
    free(cliente->carrinho);
    cliente->carrinho = NULL;
}

void fechar_caixa(){
    char confirmacao;
    float fechamento_caixa, ajuste;

    if(!caixa_aberto){
        printf("O caixa já está Fechado!!!\n");
        return ;
    }
    
    // Calcule o fechamento de caixa conforme a regra:
    fechamento_caixa = faturamento_total - saldo_inicial_caixa
                       - valor_pago_dinheiro - valor_pago_cartao
                       - valor_pago_dinheiro_cartao;
    ajuste = fechamento_caixa;

    printf("\n\t\t=========FECHAMENTO DE CAIXA=======\n");
    printf("\t\t|Quantidade de vendas realizadas: %d\n", quantidade_vendas);
    printf("\t\t|Total do faturamento no dia: R$ %.2f\n", faturamento_total);
    printf("\t\t|Valor da abertura de caixa (VlAbertura): R$ %.2f\n", saldo_inicial_caixa);
    printf("\t\t|Valor pago em dinheiro (VlpagoD): R$ %.2f\n", valor_pago_dinheiro);
    printf("\t\t|Valor pago em cartão (VlpagoC): R$ %.2f\n", valor_pago_cartao);
    printf("\t\t|Valor pago em dinheiro/cartão (VlpagoDC): R$ %.2f\n", valor_pago_dinheiro_cartao);
    printf("\t\t|Ajuste (Divergência): R$ %.2f\n", ajuste);
    printf("\t\t=================================\n");

    if(fechamento_caixa != 0.0){
        printf("\nATENÇÃO: O caixa não poderá ser fechado porque há divergência de valores\n");
        printf("Por favor, realize o ajuste de valores. Informe o valor de ajuste (positivo para sobra, negativo para falta): ");
        scanf("%f", &valor_ajuste);
        ajuste -= valor_ajuste;
        if(ajuste == 0){
            printf("Ajuste realizado com sucesso. Caixa pode ser fechado.\n");
        } else {
            printf("Ainda há divergência! Caixa não pode ser fechado.\n");
            return;
        }
    }

    printf("\t\t====== FECHAMENTO DE CAIXA CONCLUÍDO =====\n");

    caixa_aberto = 0;
    saldo_inicial = 0.0;
    saldo_inicial_caixa = 0.0;
    printf("\n\t\t| Caixa fechado com sucesso!! |\n");
    printf("\t\t==========================================\n");
    //exit(0); // Se quiser encerrar o sistema aqui
}

int main()
{

    printf("          \t    _________________________ \n");
    printf("     \t\t   /________|LOJA|_____________ \n");
    printf("  \t\t  /___________| DA |_______________ \n");
    printf(" \t\t /______________|BERE|_______________ \n");
    printf("\t\t/_____________________________________\n");
    printf("\t\t|______________________|  |  |________|\n");
    printf("\t\t|______________________|--|--|________|\n");
    printf("\t\t|______________________|__|__|________|\n");
    printf("\t\t|__|    |_____________________________|\n");
    printf("\t\t|__|    |_____________________________|\n");
    printf("\t\t|__|°-  |_____________________________|\n");
    printf("\t\t|__|____|_____________________________|\n\n");
    printf("\n\t\t_______________________________________________\n");
    printf("\t\t__________BEM VINDO AO SISTEMA DA BERE___________\n");
    printf("\t\t_________________________________________________\n\n");
    printf("===================================================== \n");
    printf("| !ANTES DE TUDO VAMOS ABRIR O CAIXA POR GENTILEZA! |\n");
    printf("=====================================================\n");

    // Força o usuário a abrir o caixa antes de qualquer coisa
    while (!caixa_aberto)
    {
        printf("\n---------------------------------");
        printf("\n| O caixa ainda não foi Aberto! |\n");
        printf("---------------------------------\n");
        abertura_caixa();
    }

    abrir_menu_principal();
    liberar_lista_clientes();
    return 0;
}
    