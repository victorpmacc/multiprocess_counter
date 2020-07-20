#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/types.h> /* define pid_t */
#include <unistd.h> /* fork() */
#include <sys/wait.h> /* define waitpid*/
#define MAX_PROCESSOS 4

int eh_primo(long int num){
  int eh;
  int divisores = 0; // número é primo se possui 2 divisores (ele próprio e 1)
  for(long int i = 1; i <= num; i++){
    if(num % i == 0)
	divisores++;
    if(divisores > 2) // para contornar problema de números muito grandes consumirem muito tempo de execução
    	break;
  }
  if(divisores == 2)
    eh = 1;
  else
    eh = 0;
  return eh;
}

int main() {
	/* CRIAÇÃO DA MEMÓRIA COMPARTILHADA */
  // um vetor com os números a serem analisados e um endereço com a quantidade de primos
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANON;
  int* primos;
  primos = (int*)mmap(NULL, 1000*sizeof(int), protection, visibility, 0, 0);
  int* cont_primos;
  cont_primos = (int*)mmap(NULL, sizeof(int), protection, visibility, 0, 0);

	/* LEITURA DOS NUMEROS ENTRADOS */
  int num;
  int numeros_inseridos = 0;
  char c; /* caractere sendo lido */
  do{
    scanf("%d", &num);
    primos[numeros_inseridos] = num;
    numeros_inseridos++;
  }while(c = getchar() != '\n'); /* a leitura da linha inserida acontece até encontrar a quebra de linha */
  
  	/* PROCESSOS */
  // processos filho criados para cada numero entrado
 pid_t pid;
 int n_processos = 0;
 for(int i = 0; i<numeros_inseridos;i++){
   pid = fork();
   if (pid == 0){ // se estiver no filho, verifica se é primo
     *cont_primos += eh_primo(primos[i]);
     exit(0);
   }else{ // se estiver no pai, contabiliza quantos processos estão ativos
     n_processos++;
     if(n_processos == MAX_PROCESSOS){
     	wait(NULL);
     	n_processos--;
     }
   }
  }  
  
  while(wait(NULL) > 0); // aguarda retorno de todos os processos filho
  
  printf("%d\n", *cont_primos);
  
  return 0;
}
