#include "funcoes.h"

void printHash(unsigned char hash[], int length)
{
  int i;
  for(i=0;i<length;++i)
    printf("%02x", hash[i]);
  printf("\n");

  printf("\n");
}
void iniciarCarteira(CarteiraSistema *carteira){
    if(carteira == NULL)return;
    for(int i=0; i<NUM_ENDERECO; ++i){
        carteira->endereco[i] = 0;
        
    }
}
void IniciarTransacao(BlocoNaoMinerado *blN,MTRand *rand , estatistica *blockchain){
    iniciarData(blN);
    CarteiraSistema carteiraTemporaria[255];
    blN->data[183] = genRandLong(rand) % NUM_ENDERECO;
    printf("ele possui %d bitcoins", blockchain->monitoraCarteira->endereco[140]);
   if(blN->numero > 1){
        unsigned char numeroTransacao = genRandLong(rand) % MAX_TRANSACAO;
        for(int i=0; i<numeroTransacao; i++){
            unsigned char sorteandoEndereco = genRandLong(rand) % blockchain->tamListaPossui;
            unsigned char enderecoOrigem = buscaEndereco(sorteandoEndereco,blockchain);
            unsigned char enderecoDestino =  enderecoOrigem;
            while (enderecoDestino == enderecoOrigem) enderecoDestino = genRandLong(rand) % NUM_ENDERECO;
            //abaixo crio uma função para me retornar o numero exato do endereço na carteira
            unsigned int valor = genRandLong(rand) % *((blockchain->monitoraCarteira->endereco)+enderecoOrigem)-1;
            //blN->data[i+0] = enderecoOrigem;
            //blN->data[i+1] = enderecoDestino;
            //blN->data[i+2] = valor;
        }

   }else{
       iniciaGenesis(blN);
   }
    minerar(blN,blockchain);
    Recompensa(blockchain);
}
void iniciarData(BlocoNaoMinerado *blN){
    for(size_t i=0; i<MAX_DATA;i++){
        blN->data[i] = 0;
    }
}
void iniciaGenesis(BlocoNaoMinerado *blN){
    const char *mensagem = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    memcpy(blN->data,mensagem,70);
    for (int i=0; i< SHA256_DIGEST_LENGTH; ++i){
        blN->hashAnterior[i] = 0;
    }
}
void gerarBloco(estatistica *blockchain, CarteiraSistema *carteira){
    //inicializa os valores aleatores e o numero do bloco;
    MTRand rand = seedRand(1234567);
    unsigned int numero = 1;
    blockchain->monitoraCarteira = carteira;
    for (size_t i = 0; i < TOTAL_BLOCOS; i++) //roda 1000 vezes que é a quantidade de blocos solicitado
    {
        //definação das variaveveis atribui o numero do bloco se for maior que 1 ele copia o hash do bloco anterior
        BlocoNaoMinerado blN;
        blN.numero = numero;
        if(blN.numero != 1){
            memcpy(blN.hashAnterior, blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
            //printHash(blN.hashAnterior, SHA256_DIGEST_LENGTH);
        }     
        //----------fim das atribuições do bloco---------
        IniciarTransacao(&blN,&rand,blockchain);   
        numero++;
    }
}
void minerar(BlocoNaoMinerado *blN, estatistica *blockchain){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int nonce = 0;
    do
    {
        blN->nonce = nonce;
        SHA256((unsigned char*)blN, sizeof(BlocoNaoMinerado), hash);
        nonce++;
    }while(hash[0] != 0);
    printHash(hash, SHA256_DIGEST_LENGTH);
    CriarBlocoMinerado(blN,blockchain,hash);
}
void CriarBlocoMinerado(BlocoNaoMinerado *blN, estatistica *blockchain, unsigned char *hash){
        BlocoMinerado *bl = malloc(sizeof(BlocoMinerado));
        bl->bloco = *blN;
        memcpy(bl->hash,hash, sizeof(unsigned char)*SHA256_DIGEST_LENGTH);
        //printHash(hash, SHA256_DIGEST_LENGTH);
        bl->prox =  blockchain->BlocoMinerado;
        blockchain->BlocoMinerado = bl;
        //printHash(blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
}
void Recompensa(estatistica *blockchain){
    *(blockchain->monitoraCarteira->endereco + blockchain->BlocoMinerado->bloco.data[183]) =50;
    PossuiBitcoin *possui = malloc(sizeof(PossuiBitcoin));
    possui->data = blockchain->BlocoMinerado->bloco.data[183];
    blockchain->tamListaPossui++;
    possui->prox = blockchain->Possui;
    blockchain->Possui = possui;
   /* if(i>0){
    printf("A lista possui %d elementos \nos mineradores %d e %d possui saldo\n", blockchain->tamListaPossui, blockchain->Possui->data, blockchain->Possui->prox->data);
    }
    i++;
    */
}
unsigned char buscaEndereco(unsigned char numero_sorteado, estatistica *blockchain){
    int n = 0;
    PossuiBitcoin *aux = blockchain->Possui;
    while (n != numero_sorteado)
    {
       aux = blockchain->Possui->prox;
    }
    return aux->data;
    
}
void imprimeBlockchain(BlocoMinerado *bloco){
    if (bloco == NULL) return;
    while (bloco)
    {
       printf("%d", bloco->bloco.numero);
       bloco = bloco->prox;
    }   
}
