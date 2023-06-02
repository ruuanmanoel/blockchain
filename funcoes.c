#include "funcoes.h"

void printHash(unsigned char hash[], int length)
{
    int i;
    for(i=0;i<length;++i)
        printf("%02x", hash[i]);
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
    if(blockchain == NULL)return;
    PossuiBitcoin *possui = malloc(sizeof(PossuiBitcoin));
    possui->data = endereco;
    if(blockchain->Possui == NULL) possui->prox = NULL;
    else possui->prox = blockchain->Possui;
    blockchain->Possui = possui;
    blockchain->tamListaPossui+=1;
}

void removeEndereco(estatistica *blockchain, unsigned char endereco){
    if(blockchain->Possui == NULL)return;
    PossuiBitcoin *aux = blockchain->Possui;
    PossuiBitcoin *anterior = NULL;
    if (aux != NULL && aux->data == endereco) {
        blockchain->Possui = aux->prox;
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
    blockchain->tamListaPossui--;
    free(aux);
    
}
int procuraEndereco(estatistica *blockchain,unsigned char endereco){
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
    int enderecoEncontrado = 0;

    while (enderecoEncontrado != endereco)
    {
        aux = aux->prox;
        enderecoEncontrado++;
    }
    return aux->data;
}

void gerarBloco(estatistica *blockchain, CarteiraSistema *carteira){
    //inicializa os valores aleatores e o numero do bloco;
    MTRand rand = seedRand(1234567);
    blockchain->monitoraCarteira = carteira;
    for (size_t i = 1; i <= TOTAL_BLOCOS; i++){
        //definação das variaveveis atribui o numero do bloco se for maior que 1 ele copia o hash do bloco anterior
        BlocoNaoMinerado blN;
        blN.numero = i;
        iniciarData(&blN);
        if(blN.numero == 1){
            iniciaGenesis(&blN, blockchain,&rand);
            continue;      
        }else{
            memcpy(blN.hashAnterior, blockchain->BlocoMinerado->hash, SHA256_DIGEST_LENGTH);
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
    imprimeBloco(blockchain);

}

void realizarTransacao(BlocoNaoMinerado *blN, estatistica *blockchain, MTRand *rand){
   
    unsigned int *carteira = blockchain->monitoraCarteira->endereco;
    unsigned char numero_transacao = genRandLong(rand) % MAX_TRANSACAO;
    if(blockchain->maior_transacao < numero_transacao) blockchain->maior_transacao = numero_transacao;
    for(unsigned char i = 0; i < numero_transacao; i++){
        unsigned char sortear_endereco = genRandLong(rand) % blockchain->tamListaPossui;
        unsigned char endereco_origem =  buscaEndereco(blockchain,sortear_endereco);
        unsigned char endereco_destino = endereco_origem;
        while (endereco_origem == endereco_destino)endereco_destino = genRandLong(rand) % NUM_ENDERECO;
        unsigned int valor = *(carteira+endereco_origem)+1 == 0 ? 0 : 
            genRandLong(rand) % (*(carteira+endereco_origem)+1);
        blN->data[i*3] = endereco_origem;
        blN->data[i*3+1] = endereco_destino;
        blN->data[i*3+2] = valor;
        *(carteira+endereco_origem) -=valor;
        blockchain->numero_medio_bitcoin +=valor;
    }
    
    blN->data[183] = genRandLong(rand) % NUM_ENDERECO;
    minerar(blN, blockchain);
    recompensa(blockchain);
    atualizarCarteira(blockchain,numero_transacao);
    imprimeBloco(blockchain);
     
    
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
    //printHash(hash, SHA256_DIGEST_LENGTH);
    CriarBlocoMinerado(blN,blockchain,hash);
}
void CriarBlocoMinerado(BlocoNaoMinerado *blN, estatistica *blockchain, unsigned char *hash){
        if(blockchain == NULL) return;
        BlocoMinerado *bl = malloc(sizeof(BlocoMinerado));
        if(bl == NULL) return;
        bl->bloco = *blN;
        memcpy(bl->hash,hash, SHA256_DIGEST_LENGTH);
        if(blockchain->BlocoMinerado == NULL){
            bl->prox =  bl;
        }else{
            bl->prox = blockchain->BlocoMinerado->prox;
            blockchain->BlocoMinerado->prox = bl;
        }
        blockchain->BlocoMinerado = bl;
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
    blockchain->minerou_mais_bloco[minerador]++;
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
        if(valor>0){
            *(carteira + endereco_destino) += valor;
            //if(procuraEndereco(blockchain,endereco_destino)==1)adicionaEndereco(blockchain,endereco_destino);
        }
    }
    for (int i = 0; i < NUM_ENDERECO; i++)
    {
        if(*(carteira + i) > 0 ){
            if(procuraEndereco(blockchain, i) ==1) adicionaEndereco(blockchain,i);
        }
    }
    
    for(int i =0; i<NUM_ENDERECO; i++){
        if(*(carteira + i) <=0 )removeEndereco(blockchain,i);
    }
    //imprimeLista(blockchain);
    
    
}
int contaTransacao(BlocoMinerado bl){
    unsigned int contador =0;
    for (int i = 1; i < MAX_TRANSACAO; i++)
    {
        
        contador++;
        if(CALCULA_TRANSACAO){ break;}
        

    }
    return contador;
}
estatistica* criaBlockchain(){
    estatistica *blockchain = malloc(sizeof(estatistica));
    if(blockchain)
        return blockchain;
    return NULL;
    
}
void imprimeBloco(estatistica *blockchain){
    BlocoMinerado *bl = blockchain->BlocoMinerado;
    printf("\n----------------------------------------\n");
    printf("Bloco #0%d\n", bl->bloco.numero);
    printf("Nonce #%d\n", bl->bloco.nonce);
    printf("Hash anterior ");
    printHash(bl->bloco.hashAnterior,SHA256_DIGEST_LENGTH);
    printf("Hash Atual ");
    printHash(bl->hash,SHA256_DIGEST_LENGTH);
    printf("Minerador #%d\n", bl->bloco.data[183]);
    printf("TESTANDO %d\n", contaTransacao(*bl));
    printf("Numero de transacao %d\n", blockchain->maior_transacao);
    printf("\n----------------------------------------\n");
    
}
void imprimeBlockchain(estatistica *blockchain){
    if (blockchain == NULL) return;
    BlocoMinerado *aux = blockchain->BlocoMinerado;
    while (aux != NULL)
    {
        printf("%d\n", aux->bloco.numero);
        aux = aux->prox;
    }
    
}
void imprimeLista(estatistica *blockchain){
    if(blockchain == NULL) return;
    PossuiBitcoin *aux=blockchain->Possui;
    int contador =1;
    while (aux != NULL)
    {
        printf("CONTADOR %d ", contador);
        printf("%d\n", aux->data);
        aux = aux->prox;
        contador++;
    }
    
}

void imprimeMaisBitcoin(estatistica blockchain){
    //anda apenas pela lista das pessoas que tem bitcoin e imprime o maior valor junto com os endereçoes
    PossuiBitcoin *endereco_com_bitcoin = blockchain.Possui;
    unsigned int *carteira = blockchain.monitoraCarteira->endereco;
    unsigned int maior_valor = *(blockchain.monitoraCarteira->endereco);

    while (endereco_com_bitcoin)
    {
        if(*(carteira + endereco_com_bitcoin->data) > maior_valor)
            maior_valor = *(carteira + endereco_com_bitcoin->data);
        
       endereco_com_bitcoin = endereco_com_bitcoin->prox;
    }
    
    endereco_com_bitcoin = blockchain.Possui;
    printf("\n------------------MAIOR VALOR--------------------");
    printf("\nO maior valor eh %d:", maior_valor);
    while (endereco_com_bitcoin)
    {
        if(*(carteira + endereco_com_bitcoin->data) == maior_valor){
            printf("\nO(s) endereco(s) com maior bitcoin eh/sao:%d", 
                endereco_com_bitcoin->data);
        }
            endereco_com_bitcoin = endereco_com_bitcoin->prox;

    }
    printf("\n------------------FIM-----------------------------\n");
}
void imprimeMaisMinerou(estatistica blockchain){
    
    unsigned int maior_quantidade = 0;
    for(int i =0; i<NUM_ENDERECO; i++){
        if(blockchain.minerou_mais_bloco[i]> maior_quantidade)
            maior_quantidade = blockchain.minerou_mais_bloco[i];
    } 
    printf("--------------OS MAIORES MINERADORES--------------\n");
    for(int i = 0; i<NUM_ENDERECO; i++){
        if(maior_quantidade == blockchain.minerou_mais_bloco[i]){
            printf("O(s) endereco(s) que mais minerou foi:%d \n", i);
        }
    }
    printf("O maior numero de mineracao eh: %d", maior_quantidade);
    printf("\n------------------FIM----------------------------\n");

}
void imprimeBlocoMaiorTransacao(estatistica blockchain){
    BlocoMinerado *bl = blockchain.BlocoMinerado->prox;
    printf("\n-----------BLOCO COM MAIOR TRANSACAO------------\n");
    printf("A maior transacao eh: %d\n", blockchain.maior_transacao);
    for(int i =0; i < TOTAL_BLOCOS; i++){
        if(blockchain.maior_transacao == contaTransacao(*bl)){
            printHash(bl->hash, SHA256_DIGEST_LENGTH);
        }
        bl = bl->prox;
    }
    printf("\n-------FIM DO BLOCO COM MAIOR TRANSACAO--------\n");

    
}
void imprimeBlocoMenorTransacao(estatistica blockchain){
    //esse proximo é para atribuir o inicio da lista circular
    BlocoMinerado *bl = blockchain.BlocoMinerado->prox;
    printf("\n-----------BLOCO COM MAIOR TRANSACAO------------\n");
    printf("A maior transacao eh: %d\n", blockchain.maior_transacao);
    for(int i =0; i < TOTAL_BLOCOS; i++){
        if(blockchain.maior_transacao == contaTransacao(*bl)){
            printHash(bl->hash, SHA256_DIGEST_LENGTH);
        }
        bl = bl->prox;
    }
    printf("\n-------FIM DO BLOCO COM MAIOR TRANSACAO--------\n");

    
}
void mediaBitcoin(estatistica blockchain){
    printf("%.2f", (blockchain.numero_medio_bitcoin / (TOTAL_BLOCOS - 1)));
}
void inicializaBlockchain(estatistica *blockchain){
    blockchain->BlocoMinerado = NULL;
    blockchain->tamListaPossui = 0;
    blockchain->Possui = NULL;
    blockchain->monitoraCarteira = NULL;
    blockchain->numero_medio_bitcoin = 0;
    memset(blockchain->minerou_mais_bloco, 0, sizeof(blockchain->minerou_mais_bloco));
}
void liberaBlockchain(estatistica *blockchain){
    if(blockchain == NULL) return;

    BlocoMinerado *aux_blN = blockchain->BlocoMinerado;
    do
    {
        aux_blN = blockchain->BlocoMinerado->prox;
        blockchain->BlocoMinerado->prox = aux_blN->prox;
        free(aux_blN);

    } while (aux_blN != blockchain->BlocoMinerado);
    /* free(blockchain->BlocoMinerado); */
    PossuiBitcoin *aux_possui = blockchain->Possui;
    while (aux_possui != NULL)
    {
        blockchain->Possui = blockchain->Possui->prox;
        free(aux_possui);
        aux_possui = blockchain->Possui;
    }
    free(blockchain);   
    blockchain = NULL;
}