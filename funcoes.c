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
void iniciarData(BlocoNaoMinerado *blN){
    for(size_t i=0; i<MAX_DATA;i++){
        blN->data[i] = 0;
    }
}
void adicionaEndereco(estatistica *blockchain, unsigned char endereco){
    PossuiBitcoin *possui = malloc(sizeof(PossuiBitcoin));
    possui->data = endereco;
    blockchain->tamListaPossui++;
    possui->prox = blockchain->Possui;
    blockchain->Possui = possui;
}
void removeEndereco(estatistica *blockchain, unsigned char endereco){
    if(blockchain == NULL)return;
    PossuiBitcoin *aux = blockchain->Possui;
    PossuiBitcoin *anterior = NULL;
    if (aux != NULL && aux->data == endereco) {
        blockchain = aux->prox;
        free(aux);
        return;
    }
    while (aux != NULL && aux->data != endereco )
    {
        anterior = aux;
        aux = aux->prox;
    }
    if (aux == NULL) {
        return;
    }

    // Remove o item da lista
    anterior->prox = aux->prox;
    free(aux);
    
}
unsigned char procuraEndereco(estatistica *blockchain,unsigned char endereco){
    PossuiBitcoin *aux = blockchain->Possui;
    while (aux != NULL)
    {
        if(aux->data == endereco) return 0;
        aux = aux->prox;
    }
    return 1;
    
}
unsigned char buscaEndereco(estatistica *blockchain, unsigned char endereco){
    PossuiBitcoin *aux = blockchain->Possui;
    int n=0;
    while (n != endereco)
    {
        if(aux->data == endereco)  break;
        aux = aux->prox;
    }
    return aux->data;
}

void gerarBloco(estatistica *blockchain, CarteiraSistema *carteira){
    //inicializa os valores aleatores e o numero do bloco;
    MTRand rand = seedRand(1234567);
    blockchain->monitoraCarteira = carteira;
    for (size_t i = 1; i <= TOTAL_BLOCOS; i++) 
    {
        //definação das variaveveis atribui o numero do bloco se for maior que 1 ele copia o hash do bloco anterior
        BlocoNaoMinerado blN;
        blN.numero = i;
        iniciarData(&blN);
        if(blN.numero == 1){
            iniciaGenesis(&blN, blockchain,&rand);
            continue;      
        }else{
            memcpy(blN.hashAnterior, blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
            //printf("\n%d\n%d\n%d\n", blockchain->BlocoMinerado->bloco.numero, blockchain->BlocoMinerado->bloco.nonce,blockchain->BlocoMinerado->bloco.data[183]);
            //printHash(blN.hashAnterior,SHA256_DIGEST_LENGTH);
            realizarTransacao(&blN,blockchain,&rand);
        }
    }
}
void iniciaGenesis(BlocoNaoMinerado *blN,estatistica *blockchain, MTRand *rand){
    const char *mensagem = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    memcpy(blN->data,mensagem,70);
    blN->data[183] = genRandLong(rand) % NUM_ENDERECO;
    for (int i=0; i< SHA256_DIGEST_LENGTH; ++i){
        blN->hashAnterior[i] = 0;
    }
    minerar(blN,blockchain);
    recompensa(blockchain);
}
void realizarTransacao(BlocoNaoMinerado *blN, estatistica *blockchain, MTRand *rand){
    unsigned int *carteira = blockchain->monitoraCarteira->endereco;
    unsigned char tamanho_lista_bitcoin = blockchain->tamListaPossui; 
    unsigned char numero_transacao = genRandLong(rand) % MAX_TRANSACAO;
    for(unsigned char i = 0; i < numero_transacao; i++){
        unsigned char sortear_endereco = genRandLong(rand) % tamanho_lista_bitcoin;
        unsigned char endereco_origem =  buscaEndereco(blockchain,sortear_endereco);
        unsigned char endereco_destino = endereco_origem;
        while (endereco_origem == endereco_destino) endereco_destino = genRandLong(rand) % NUM_ENDERECO;
        unsigned int valor = *(carteira+endereco_origem) == 0 ? 0 : 
            genRandLong(rand) % *(carteira+endereco_origem)+1;
        blN->data[i*3] = endereco_origem;
        blN->data[i*3+1] = endereco_destino;
        blN->data[i*3+2] = valor;
        //printf("ele tem %d na carteira e deu %d\n", *(carteira+endereco_origem),valor);
        *(carteira+endereco_origem) -=valor;
        
    }
    blN->data[183] = genRandLong(rand) %NUM_ENDERECO;
    minerar(blN, blockchain);
    recompensa(blockchain);
    atualizarCarteira(blockchain,numero_transacao);
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
        bl->prox =  blockchain->BlocoMinerado;
        blockchain->BlocoMinerado = bl;
        //printHash(blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
}
void recompensa(estatistica *blockchain){
   //definição de variaveis auxiliares
    unsigned int *carteira = blockchain->monitoraCarteira->endereco;
    unsigned char minerador = blockchain->BlocoMinerado->bloco.data[183];
    //atribuição da recompensa ao minerador
    *(carteira + minerador) += RECOMPENSA;
    //se o minerador não estiver na lista dos que possui bitcoin, adiciona ele.
    if(procuraEndereco(blockchain,minerador) == 1){
        adicionaEndereco(blockchain,minerador);
    }
}
void atualizarCarteira(estatistica *blockchain, unsigned char numeroTransacao){
    unsigned int *carteira = blockchain->monitoraCarteira->endereco;
    unsigned char *data_temporaria = blockchain->BlocoMinerado->bloco.data;
    unsigned char endereco_origem, endereco_destino;
    unsigned int valor;
    for(int i=0; i<numeroTransacao; i++){
        endereco_origem = *(data_temporaria+i*3);
        endereco_destino = *(data_temporaria+i*3+1);
        valor = *(data_temporaria+i*3+2);
        *(carteira + endereco_destino) = valor;
        if(procuraEndereco(blockchain,endereco_destino)==NULL)adicionaEndereco(blockchain,endereco_destino);

        //if(endereco_origem <=0 )removeEndereco();

    }
}
void imprimeBlockchain(BlocoMinerado *bloco){
    if (bloco == NULL) return;
    while (bloco)
    {
       printf("%d", bloco->bloco.numero);
       bloco = bloco->prox;
    }   
}
