#include "openssl/crypto.h" //arquivo de definição necessário para SHA256_DIGEST_LENGTH
#include "openssl/sha.h" 
#include "mtwister.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_ENDERECO 256
#define TOTAL_BLOCO 10
#define TAM_DATA 184
#define RECOMPENSA 50
#define MAX_TRANSACAO 61
#define INDICE_MINERADOR 183
#define CALCULA_TRANSACAO bl.bloco.data[i* 3 ] ==0 && bl.bloco.data[i*3+1]  ==0 &&bl.bloco.data[i*3+2]  ==0

typedef struct 
{
  unsigned int numero;
  unsigned int nonce;
  unsigned char data[184];
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}BlocoNaoMinerado;

typedef struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH];
  struct BlocoMinerado *prox;
} BlocoMinerado;


typedef struct{
  unsigned int endereco[256];
}CarteiraSistema;

typedef struct PessoaBitcoin{
  unsigned char chave;
  struct PessoaBitcoin *prox;
}PessoaBitcoin;

typedef struct 
{
  unsigned int minerou[NUM_ENDERECO];
  BlocoMinerado *bloco_minerado;
  PessoaBitcoin *pessoa_bitcoin;
  unsigned char transacao_atual;
  unsigned char maior_transacao;
  unsigned char menor_transacao;
  float media_bitcoins;
  unsigned char tam_pessoa_bitcoin;
}Controller;

/* MENU */
void menu();

/* PROTOTIPO DE FUNÇÕES */
Controller *criaCotroller();
void inicializaCarteira(CarteiraSistema *carteira);
void inicializaController(Controller *controle);
void liberaController(Controller *controle);
void iniciarBlockchain(Controller *controle,CarteiraSistema *carteira,MTRand *rand);
void iniciaGenesis(BlocoNaoMinerado *bloco_nao_minerado,MTRand *rand);
void minerar(Controller *controle, BlocoNaoMinerado *bloco_nao_minerado);
void gerarBloco(Controller *controle, BlocoNaoMinerado *bloco_nao_minerado,unsigned char *hash);
void recompensa(Controller *controle, CarteiraSistema *carteira);
void realizarTransacao(Controller *controle, CarteiraSistema *carteira, BlocoNaoMinerado *bloco_nao_minerado, MTRand *rand);
void atualizaCarteira(Controller *controle, CarteiraSistema *carteira);

/* PROTOTIPO INTERAÇÃO COM LISTA */
PessoaBitcoin* buscaEndereco(Controller *controle,unsigned char chave);
void adicionaPessoaBitcoin(Controller *controle,unsigned char chave);
unsigned char retornaEndereco(Controller *controle, unsigned char indice);
void removeEndereco(Controller *controle, unsigned char chave);

/* PROTOTIPO IMPRESSAO */
void printHash(unsigned char hash[], int length);
void imprimeBloco(BlocoMinerado bl);
void imprimePossuiBitcoin(Controller controle);


/* FUNÇÕES ESPECIAIS DO MENU */
void pessoaMaisBitcoin(CarteiraSistema carteira);
int contaTransacao(BlocoMinerado bl);
void maiorMinerador(Controller controle);
void blocoMaisTransacao(Controller controle);
void blocoMenosTransacao(Controller controle);
void blocoMediaBitcoin(Controller Controller);
void imprimirBloco(Controller controle, unsigned int numero);
void ImprimirNBlocoMinerador();
void nBlocosOrdenados();
void imprimirBlocoPorNonce();