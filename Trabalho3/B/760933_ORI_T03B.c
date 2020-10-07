/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organizacao e Recuperacao da Informacao
 * Prof. Tiago A. Almeida
 *
 * Trabalho 03B - Hashing com encadeamento
 *
 * RA:
 * Aluno:
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 31
#define TAM_GENERO 2
#define TAM_NASCIMENTO 11
#define TAM_CELULAR 16
#define TAM_VEICULO 31
#define TAM_PLACA 9
#define TAM_DATA 9
#define TAM_HORA 6
#define TAM_TRAJETO 121
#define TAM_VALOR 7
#define TAM_VAGAS 2

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO "**********************EXCLUIR*********************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; /* DD/MM/AAAA */
	char celular[TAM_CELULAR];
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   /* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   /* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; /* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox; //USAR PARA SABER OS RGISTROS QUE VÃO ESTAR DO LADO
} Chave;

/* Estrutura da Tabela Hash */
typedef struct
{
	int tam;
	Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/*Recupera um registro do arquivo de dados*/
Carona recuperar_registro(int rrn);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

/*Retorna o primeiro número primo >= a*/
int prox_primo(int a);

/*Função de Hash*/
short hash(const char *chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Funcoes do menu*/
void ler_entrada(char *registro, Carona *novo);
void cadastrar(Hashtable *tabela);
int alterar(Hashtable tabela);
// void buscar(Hashtable tabela);


int remover(Hashtable *tabela);
void imprimir_tabela(Hashtable tabela);
void liberar_tabela(Hashtable *tabela);

/*Funcoes auxiliares*/
void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable *tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */

int buscar(Hashtable tabela, char chave[]);
void cadastrarComArquivo(Hashtable *tabela, Carona j);
void gerarChave(Carona *novo);
Carona inserirCarona(Carona carona);
void colocarNoArquivo(Carona carona);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	char chave[TAM_PRIMARY_KEY];
	int encontrouPK = 0;

	/* Arquivo */
	int carregarArquivo = nregistros = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo)
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo)
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while (opcao != 6)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
			case 1:
				cadastrar(&tabela);
				break;
			case 2:
				printf(INICIO_ALTERACAO);
				printf("%s", (alterar(tabela)) ? SUCESSO : FALHA);
				break;
			case 3:
				// printf(INICIO_BUSCA);
				// buscar(tabela);
				printf(INICIO_BUSCA);

				scanf("\n%[^\n]s", chave);

				// buscar(tabela);
				encontrouPK = buscar(tabela, chave);

				// printf("encontrouPK: %d\n", encontrouPK);

				if(encontrouPK != (-1))
					exibir_registro(encontrouPK);
				else
					printf(REGISTRO_N_ENCONTRADO);

				break;
			case 4:
				printf(INICIO_EXCLUSAO);
				printf("%s", (remover(&tabela)) ? SUCESSO : FALHA);
				break;
			case 5:
				printf(INICIO_LISTAGEM);
				imprimir_tabela(tabela);
				break;
			case 6:
				liberar_tabela(&tabela);
				break;
			case 7:
				printf(INICIO_ARQUIVO);
				printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
				break;
			default:
				printf(OPCAO_INVALIDA);
				break;
		}
	}
	return 0;
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
	if (rrn < 0)
		return 0;

	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO];

	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.genero);
	printf("%s\n", j.nascimento);
	printf("%s\n", j.celular);
	printf("%s\n", j.veiculo);
	printf("%s\n", j.placa);
	printf("%s\n", j.data);
	printf("%s\n", j.hora);
	printf("%s\n", j.valor);
	printf("%s\n", j.vagas);

	strcpy(trajeto, j.trajeto);

	traj = strtok(trajeto, "|");

	while (traj != NULL)
	{
		printf("%s", traj);
		traj = strtok(NULL, "|");
		if (traj != NULL)
		{
			printf(", ");
		}
	}

	printf("\n");

	return 1;
}

short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn)
{
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[TAM_REGISTRO] = '\0';
	Carona j;

	p = strtok(temp, "@");
	strcpy(j.nome, p);
	p = strtok(NULL, "@");
	strcpy(j.genero, p);
	p = strtok(NULL, "@");
	strcpy(j.nascimento, p);
	p = strtok(NULL, "@");
	strcpy(j.celular, p);
	p = strtok(NULL, "@");
	strcpy(j.veiculo, p);
	p = strtok(NULL, "@");
	strcpy(j.placa, p);
	p = strtok(NULL, "@");
	strcpy(j.trajeto, p);
	p = strtok(NULL, "@");
	strcpy(j.data, p);
	p = strtok(NULL, "@");
	strcpy(j.hora, p);
	p = strtok(NULL, "@");
	strcpy(j.valor, p);
	p = strtok(NULL, "@");
	strcpy(j.vagas, p);

	gerarChave(&j);

	return j;
}

/* Libera a Tabela Hash */
void liberar_tabela(Hashtable *tabela)
{
	Chave *aux1, *aux2, *aux3;
	short i;
	for (i = 0, aux1 = tabela->v[i]; i < tabela->tam; aux1 = tabela->v[++i])
		for (aux2 = aux1; aux2; aux3 = aux2, aux2 = aux2->prox, free(aux3), aux3 = NULL)
			;
	free(tabela->v);
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

void gerarChave(Carona *novo) //GERANDO A CHAVE PRIMÁRIA COM AS COISAS QUE O TRABALHO DIZ PARA PEGAR
{
	novo->pk[0] = novo->nome[0];
	novo->pk[1] = novo->placa[0];
	novo->pk[2] = novo->placa[1];
	novo->pk[3] = novo->placa[2];
	novo->pk[4] = novo->data[0];
	novo->pk[5] = novo->data[1]; //PULEI O [2] PORQUE ERA UMA / DA DATA
	novo->pk[6] = novo->data[3];
	novo->pk[7] = novo->data[4];
	novo->pk[8] = novo->hora[0];
	novo->pk[9] = novo->hora[1];
	novo->pk[10] = '\0';
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

Carona inserirCarona(Carona carona)
{
	// Carona carona;

	scanf("\n%[^\n]s", carona.nome);
	scanf("\n%[^\n]s", carona.genero);
	scanf("\n%[^\n]s", carona.nascimento);
	scanf("\n%[^\n]s", carona.celular);
	scanf("\n%[^\n]s", carona.veiculo);
	scanf("\n%[^\n]s", carona.placa);
	scanf("\n%[^\n]s", carona.trajeto);
	scanf("\n%[^\n]s", carona.data);
	scanf("\n%[^\n]s", carona.hora);
	scanf("\n%[^\n]s", carona.valor);
	scanf("\n%[^\n]s", carona.vagas);

	gerarChave(&carona);
	return (carona);
}



//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//COLOCANDO OS CAMPOS NO ARQUIVO

void colocarNoArquivo(Carona carona)
{
	char extra[256];
	int tamanho;

	sprintf(extra, "%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@%s@", carona.nome, carona.genero, carona.nascimento, carona.celular, carona.veiculo, carona.placa, carona.trajeto, carona.data, carona.hora, carona.valor, carona.vagas);
	tamanho = strlen(extra);

	for(int i = tamanho ; i < 256 ; i++)
		strcat(extra, "#");

	strcat(ARQUIVO, extra);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

short hash(const char *chave, int tam)
{
	int posicao = 0;

	for(int i = 1 ; i <= 8 ; i++)
		posicao = posicao + (i * f(chave[i - 1]));

	posicao = posicao % tam;
	return(posicao);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//PROCURANDO OS PRIMOS
int prox_primo(int tam)
{
	for(int i = 2 ; i < tam ; i++)
		if((tam % i) == 0)
			tam++;

	return(tam);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//IMPRIMIR A TABELA HASH
void imprimir_tabela(Hashtable tabela)
{
	Chave *extra;

	for(int i = 0 ; i < tabela.tam ; i++)
	{
		extra = tabela.v[i];
		printf("[%d]", i); //POSICAO EM QUE ESTAMOS

		for(; extra != NULL ; extra = extra->prox)
			printf(" %s", extra->pk);

		printf("\n");
	}

	return;
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

void criar_tabela(Hashtable *tabela, int tam)
{
	tabela->v = (Chave**)calloc(tam, sizeof(Chave));
	tabela->tam = tam;

	for(int i = 0 ; i < tam ; i++)
		tabela->v[i] = NULL;

	nregistros = 0;
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//BUSCAR UMA PK NA TABELA E RETORNAR UM RRN
int buscar(Hashtable tabela, char chave[])
{
	Chave *extra;
	int posicao;

	posicao = hash(chave, tabela.tam);
	extra = tabela.v[posicao];

	// printf("EXTRA PK %s\n", extra->pk);

	for( ; extra != NULL ; extra = extra->prox)
		if((strcmp(extra->pk, chave) == 0))
			// if(extra->rrn != (-1))
			// {
				// printf("RRN DO BUSCAR: %d\n", tabela.v[i].rrn);
				// printf("PK DO BUSCAR: %d\n", tabela.v);
				return(extra->rrn);
			// }

	return(-1);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//ALTERAR O NUMERO DE VAGAS A PARTIR DA BUSCA DA TABELA
int alterar(Hashtable tabela)
{
	char indicePrimario[TAM_PRIMARY_KEY];
	char novasVagas;
	int localizacao;
	int m;
	int extra2 = 0;
	int encontrouPK = 0;

	scanf("\n%[^\n]s", indicePrimario); //RECEBENDO PRIMARIO QUE QUEREMOS

	encontrouPK = buscar(tabela, indicePrimario);

	// printf("encontrouPK: %d\n", encontrouPK);

	if(encontrouPK == (-1))
	{
		printf(REGISTRO_N_ENCONTRADO);
		return(0);
	}

	scanf("\n%[^\n]s", &novasVagas);

	while(strlen(&novasVagas) != 1) //CHECANDO SE A VAGA ESTA NO INTERVALO CORRETO
	{
		printf(CAMPO_INVALIDO);
		scanf("%s", &novasVagas);
	}

	localizacao = (256 * encontrouPK);

	for(m = localizacao ; (extra2 < 10) ; m++)
	{
		if(ARQUIVO[m] == '@')
			extra2++;
	}

	ARQUIVO[m] = novasVagas;

	return(1);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//REMOVER UM ELEMENTO DA TABELA E DO ARQUIVO COLOCANDO RRN -1
int remover(Hashtable *tabela)
{
	Chave *extra;
	Chave *extra2 = NULL;
	char chave[TAM_PRIMARY_KEY];
	int posicao = 0;

	scanf("\n%[^\n]s", chave);
	posicao = hash(chave, tabela->tam);

	extra = tabela->v[posicao];

	//PERCORRENDO TODOS OS NOS
	// for(; extra != NULL ; extra = extra->prox)
	// {
	// 	if(strcmp(extra->pk, chave) == 0)
	// 	{
	// 		tabela->v[posicao] = extra->prox;
	//
	// 		ARQUIVO[(extra->rrn * 256)] = '*';
	// 		ARQUIVO[(extra->rrn * 256) + 1] = '|';
	//
	// 		// extra->rrn = (-1);
	//
	// 		return(1);
	// 	}
	// 	// else if(strcmp(extra->pk, chave) != 0)
	// 	// 	extra2 = extra;
	// }

	while(extra != NULL)
	{
		if(strcmp(extra->pk, chave) == 0)
		{
			if(extra2 != NULL)
			{
				if(extra->prox != NULL)
					extra2->prox = extra->prox;

				else
					extra2->prox = NULL;

				ARQUIVO[(extra->rrn * 256)] = '*';
				ARQUIVO[(extra->rrn * 256) + 1] = '|';
				return(1);
			}
			else if(extra2 == NULL)
			{
				if(extra->prox != NULL)
					tabela->v[posicao] = extra->prox;

				else
					tabela->v[posicao] = NULL;
			}

			ARQUIVO[(extra->rrn * 256)] = '*';
			ARQUIVO[(extra->rrn * 256) + 1] = '|';
			return(1);
		}

		extra2 = extra;
		extra = extra->prox;
	}

	// ARQUIVO[(extra->rrn * 256)] = '*';
	// ARQUIVO[(extra->rrn * 256) + 1] = '|';
	//
	// extra->rrn = (-1);
	// extra->estado = REMOVIDO;
	// return(1);

	printf(REGISTRO_N_ENCONTRADO);
	return(0);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//PROCUREI NO GEEKS FOR GEEKS COMO FAZER INSERÇÃO ORDENADA E ADAPTEI
void cadastrar(Hashtable *tabela)
{
	Carona j;
	int posicao;
	Chave *extra;
	Chave *noNovo;

	j = inserirCarona(j);
	posicao = hash(j.pk, tabela->tam);
	extra = tabela->v[posicao]; //PEGANDO PK E OUTRAS COISAS

	noNovo = (Chave*)calloc(1, sizeof(Chave));
	strcpy(noNovo->pk, j.pk);
	noNovo->rrn = nregistros;
	noNovo->prox = NULL;

	if(tabela->v[posicao]) //CHECANDO SE JÁ EXISTE A MESMA PK NA POSICAO
		if(buscar(*tabela, j.pk) != (-1))
		{
				// printf("VALOR DO BUSCAR: %d\n", buscar(*tabela, extra->pk) != (-1));
			printf(ERRO_PK_REPETIDA, extra->pk);
			return;
		}

		// for(; extra != NULL ; extra = extra->prox)
		// 	if(strcmp(extra->pk, j.pk) == 0)
		// 	{
		// 		printf(ERRO_PK_REPETIDA, extra->pk);
		// 		return;
		// 	}

	if(tabela->v[posicao] == NULL || strcmp(noNovo->pk, tabela->v[posicao]->pk) <= 0)
	{
		noNovo->prox = tabela->v[posicao];
		tabela->v[posicao] = noNovo;
	}
	else
	{
		extra = tabela->v[posicao];

		while(extra->prox != NULL && strcmp(noNovo->pk, extra->prox->pk) > 0)
			extra = extra->prox;

		noNovo->prox = extra->prox;
		extra->prox = noNovo;
	}

	nregistros++;
	colocarNoArquivo(j);
	printf(REGISTRO_INSERIDO, j.pk);

}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
void carregar_tabela(Hashtable *tabela)
{
	Carona k;
	int extra;

	extra = nregistros;
	for(nregistros = 0 ; nregistros < extra ; nregistros++)
	{
		k = recuperar_registro(nregistros);

		cadastrarComArquivo(tabela, k);
	}
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
void cadastrarComArquivo(Hashtable *tabela, Carona j)
{
	// Carona j;
	int posicao;
	Chave *extra;
	Chave *noNovo;

	// j = inserirCarona(j);
	posicao = hash(j.pk, tabela->tam);
	extra = tabela->v[posicao]; //PEGANDO PK E OUTRAS COISAS

	noNovo = (Chave*)calloc(1, sizeof(Chave));
	strcpy(noNovo->pk, j.pk);
	noNovo->rrn = nregistros;
	noNovo->prox = NULL;

	if(tabela->v[posicao]) //CHECANDO SE JÁ EXISTE A MESMA PK NA POSICAO
		if(buscar(*tabela, j.pk) != (-1))
		{
				// printf("VALOR DO BUSCAR: %d\n", buscar(*tabela, extra->pk) != (-1));
			printf(ERRO_PK_REPETIDA, extra->pk);
			return;
		}

		// for(; extra != NULL ; extra = extra->prox)
		// 	if(strcmp(extra->pk, j.pk) == 0)
		// 	{
		// 		printf(ERRO_PK_REPETIDA, extra->pk);
		// 		return;
		// 	}

	if(tabela->v[posicao] == NULL || strcmp(noNovo->pk, tabela->v[posicao]->pk) <= 0)
	{
		noNovo->prox = tabela->v[posicao];
		tabela->v[posicao] = noNovo;
	}
	else
	{
		extra = tabela->v[posicao];

		while(extra->prox != NULL && strcmp(noNovo->pk, extra->prox->pk) > 0)
			extra = extra->prox;

		noNovo->prox = extra->prox;
		extra->prox = noNovo;
	}

	// nregistros++;
	// colocarNoArquivo(j);
	// printf(REGISTRO_INSERIDO, j.pk);

}
