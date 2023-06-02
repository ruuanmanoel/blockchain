#include "funcoes.h"

int main()
{
  //FALTA LIBERAR OS MALLOC FEITOS
  int escolha;
  estatistica *blockchain = malloc(sizeof(estatistica));
  blockchain->BlocoMinerado=NULL;
  memset(blockchain->minerou_mais_bloco, 0, sizeof(blockchain->minerou_mais_bloco));
  blockchain->Possui=NULL;
  blockchain->tamListaPossui = 0;
  blockchain->numero_medio_bitcoin =0;
  CarteiraSistema carteira;
  iniciarCarteira(&carteira);
  gerarBloco(blockchain,&carteira);  

  switch (escolha)
  {
    case 'a':
      imprimeMaisBitcoin(*blockchain);
      break;
    case 'b':
      imprimeMaisMinerou(*blockchain);
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
  
  //liberaBlockchain(&blockchain);
}
