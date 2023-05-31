#include "funcoes.h"

int main()
{
  estatistica *blockchain = malloc(sizeof(estatistica));
  blockchain->BlocoMinerado=NULL;
  blockchain->Possui=NULL;
  blockchain->tamListaPossui = 0;
  CarteiraSistema carteira;
  iniciarCarteira(&carteira);
  gerarBloco(blockchain,&carteira);  
  free(blockchain);
}
