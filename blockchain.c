#include "blockchain.h"


void printHash(unsigned char hash[], int length)
{
    int i;
    for(i=0;i<length;++i)
        printf("%02x", hash[i]);
    printf("\n");
}
Controller *criaCotroller(){
    Controller *controle = malloc(sizeof(Controller));
    if(controle == NULL){
        printf("Nãoo foi possivel alocar, tente liberar memória.");
        return NULL;
    }
    return controle;
}
void inicializaCarteira(CarteiraSistema *carteira){
    if(carteira == NULL) return;
    for(int i=0; i < NUM_ENDERECO; i++)carteira->endereco[i] =0;
}
void inicializaController(Controller *controle){
    if(controle == NULL)return;
    controle->pessoa_bitcoin = NULL;
    controle->bloco_minerado = NULL;
    controle->tam_pessoa_bitcoin =0;
    controle->menor_transacao = MAX_TRANSACAO;
    controle->media_bitcoins = 0;
    
}
void liberaController(Controller *controle){
    BlocoMinerado *aux = controle->bloco_minerado;
    PessoaBitcoin *pessoa = controle->pessoa_bitcoin;
    do
    {
        aux = controle->bloco_minerado->prox;
        controle->bloco_minerado->prox = aux->prox;
        free(aux);

    } while (aux != controle->bloco_minerado);
    controle->bloco_minerado=NULL;

    while (pessoa != NULL)
    {
       PessoaBitcoin* temp = pessoa;
        pessoa = pessoa->prox;
        free(temp);
    }
    controle->pessoa_bitcoin = NULL;
    free(controle);
    
}
void inicializaData(BlocoNaoMinerado *bloco){
    if(bloco == NULL)return;
    for(int i=0; i<TAM_DATA; i++) bloco->data[i] =0;
}
void iniciaGenesis(BlocoNaoMinerado *bloco_nao_minerado,MTRand *rand){
    const char *mensagem = "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks";
    memcpy(bloco_nao_minerado->data,mensagem,70);
    for (int i=0; i< SHA256_DIGEST_LENGTH; ++i)bloco_nao_minerado->hashAnterior[i] = 0;
    bloco_nao_minerado->data[INDICE_MINERADOR] = genRandLong(rand) % NUM_ENDERECO;
}
void iniciarBlockchain(Controller *controle,CarteiraSistema *carteira,MTRand *rand){
    int numero = 1;
    for (int i = 0; i < TOTAL_BLOCO; i++)
    {
        BlocoNaoMinerado bloco_nao_minerado;
        bloco_nao_minerado.numero = numero;
        inicializaData(&bloco_nao_minerado);
        if(bloco_nao_minerado.numero == 1){
            iniciaGenesis(&bloco_nao_minerado,rand);
        } else{
            memcpy(bloco_nao_minerado.hashAnterior, controle->bloco_minerado->hash, SHA256_DIGEST_LENGTH);
            realizarTransacao(controle,carteira,&bloco_nao_minerado,rand);
        }
        minerar(controle,&bloco_nao_minerado);
        recompensa(controle, carteira);
        atualizaCarteira(controle,carteira);
        //imprimeBloco(*controle);
        numero++;
    }
    
}
void realizarTransacao(Controller *controle, CarteiraSistema *carteira, BlocoNaoMinerado *bloco_nao_minerado, MTRand *rand){
    unsigned char numero_transacao  = genRandLong(rand) % (MAX_TRANSACAO + 1);
    controle->transacao_atual = numero_transacao;
    if(controle->maior_transacao < numero_transacao) controle->maior_transacao = numero_transacao;
    if(controle->menor_transacao > numero_transacao) controle->menor_transacao = numero_transacao;
    for (unsigned char i = 0; i < numero_transacao; i++)
    {
        unsigned char posicao_endereco_origem = genRandLong(rand) % controle->tam_pessoa_bitcoin;
        unsigned char endereco_origem = retornaEndereco(controle, posicao_endereco_origem);
        unsigned char endereco_destino = endereco_origem;
        while (endereco_destino == endereco_origem) endereco_destino = genRandLong(rand) % NUM_ENDERECO;
        unsigned int valor = genRandLong(rand) % (carteira->endereco[endereco_origem]+1);
        bloco_nao_minerado->data[i*3] = endereco_origem;
        bloco_nao_minerado->data[i*3+1] = endereco_destino;
        bloco_nao_minerado->data[i*3+2] = valor;
        //printf("  Endereco de origem %d, Endereco destino %d, Quantidade %d,CRTEIRA  %d \n",endereco_origem,endereco_destino,valor, carteira->endereco[endereco_origem]);
        carteira->endereco[endereco_origem]-=valor;
        controle->media_bitcoins += valor;

    }
    bloco_nao_minerado->data[INDICE_MINERADOR] = genRandLong(rand) % NUM_ENDERECO;
    
}
void minerar(Controller *controle, BlocoNaoMinerado *bloco_nao_minerado){
    unsigned int nonce =0;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    do
    {
        bloco_nao_minerado->nonce = nonce;
        SHA256((unsigned char*)bloco_nao_minerado, sizeof(BlocoNaoMinerado), hash);
        nonce++;   
    } while (hash[0] != 0);
    gerarBloco(controle, bloco_nao_minerado, hash);
}
void gerarBloco(Controller *controle, BlocoNaoMinerado *bloco_nao_minerado,unsigned char *hash){
    if(controle == NULL || bloco_nao_minerado == NULL) return;
        BlocoMinerado *bl = malloc(sizeof(BlocoMinerado));
        if(bl == NULL) return;
        bl->bloco = *bloco_nao_minerado;
        memcpy(bl->hash,hash, SHA256_DIGEST_LENGTH);
        if(controle->bloco_minerado == NULL){
            bl->prox =  bl;
        }else{
            bl->prox = controle->bloco_minerado->prox;
            controle->bloco_minerado->prox = bl;
        }
        controle->bloco_minerado = bl;
}
void recompensa(Controller *controle, CarteiraSistema *carteira){
    unsigned char minerador = controle->bloco_minerado->bloco.data[183];
    carteira->endereco[minerador] += RECOMPENSA;
    controle->minerou[minerador]++;
}

void atualizaCarteira(Controller *controle, CarteiraSistema *carteira){
    unsigned char endereco_origem, endereco_destino;
    unsigned char *data_temporaria = controle->bloco_minerado->bloco.data;
    unsigned int valor;
    for(unsigned char i=0; i<controle->transacao_atual;i++ ){
        endereco_origem = data_temporaria[i*3];  
        endereco_destino = data_temporaria[i*3+1];
        valor = data_temporaria[i*3+2];
        carteira->endereco[endereco_destino] += valor;
    }
    for(int i=0; i< 256; i++){
        if(carteira->endereco[i]<=0){
            if(buscaEndereco(controle,i)!= NULL)removeEndereco(controle,i);
        }
    }
    for (int i = 0; i <256; i++)
    {
        if(carteira->endereco[i]>0){
            if(buscaEndereco(controle,i)==NULL)adicionaPessoaBitcoin(controle,i);
        }
    }
    

} 

/* INTERAÇÕES COM LISTA */
void adicionaPessoaBitcoin(Controller *controle,unsigned char chave){
    if(controle == NULL) return;
    PessoaBitcoin* pessoa = malloc(sizeof(PessoaBitcoin));
    if(pessoa == NULL){
        printf("Não foi possivel alocar, tente liberar memória.\n");
        return;
    }
    pessoa->chave = chave;
    if (controle->pessoa_bitcoin == NULL) {
        controle->pessoa_bitcoin = pessoa;
        pessoa->prox = NULL;
    } else {
        pessoa->prox = controle->pessoa_bitcoin;
        controle->pessoa_bitcoin = pessoa;
    }
    controle->tam_pessoa_bitcoin++;
}
void removeEndereco(Controller *controle, unsigned char chave){
    /*  */
    if(controle == NULL)return;
    PessoaBitcoin *aux = controle->pessoa_bitcoin;
    PessoaBitcoin *anterior = NULL;
    if (aux != NULL && aux->chave == chave) {
        controle->pessoa_bitcoin = aux->prox;
        free(aux);
        controle->tam_pessoa_bitcoin--;
        return;
    }
    while (aux != NULL && aux->chave != chave)
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
    controle->tam_pessoa_bitcoin--;
    
}
PessoaBitcoin* buscaEndereco(Controller *controle,unsigned char chave){
    PessoaBitcoin *aux = controle->pessoa_bitcoin;
    while (aux !=NULL)
    {   
        if(aux->chave == chave) return aux;
        aux = aux->prox;
    }
    return NULL;
}
unsigned char retornaEndereco(Controller *controle, unsigned char indice){
    /* ao contrario da busca endereco essa funcao retorna, retorna a chave do indice buscado. */
    PessoaBitcoin *pessoa = controle->pessoa_bitcoin;
    for(unsigned char i = 0; i<indice;i++){
        pessoa = pessoa->prox;
    }
    return pessoa->chave;
}

/* IMPRESSAO */
void imprimeBloco(BlocoMinerado bl){

    printf("\n----------------------------------------\n");
    printf("Bloco #0%d\n", bl.bloco.numero);
    printf("Nonce #%d\n", bl.bloco.nonce);
       printf("DATA: ");
    if(bl.bloco.numero!=1){
        for(int i=0; i <MAX_TRANSACAO+1;i++){
           printf("%d", bl.bloco.data[i]);
        }
    }else{
        for(int i=0; i <70;i++){
           printf("%c", bl.bloco.data[i]);
        }
    }
    printf("\nHash anterior ");
    printHash(bl.bloco.hashAnterior,SHA256_DIGEST_LENGTH);
    printf("Hash Atual ");
    printHash(bl.hash,SHA256_DIGEST_LENGTH);
    printf("Numero de Transacoes %d\n", contaTransacao(bl));
    printf("Minerador #%d\n", bl.bloco.data[183]);
    printf("\n----------------------------------------\n"); 
}
void imprimePossuiBitcoin(Controller controle){
    if (controle.pessoa_bitcoin == NULL) {
        printf("A lista está vazia.\n");
        return;
    }
    PessoaBitcoin *pessoa = controle.pessoa_bitcoin;
    while (pessoa != NULL) {
        printf("%d ", pessoa->chave);
        pessoa = pessoa->prox;
    }
    printf("\n");
}

/* FUNCOES ESPECIAIS PARA MENU */
void menu(){
    printf("\n-----------MENU------------\n");
    printf("a) Endereco com mais bitcoin e o numero de bitcoin dele \n");
    printf("b) Endereco que minerou mais blocos\n");
    printf("c) Hash do bloco com mais transações e o numero de transações dele\n");
    printf("d) Hash do bloco com menos transações e o numero de transações dele\n");
    printf("e) Quantidade media de bitcoin por bloco\n");
    printf("f) Imprimir todos os campos de um bloco\n");
    printf("g) Imprimir todos os blocos minerados por um dado endereco\n");
    printf("h) Imprimir todos os campos dos N primeiros bloco\n");
    printf("i) Imprimir todos os campos de um bloco com determinado nonce:\n");
    printf("0 - para sair\n");

    printf("\n-----------Fim DO MENU------------\n");

    
}
void pessoaMaisBitcoin(CarteiraSistema carteira){
    unsigned int maior_valor =0;
    for(int i = 0; i< NUM_ENDERECO; i++){
        if(carteira.endereco[i] > maior_valor){
            maior_valor = carteira.endereco[i];
        } 
    }
    printf("\n-------------O MAIOR VALOR-----------------\n");
    printf("o maior valor eh: %d\n", maior_valor);
    for (int i = 0; i < NUM_ENDERECO; i++)
    {
        if(maior_valor == carteira.endereco[i]){
            printf("os contemplado(s) com esse valor eh %d\n", i);
        }
    }
    printf("\n---------------- FIM  ---------------------\n");
}
void maiorMinerador(Controller controle){
    unsigned int quantidade_mineracao =0;
    for(int i = 0; i< NUM_ENDERECO; i++){
        if(controle.minerou[i] > quantidade_mineracao){
            quantidade_mineracao = controle.minerou[i];
        } 
    }
    printf("\n-------------O MAIOR MINERADOR-----------------\n");
    printf("A maior quantidade de mineracao eh: %d\n",quantidade_mineracao);
    for (int i = 0; i < NUM_ENDERECO; i++)
    {
        if(quantidade_mineracao == controle.minerou[i]){
            printf("os contemplado(s) com essa mineracao eh %d\n", i);
        }
    }
    printf("\n---------------- FIM  ---------------------\n");
}
int contaTransacao(BlocoMinerado bl){
    if(bl.bloco.numero ==1) return 0;
    unsigned int contador =0;
    for (int i = 1; i < MAX_TRANSACAO; i++)
    {
        contador++;
        if(CALCULA_TRANSACAO){ break;}
        
    }
    return contador;
}
void blocoMaisTransacao(Controller controle){
    BlocoMinerado *bl = controle.bloco_minerado;
    printf("\n-------------O MAIOR TRANSACAO-----------------\n");
    printf("A maior transacao eh: %d\n",controle.maior_transacao);
    do
    {   
        if(controle.maior_transacao == contaTransacao(*bl)){
            printf("O hash do(s) bloco eh: ");
            printHash(bl->hash,SHA256_DIGEST_LENGTH);
        }
        bl = bl->prox;
    } while (bl != controle.bloco_minerado);
    
    printf("\n---------------- FIM  ---------------------\n");

}
void blocoMenosTransacao(Controller controle){
    BlocoMinerado *bl = controle.bloco_minerado;
    printf("\n-------------A MENOR TRANSACAO-----------------\n");
    printf("A menor transacao eh: %d\n",controle.menor_transacao);
    do
    {   
        if(controle.menor_transacao == contaTransacao(*bl)){
            printf("O hash do(s) bloco eh: ");
            printHash(bl->hash,SHA256_DIGEST_LENGTH);
        }
        bl = bl->prox;
    } while (bl != controle.bloco_minerado);
    
    printf("\n---------------- FIM  ---------------------\n");

}

void blocoMediaBitcoin(Controller controle){
    printf("\n-------------MEDIA BITCOIN POR BLOCO-----------------\n");
    printf("A media de bitcoins eh: %.2f", controle.media_bitcoins /TOTAL_BLOCO);
    printf("\n---------------------FIM-----------------------------\n");
}
void imprimirBloco(Controller controle, unsigned int numero){
    BlocoMinerado *bl = controle.bloco_minerado;
    for(int i=1; i<=numero; i++){
        bl = bl->prox;
    }
    printf("\n----------------------------------------\n");
    printf("Bloco #0%d\n", bl->bloco.numero);
    printf("Nonce #%d\n", bl->bloco.nonce);
    printf("DATA: ");
    if(bl->bloco.numero!=1){
        for(int i=0; i <MAX_TRANSACAO+1;i++){
           printf("%d", bl->bloco.data[i]);
        }
    }
    printf("\nHash anterior  ");
    printHash(bl->bloco.hashAnterior,SHA256_DIGEST_LENGTH);
    printf("Hash Atual ");
    printHash(bl->hash,SHA256_DIGEST_LENGTH);
    printf("Minerador #%d\n", bl->bloco.data[183]);
    printf("\n----------------------------------------\n"); 
}
void imprimirNBlocoMinerador(Controller controle,unsigned char minerador, unsigned int n_bloco ){
    BlocoMinerado *bl = controle.bloco_minerado;
    unsigned int contador = 0;
    do
    {
        if(bl->bloco.data[INDICE_MINERADOR]==minerador){
            imprimeBloco(*bl);
            contador++;
        }
        bl = bl->prox;
    } while (bl!=controle.bloco_minerado && contador< n_bloco);
}
void nBlocosOrdenados(Controller controle, unsigned int num_blocos){
    BlocoMinerado *bl = controle.bloco_minerado;
    if(num_blocos > TOTAL_BLOCO)return;
    unsigned char vetor[num_blocos], minIndex,temp;
    for(int i= 0; i<num_blocos; i++){
       bl = bl->prox;  
     /*   if(bl->bloco.numero == 1){
        bl = bl->prox;
       }  */
       vetor[i] = contaTransacao(*bl);
       
    }
    for (int i = 0; i < num_blocos - 1; i++) {
        minIndex = i;

        // Encontra o índice do menor elemento no restante do array
        for (int j = i + 1; j < num_blocos; j++) {
            if (vetor[j] < vetor[minIndex]) {
                minIndex = j;
            }
        }

        // Troca o elemento mínimo com o elemento na posição atual
        temp = vetor[i];
        vetor[i] = vetor[minIndex];
        vetor[minIndex] = temp;
    }

    for (size_t i = 0; i < num_blocos; i++)
    {
        bl = controle.bloco_minerado->prox;
/*         if (bl->bloco.numero == 1) bl=bl->prox; */
        
        for (size_t j = 0; j < num_blocos; j++)
        {
            if(vetor[i] == contaTransacao(*bl)){
                imprimeBloco(*bl);
                break;
            }else{
                bl = bl->prox;
            }
        } 
    }
    

}
void imprimirBlocoPorNonce(Controller controle, unsigned int nonce){
    BlocoMinerado *bl = controle.bloco_minerado;
    do
    {
        if(bl->bloco.nonce==nonce){
            imprimeBloco(*bl);
        }
        bl = bl->prox;
    } while (bl!=controle.bloco_minerado);
     
}