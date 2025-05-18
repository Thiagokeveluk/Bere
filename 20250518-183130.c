#include <stdio.h>

    
    
    typedef struct ItemVenda {
    int codigoProduto;
    char descricao[100];
    float precoVenda;
    int quantidade;
    float totalItem;
    struct ItemVenda *proximo;
} ItemVenda;

typedef struct Venda {
    int codigoVenda;
    Cliente *cliente;
    ItemVenda *itens;
    float totalVenda;
    float desconto;
    char status; // 'a' = Aberto, 'p' = Pago
    char tipoPagamento[3]; // "d", "c", "dc"
    struct Venda *proximo;
} Venda;

typedef struct Pagamento {
    int codigoVenda;
    float valor;
    char tipo[3]; // "d", "c", "dc"
    struct Pagamento *proximo;
} Pagamento;
Venda *inicio_vendas = NULL;
Pagamento *inicio_pagamentos = NULL;
int contador_venda = 1;

float total_pago_dinheiro = 0;
float total_pago_cartao = 0;
float total_pago_misto = 0;
float ajuste = 0;


void Nova_Venda() {
    if (!caixa_aberto) {
        printf("Caixa fechado! Abra o caixa antes de realizar uma venda.\n");
        return;
    }

    Cliente *cliente = selecionar_cliente();
    if (!cliente) return;

    Venda *novaVenda = malloc(sizeof(Venda));
    novaVenda->codigoVenda = contador_venda++;
    novaVenda->cliente = cliente;
    novaVenda->itens = NULL;
    novaVenda->totalVenda = 0;
    novaVenda->desconto = 0;
    novaVenda->status = 'a';
    strcpy(novaVenda->tipoPagamento, "a");
    novaVenda->proximo = inicio_vendas;
    inicio_vendas = novaVenda;

    char continuar = 's';
    while (continuar == 's' || continuar == 'S') {
        exibir_produtos();
        int codigo, qtd;
        printf("Código do produto: ");
        scanf("%d", &codigo);
        Produto *p = buscar_produto_por_codigo(codigo);
        if (!p) {
            printf("Produto não encontrado!\n");
            continue;
        }

        if (p->estoque == 0) {
            printf("Estoque zerado! Não é possível vender.\n");
            continue;
        }

        printf("Quantidade: ");
        scanf("%d", &qtd);

        if (qtd > p->estoque) {
            printf("Você quer vender %d, mas só há %d em estoque. Continuar? (s/n): ", qtd, p->estoque);
            limparBuffer();
            scanf("%c", &continuar);
            if (continuar != 's' && continuar != 'S') continue;
            qtd = p->estoque;
        }

        if (p->estoque - qtd <= p->estoque_min) {
            printf("ALERTA: Estoque mínimo atingido!\n");
        }

        // Adicionar item na venda
        ItemVenda *item = malloc(sizeof(ItemVenda));
        item->codigoProduto = p->Codigo;
        strcpy(item->descricao, p->nome);
        item->precoVenda = p->preco;
        item->quantidade = qtd;
        item->totalItem = qtd * p->preco;
        item->proximo = novaVenda->itens;
        novaVenda->itens = item;

        novaVenda->totalVenda += item->totalItem;
        p->estoque -= qtd;

        printf("Deseja adicionar outro item? (s/n): ");
        limparBuffer();
        scanf("%c", &continuar);
    }

    printf("Total da venda: R$ %.2f\n", novaVenda->totalVenda);
    printf("Deseja aplicar desconto? (0 para não): ");
    scanf("%f", &novaVenda->desconto);
    novaVenda->totalVenda -= (novaVenda->totalVenda * novaVenda->desconto / 100);
    printf("Total com desconto: R$ %.2f\n", novaVenda->totalVenda);

    faturamento_total += novaVenda->totalVenda;
    clientes_atendidos++;
}
void efetuar_pagamentos() {
    if (!inicio_vendas) {
        printf("Nenhuma venda registrada.\n");
        return;
    }

    Venda *vendaAtual = inicio_vendas;
    int encontrou = 0;

    // 1. Mostrar vendas em aberto
    printf("\n=== Vendas em Aberto ===\n");
    while (vendaAtual != NULL) {
        if (vendaAtual->status == 'a') {
            printf("Venda #%d | Cliente: %s | Total: R$ %.2f\n", vendaAtual->codigoVenda, vendaAtual->cliente->nome_completo, vendaAtual->totalVenda);
            encontrou = 1;
        }
        vendaAtual = vendaAtual->proximo;
    }

    if (!encontrou) {
        printf("Nenhuma venda em aberto!\n");
        return;
    }

    int codigoBusca;
    printf("\nInforme o código da venda para efetuar o pagamento: ");
    scanf("%d", &codigoBusca);

    vendaAtual = inicio_vendas;
    while (vendaAtual != NULL && vendaAtual->codigoVenda != codigoBusca)
        vendaAtual = vendaAtual->proximo;

    if (!vendaAtual || vendaAtual->status != 'a') {
        printf("Venda inválida ou já paga.\n");
        return;
    }

    int opcaoPagamento;
    float valorRecebido = 0.0, troco = 0.0;

    printf("\nForma de Pagamento:\n");
    printf("1. Dinheiro\n");
    printf("2. Cartão\n");
    printf("3. Dinheiro + Cartão\n");
    printf("Opção: ");
    scanf("%d", &opcaoPagamento);

    Pagamento *pag = malloc(sizeof(Pagamento));
    pag->codigoVenda = vendaAtual->codigoVenda;
    pag->valor = vendaAtual->totalVenda;
    pag->proximo = inicio_pagamentos;

    switch (opcaoPagamento) {
        case 1: // Dinheiro
            printf("Valor recebido em dinheiro: R$ ");
            scanf("%f", &valorRecebido);
            if (valorRecebido < vendaAtual->totalVenda) {
                printf("Valor insuficiente! Deseja completar com cartão? (s/n): ");
                char resp;
                limparBuffer();
                scanf("%c", &resp);
                if (resp == 's' || resp == 'S') {
                    printf("Pagamento complementado no cartão. Valor restante: R$ %.2f\n", vendaAtual->totalVenda - valorRecebido);
                    strcpy(pag->tipo, "dc");
                    total_pago_misto += vendaAtual->totalVenda;
                } else {
                    printf("Pagamento cancelado.\n");
                    free(pag);
                    return;
                }
            } else {
                troco = valorRecebido - vendaAtual->totalVenda;
                printf("Troco: R$ %.2f\n", troco);
                strcpy(pag->tipo, "d");
                total_pago_dinheiro += vendaAtual->totalVenda;
            }
            break;

        case 2: // Cartão
            printf("Pagamento no cartão realizado na maquininha? (1 - Sim / 0 - Não): ");
            int confirmado;
            scanf("%d", &confirmado);
            if (confirmado != 1) {
                printf("Pagamento não confirmado. Cancelado.\n");
                free(pag);
                return;
            }
            strcpy(pag->tipo, "c");
            total_pago_cartao += vendaAtual->totalVenda;
            break;

        case 3: // Misto
            float parteDinheiro, parteCartao;
            printf("Informe o valor pago em dinheiro: R$ ");
            scanf("%f", &parteDinheiro);
            parteCartao = vendaAtual->totalVenda - parteDinheiro;

            if (parteDinheiro < 0 || parteCartao < 0) {
                printf("Valores inválidos!\n");
                free(pag);
                return;
            }

            troco = parteDinheiro + parteCartao - vendaAtual->totalVenda;
            if (troco > 0)
                printf("Troco: R$ %.2f\n", troco);

            strcpy(pag->tipo, "dc");
            total_pago_misto += vendaAtual->totalVenda;
            break;

        default:
            printf("Opção inválida!\n");
            free(pag);
            return;
    }

    inicio_pagamentos = pag;
    vendaAtual->status = 'p';
    strcpy(vendaAtual->tipoPagamento, pag->tipo);

    printf("\nPagamento efetuado com sucesso!\n");
}
void fechar_caixa() {
    if (!caixa_aberto) {
        printf("Caixa já está fechado!\n");
        return;
    }

    printf("\n=== Fechamento de Caixa – Resumo ===\n");
    printf("• Quantidade de vendas realizadas: %d\n", clientes_atendidos);
    printf("• Total do faturamento no dia (Faturamento): R$ %.2f\n", faturamento_total);
    printf("• Valor da abertura de caixa (VlAbertura): R$ %.2f\n", abrir_caixa);
    printf("• Valor pago em dinheiro (VlpagoD): R$ %.2f\n", total_pago_dinheiro);
    printf("• Valor pago em cartão (VlpagoC): R$ %.2f\n", total_pago_cartao);
    printf("• Valor pago em dinheiro/cartão (VlpagoDC): R$ %.2f\n", total_pago_misto);

    ajuste = faturamento_total - abrir_caixa - total_pago_dinheiro - total_pago_cartao - total_pago_misto;

    if (ajuste == 0) {
        printf("\nCaixa fechado corretamente. Tudo certo!\n");
    } else {
        printf("\nATENÇÃO: O caixa não poderá ser fechado porque há divergência de valores\n");
        printf("Valor de AJUSTE: R$ %.2f\n", ajuste);

        if (ajuste > 0) {
            printf("SOBRA no caixa de R$ %.2f\n", ajuste);
        } else {
            printf("FALTA no caixa de R$ %.2f\n", ajuste);
        }

        printf("Corrija os valores ou faça um ajuste manual para continuar.\n");
        return; // Não fecha o caixa se tiver divergência
    }

    // Resetar variáveis
    caixa_aberto = 0;
    abrir_caixa = 0.0;
    faturamento_total = 0.0;
    total_pago_dinheiro = 0.0;
    total_pago_cartao = 0.0;
    total_pago_misto = 0.0;
    clientes_atendidos = 0;

    printf("\nCaixa finalizado. Até a próxima, Dona Berê!\n");
}
void listar_clientes() {
    Cliente *cursor = listaClientes;

    if (!cursor) {
        printf("Nenhum cliente cadastrado!\n");
        return;
    }

    printf("\n=== LISTA DE CLIENTES ===\n");
    while (cursor != NULL) {
        printf("Código: %d | Nome: %s | CPF: %s | Celular: %s\n",
               cursor->codigo, cursor->nome_completo, cursor->cpf, cursor->celular);
        cursor = cursor->proximo;
    }
}
void listar_produtos() {
    Produto *cursor = listaProdutos;

    if (!cursor) {
        printf("Nenhum produto cadastrado!\n");
        return;
    }

    printf("\n=== LISTA DE PRODUTOS ===\n");
    while (cursor != NULL) {
        printf("Código: %d | %s | Categoria: %s | R$%.2f | Estoque: %d\n",
               cursor->Codigo, cursor->nome, cursor->categoria, cursor->preco, cursor->estoque);
        cursor = cursor->proximo;
    }
}
void listar_vendas() {
    Venda *v = inicio_vendas;
    if (!v) {
        printf("Nenhuma venda registrada!\n");
        return;
    }

    printf("\n=== LISTA DE VENDAS ===\n");
    while (v != NULL) {
        printf("Venda #%d | Cliente: %s | Total: R$%.2f | Status: %s\n",
               v->codigoVenda, v->cliente->nome_completo, v->totalVenda,
               v->status == 'a' ? "Aberta" : "Paga");
        v = v->proximo;
    }
}
void listar_pagamentos() {
    Pagamento *p = inicio_pagamentos;

    if (!p) {
        printf("Nenhum pagamento registrado!\n");
        return;
    }

    printf("\n=== LISTA DE PAGAMENTOS ===\n");
    while (p != NULL) {
        printf("Venda #%d | Valor: R$%.2f | Tipo: %s\n",
               p->codigoVenda, p->valor, p->tipo);
        p = p->proximo;
    }
}
typedef struct {
    int codigo;
    char nome[100];
    int vendidos;
} ProdutoEstatistica;

void top_produtos_mais_vendidos() {
    ProdutoEstatistica stats[100];
    int count = 0;

    Venda *v = inicio_vendas;
    while (v != NULL) {
        ItemVenda *item = v->itens;
        while (item != NULL) {
            // Buscar se já existe no vetor
            int encontrado = 0;
            for (int i = 0; i < count; i++) {
                if (stats[i].codigo == item->codigoProduto) {
                    stats[i].vendidos += item->quantidade;
                    encontrado = 1;
                    break;
                }
            }

            if (!encontrado && count < 100) {
                stats[count].codigo = item->codigoProduto;
                strcpy(stats[count].nome, item->descricao);
                stats[count].vendidos = item->quantidade;
                count++;
            }

            item = item->proximo;
        }
        v = v->proximo;
    }

    // Ordenar por mais vendidos (bolha simples)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (stats[j].vendidos > stats[i].vendidos) {
                ProdutoEstatistica temp = stats[i];
                stats[i] = stats[j];
                stats[j] = temp;
            }
        }
    }

    printf("\n=== TOP 3 PRODUTOS MAIS VENDIDOS ===\n");
    for (int i = 0; i < count && i < 3; i++) {
        printf("%d. %s - %d unidades vendidas\n", i + 1, stats[i].nome, stats[i].vendidos);
    }
}
void menuRelatorios() {
    int opcao;
    do {
        printf("\n=== MENU RELATÓRIOS ===\n");
        printf("1. Listagem de Clientes\n");
        printf("2. Listagem de Produtos\n");
        printf("3. Listagem de Vendas\n");
        printf("4. Listagem de Pagamentos\n");
        printf("5. TOP 3 Produtos Mais Vendidos\n");
        printf("6. Voltar ao Menu Principal\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: listar_clientes(); break;
            case 2: listar_produtos(); break;
            case 3: listar_vendas(); break;
            case 4: listar_pagamentos(); break;
            case 5: top_produtos_mais_vendidos(); break;
            case 6: printf("Voltando...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 6);
}
void limparBuffer();
void menuCadastro();
void menuCaixa();
void menuRelatorios();
int abrir_menu_principal(); // se você usar essa versão
void cadastrarCliente();
void listar_clientes();
Cliente* selecionar_cliente();
void cadastrarProduto();
void listar_produtos();
Produto* buscar_produto_por_codigo(int codigo);
void Nova_Venda();
void listar_vendas();
void efetuar_pagamentos();
void listar_pagamentos();
void abertura_caixa();
void fechar_caixa();

int caixa_aberto = 0;
int clientes_atendidos = 0;
float faturamento_total = 0;
float abrir_caixa = 0.0;
float total_pago_dinheiro = 0.0;
float total_pago_cartao = 0.0;
float total_pago_misto = 0.0;
float ajuste = 0.0;

int main() {
    setlocale(LC_ALL, "Portuguese");
    int opcao;

    do {
        printf("1. Cadastro\n");
        printf("2. Caixa\n");
        printf("3. Relatórios\n");
        printf("4. Sair\n");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: menuCadastro(); break;
            case 2: menuCaixa(); break;
            case 3: menuRelatorios(); break;
            case 4: printf("Saindo..."); break;
            default: printf("Opção inválida!"); break;
        }
    } while (opcao != 4);

    return 0;
}
