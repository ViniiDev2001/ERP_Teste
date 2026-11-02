#ifndef CLIENTE_H
#define CLIENTE_H

#include <stdbool.h>

/*
 * TAD Cliente e TAD ListaClientes.
 * Os tipos sao opacos: a estrutura interna so existe em cliente.c.
 * Quem usa o modulo acessa os dados apenas pelas funcoes abaixo.
 */
typedef struct cliente Cliente;
typedef struct lista_clientes ListaClientes;

/* ---------- TAD Cliente (somente leitura + desativar) ---------- */
int         cliente_id(const Cliente *c);
const char *cliente_nome(const Cliente *c);
const char *cliente_documento(const Cliente *c);
const char *cliente_telefone(const Cliente *c);
bool        cliente_ativo(const Cliente *c);
void        cliente_desativar(Cliente *c);   /* exclusao logica */

/* ---------- TAD ListaClientes ---------- */
ListaClientes *lista_clientes_criar(void);
void           lista_clientes_destruir(ListaClientes *l);

/* Cria um cliente com novo ID e o insere na lista. Retorna NULL se falhar. */
Cliente *lista_clientes_cadastrar(ListaClientes *l, const char *nome,
                                  const char *documento, const char *telefone);

/* Busca apenas clientes ativos. Retorna NULL se nao achar. */
Cliente *lista_clientes_buscar(const ListaClientes *l, int id);

/* Nome por ID, inclusive de clientes excluidos (para historico). */
const char *lista_clientes_nome_por_id(const ListaClientes *l, int id);

int      lista_clientes_tamanho(const ListaClientes *l);
Cliente *lista_clientes_obter(const ListaClientes *l, int indice);
int      lista_clientes_total_ativos(const ListaClientes *l);

/* Persistencia. Carregar com arquivo inexistente retorna true (lista vazia). */
bool lista_clientes_salvar(const ListaClientes *l, const char *arquivo);
bool lista_clientes_carregar(ListaClientes *l, const char *arquivo);

#endif
