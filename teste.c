#include "funcoes.h"

int main()
{
  //FALTA LIBERAR OS MALLOC FEITOS
  int escolha;
  estatistica *blockchain = malloc(sizeof(estatistica));
  blockchain->BlocoMinerado=NULL;
  blockchain->Possui=NULL;
  blockchain->tamListaPossui = 0;
  blockchain->numero_medio_bitcoin =0;
  CarteiraSistema carteira;
  iniciarCarteira(&carteira);
  gerarBloco(blockchain,&carteira);  

  switch (escolha)
  {
  case 'a':
    printf("Endereço com mais bitcoin %d com %d bitcoin ");
    break;
  
  default:
    break;
  }
  mediaBitcoin(blockchain);
  imprimeMaisBitcoin(*blockchain);  
  for (size_t i = 0; i < 255; i++)
  {
    if(carteira.endereco[i] > 0){
    printf("endereco %d possxui %d bitcoin: \n ", i, carteira.endereco[i] );

    }
  }
  
  //liberaBlockchain(&blockchain);
}
