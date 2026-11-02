#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "veiculo.h"
#include "colecao.h"

#define PLACA_TAM 7

struct veiculo {
    int    id;
    char   marca[30];
    char   modelo[40];
    int    ano;
    char   placa[PLACA_TAM + 1];
    char   cor[20];
    int    km;
    double preco_compra;
    double preco_venda;
    int    status;
};

struct lista_veiculos {
    Colecao dados;
};

/* ---------- auxiliares internas ---------- */

/* Normaliza (maiuscula, sem hifen/espaco) e valida. Retorna false se invalida. */
static bool normalizar_placa(const char *in, char *out) {
    int n = 0;
    for (; *in; in++) {
        if (*in == '-' || *in == ' ') continue;
        if (!isalnum((unsigned char)*in) || n >= PLACA_TAM) return false;
        out[n++] = (char)toupper((unsigned char)*in);
    }
    out[n] = '\0';
    return n == PLACA_TAM;
}

static bool contem_ci(const char *texto, const char *termo) {
    size_t n = strlen(termo);
    if (n == 0) return true;
    for (const char *p = texto; *p; p++) {
        size_t i = 0;
        while (i < n && p[i] &&
               tolower((unsigned char)p[i]) == tolower((unsigned char)termo[i]))
            i++;
        if (i == n) return true;
    }
    return false;
}

/* ---------- Veiculo ---------- */

int           veiculo_id(const Veiculo *v)           { return v->id; }
const char   *veiculo_marca(const Veiculo *v)        { return v->marca; }
const char   *veiculo_modelo(const Veiculo *v)       { return v->modelo; }
int           veiculo_ano(const Veiculo *v)          { return v->ano; }
const char   *veiculo_placa(const Veiculo *v)        { return v->placa; }
const char   *veiculo_cor(const Veiculo *v)          { return v->cor; }
int           veiculo_km(const Veiculo *v)           { return v->km; }
double        veiculo_preco_compra(const Veiculo *v) { return v->preco_compra; }
double        veiculo_preco_venda(const Veiculo *v)  { return v->preco_venda; }
StatusVeiculo veiculo_status(const Veiculo *v)       { return (StatusVeiculo)v->status; }
bool          veiculo_disponivel(const Veiculo *v)   { return v->status == VEICULO_DISPONIVEL; }
double        veiculo_lucro_previsto(const Veiculo *v) { return v->preco_venda - v->preco_compra; }

const char *veiculo_status_nome(const Veiculo *v) {
    switch (v->status) {
        case VEICULO_DISPONIVEL: return "Disponivel";
        case VEICULO_VENDIDO:    return "Vendido";
        case VEICULO_REMOVIDO:   return "Removido";
        default:                 return "?";
    }
}

bool veiculo_corresponde(const Veiculo *v, const char *termo) {
    return contem_ci(v->marca, termo) || contem_ci(v->modelo, termo) ||
           contem_ci(v->placa, termo);
}

bool veiculo_definir_preco_venda(Veiculo *v, double preco) {
    if (preco <= 0) return false;
    v->preco_venda = preco;
    return true;
}

bool veiculo_marcar_vendido(Veiculo *v) {
    if (v->status != VEICULO_DISPONIVEL) return false;
    v->status = VEICULO_VENDIDO;
    return true;
}

bool veiculo_remover(Veiculo *v) {
    if (v->status != VEICULO_DISPONIVEL) return false;
    v->status = VEICULO_REMOVIDO;
    return true;
}

bool veiculo_placa_valida(const char *placa) {
    char tmp[PLACA_TAM + 1];
    return normalizar_placa(placa, tmp);
}

/* ---------- ListaVeiculos ---------- */

ListaVeiculos *lista_veiculos_criar(void) {
    return calloc(1, sizeof(ListaVeiculos));
}

void lista_veiculos_destruir(ListaVeiculos *l) {
    if (!l) return;
    colecao_limpar(&l->dados);
    free(l);
}

static int proximo_id(const ListaVeiculos *l) {
    int max = 0;
    for (size_t i = 0; i < l->dados.tamanho; i++)
        if (((Veiculo *)l->dados.itens[i])->id > max) max = ((Veiculo *)l->dados.itens[i])->id;
    return max + 1;
}

bool lista_veiculos_placa_existe(const ListaVeiculos *l, const char *placa) {
    char norm[PLACA_TAM + 1];
    if (!normalizar_placa(placa, norm)) return false;
    for (size_t i = 0; i < l->dados.tamanho; i++) { Veiculo *v = l->dados.itens[i];
        if (v->status != VEICULO_REMOVIDO && strcmp(v->placa, norm) == 0)
            return true;
    }
    return false;
}

Veiculo *lista_veiculos_cadastrar(ListaVeiculos *l, const char *marca,
                                  const char *modelo, int ano, const char *placa,
                                  const char *cor, int km,
                                  double preco_compra, double preco_venda) {
    char norm[PLACA_TAM + 1];
    if (marca[0] == '\0' || modelo[0] == '\0') return NULL;
    if (ano < 1900 || ano > 2100 || km < 0) return NULL;
    if (preco_compra <= 0 || preco_venda <= 0) return NULL;
    if (!normalizar_placa(placa, norm)) return NULL;
    if (lista_veiculos_placa_existe(l, norm)) return NULL;

    Veiculo *v = calloc(1, sizeof *v);
    if (!v) return NULL;
    v->id = proximo_id(l);
    snprintf(v->marca, sizeof v->marca, "%s", marca);
    snprintf(v->modelo, sizeof v->modelo, "%s", modelo);
    v->ano = ano;
    snprintf(v->placa, sizeof v->placa, "%s", norm);
    snprintf(v->cor, sizeof v->cor, "%s", cor);
    v->km = km;
    v->preco_compra = preco_compra;
    v->preco_venda = preco_venda;
    v->status = VEICULO_DISPONIVEL;
    if (!colecao_adicionar(&l->dados, v, 16)) {
        free(v);
        return NULL;
    }
    return v;
}

Veiculo *lista_veiculos_buscar(const ListaVeiculos *l, int id) {
    for (size_t i = 0; i < l->dados.tamanho; i++) { Veiculo *v = l->dados.itens[i];
        if (v->id == id && v->status != VEICULO_REMOVIDO) return v; }
    return NULL;
}

void lista_veiculos_descricao_por_id(const ListaVeiculos *l, int id,
                                     char *buf, size_t tam) {
    for (size_t i = 0; i < l->dados.tamanho; i++) {
        const Veiculo *v = l->dados.itens[i];
        if (v->id == id) {
            snprintf(buf, tam, "%s %s %d (%s)", v->marca, v->modelo, v->ano, v->placa);
            return;
        }
    }
    snprintf(buf, tam, "?");
}

int lista_veiculos_tamanho(const ListaVeiculos *l) { return (int)l->dados.tamanho; }

Veiculo *lista_veiculos_obter(const ListaVeiculos *l, int indice) {
    if (indice < 0 || (size_t)indice >= l->dados.tamanho) return NULL;
    return l->dados.itens[indice];
}

int lista_veiculos_total_por_status(const ListaVeiculos *l, StatusVeiculo s) {
    int total = 0;
    for (size_t i = 0; i < l->dados.tamanho; i++) if (((Veiculo *)l->dados.itens[i])->status == (int)s) total++;
    return total;
}

double lista_veiculos_valor_investido(const ListaVeiculos *l) {
    double total = 0.0;
    for (size_t i = 0; i < l->dados.tamanho; i++) { Veiculo *v = l->dados.itens[i];
        if (v->status == VEICULO_DISPONIVEL) total += v->preco_compra; }
    return total;
}

double lista_veiculos_valor_potencial(const ListaVeiculos *l) {
    double total = 0.0;
    for (size_t i = 0; i < l->dados.tamanho; i++) { Veiculo *v = l->dados.itens[i];
        if (v->status == VEICULO_DISPONIVEL) total += v->preco_venda; }
    return total;
}

/* ---------- Persistencia ---------- */

bool lista_veiculos_salvar(const ListaVeiculos *l, const char *arquivo) {
    return colecao_salvar(&l->dados, arquivo, sizeof(Veiculo));
}

bool lista_veiculos_carregar(ListaVeiculos *l, const char *arquivo) {
    return colecao_carregar(&l->dados, arquivo, sizeof(Veiculo), 16);
}
