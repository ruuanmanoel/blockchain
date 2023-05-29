#include "funcoes.h"

int main()
{
  estatistica *blockchain = malloc(sizeof(estatistica));
  blockchain->BlocoMinerado=NULL;
  CarteiraSistema carteira;
  iniciarCarteira(&carteira);
  gerarBloco(blockchain,&carteira);  
}
