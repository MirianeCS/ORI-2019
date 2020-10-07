/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organizacao e Recuperacao da Informacao
 * Prof. Tiago A. Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
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

#define POS_OCUPADA "[%d] Ocupado: %s\n"
#define POS_LIVRE "[%d] Livre\n"
#define POS_REMOVIDA "[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE 0
#define OCUPADO 1
#define REMOVIDO 2

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
#define ERRO_TABELA_CHEIA "ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"

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
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct
{
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct
{
	int tam;
	Chave *v;
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

/*Auxiliar para a função de hash*/
short f(char x);

/*Função de Hash*/
short hash(const char *chave, int tam);

/*Funcoes do menu*/
void ler_entrada(char *registro, Carona *novo);
void cadastrar(Hashtable *tabela);
int alterar(Hashtable tabela);
// void buscar(Hashtable tabela);

int buscar(Hashtable tabela, char chave[]);

int remover(Hashtable *tabela);
void imprimir_tabela(Hashtable tabela);
void liberar_tabela(Hashtable *tabela);

/*Funcoes auxiliares*/
void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable *tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
Carona inserirCarona();
void gerarChave(Carona *novo);
void colocarNoArquivo(Carona carona);
void cadastrarComArquivo(Hashtable *tabela, Carona carona);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
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

	char chave[TAM_PRIMARY_KEY];
	int encontrouPK;

	if(carregarArquivo)
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
				if (alterar(tabela))
					printf(SUCESSO);
				else
					printf(FALHA);
				break;
			case 3:
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
				if (remover(&tabela))
					printf(SUCESSO);
				else
					printf(FALHA);
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

void liberar_tabela(Hashtable *tabela)
{
	free(tabela->v);
	tabela->v = NULL;
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

short f(char x)
{
	return (x < 59) ? x - 48 : x - 54;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

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

void cadastrarComArquivo(Hashtable *tabela, Carona j)
{
	int contador = 0;
	int posicao;
	// int encontrouPK = (-1);

	posicao = hash(j.pk, tabela->tam);

	// printf("POSICAO: %d\nTAMANHO: %d\n", posicao, tabela->tam);

	for(int i = posicao ; i < (posicao + tabela->tam) ; i++)
	{
		if(tabela->v[i % tabela->tam].estado == LIVRE)
		{
			// if(inserir(tabela, posicao, j, colisao))

			if(tabela->v[posicao].estado == LIVRE)
			{
				strcpy(tabela->v[posicao].pk, j.pk);
				tabela->v[posicao].estado = OCUPADO;
				tabela->v[posicao].rrn = nregistros;

				// printf("NREGISTROS PRIMEIRO: %d\n", nregistros);

				// nregistros++;
				// colocarNoArquivo(j);
				// printf(REGISTRO_INSERIDO, j.pk, contador);

				return;
			}
			else
			{
				for(int i = posicao ; i < (posicao + tabela->tam) ; i++, contador++)
					if(tabela->v[i % tabela->tam].estado == LIVRE || tabela->v[i % tabela->tam].estado == REMOVIDO)
					{
						strcpy(tabela->v[i % tabela->tam].pk, j.pk);
						tabela->v[i % tabela->tam].estado = OCUPADO;
						tabela->v[i % tabela->tam].rrn = nregistros;

						// printf("NREGISTROS PRIMEIRO: %d\n", nregistros);

						// nregistros++;
						// colocarNoArquivo(j);
						// printf(REGISTRO_INSERIDO, j.pk, contador);

						return;
					}
			}
			printf(ERRO_TABELA_CHEIA);

			return;
		}
		else if(tabela->v[i % tabela->tam].estado == OCUPADO)
			if(strcmp(tabela->v[i % tabela->tam].pk, j.pk) == 0)
				if(tabela->v[i % tabela->tam].rrn != (-1))
				{
					// printf(ERRO_PK_REPETIDA, j.pk);

					return;
				}
	}


}

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
//ALLOCAR A TABELA
void criar_tabela(Hashtable *tabela, int tam)
{
	tabela->v = (Chave*)calloc(tam, sizeof(Chave));
	tabela->tam = tam;

	for(int i = 0 ; i < tam ; i++)
	{
		tabela->v[i].estado = LIVRE;
		tabela->v[i].rrn = (-1);
	}
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//CADASTRAR UMA PK NA TABELA
void cadastrar(Hashtable *tabela)
{
	Carona j;
	int contador = 0;
	int posicao;
	// int encontrouPK = (-1);

	j = inserirCarona(j);
	posicao = hash(j.pk, tabela->tam);

	// printf("POSICAO: %d\nTAMANHO: %d\n", posicao, tabela->tam);

	for(int i = posicao ; i < (posicao + tabela->tam) ; i++)
	{
		if(tabela->v[i % tabela->tam].estado == LIVRE)
		{
			// if(inserir(tabela, posicao, j, colisao))

			if(tabela->v[posicao].estado == LIVRE)
			{
				strcpy(tabela->v[posicao].pk, j.pk);
				tabela->v[posicao].estado = OCUPADO;
				tabela->v[posicao].rrn = nregistros;

				// printf("NREGISTROS PRIMEIRO: %d\n", nregistros);

				nregistros++;
				colocarNoArquivo(j);
				printf(REGISTRO_INSERIDO, j.pk, contador);

				return;
			}
			else
			{
				for(int i = posicao ; i < (posicao + tabela->tam) ; i++, contador++)
					if(tabela->v[i % tabela->tam].estado == LIVRE || tabela->v[i % tabela->tam].estado == REMOVIDO)
					{
						strcpy(tabela->v[i % tabela->tam].pk, j.pk);
						tabela->v[i % tabela->tam].estado = OCUPADO;
						tabela->v[i % tabela->tam].rrn = nregistros;

						// printf("NREGISTROS PRIMEIRO: %d\n", nregistros);

						nregistros++;
						colocarNoArquivo(j);
						printf(REGISTRO_INSERIDO, j.pk, contador);

						return;
					}
			}
			printf(ERRO_TABELA_CHEIA);

			return;
		}
		else if(tabela->v[i % tabela->tam].estado == OCUPADO)
			if(strcmp(tabela->v[i % tabela->tam].pk, j.pk) == 0)
				if(tabela->v[i % tabela->tam].rrn != (-1))
				{
					printf(ERRO_PK_REPETIDA, j.pk);

					return;
				}
	}
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//BUSCAR UMA PK NA TABELA E RETORNAR UM RRN
int buscar(Hashtable tabela, char chave[])
{
	int posicao;

	posicao = hash(chave, tabela.tam);

	for(int i = posicao ; i < (posicao + tabela.tam) ; i++)
		if((strcmp(tabela.v[i % tabela.tam].pk, chave) == 0))
			if(tabela.v[i % tabela.tam].rrn != (-1))
			{
				// printf("RRN DO BUSCAR: %d\n", tabela.v[i].rrn);
				// printf("PK DO BUSCAR: %d\n", tabela.v);
				return(tabela.v[i % tabela.tam].rrn);
			}

	return(-1);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//REMOVER UM ELEMENTO DA TABELA E DO ARQUIVO COLOCANDO RRN -1
int remover(Hashtable *tabela)
{
	char chave[TAM_PRIMARY_KEY];
	// int posicao = 0;

	scanf("\n%[^\n]s", chave);

	//PERCORRENDO TODOS OS NOS
	for(int i = 0 ; i < tabela->tam ; i++)
		if(strcmp(tabela->v[i].pk, chave) == 0)
			if(tabela->v[i].rrn != (-1))
			{
				ARQUIVO[(tabela->v[i].rrn * 256)] = '*';
				ARQUIVO[(tabela->v[i].rrn * 256) + 1] = '|';

				tabela->v[i].rrn = (-1);
				tabela->v[i].estado = REMOVIDO;

				return(1);
			}

	printf(REGISTRO_N_ENCONTRADO);
	return(0);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//IMPRIMIR A TABELA HASH
void imprimir_tabela(Hashtable tabela)
{
	for(int i = 0 ; i < tabela.tam ; i++)
	{
		if(tabela.v[i].estado == LIVRE)
			printf(POS_LIVRE, i);

		else if(tabela.v[i].estado == OCUPADO)
			printf(POS_OCUPADA, i, tabela.v[i].pk);

		else if(tabela.v[i].estado == REMOVIDO)
			printf(POS_REMOVIDA, i);

	}

	return;
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
