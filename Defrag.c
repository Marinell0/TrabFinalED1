void defrag(ptnoSet *Area, ptnoArq *Arq, memoria Memo) {
	ptnoArq ArqAux = *arq;
	ptnoSet AreaAux = *Area
	int qtdSec = 0;
	
	while(ArqAux->prox || AreaAux->prox){
		ptno Percorre = *arq;
		while(Percorre){
			while(Percorre->setor) {
				
				Percorre->setor = Percorre->setor->prox;
			}
			Percorre = Percorre->prox;
		}
		while((AreaAux->fim != AreaAux->inicio) && ArqAux->setor->prox){
			if(ArqAux -> setor -> inicio > qtdSec){
				char aux[3];
				int i;
				for(i=0; i<TAM_GRANULO; i++){
					aux[i] = Memo[ArqAux->setor->inicio][i];
					Memo[ArqAux->setor->inicio][i] = Memo[qtdSec][i];
					Memo[qtdSec][i] = aux[i];
				}
				(ArqAux->setor->inicio)++;
				qtdSec++;
			}
		}
	}
}
