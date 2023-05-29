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
    unsigned int NumeroTransacao = genRandLong(rand) % MAX_TRANSACAO;
    unsigned char EnderecoOrigem = genRandLong(rand) % NUM_ENDERECO;
    unsigned int ValorTransferencia = genRandLong(rand) % blockchain->Possui->tam;

    for (size_t i = 0; i < 184; i++)
    {
        blN->data[i]=i;
        //printf("%d\n",blN->data[i]);
    }   
}
void gerarBloco(estatistica *blockchain, CarteiraSistema *carteira){
    //inicializa os valores aleatores e o numero do bloco;
    MTRand rand = seedRand(1234567);
    unsigned int numero = 1;

    for (size_t i = 0; i < TOTAL_BLOCOS; i++)
    {
        //definação das variaveveis 
        BlocoNaoMinerado blN;
        unsigned char endereco;
        //atribuição dos valores do bloco com condicional do primeiro bloco que nao tem transação
        //e o hash anterior é todo zerado. aqui tambem fica chamada de funções para definir hash e transações
        //do bloco com numero diferente de 1;
        blN.numero = numero;
        if(blN.numero == 1){
            const char *mensagem = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
            for (int i = 0; i < 70; i++)
            {
                blN.data[i] = *(mensagem+i);
            }
            

            for (int i=0; i< SHA256_DIGEST_LENGTH; ++i){
                blN.hashAnterior[i] = 0;
            }
        }else{
            //IniciarTransacao(&blN,&rand,blockchain);
            memcpy(blN.hashAnterior, blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
            printHash(blN.hashAnterior, SHA256_DIGEST_LENGTH);
        }
            
        endereco = blN.data[183] = genRandLong(&rand) % NUM_ENDERECO;
        //fim das atribuições de valores ao bloco
        minerar(&blN, blockchain);

        //printHash(blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
        carteira->endereco[endereco] +=50;      
        numero++;
    }
}
void minerar(BlocoNaoMinerado *blN, estatistica *blockchain){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int nonce = 0;
    do
        {
            blN->nonce = nonce;
            SHA256((unsigned char*)blN, sizeof(blN), hash);
            nonce++;
        } while(hash[0] != 0);
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
void Recompensa(BlocoNaoMinerado *blN, estatistica *blockchain){
   
}

void CopiaHash(){

}
void imprimeBlockchain(BlocoMinerado *bloco){
    if (bloco == NULL) return;
    while (bloco)
    {
       printf("%d", bloco->bloco.numero);
       bloco = bloco->prox;
    }   
}
