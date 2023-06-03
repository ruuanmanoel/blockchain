#include "blockchain.h"

int main(){
    MTRand rand = seedRand(1234567);
    CarteiraSistema carteira;
    unsigned char escolha;
    unsigned int quantidade;
    Controller *controle = criaCotroller();
    inicializaCarteira(&carteira);
    inicializaController(controle);
    iniciarBlockchain(controle,&carteira,&rand);
    
   
        menu();
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
        case 'i':
            printf("Digite o nonce que você quer procurar: ");
            scanf("%d", &quantidade);
            imprimirBlocoPorNonce(*controle, quantidade);
            break;
        default:
            break;
        }
    


    liberaController(controle);
    controle = NULL;
    return 0;
}