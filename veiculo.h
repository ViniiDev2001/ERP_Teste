#ifndef VEICULO_H
#define VEICULO_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    VEICULO_DISPONIVEL = 1,
    VEICULO_VENDIDO    = 2,
    VEICULO_REMOVIDO   = 3   /* exclusao logica */
} StatusVeiculo;

/*
 * TAD Veiculo e TAD ListaVeiculos (tipos opacos).
 * Regras como "placa unica" e "so vende se estiver disponivel"
 * ficam dentro do modulo.
 */
typedef struct veiculo Veiculo;
typedef struct lista_veiculos ListaVeiculos;

/* ---------- TAD Veiculo ---------- */
int           veiculo_id(const Veiculo *v);
const char   *veiculo_marca(const Veiculo *v);
const char   *veiculo_modelo(const Veiculo *v);
int           veiculo_ano(const Veiculo *v);
const char   *veiculo_placa(const Veiculo *v);
const char   *veiculo_cor(const Veiculo *v);
int           veiculo_km(const Veiculo *v);
double        veiculo_preco_compra(const Veiculo *v);
double        veiculo_preco_venda(const Veiculo *v);
StatusVeiculo veiculo_status(const Veiculo *v);
const char   *veiculo_status_nome(const Veiculo *v);
bool          veiculo_disponivel(const Veiculo *v);
double        veiculo_lucro_previsto(const Veiculo *v);

/* true se marca, modelo ou placa contem o termo (ignora maiusculas) */
bool veiculo_corresponde(const Veiculo *v, const char *termo);

bool veiculo_definir_preco_venda(Veiculo *v, double preco);  /* false se <= 0 */
bool veiculo_marcar_vendido(Veiculo *v);   /* false se nao estiver disponivel */
bool veiculo_remover(Veiculo *v);          /* false se ja foi vendido/removido */

/* Placa no formato de 7 letras/numeros (aceita hifen e minusculas) */
bool veiculo_placa_valida(const char *placa);

/* ---------- TAD ListaVeiculos ---------- */
ListaVeiculos *lista_veiculos_criar(void);
void           lista_veiculos_destruir(ListaVeiculos *l);

/* NULL se algum dado for invalido ou a placa ja existir */
Veiculo *lista_veiculos_cadastrar(ListaVeiculos *l, const char *marca,
                                  const char *modelo, int ano, const char *placa,
                                  const char *cor, int km,
                                  double preco_compra, double preco_venda);

bool     lista_veiculos_placa_existe(const ListaVeiculos *l, const char *placa);
Veiculo *lista_veiculos_buscar(const ListaVeiculos *l, int id);   /* ignora removidos */
void     lista_veiculos_descricao_por_id(const ListaVeiculos *l, int id,
                                         char *buf, size_t tam); /* inclui removidos */

int      lista_veiculos_tamanho(const ListaVeiculos *l);
Veiculo *lista_veiculos_obter(const ListaVeiculos *l, int indice);
int      lista_veiculos_total_por_status(const ListaVeiculos *l, StatusVeiculo s);
double   lista_veiculos_valor_investido(const ListaVeiculos *l);  /* compra, disponiveis */
double   lista_veiculos_valor_potencial(const ListaVeiculos *l);  /* venda, disponiveis */

bool lista_veiculos_salvar(const ListaVeiculos *l, const char *arquivo);
bool lista_veiculos_carregar(ListaVeiculos *l, const char *arquivo);

#endif
