/*
 * ERP para revenda de carros - interface de texto.
 * O main so conhece as INTERFACES (.h) dos TADs Cliente, Veiculo e Venda;
 * nunca mexe nas estruturas internas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"
#include "cliente.h"
#include "veiculo.h"
#include "venda.h"

#define ARQ_CLIENTES "clientes.dat"
#define ARQ_VEICULOS "veiculos.dat"
#define ARQ_VENDAS   "vendas.dat"

static ListaClientes *clientes;
static ListaVeiculos *veiculos;
static ListaVendas   *vendas;

static void salvar_tudo(void) {
    if (!lista_clientes_salvar(clientes, ARQ_CLIENTES) ||
        !lista_veiculos_salvar(veiculos, ARQ_VEICULOS) ||
        !lista_vendas_salvar(vendas, ARQ_VENDAS))
        printf("AVISO: nao foi possivel gravar os dados em disco.\n");
}

/* Le um valor; se o usuario so apertar ENTER, devolve o padrao. */
static double ler_valor_ou_padrao(const char *prompt, double padrao) {
    char buf[64];
    char *fim;
    for (;;) {
        ler_texto(prompt, buf, sizeof buf);
        if (buf[0] == '\0') return padrao;
        for (char *p = buf; *p; p++) if (*p == ',') *p = '.';
        double v = strtod(buf, &fim);
        if (fim != buf && *fim == '\0') return v;
        printf("Valor invalido.\n");
    }
}

/* ===================== Clientes ===================== */

static void cadastrar_cliente(void) {
    char nome[60], doc[20], tel[20];
    ler_texto("Nome: ", nome, sizeof nome);
    if (nome[0] == '\0') {
        printf("Nome nao pode ser vazio.\n");
        return;
    }
    ler_texto("CPF/CNPJ: ", doc, sizeof doc);
    ler_texto("Telefone: ", tel, sizeof tel);
    Cliente *c = lista_clientes_cadastrar(clientes, nome, doc, tel);
    if (!c) {
        printf("Erro ao cadastrar cliente.\n");
        return;
    }
    salvar_tudo();
    printf("Cliente cadastrado com ID %d.\n", cliente_id(c));
}

static void listar_clientes(void) {
    printf("\n%-5s %-30s %-18s %-15s\n", "ID", "Nome", "Documento", "Telefone");
    printf("---------------------------------------------------------------------\n");
    int count = 0;
    for (int i = 0; i < lista_clientes_tamanho(clientes); i++) {
        const Cliente *c = lista_clientes_obter(clientes, i);
        if (!cliente_ativo(c)) continue;
        printf("%-5d %-30s %-18s %-15s\n", cliente_id(c), cliente_nome(c),
               cliente_documento(c), cliente_telefone(c));
        count++;
    }
    if (count == 0) printf("(nenhum cliente cadastrado)\n");
}

static void excluir_cliente(void) {
    int id = ler_int("ID do cliente a excluir: ");
    Cliente *c = lista_clientes_buscar(clientes, id);
    if (!c) {
        printf("Cliente nao encontrado.\n");
        return;
    }
    cliente_desativar(c);  /* exclusao logica: preserva historico de vendas */
    salvar_tudo();
    printf("Cliente '%s' excluido.\n", cliente_nome(c));
}

static void menu_clientes(void) {
    int op;
    do {
        printf("\n=== CLIENTES ===\n");
        printf("1. Cadastrar\n2. Listar\n3. Excluir\n0. Voltar\n");
        op = ler_int("Opcao: ");
        switch (op) {
            case 1: cadastrar_cliente(); break;
            case 2: listar_clientes(); break;
            case 3: excluir_cliente(); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

/* ===================== Veiculos ===================== */

static void cadastrar_veiculo(void) {
    char marca[30], modelo[40], placa[16], cor[20];
    ler_texto("Marca: ", marca, sizeof marca);
    ler_texto("Modelo: ", modelo, sizeof modelo);
    if (marca[0] == '\0' || modelo[0] == '\0') {
        printf("Marca e modelo sao obrigatorios.\n");
        return;
    }
    int ano = ler_int("Ano: ");
    ler_texto("Placa (ex: ABC1D23): ", placa, sizeof placa);
    if (!veiculo_placa_valida(placa)) {
        printf("Placa invalida: use 7 letras/numeros.\n");
        return;
    }
    if (lista_veiculos_placa_existe(veiculos, placa)) {
        printf("Ja existe um veiculo com essa placa.\n");
        return;
    }
    ler_texto("Cor: ", cor, sizeof cor);
    int km = ler_int("Quilometragem: ");
    double compra = ler_double("Preco de compra (R$): ");
    double venda = ler_double("Preco de venda (R$): ");

    Veiculo *v = lista_veiculos_cadastrar(veiculos, marca, modelo, ano, placa,
                                          cor, km, compra, venda);
    if (!v) {
        printf("Dados invalidos (ano entre 1900 e 2100, km >= 0, precos > 0).\n");
        return;
    }
    salvar_tudo();
    printf("Veiculo cadastrado com ID %d (lucro previsto: R$ %.2f).\n",
           veiculo_id(v), veiculo_lucro_previsto(v));
}

/* modo: 0 = so disponiveis, 1 = todos (exceto removidos). Retorna quantos listou. */
static int listar_veiculos(int modo, const char *filtro) {
    printf("\n%-4s %-12s %-16s %-4s %-9s %-8s %8s %13s %s\n", "ID", "Marca",
           "Modelo", "Ano", "Cor", "Placa", "Km", "Preco (R$)", modo ? "Status" : "");
    printf("-----------------------------------------------------------------------------------------\n");
    int count = 0;
    for (int i = 0; i < lista_veiculos_tamanho(veiculos); i++) {
        const Veiculo *v = lista_veiculos_obter(veiculos, i);
        if (veiculo_status(v) == VEICULO_REMOVIDO) continue;
        if (!modo && !veiculo_disponivel(v)) continue;
        if (filtro && !veiculo_corresponde(v, filtro)) continue;
        printf("%-4d %-12s %-16s %-4d %-9s %-8s %8d %13.2f %s\n", veiculo_id(v),
               veiculo_marca(v), veiculo_modelo(v), veiculo_ano(v), veiculo_cor(v),
               veiculo_placa(v), veiculo_km(v), veiculo_preco_venda(v),
               modo ? veiculo_status_nome(v) : "");
        count++;
    }
    if (count == 0) printf("(nenhum veiculo encontrado)\n");
    return count;
}

static void buscar_veiculo(void) {
    char termo[40];
    ler_texto("Buscar por marca, modelo ou placa: ", termo, sizeof termo);
    listar_veiculos(1, termo);
}

static void alterar_preco_venda(void) {
    Veiculo *v = lista_veiculos_buscar(veiculos, ler_int("ID do veiculo: "));
    if (!v) {
        printf("Veiculo nao encontrado.\n");
        return;
    }
    if (!veiculo_disponivel(v)) {
        printf("Veiculo ja vendido; nao e possivel alterar o preco.\n");
        return;
    }
    if (!veiculo_definir_preco_venda(v, ler_double("Novo preco de venda (R$): "))) {
        printf("Preco invalido.\n");
        return;
    }
    salvar_tudo();
    printf("Preco atualizado. Lucro previsto: R$ %.2f\n", veiculo_lucro_previsto(v));
}

static void remover_veiculo(void) {
    Veiculo *v = lista_veiculos_buscar(veiculos, ler_int("ID do veiculo a remover: "));
    if (!v) {
        printf("Veiculo nao encontrado.\n");
        return;
    }
    if (!veiculo_remover(v)) {
        printf("So e possivel remover veiculos disponiveis (este ja foi vendido).\n");
        return;
    }
    salvar_tudo();
    printf("Veiculo %s %s (%s) removido do estoque.\n", veiculo_marca(v),
           veiculo_modelo(v), veiculo_placa(v));
}

static void menu_veiculos(void) {
    int op;
    do {
        printf("\n=== VEICULOS ===\n");
        printf("1. Cadastrar\n2. Estoque (disponiveis)\n3. Todos (com status)\n"
               "4. Buscar\n5. Alterar preco de venda\n6. Remover do estoque\n0. Voltar\n");
        op = ler_int("Opcao: ");
        switch (op) {
            case 1: cadastrar_veiculo(); break;
            case 2: listar_veiculos(0, NULL); break;
            case 3: listar_veiculos(1, NULL); break;
            case 4: buscar_veiculo(); break;
            case 5: alterar_preco_venda(); break;
            case 6: remover_veiculo(); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

/* ===================== Vendas ===================== */

static void nova_venda(void) {
    if (lista_veiculos_total_por_status(veiculos, VEICULO_DISPONIVEL) == 0) {
        printf("Nao ha veiculos disponiveis para venda.\n");
        return;
    }

    listar_clientes();
    int cid = ler_int("\nID do cliente: ");
    if (!lista_clientes_buscar(clientes, cid)) {
        printf("Cliente nao encontrado.\n");
        return;
    }

    listar_veiculos(0, NULL);
    int vid = ler_int("\nID do veiculo: ");
    Veiculo *v = lista_veiculos_buscar(veiculos, vid);
    if (!v || !veiculo_disponivel(v)) {
        printf("Veiculo nao encontrado ou indisponivel.\n");
        return;
    }

    double tabela = veiculo_preco_venda(v);
    printf("\nPreco de tabela: R$ %.2f\n", tabela);
    double final = ler_valor_ou_padrao("Valor final negociado (ENTER = tabela): R$ ", tabela);
    if (final <= 0) {
        printf("Valor invalido.\n");
        return;
    }

    printf("\nForma de pagamento:\n1. A vista\n2. Financiamento\n3. Cartao\n4. Troca + diferenca\n");
    int forma = ler_int("Opcao: ");
    if (forma < PAG_A_VISTA || forma > PAG_TROCA) {
        printf("Forma de pagamento invalida.\n");
        return;
    }
    double entrada = 0.0;
    if (forma == PAG_FINANCIAMENTO)
        entrada = ler_double("Valor da entrada (R$): ");

    Venda *venda = venda_criar(lista_vendas_proximo_id(vendas), cid, vid, tabela,
                               veiculo_preco_compra(v), final,
                               (FormaPagamento)forma, entrada);
    if (!venda) {
        printf("Dados invalidos (a entrada deve estar entre 0 e o valor final).\n");
        return;
    }

    printf("\n--- Resumo ---\n");
    printf("Veiculo:   %s %s %d (%s)\n", veiculo_marca(v), veiculo_modelo(v),
           veiculo_ano(v), veiculo_placa(v));
    printf("Cliente:   %s\n", lista_clientes_nome_por_id(clientes, cid));
    printf("Valor:     R$ %.2f (%s)\n", final, forma_pagamento_nome((FormaPagamento)forma));
    printf("Lucro:     R$ %.2f%s\n", venda_lucro(venda),
           venda_lucro(venda) < 0 ? "  <- PREJUIZO" : "");

    char conf[8];
    ler_texto("Confirmar venda? (s/n): ", conf, sizeof conf);
    if (conf[0] != 's' && conf[0] != 'S') {
        printf("Venda cancelada.\n");
        venda_destruir(venda);
        return;
    }

    if (!veiculo_marcar_vendido(v)) {
        printf("Veiculo nao esta mais disponivel.\n");
        venda_destruir(venda);
        return;
    }
    venda_finalizar(venda);
    int id = venda_id(venda);
    if (!lista_vendas_registrar(vendas, venda)) {
        printf("Erro ao registrar a venda.\n");
        venda_destruir(venda);
        return;
    }
    salvar_tudo();
    printf("Venda %d registrada com sucesso!\n", id);
}

static void listar_vendas(void) {
    if (lista_vendas_tamanho(vendas) == 0) {
        printf("(nenhuma venda registrada)\n");
        return;
    }
    for (int i = 0; i < lista_vendas_tamanho(vendas); i++) {
        const Venda *v = lista_vendas_obter(vendas, i);
        char data[32], desc[100];
        time_t t = venda_data(v);
        strftime(data, sizeof data, "%d/%m/%Y %H:%M", localtime(&t));
        lista_veiculos_descricao_por_id(veiculos, venda_veiculo_id(v), desc, sizeof desc);
        printf("\nVenda #%d | %s\n", venda_id(v), data);
        printf("   Cliente:   %s\n", lista_clientes_nome_por_id(clientes, venda_cliente_id(v)));
        printf("   Veiculo:   %s\n", desc);
        printf("   Tabela:    R$ %.2f | Desconto: R$ %.2f\n", venda_preco_tabela(v), venda_desconto(v));
        printf("   Pagamento: %s", forma_pagamento_nome(venda_forma(v)));
        if (venda_entrada(v) > 0) printf(" (entrada R$ %.2f)", venda_entrada(v));
        printf("\n   TOTAL: R$ %.2f | Lucro: R$ %.2f\n", venda_valor_final(v), venda_lucro(v));
    }
}

static void menu_vendas(void) {
    int op;
    do {
        printf("\n=== VENDAS ===\n");
        printf("1. Nova venda\n2. Listar vendas\n0. Voltar\n");
        op = ler_int("Opcao: ");
        switch (op) {
            case 1: nova_venda(); break;
            case 2: listar_vendas(); break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
        if (op != 0) pausar();
    } while (op != 0);
}

/* ===================== Relatorios ===================== */

static void relatorio_resumo(void) {
    int n_vendas = lista_vendas_tamanho(vendas);
    double faturamento = lista_vendas_faturamento(vendas);
    double lucro = lista_vendas_lucro_total(vendas);

    printf("\n=== RESUMO DA REVENDA ===\n");
    printf("Clientes ativos:          %d\n", lista_clientes_total_ativos(clientes));
    printf("\n--- Estoque ---\n");
    printf("Veiculos disponiveis:     %d\n",
           lista_veiculos_total_por_status(veiculos, VEICULO_DISPONIVEL));
    printf("Valor investido (compra): R$ %.2f\n", lista_veiculos_valor_investido(veiculos));
    printf("Valor potencial (venda):  R$ %.2f\n", lista_veiculos_valor_potencial(veiculos));
    printf("\n--- Vendas ---\n");
    printf("Veiculos vendidos:        %d\n", n_vendas);
    printf("Faturamento:              R$ %.2f\n", faturamento);
    printf("Descontos concedidos:     R$ %.2f\n", lista_vendas_descontos_total(vendas));
    printf("Lucro total:              R$ %.2f\n", lucro);
    if (n_vendas > 0) {
        printf("Lucro medio por venda:    R$ %.2f\n", lucro / n_vendas);
        printf("Ticket medio:             R$ %.2f\n", faturamento / n_vendas);
    }
    if (faturamento > 0)
        printf("Margem de lucro:          %.1f%%\n", lucro / faturamento * 100.0);
}

/* ===================== Menu principal ===================== */

int main(void) {
    clientes = lista_clientes_criar();
    veiculos = lista_veiculos_criar();
    vendas   = lista_vendas_criar();
    if (!clientes || !veiculos || !vendas) {
        printf("Erro de memoria.\n");
        return 1;
    }

    if (!lista_clientes_carregar(clientes, ARQ_CLIENTES) ||
        !lista_veiculos_carregar(veiculos, ARQ_VEICULOS) ||
        !lista_vendas_carregar(vendas, ARQ_VENDAS))
        printf("AVISO: arquivo de dados corrompido; parte dos dados nao foi carregada.\n");

    int op;
    do {
        printf("\n==============================\n");
        printf("      ERP - REVENDA DE CARROS\n");
        printf("==============================\n");
        printf("1. Clientes\n2. Veiculos\n3. Vendas\n4. Relatorios\n0. Sair\n");
        op = ler_int("Opcao: ");
        switch (op) {
            case 1: menu_clientes(); break;
            case 2: menu_veiculos(); break;
            case 3: menu_vendas(); break;
            case 4: relatorio_resumo(); pausar(); break;
            case 0: salvar_tudo(); printf("Ate logo!\n"); break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);

    lista_vendas_destruir(vendas);
    lista_veiculos_destruir(veiculos);
    lista_clientes_destruir(clientes);
    return 0;
}
