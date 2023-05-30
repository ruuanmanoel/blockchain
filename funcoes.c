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
    unsigned int tamanho_lista = blockchain->tamListaPossui;

    PossuiBitcoin *possui = malloc(sizeof(PossuiBitcoin));
    possui->data = endereco;
    tamanho_lista++;
    possui->prox = blockchain->Possui;
    blockchain->Possui = possui;
}
int procuraEndereco(estatistica *blockchain,unsigned char endereco){
    PossuiBitcoin *aux = blockchain->Possui;
    while (aux != NULL)
    {
        if(aux->data == endereco) return 1;
        aux = aux->prox;
    }
    return 0;
    
}

/*void IniciarTransacao(BlocoNaoMinerado *blN,MTRand *rand , estatistica *blockchain){
    iniciarData(blN);
    int teste = 51;
    unsigned char numeroTransacao=0; 
    blN->data[183] = genRandLong(rand) % NUM_ENDERECO;
   if(blN->numero > 1){
        numeroTransacao = genRandLong(rand) % MAX_TRANSACAO;
        for(int i = 0; i < numeroTransacao; i+=3){
            unsigned char sorteandoEndereco = genRandLong(rand) % blockchain->tamListaPossui;
            unsigned char enderecoOrigem = buscaEndereco(sorteandoEndereco,blockchain);
            unsigned char enderecoDestino =  enderecoOrigem;
            while (enderecoDestino == enderecoOrigem) enderecoDestino = genRandLong(rand) % NUM_ENDERECO;
            //abaixo crio uma função para me retornar o numero exato do endereço na carteira
            unsigned int valor = genRandLong(rand) % 1+ *((blockchain->monitoraCarteira->endereco)+enderecoOrigem);
            blN->data[i] = enderecoOrigem;
            //printf("\n endereco de origem %d \n\n\n\n\n endereco de destino %d\n valor enviado %d", (*((blockchain->monitoraCarteira->endereco)+enderecoOrigem))+1, enderecoDestino,valor);
            blockchain->monitoraCarteira->endereco[enderecoOrigem] -= valor;
            blN->data[i+1] = enderecoDestino;
            blN->data[i+2] = valor;
        }

   }else{
       iniciaGenesis(blN);
   }
    minerar(blN,blockchain);
    Recompensa(blockchain, numeroTransacao);

}*/

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
            blN.data[183] = genRandLong(&rand) % NUM_ENDERECO;
        }
        minerar(&blN,blockchain);
        recompensa(blockchain);
            
        //----------fim das atribuições do bloco---------
        //IniciarTransacao(&blN,&rand,blockchain);   
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


void recompensa(estatistica *blockchain){
   //definição de variaveis auxiliares
    unsigned int *carteira = blockchain->monitoraCarteira->endereco;
    unsigned char minerador = blockchain->BlocoMinerado->bloco.data[183];
    //atribuição da recompensa ao minerador
    *(carteira + minerador) += RECOMPENSA;
    //se o minerador não estiver na lista dos que possui bitcoin, adiciona ele.
    if(procuraEndereco(blockchain,minerador) == 0){
        adicionaEndereco(blockchain,minerador);
    }
}
void atualizarCarteira(estatistica *blockchain, unsigned char numeroTransacao){
    if(blockchain->BlocoMinerado->bloco.numero >1){
        for(size_t i = 0; i < numeroTransacao; i+=3){
            *(blockchain->monitoraCarteira->endereco +blockchain->BlocoMinerado->bloco.data[i+1])
                += blockchain->BlocoMinerado->bloco.data[i+2];     
            PossuiBitcoin *possui = malloc(sizeof(PossuiBitcoin));
            possui->data = blockchain->BlocoMinerado->bloco.data[i+1];
            blockchain->tamListaPossui++;
            possui->prox = blockchain->Possui;
            blockchain->Possui = possui;

        }
    }
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
