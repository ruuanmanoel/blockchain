#include "funcoes.h"

int main()
{
  unsigned char escolha;
  estatistica *blockchain = malloc(sizeof(estatistica));
  inicializaBlockchain(blockchain);
  CarteiraSistema carteira;
  iniciarCarteira(&carteira);
  gerarBloco(blockchain,&carteira);  

  printf("Escolha uma das opções abaixo: ");
  scanf("%c",&escolha);
  switch (escolha)
  {
    case 'a':
      imprimeMaisBitcoin(*blockchain);
      break;
    case 'b':
      imprimeMaisMinerou(*blockchain);
      break;
    case 'c':
      imprimeBlocoMaiorTransacao(*blockchain);
      break;
    case 'e':
      mediaBitcoin(*blockchain);
      break;
  
  default:
    break;
  }
  
  
  /* for (size_t i = 0; i < 255; i++)
  {
    if(carteira.endereco[i] > 0){
    printf("endereco %d possxui %d bitcoin: \n ", i, carteira.endereco[i] );

    }
  } */
  
  liberaBlockchain(blockchain);
}
