#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" 
#include "mtwister.h"
#include "string.h"
#include <stdlib.h>

#define NUM_ENDERECO 256
#define TOTAL_BLOCOS 4
#define MAX_TRANSACAO 62
#define MAX_DATA 184
#define RECOMPENSA 50
#define INCREMENTO 1

struct BlocoNaoMinerado
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
};
typedef struct BlocoNaoMinerado BlocoNaoMinerado;

typedef struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  struct BlocoMinerado *prox;
} BlocoMinerado;

typedef struct CarteiraSistema{
  unsigned int endereco[256];
}CarteiraSistema;

//cria uma lista para todas as pessoas que possuem bicotcoin, tamanho dela e uma ponteiro para o proximo nó
typedef struct PossuiBitcoin{
  unsigned char data;
  struct PossuiBitcoin *prox;
}PossuiBitcoin;

typedef struct estatistica
{
  unsigned int minerou_mais_bloco[256];
  float numero_medio_bitcoin;
  unsigned char maior_transacao;
  struct BlocoMinerado *BlocoMinerado;
  struct PossuiBitcoin *Possui;
  CarteiraSistema *monitoraCarteira;
  unsigned char tamListaPossui;
  unsigned char *HashMenosTransacao[SHA256_DIGEST_LENGTH];
} estatistica;

void printHash(unsigned char hash[], int length);
void mediaBitcoin(estatistica blockchain);
void iniciarData(BlocoNaoMinerado *blN);
void iniciaGenesis(BlocoNaoMinerado *blN, estatistica *blockchain, MTRand *rand);
void iniciarCarteira(CarteiraSistema *carteira);
void IniciarTransacao(BlocoNaoMinerado *blN,MTRand *rand, estatistica *blockchain);
void gerarBloco(estatistica *blockchain,CarteiraSistema *carteira);
void imprimeBlockchain(estatistica *blockchain);
void minerar(BlocoNaoMinerado *blN, estatistica *blockchain);
void recompensa(estatistica *blockchain);
void atualizarCarteira(estatistica *blockchain, unsigned char numeroTransacao);
void CriarBlocoMinerado(BlocoNaoMinerado *blN, estatistica *blockchain, unsigned char *hash);
unsigned char procuraEndereco(estatistica *blockchain,unsigned char endereco);
void realizarTransacao(BlocoNaoMinerado *blN, estatistica *blockchain, MTRand *rand);
void adicionaEndereco(estatistica *blockchain, unsigned char endereco);
unsigned char buscaEndereco(estatistica *blockchain, unsigned char endereco);
void removeEndereco(estatistica *blockchain, unsigned char endereco);
void imprimeBloco(estatistica *blockchain);
void imprimeLista(estatistica *blockchain);
void imprimeMaisBitcoin(estatistica blockcahin);
void imprimeBloco(estatistica *blockchain);
void imprimeMaisMinerou(estatistica blockchain);