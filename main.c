#include "blockchain.h"

int main(){
    MTRand rand = seedRand(1234567);
    CarteiraSistema carteira;
    unsigned char escolha,minerador;
    unsigned int quantidade;
    Controller *controle = criaCotroller();
    inicializaCarteira(&carteira);
    inicializaController(controle);
    iniciarBlockchain(controle,&carteira,&rand);
    
   
    menu();
    do
    {
        printf("Escolha uma opcao: ");
        scanf("%c",&escolha);
        switch (escolha)
        {
        case 'a':
            pessoaMaisBitcoin(carteira);
            break;
        case 'b':
            maiorMinerador(*controle);
            break;
        case 'c':
            blocoMaisTransacao(*controle);
            break;
        case 'd':
            blocoMenosTransacao(*controle);
            break;
        case 'e':
            blocoMediaBitcoin(*controle);
            break;
        case 'f':
            printf("Digite o numero do bloco que voce quer ver: ");
            scanf("%d", &quantidade);
            imprimirBloco(*controle,quantidade);
            break;
        case 'g':
            printf("Digite o endereco do minerador: ");
            scanf("%d", &minerador);
            printf("Agora digite o numero maximo de blocos: ");
            scanf("%d",&quantidade);
            imprimirNBlocoMinerador(*controle, minerador, quantidade);
            break;
        case 'h':
            printf("Escolha o numero de blocos que você quer ordenar e imprimir ");
            scanf("%d", &quantidade);
            nBlocosOrdenados(*controle,quantidade);
            break;
        case 'i':
            printf("Digite o nonce que você quer procurar: ");
            scanf("%c", &quantidade);
            imprimirBlocoPorNonce(*controle, quantidade);
            break;
        case '0':
            EXIT_SUCCESS;
            break;
        default:
            break;
        }
    } while (escolha != '0');


    liberaController(controle);
    controle = NULL;
    return 0;
}