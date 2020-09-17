/**********
Bingo Game - FM32 2016
***********/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "websocket.h"

pthread_t tid[2];
#define  MAXPOINTS  120 /** Quantidade numeros sorteados possiveis  **/
#define  CARD_NUMS 24  /** define a quandidade  de numeros nas  cartelas do jogadores */
#define  INTERVAL_TIME_S 20 /** Tempo de intervalo para o Sorteio **/
#define  INTERVAL_TIME_I 10 /** Temjpo de itervalo para o ebiio de informaçõs statisticas */
int NumerosSorteados[120]; /** Nuemros sorteados  **/

/***
Essa função sortea numeros Aleatorios para o bingo
**/
int  SortearNumero(){
	int i,newNumero;
	srand(time(NULL));

	do{
		newNumero = (rand()%MAXPOINTS);
	}while(NumerosSorteados[newNumero]);
	NumerosSorteados[newNumero]=1;
	return newNumero;
}


/****
Essa função verifica se  o jogador  é vencedor
****/
int checkarGanhador(ws_client *n  ){
	int s=0,c=0,m=0,comb=0;
	if(n!=NULL){
		while(NumerosSorteados[s]){
			while(n->cardNums[c]){
				while(n->selectedNums[m]){
					if((NumerosSorteados[s]==n->cardNums[c]) && (NumerosSorteados[s]==n->selectedNums[m])) comb++;
					m++;
				}
				c++;
			}
			s++;
		}
		if(comb==CARD_NUMS)
			return 1;
		else	return 0 ;
	}
}

/***
Rebeber Menssagens
***/
void receberMenssagem(char **message, ws_client *n){

	char *type = message[0];
	char *contents = message[1];

	if(!strcmp("C",type)){
		printf("Recebendo e guardando numeros (%s)...\n",contents);

		char **numeros =(char**) str_split(contents,",");
		int num = 0;

		while(numeros[num] != NULL){

			n->cardNums[num]  = atoi(numeros[num]);
			num++;
		}

		printf("Nuemros Recebido com sucesso\n");
	}else if(!strcmp("S",type)){

		printf("Usuario selecionou o numero %s!\n",contents);

		int num = 0;

		while(n->selectedNums[num] != 0 ) num++ ;
		n->selectedNums[num] = atoi(contents);

		printf("Numero guardado com sucesso!\n");

	}else  if(!strcmp("B",type)){
		printf("Usuario Gritou Bingo\n");

		if(checkarGanhador(n)){
			printf("Usario Ganhou\n");
			char msg[] = "YW-YW";
			sendMsgUnicast(msg, n);
		}else{
			printf("Usuario Perdeu\n");
			char msg[] = "YL-YL";
			sendMsgUnicast(msg, n);
		}

	}else if(!strcmp("NC",type)){
		int i=0;

		while(NumerosSorteados[i]) i++;

		if(i<(MAXPOINTS/2)){

			char msg[] = "NG-NG";
			sendMsgUnicast(msg, n);
		}
	}

}


/****
Essa função faz  o sorteio e verifica se tem vencedores
****/
void FazerSorteios(){
	/*pthread_t id = pthread_self();*/
	int index,NumeroSorteado;
	char msg[sizeof(int)];

	printf("Inciando  Sorteio... \r\n");

	for(index=0;index<MAXPOINTS;index++){
		sleep(INTERVAL_TIME_S);

		NumeroSorteado =SortearNumero();

		printf("Sorteado numero %i! \n",NumeroSorteado);
		sprintf(msg,"N-%i",NumeroSorteado);
		pthread_create(&pthread_id, &pthread_attr  ,  &sendMsgBroadcast, (void*) msg);

	}
	printf("Fim do Sorteio! \r\n");

	/*pthread_exit((void  *) EXIT_SUCCESS);*/
}
/***
Essa  função retorna o numero de ganhadores matematicamente possivel na lista de  jogadores
***/
int ganhadoresPossiveis(){

	ws_client  *n =  l->first;
	int comb =0,l =0,c = 0,qtd=0 ;/** comb= Combinacoes , l= indice numeros sorteados, c = indice numeros da cartela, qtd = quatidades de ganhadres*/
	while( n !=  NULL){
		while(NumerosSorteados[l]){
			while(n->cardNums[c]){
				if(n->cardNums[c] == NumerosSorteados[l]) comb++;
				c++;
			}
			l++;
		}
		if(comb==CARD_NUMS)  qtd++;
		n = n->next;
		comb=0;
	}
	return qtd;
}

/****
Enviar informações Estatiticas sobre o jogo
****/
void * enviar_informacoes(void *args){
	char msg[sizeof(char*)];
	int users_online,ganhadores_possiveis  ;
	while(1){
		sleep(INTERVAL_TIME_I);
		users_online = l->len;
		ganhadores_possiveis = ganhadoresPossiveis();
		sprintf(msg,"I-%i,%i",users_online,ganhadores_possiveis);
		pthread_create(&pthread_id, &pthread_attr  ,  &sendMsgBroadcast, (void*) msg);
	}
}



void   main(){
	int err;

	/*Iniciar Servidor */
	err = pthread_create(&tid[0],NULL, &init_websocket, (void*)  32); 

	if(err!=0)  printf("ERRo! %s", strerror(err));

	/***Mandar informaçoes para todos usuarios***/
	err  = pthread_create(&tid[1],NULL,&enviar_informacoes,NULL);

	if(err!=0)  printf("ERRo! %s", strerror(err));


	while(1){
		/**Iniciar Novo Jogo ***/

		char *msg = "NG-NG";

		err = pthread_create(&pthread_id, &pthread_attr  ,  &sendMsgBroadcast, (void*) msg);

		if(err!=0)  printf("ERRo! %s", strerror(err));

		/*Fazer Sorteios dos numeros */
		FazerSorteios();

		sleep(5*60);

	}



}
