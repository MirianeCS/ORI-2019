/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização de Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 760933
 * Aluno: Miriane Cardoso Stefanelli
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 		31
#define TAM_GENERO 		2
#define TAM_NASCIMENTO 	11
#define TAM_CELULAR 	16
#define TAM_VEICULO 	31
#define TAM_PLACA 		9
#define TAM_DATA 		9
#define TAM_HORA 		6
#define TAM_TRAJETO 	121
#define TAM_VALOR 		7
#define TAM_VAGAS 		2
#define TAM_REGISTRO 	256
#define MAX_REGISTROS 	1000
#define MAX_TRAJETOS 	30
#define TAM_ARQUIVO 	(MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 				"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM 			"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 			"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO 			"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO 				"**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO 	"*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO 					"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 						"FALHA AO REALIZAR OPERACAO!\n"

/* Registro da Carona */
typedef struct
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char genero[TAM_GENERO];
	char nascimento[TAM_NASCIMENTO]; 	/* DD/MM/AAAA */
	char celular[TAM_CELULAR]; 			/*(99) 99999-9999 */
	char veiculo[TAM_VEICULO];
	char placa[TAM_PLACA];				/* AAA-9999 */
	char trajeto[TAM_TRAJETO];
	char data[TAM_DATA];   				/* DD/MM/AA, ex: 24/09/19 */
	char hora[TAM_HORA];   				/* HH:MM, ex: 07:30 */
	char valor[TAM_VALOR]; 				/* 999.99, ex: 004.95 */
	char vagas[TAM_VAGAS];
} Carona;

/*----- Registros dos Índices -----*/

/* Struct para índice Primário */
typedef struct primary_index
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Ip;

/* Struct para índice secundário de nome */
typedef struct secundary_index
{
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
} Is;

/* Struct para índice secundário de data */
typedef struct secundary_index_of_date
{
	char pk[TAM_PRIMARY_KEY];
	char data[TAM_DATA];
	// char hora[TAM_HORA];
} Isd;

/* Struct para índice secundário de hora */
typedef struct secundary_index_of_time
{
	char pk[TAM_PRIMARY_KEY];
	char hora[TAM_HORA];
} Ist;

/* Lista ligada usada no índice com lista invertida */
typedef struct linked_list
{
	char pk[TAM_PRIMARY_KEY];
	struct linked_list *prox;
} ll;

/* Struct para lista invertida de trajetos*/
typedef struct reverse_index
{
	char trajeto[TAM_TRAJETO];
	ll *lista;
} Ir;

/* GLOBAL: ARQUIVO DE DADOS */
char ARQUIVO[TAM_ARQUIVO];

//STRUCT NOVA
typedef struct nova_struct
{
	char pk[TAM_PRIMARY_KEY];
	char data[TAM_DATA];
	char hora[TAM_HORA];
} idh;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe a Carona */
int exibir_registro(int rrn);

/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Carona */
Carona recuperar_registro(int rrn);

/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *indice_primario, int *nregistros);

/* Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Gera a chave para determinada struct de carona */
void gerarChave(Carona *novo);

/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj);


/* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES >>> */

Carona inserirCarona(); //DANDO SCANF E COLOCANDO AS COISAS NA NOVA CARONA - USANDO INSERÇÃO DE CADA CARONA
void inserirIndicePrimario(Ip *iprimary, int nregistros, Carona carona);
void inserirIndiceSecundario(Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int nregistros, int *ntraj, Carona carona, idh *idatahora);
void colocarNoArquivo(Carona carona);
int compararDriver(const void *primeiro, const void *segundo);
void ordenaIndiceDriver(Is *idriver, int nregistros);
int compararDate(const void *primeiro, const void *segundo);
int compararDate2(const void *primeiro, const void *segundo);
void ordenaIndiceDate(Isd *idate, int nregistros);
int compararTime(const void *primeiro, const void *segundo);
void ordenaIndiceTime(Ist *itime, int nregistros);
void insereNo(ll **irouteLista, char *pk);
int alterar(Ip *iprimary, int nregistros);
int remover(Ip *iprimary, int nregistros);
void liberarEspaco(int *nregistros);
void criar_secundario(Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int *nregistros, int *ntraj, idh *idatahora);
void listar(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int nregistros, int ntraj, idh *idatahora);
void busca(Ip *iprimary, Isd *idate, Ir *iroute, idh *idatahora, int nregistros, int ntraj);

int compararIndicePrimario2(const void *primeiro, const void *segundo);

int encontrarIndicePrimarioCharIp(const void *pk, const void *iprimary);

void liberarMemoria(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, idh *idatahora, int nregistros, int ntraj);

int compararRoute2(const void *primeiro, const void *segundo); //STRING COM IROUTE

int compararDate3(const void *primeiro, const void *segundo);


//IMPRIMIR ---------------------------------------------
void imprimirIndicePrimario(Ip *iprimary, int nregistros);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
	/* Verifica se há arquivo de dados */
	int carregarArquivo = 0, nregistros = 0, ntraj = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();


	/* === ALOCAÇÃO E CRIAÇÃO DOS ÍNDICES === */


	/* Índice primário */
	Ip *iprimary = (Ip *)malloc(MAX_REGISTROS * sizeof(Ip));
	if (!iprimary)
	{
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, &nregistros);

	/* <<< COMPLETE AQUI A ALOCAÇÃO E CRIAÇÃO DOS INDICES SECUNDÁRIOS >>> */

	Is *idriver = (Is*)malloc(MAX_REGISTROS * sizeof(Is));
	Isd *idate = (Isd*)malloc(MAX_REGISTROS * sizeof(Isd));
	Ist *itime = (Ist*)malloc(MAX_REGISTROS * sizeof(Ist));
	Ir *iroute = (Ir*)malloc(MAX_REGISTROS * sizeof(Ir));
	idh *idatahora = (idh*)malloc(MAX_REGISTROS * sizeof(idh));

	criar_secundario(idriver, idate, itime, iroute, &nregistros, &ntraj, idatahora);

	/* === ROTINA DE EXECUÇÃO DO PROGRAMA === */
	int opcao = 0;
	int repetiu = 0;

	Carona carona;

	while (1)
	{
		// printf("escolha:");  //TIRAR ISSO DAQUI NA HORA DE SUBMETER
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1:
			/* cadastrar */

			/* <<< COLOQUE AQUI A CHAMADA PARA A FUNCAO CADASTRAR >>> */

			carona = inserirCarona();

			for(int i = 0 ; i < nregistros ; i++) //CHECANDO SE JÁ NÃO EXISTE A CHAVE PRIMÁRIA
				if(strcmp(iprimary[i].pk, carona.pk) == 0)
					if(iprimary[i].rrn != (-1))
					{
						printf(ERRO_PK_REPETIDA, iprimary[i].pk);
						repetiu = 1;
					}


			if(!repetiu)
			{
				inserirIndicePrimario(iprimary, nregistros, carona);
				inserirIndiceSecundario(idriver, idate, itime, iroute, nregistros, &ntraj, carona, idatahora);

				nregistros++;

				// imprimirIndicePrimario(iprimary, nregistros); //TIRAR ISSO DAQUI NA HORA DE SUBMETER

				colocarNoArquivo(carona);
			}

			break;

		case 2:
			/* alterar */
			printf(INICIO_ALTERACAO);

			if(alterar(iprimary, nregistros)){
				printf(SUCESSO);
			}
			else
				printf(FALHA);

			break;

		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);

			if(remover(iprimary, nregistros))
				printf(SUCESSO);
			else
				printf(FALHA);

			// imprimirIndicePrimario(iprimary, nregistros); //TIRAR ISSO DAQUI NA HORA DE SUBMETER

			break;

		case 4:
			/* buscar */
			printf(INICIO_BUSCA);

			busca(iprimary, idate, iroute, idatahora, nregistros, ntraj);

			break;

		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);

			listar(iprimary, idriver, idate, itime, iroute, nregistros, ntraj, idatahora);

			break;

		case 6:
			/*	libera espaço */

			/* <<< COLOQUE AQUI A CHAMADA PARA A FUNCAO LIBERAR ESPACO >>> */

			liberarEspaco(&nregistros);
			criar_iprimary(iprimary, &nregistros);
			criar_secundario(idriver, idate, itime, iroute, &nregistros, &ntraj, idatahora);

			// imprimirIndicePrimario(iprimary, nregistros); //TIRAR ISSO DAQUI NA HORA DE SUBMETER

			break;

		case 7:
			/* imprime arquivo de dados */
			printf(INICIO_ARQUIVO);

			if (nregistros == 0)
				printf(ARQUIVO_VAZIO);
			else
				printf("%s\n", ARQUIVO);

			break;

		case 8:
			/*	imprime os índices secundários */
			imprimirSecundario(idriver, iroute, idate, itime, nregistros, ntraj);
			break;

		case 9:
			/*	liberar memória e finalizar o programa */

			/* <<< LIBERE A MEMÓRIA DAS ESTRUTURAS CRIADAS >>> */

			liberarMemoria(iprimary, idriver, idate, itime, iroute, idatahora, nregistros, ntraj);

			return (0);

			break;

		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}

/* ==========================================================================
 * =========================== CÓDIGO DAS FUNÇÕES ===========================
 * ========================================================================== */

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

/* carrega dados do arquivo e retorna
 * a quantidade de registros lidos */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Carona */
Carona recuperar_registro(int rrn)
{
	char temp[257], *p;
	strncpy(temp, ARQUIVO + ((rrn)*TAM_REGISTRO), TAM_REGISTRO);
	temp[256] = '\0';
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

/* ==========================================================================
 * ============================= CRIAR ÍNDICES ==============================
 * ========================================================================== */

/* Imprimir indices secundarios */
void imprimirSecundario(Is *idriver, Ir *iroute, Isd *idate, Ist *itime, int nregistros, int ntraj)
{
	int opPrint = 0;
	ll *aux;
	printf(INICIO_ARQUIVO_SECUNDARIO);
	scanf("%d", &opPrint);
	if (!nregistros)
		printf(ARQUIVO_VAZIO);
	switch (opPrint)
	{
	case 1:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idriver[i].pk, idriver[i].nome);
		}
		break;
	case 2:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", idate[i].pk, idate[i].data);
		}
		break;
	case 3:
		for (int i = 0; i < nregistros; i++)
		{
			printf("%s %s\n", itime[i].pk, itime[i].hora);
		}
		break;
	case 4:
		for (int i = 0; i < ntraj; i++)
		{
			printf("%s", iroute[i].trajeto);
			aux = iroute[i].lista;
			while (aux != NULL)
			{
				printf(" %s", aux->pk);
				aux = aux->prox;
			}
			printf("\n");
		}
		break;
	default:
		printf(OPCAO_INVALIDA);
		break;
	}
}

/* <<< IMPLEMENTE AQUI AS DEMAIS FUNÇÕES >>> */

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

//----------------------------------------------------

Carona inserirCarona()
{
	Carona carona;

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

//============================================================
//============================================================
//COISAS INDICE PRIMARIO

int compararIndicePrimario(const void *primeiro, const void *segundo)
{
	Ip *extra1;
	Ip *extra2;

	extra1 = (Ip*)primeiro;
	extra2 = (Ip*)segundo;

	return(strcmp(extra1->pk, extra2->pk));
}

int compararIndicePrimario2(const void *primeiro, const void *segundo)
{
	idh *extra1;
	idh *extra2;

	extra1 = (idh*)primeiro;
	extra2 = (idh*)segundo;

	return(strcmp(extra1->pk, extra2->pk));
}

void ordenarPrimario(Ip *iprimary, int nregistros)
{
	qsort(iprimary, nregistros, sizeof(Ip), compararIndicePrimario);
}

int encontrarIndicePrimarioCharIp(const void *pk, const void *iprimary)
{
	return strcmp(((char*)pk), ((Ip*)iprimary)->pk);
}


void inserirIndicePrimario(Ip *iprimary, int nregistros, Carona carona)
{
	strcpy(iprimary[nregistros].pk, carona.pk);
	iprimary[nregistros].rrn = nregistros;

	ordenarPrimario(iprimary, nregistros + 1);
}

void inserirIndicePrimarioSemOrdenar(Ip *iprimary, int nregistros, Carona carona)
{
	strcpy(iprimary[nregistros].pk, carona.pk);
	iprimary[nregistros].rrn = nregistros;
}

void imprimirIndicePrimario(Ip *iprimary, int nregistros)
{
	for(int i = 0 ; i < nregistros ; i++)
		printf("Indice primário: %s -> RRN: %d\n", iprimary[i].pk, iprimary[i].rrn);
}

//============================================================
//============================================================
//COISA INDICE SECUNDARIO

int compararDriver(const void *primeiro, const void *segundo)
{
	Is *extra1;
	Is *extra2;

	extra1 = (Is*)primeiro;
	extra2 = (Is*)segundo;

	if(strcmp(extra1->nome, extra2->nome) > 0)
		return(1);

	if(strcmp(extra1->nome, extra2->nome) < 0)
		return(-1);

	return(compararIndicePrimario(extra1, extra2));
}

void ordenaIndiceDriver(Is *idriver, int nregistros)
{
	qsort(idriver, nregistros, sizeof(Is), compararDriver);
}

//============================================================
//============================================================
//FUNCOES PARA ORDENACAO E COMPARACAO DA STRUCT DATE

//USADA PARA COMPARAR DATA -> PK
int compararDate(const void *primeiro, const void *segundo)
{
	Isd *extra1;
	Isd *extra2;
	char *dia1, *mes1, *ano1, copia1[TAM_DATA];
	char *dia2, *mes2, *ano2, copia2[TAM_DATA];

	extra1 = (Isd*)primeiro;
	extra2 = (Isd*)segundo;

	strcpy(copia1, extra1->data);
	strcpy(copia2, extra2->data);

	dia1 = strtok(copia1, "/");
	mes1 = strtok(NULL, "/");
	ano1 = strtok(NULL, "/");

	dia2 = strtok(copia2, "/");
	mes2 = strtok(NULL, "/");
	ano2 = strtok(NULL, "/");

	if(strcmp(ano1, ano2) > 0)
		return(1);
	if(strcmp(ano2, ano1) > 0)
		return(-1);

	if(strcmp(mes1, mes2) > 0)
		return(1);
	if(strcmp(mes2, mes1) > 0)
		return(-1);

	if(strcmp(dia1, dia2) > 0)
		return(1);
	if(strcmp(dia2, dia1) > 0)
		return(-1);

	// if(compararIndicePrimario(extra1, extra2))
	// 	return(1);

	return strcmp(extra1->pk, extra2->pk);

	// return(0);
}

//USADA PARA COMPARAR OS CAMPOS DE DATA -> TEMPO -> PK
int compararDate2(const void *primeiro, const void *segundo)
{
	idh *extra1;
	idh *extra2;
	char *dia1, *mes1, *ano1, copia1[TAM_DATA];
	char *dia2, *mes2, *ano2, copia2[TAM_DATA];

	extra1 = (idh*)primeiro;
	extra2 = (idh*)segundo;

	strcpy(copia1, extra1->data);
	strcpy(copia2, extra2->data);

	dia1 = strtok(copia1, "/");
	mes1 = strtok(NULL, "/");
	ano1 = strtok(NULL, "/");

	dia2 = strtok(copia2, "/");
	mes2 = strtok(NULL, "/");
	ano2 = strtok(NULL, "/");

	if(strcmp(ano1, ano2) > 0)
		return(1);
	if(strcmp(ano2, ano1) > 0)
		return(-1);

	if(strcmp(mes1, mes2) > 0)
		return(1);
	if(strcmp(mes2, mes1) > 0)
		return(-1);

	if(strcmp(dia1, dia2) > 0)
		return(1);
	if(strcmp(dia2, dia1) > 0)
		return(-1);

	if(strcmp(extra1->hora, extra2->hora) > 0)
		return(1);

	if(strcmp(extra1->hora, extra2->hora) < 0)
		return(-1);

	return strcmp(extra1->pk, extra2->pk);
}

int compararDate3(const void *primeiro, const void *segundo) //COMPARAR SEM COMPARAR A HORA
{
	idh *extra1;
	idh *extra2;
	char *dia1, *mes1, *ano1, copia1[TAM_DATA];
	char *dia2, *mes2, *ano2, copia2[TAM_DATA];

	extra1 = (idh*)primeiro;
	extra2 = (idh*)segundo;

	strcpy(copia1, extra1->data);
	strcpy(copia2, extra2->data);

	dia1 = strtok(copia1, "/");
	mes1 = strtok(NULL, "/");
	ano1 = strtok(NULL, "/");

	dia2 = strtok(copia2, "/");
	mes2 = strtok(NULL, "/");
	ano2 = strtok(NULL, "/");

	if(strcmp(ano1, ano2) > 0)
		return(1);
	if(strcmp(ano2, ano1) > 0)
		return(-1);

	if(strcmp(mes1, mes2) > 0)
		return(1);
	if(strcmp(mes2, mes1) > 0)
		return(-1);

	if(strcmp(dia1, dia2) > 0)
		return(1);
	if(strcmp(dia2, dia1) > 0)
		return(-1);

	return strcmp(extra1->pk, extra2->pk);
}

void ordenaIndiceDate(Isd *idate, int nregistros)
{
	qsort(idate, nregistros, sizeof(Isd), compararDate);
}

void ordenaIndiceDate2(idh *idatahora, int nregistros)
{
	qsort(idatahora, nregistros, sizeof(idh), compararDate2);
}

//============================================================
//============================================================
//FUNCOES PARA ORDENACAO E COMPARACAO DA STRUCT TIME

int compararTime(const void *primeiro, const void *segundo)
{
	Ist *extra1;
	Ist *extra2;

	extra1 = (Ist*)primeiro;
	extra2 = (Ist*)segundo;

	if(strcmp(extra1->hora, extra2->hora) > 0)
		return(1);

	if(strcmp(extra1->hora, extra2->hora) < 0)
		return(-1);

	return strcmp(extra1->pk, extra2->pk);
}

void ordenaIndiceTime(Ist *itime, int nregistros)
{
	qsort(itime, nregistros, sizeof(Ist), compararTime);
}

//============================================================
//============================================================
//FUNCOES PARA ORDENACAO E COMPARACAO DA STRUCT ROUTE

int compararRoute(const void *primeiro, const void *segundo)
{
	Ir *extra1;
	Ir *extra2;

	extra1 = (Ir*)primeiro;
	extra2 = (Ir*)segundo;

	if(strcmp(extra1->trajeto, extra2->trajeto) > 0)
		return(1);

	if(strcmp(extra1->trajeto, extra2->trajeto) < 0)
		return(-1);

	return(0);
}

int compararRoute2(const void *primeiro, const void *segundo) //ORDENA POR STRING E IR
{
	char *extra1;
	Ir *extra2;

	extra1 = ((char*)primeiro);
	extra2 = ((Ir*)segundo);

	return (strcmp(extra1, extra2->trajeto));
}

void ordenaIndiceRoute(Ir *iroute, int ntraj)
{
	qsort(iroute, ntraj, sizeof(Ir), compararRoute);
}

//============================================================
//============================================================
//INSERIR INDICE SECUNDARIO ENQUANTO INSERIMOS CADA CARONA

void inserirIndiceSecundario(Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int nregistros, int *ntraj, Carona carona, idh *idatahora)
{
	//INDICE DRIVER
	strcpy(idriver[nregistros].pk, carona.pk);
	strcpy(idriver[nregistros].nome, carona.nome);

	ordenaIndiceDriver(idriver, nregistros + 1);

	//INDICE DATE
	strcpy(idate[nregistros].pk, carona.pk);
	strcpy(idate[nregistros].data, carona.data);

	ordenaIndiceDate(idate, nregistros + 1);

	//INDICE TIME
	strcpy(itime[nregistros].pk, carona.pk);
	strcpy(itime[nregistros].hora, carona.hora);

	ordenaIndiceTime(itime, nregistros + 1);

	//INDICE NOVO

	strcpy(idatahora[nregistros].pk, carona.pk);
	strcpy(idatahora[nregistros].data, carona.data);
	strcpy(idatahora[nregistros].hora, carona.hora);

	ordenaIndiceDate2(idatahora, nregistros + 1);

	//INDICE ROUTE
	Ir *extra;
	char copiaCarona[TAM_TRAJETO];
	char *local;

	strcpy(copiaCarona, carona.trajeto); //COPIANDO PARA NÃO ALTERAR O MEU PRINCIPA

	local = strtok(copiaCarona, "|");

	for(int i = *ntraj ; (i < MAX_TRAJETOS) && local ; i = *ntraj)
	{
		extra = (Ir*)bsearch(local, iroute, (*ntraj), sizeof(Ir), compararRoute2);

		if(extra) //ACHOU UM IGUAL
			insereNo(&extra->lista, carona.pk);
		else
		{
			iroute[i].lista = NULL;
			strcpy(iroute[i].trajeto, local); //NÃO EXISTIA AINDA, ENTÃO COLOQUEI
			insereNo(&iroute[i].lista, carona.pk); //COLOCANDO A PROPRIA PK NA LISTA

			(*ntraj)++;

			ordenaIndiceRoute(iroute, *ntraj);
			i++;
		}

		local = strtok(NULL, "|");
	}

	ordenaIndiceRoute(iroute, *ntraj);
}

//============================================================
//============================================================
//COPIA DA FUNCAO PARA QUANDO FOR CRIAR OS INDICES E NÃO PRECISA ORDERNAR ENQUANTO VAI COLOCANDO CADA INDICE

void inserirIndiceSecundarioSemOrdenar(Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int nregistros, int *ntraj, Carona carona, idh *idatahora)
{
	//INDICE DRIVER
	strcpy(idriver[nregistros].pk, carona.pk);
	strcpy(idriver[nregistros].nome, carona.nome);

	//INDICE DATE
	strcpy(idate[nregistros].pk, carona.pk);
	strcpy(idate[nregistros].data, carona.data);

	//INDICE TIME
	strcpy(itime[nregistros].pk, carona.pk);
	strcpy(itime[nregistros].hora, carona.hora);

	//INDICE NOVO
	strcpy(idatahora[nregistros].pk, carona.pk);
	strcpy(idatahora[nregistros].data, carona.data);
	strcpy(idatahora[nregistros].hora, carona.hora);

	//INDICE ROUTE
	Ir *extra;
	char copiaCarona[TAM_TRAJETO];
	char *local;

	strcpy(copiaCarona, carona.trajeto); //COPIANDO PARA NÃO ALTERAR O MEU PRINCIPAL

	local = strtok(copiaCarona, "|");

	for(int i = *ntraj ; (i <= MAX_TRAJETOS) && local ; )
	{
		extra = (Ir*)bsearch(local, iroute, *ntraj, sizeof(Ir), compararRoute2);

		if(extra) //ACHOU UM IGUAL
			insereNo(&extra->lista, carona.pk);

		else
		{
			strcpy(iroute[i].trajeto, local); //NÃO EXISTIA AINDA, ENTÃO COLOQUEI
			insereNo(&iroute[i].lista, carona.pk); //COLOCANDO A PROPRIA PK NA LISTA

			(*ntraj)++;

			ordenaIndiceRoute(iroute, *ntraj);

			i++;
		}

		local = strtok(NULL, "|");
	}

	// ordenaIndiceRoute(iroute, *ntraj);
}

//============================================================
//============================================================
//PROCUREI INSERÇÃO ORDENADA NO SITE GEEKS FOR GEEKS E ADAPTEI PARA O CÓDIGO

//INSERÇÃO EM LISTA DE MANEIRA ORDENADA
void insereNo(ll **irouteLista, char *pk)
{
	ll *l = (ll*)malloc(sizeof(ll));
	ll *extra;
	strcpy(l->pk, pk);

	if(*irouteLista == NULL || strcmp(l->pk, (*irouteLista)->pk) <= 0) //NÃO TINHA NADA NA LISTA
	{
		l->prox = *irouteLista;
		*irouteLista = l;
	}
	else //TEM ALGUMA COISA
	{
		extra = *irouteLista;

		while(extra->prox && (strcmp(extra->prox->pk, l->pk) < 0)) //PROCURANDO ONDE INSERIR
				extra = extra->prox;

		l->prox = extra->prox;
		extra->prox = l;
	}
}

//============================================================
//============================================================
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

//============================================================
//============================================================
//ALTERAR O CAMPO DE VAGAS NO VETOR DO ARQUIVO

int alterar(Ip *iprimary, int nregistros)
{
	Ip *extra;
	char indicePrimario[TAM_PRIMARY_KEY];
	char novasVagas;
	int localizacao;
	int j = (-1), m;
	int extra2 = 0;

	scanf("\n%[^\n]s", indicePrimario); //RECEBENDO PRIMARIO QUE QUEREMOS

	for(int i = 0 ; i < nregistros ; i++)
		if(strcmp(iprimary[i].pk, indicePrimario) == 0) //PROCURANDO IGUAL
		{
			j = i;
			extra = &iprimary[i];
		}

	if(j == (-1) || iprimary[j].rrn == (-1))
	{
		printf(REGISTRO_N_ENCONTRADO); //CHECANDO SE EXISTE O CAMPO PRIMÁRIO QUE QUEREMOS ALTERAR
		return(0);
	}

	scanf("\n%[^\n]s", &novasVagas);

	while(strlen(&novasVagas) != 1) //CHECANDO SE A VAGA ESTA NO INTERVALO CORRETO
	{
		printf(CAMPO_INVALIDO);
		scanf("%s", &novasVagas);
	}

	localizacao = (256 * extra->rrn);

	for(m = localizacao ; (extra2 < 10) ; m++)
	{
		if(ARQUIVO[m] == '@')
			extra2++;
	}

	ARQUIVO[m] = novasVagas;

	return(1);
}

//============================================================
//============================================================
//COLOCAR NO COMEÇO DO ESPACO O SINAL DE *| 3

int remover(Ip *iprimary, int nregistros)
{
	char indicePrimario[TAM_PRIMARY_KEY];
	int localizacao = 0;
	int passou = 0;


	scanf("%s", indicePrimario);

	if(nregistros == 0)
	{
		printf(REGISTRO_N_ENCONTRADO);
		return(0);
	}

	for(int i = 0 ; i < nregistros ; i++)
		if(strcmp(iprimary[i].pk, indicePrimario) == 0) //PROCURANDO IGUAL
		{
			if(iprimary[i].rrn != (-1)) //NÃO ACHOU NENHUM
			{
				localizacao = (256 * iprimary[i].rrn);
				ARQUIVO[localizacao] = '*';
				localizacao++;
				ARQUIVO[localizacao] = '|';
				iprimary[i].rrn = (-1);
				passou = 1;
			}
		}

	if(passou)
		return (1);

	printf(REGISTRO_N_ENCONTRADO);
	return(0);
}

//============================================================
//============================================================
//LIBERAR ESPACO QUE FOI REMOVIDO 6

//FRANCIENE BERNARDI COMENTOU COMIGO COMO FEZ ESSA PARTE
void liberarEspaco(int *nregistros)
{
	char copiaArquivo[TAM_ARQUIVO];
	char *extra;
	char extraCopia[257];
	int numeroRegistro = 0;

	for(int i = 0 ; i < *nregistros ; i++)
	{
		extra = copiaArquivo + (numeroRegistro * 256);
		strncpy(extraCopia, (ARQUIVO + (256 * i)), 1);
		extraCopia[1] = '\0';

		if(strcmp(extraCopia, "*") != 0) //NÃO É PARA EXCLUIR
		{
			strncpy(extraCopia, ARQUIVO + (256 * i), 256); //COPIAR O REGISTRO INTEIRO QUE A GENTE NÃO QUER PERDER

			extraCopia[256] = '\0';

			strncpy(extra, extraCopia, 256);
			extra = extra + 256;
			*extra = '\0';
			numeroRegistro++;
		}
	}
	(*nregistros) = numeroRegistro;

	ARQUIVO[0] = '\0';
	strcpy(ARQUIVO, copiaArquivo);

}

//============================================================
//============================================================
//CRIAR INDICE PRIMARIO PARA QUANDO RECEBER ARQUIVO

void criar_iprimary(Ip *indice_primario, int *nregistros)
{
	Carona carona;

	if(nregistros == 0)
		return;

	for(int i = 0 ; i < (*nregistros) ; i++)
	{
		carona = recuperar_registro(i);
		inserirIndicePrimarioSemOrdenar(indice_primario, i, carona);
	}
	ordenarPrimario(indice_primario, *nregistros);
}

//============================================================
//============================================================
//CRIAR O INDICE SECUNDARIO A PARTIR DE UM ARQUIVO

void criar_secundario(Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int *nregistros, int *ntraj, idh *idatahora)
{
	Carona carona;

	if(nregistros == 0)
		return;

	//DESALOCANDO NO POR NO
	//VINICIUS ME MOSTROU COMO FAZER DESALOCACAO CORRETA DOS NOS DA LISTA
	for(int i = 0; i < *ntraj; i++)
	{
		ll *node = iroute[i].lista;
		ll *temp;

		while(node)
		{
		    temp = node;
		    node = node->prox;
		    free(temp);
		}
		iroute[i].lista = NULL;
	}

	for(int i = 0 ; i < (*nregistros) ; i++)
	{
		carona = recuperar_registro(i);
		inserirIndiceSecundarioSemOrdenar(idriver, idate, itime, iroute, i, ntraj, carona, idatahora);
		ordenaIndiceRoute(iroute, *ntraj);
	}

	ordenaIndiceRoute(iroute, *ntraj);
	ordenaIndiceDriver(idriver, *nregistros);
	ordenaIndiceDate(idate, *nregistros);
	ordenaIndiceTime(itime, *nregistros);
	ordenaIndiceDate2(idatahora, *nregistros);
}

//============================================================
//============================================================
//OPCAO DE LISTAGEM 5

void listar(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, int nregistros, int ntraj, idh *idatahora)
{
	int opcao;
	ll *extra;
	Ip *primary;
	idh carona[MAX_REGISTROS];

	int cont = 0;
	int pula = 0;
	int contadorCheio = 0;
	int contadorVazio = 0;

	scanf("%d", &opcao);

	while((opcao < 1) || (opcao > 5))
	{
		printf(OPCAO_INVALIDA);
		scanf("%d", &opcao);
	}

	switch(opcao)
	{
		case 1: //POR CÓDIGO - CHAVE PRIMÁRIA
			if(nregistros != 0)
				for(int i = 0 ; (i < nregistros) ; i++)
				{
					if(iprimary[i].rrn != (-1))
					{
						exibir_registro(iprimary[i].rrn);

						contadorCheio++;

						if((i + 1) != nregistros)
							printf("\n");
					}
					else
						contadorVazio++;
				}
			else
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 2: //POR TRAJETO

			if(nregistros != 0)
			{
				for(int i = 0 ; i < ntraj ; i++)
				{
					extra = iroute[i].lista;

						while(extra)
						{
							for(int j = 0 ; j < nregistros ; j++)
							{
								if(strcmp(iprimary[j].pk, extra->pk) == 0)
								{
									if(iprimary[j].rrn != (-1))
									{
										contadorCheio++;
										if(pula)
										{
											printf("\n");
											pula = 0;
										}
										if(exibir_registro(iprimary[j].rrn))
											pula = 1;
									}
									else
										contadorVazio++;
								}
							}

							extra = extra->prox;
						}
				}
			}
			else if(nregistros == 0)
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 3: //POR NOME DO MOTORISTA

			if(nregistros != 0)
			{
				for(int i = 0 ; i < nregistros ; i++)
					for(int j = 0 ; j < nregistros ; j++)
						if(iprimary[j].rrn != (-1))
						{
							if(strcmp(idriver[i].pk, iprimary[j].pk) == 0)
							{
								contadorCheio++;

								if(pula)
								{
									printf("\n");
									pula = 0;
								}
								if(exibir_registro(iprimary[j].rrn))
									pula = 1;
							}
						}
						else	//VER SE ISSO DAQUI ESTA FUNCIONANDO
							contadorVazio++;
				}
			else
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 4: //POR DATA E HORA  ******
			if(nregistros != 0)
			{
				for(int i = 0 ; i < nregistros ; i++)
					for(int j = 0 ; j < nregistros ; j++)
						if(iprimary[j].rrn != (-1))
						{
							if(strcmp(idatahora[i].pk, iprimary[j].pk) == 0)
							{
								contadorCheio++;

								if(pula)
								{
									printf("\n");
									pula = 0;
								}
								if(exibir_registro(iprimary[j].rrn))
									pula = 1;

							}
						}
						else	//VER SE ISSO DAQUI ESTA FUNCIONANDO
							contadorVazio++;
			}
			else
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 5: //POR TRAJETO E DATA E HORA

			if(nregistros != 0)
			{
				for(int i = 0 ; i < ntraj ; i++)
				{
					extra = iroute[i].lista;

					cont = 0;

					for(int k = 0 ; extra ; k++)
					{
						strcpy(carona[k].pk, extra->pk);

						for(int j = 0 ; (j < nregistros) ; j++)
							if(strcmp(idatahora[j].pk, carona[k].pk) == 0) //ACHANDO A PK QUE EU QUERO
							{
								strcpy(carona[k].data, idatahora[j].data); //PASSANDO O VALOR DA DATA
								strcpy(carona[k].hora, idatahora[j].hora); //PASSANDO O VALOR DA HORA
							}

						cont+= 1;
						extra = extra->prox;
					}

					//ORDENANDO NOSSO NOVO VETOR PARA SER LEVADO EM CONTA DATA E HORA
					qsort(carona, cont, sizeof(idh), compararDate2);

					for(int m = 0 ; m < cont ; m++)
					{
						//ACHANDO O IPRIMARY QUE A GENTE QUER
						primary = (Ip*)bsearch(((char*)carona[m].pk), (Ip*)iprimary, nregistros, sizeof(Ip), encontrarIndicePrimarioCharIp);

						if(primary->rrn != (-1)) //CHECA SE JÁ NÃO FOI EXCLUIDO
						{
							contadorCheio++;
							if(pula)
							{
								printf("\n");
								pula = 0;
							}

							if(exibir_registro(primary->rrn))
								pula = 1;

						}
						else //SE TIVER SIDO, DECREMENTA O RRN DO ESCOPO
							contadorVazio++;
					}
				}

			}
			else if(nregistros == 0)
				printf(REGISTRO_N_ENCONTRADO);

		break;
	}
}

//============================================================
//============================================================
//BUSCANDO A OPCAO 4

void busca(Ip *iprimary, Isd *idate, Ir *iroute, idh *idatahora, int nregistros, int ntraj)
{
	int opcao;
	int encontrou = 0;
	int pula;
	int cont;
	int contadorVazio = 0;
	int contadorCheio = 0;

	char codigo[TAM_PRIMARY_KEY];
	char data[TAM_DATA];
	char localidade[TAM_TRAJETO];
	ll *extraLista = NULL;
	idh carona[MAX_REGISTROS];
	Ip *primary;

	scanf("%d%*c", &opcao);

	while(opcao < 1 || opcao > 4)
	{
		printf(OPCAO_INVALIDA);
		scanf("%d", &opcao);
	}

	switch(opcao)
	{
		case 1: //POR CÓDIGO
			encontrou = 0;

			scanf("%[^\n]%*c", codigo);

			primary = (Ip*)bsearch((char*)codigo, (Ip*)iprimary, nregistros, sizeof(Ip), encontrarIndicePrimarioCharIp);

			if(primary)
				if(exibir_registro(primary->rrn))
					encontrou = 1;

			if(!encontrou)
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 2: //POR DATA
			encontrou = 0;
			pula = 0;

			scanf("%[^\n]%*c", data);

			for(int i = 0 ; i < nregistros ; i++)
			{
				if(strcmp(data, idate[i].data) == 0)
				{
					primary = (Ip*)bsearch((char*)idate[i].pk, (Ip*)iprimary, nregistros, sizeof(Ip), encontrarIndicePrimarioCharIp);

					if(pula)
					{
						printf("\n");
						pula = 0;
					}
					if(primary)
						if(exibir_registro(primary->rrn))
						{
							encontrou = 1;
							pula = 1;
						}
					}
				}

			if(!encontrou)
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 3: //POR LOCALIDADE
			encontrou = 0;
			pula = 0;

			scanf("%[^\n]%*c", localidade);

			cont = 0;

			for(int i = 0 ; i < ntraj ; i++)
			{
				if(strcmp(localidade, iroute[i].trajeto) == 0) //ACHEI A ROTA QUE EU QUERIA
				{
					extraLista = iroute[i].lista;
				}
			}

			for(int j = 0 ; extraLista ; j++) //GUARDANDO EM UM VETOR DE CARONAS TODAS AS CARONAS QUE PASSAM POR LÁ
			{
				strcpy(carona[j].pk, extraLista->pk);

				cont++;
				extraLista = extraLista->prox;
			}

			for(int m = 0 ; m < cont ; m++)
			{
				//ACHANDO O IPRIMARY QUE A GENTE QUER
				primary = (Ip*)bsearch(((char*)carona[m].pk), (Ip*)iprimary, nregistros, sizeof(Ip), encontrarIndicePrimarioCharIp);

				if(primary->rrn != (-1)) //CHECA SE JÁ NÃO FOI EXCLUIDO
				{
					if(pula)
					{
						printf("\n");
						pula = 0;
					}

					if(exibir_registro(primary->rrn))
					{
						pula = 1;
						encontrou = 1;
					}
				}
				else //SE TIVER SIDO, DECREMENTA O RRN DO ESCOPO
				{
					contadorVazio++;
				}
			}

			if(!encontrou)
				printf(REGISTRO_N_ENCONTRADO);

		break;

		case 4: //POR LOCALIDADE E DATA

		encontrou = 0;
		pula = 0;

		scanf("%[^\n]%*c", localidade);
		scanf("%[^\n]%*c", data);

		for(int i = 0 ; i < ntraj ; i++)
			if(strcmp(localidade, iroute[i].trajeto) == 0) //ACHEI A ROTA QUE EU QUERIA
				extraLista = iroute[i].lista;

		cont = 0;

		for(int j = 0 ; extraLista ; j++) //GUARDANDO EM UM VETOR DE CARONAS TODAS AS CARONAS QUE PASSAM POR LÁ
		{
			strcpy(carona[j].pk, extraLista->pk);

			for(int k = 0 ; (k < nregistros) ; k++)
				if(strcmp(carona[j].pk, idatahora[k].pk) == 0) //ACHANDO A PK QUE EU QUERO
					strcpy(carona[j].data, idatahora[k].data); //PASSANDO O VALOR DA DATA

			cont++;
			extraLista = extraLista->prox;
		}

		qsort(carona, cont, sizeof(idh), compararDate2); //ORDENADO PELA DATA

		for(int m = 0 ; m < cont ; m++)
		{
			if(strcmp(carona[m].data, data) == 0) //VERIFICANDO SE A DATA É IGUAL A QUE QUERO
			{
				//ACHANDO O IPRIMARY QUE A GENTE QUER
				primary = (Ip*)bsearch(((char*)carona[m].pk), (Ip*)iprimary, nregistros, sizeof(Ip), encontrarIndicePrimarioCharIp);

				if(primary->rrn != (-1)) //CHECA SE JÁ NÃO FOI EXCLUIDO
				{
					contadorCheio++;

					if(pula)
					{
						printf("\n");
						pula = 0;
					}

					if(exibir_registro(primary->rrn))
					{
						pula = 1;
						encontrou = 1;
					}
				}
				else //SE TIVER SIDO, DECREMENTA O REGISTRO DO ESCOPO
				{
					contadorVazio++;
				}
			}
		}

		if(!encontrou)
			printf(REGISTRO_N_ENCONTRADO);

		break;

	}
}

//============================================================
//============================================================
//LIBERANDO OS INDICES DA MEMÓRIA

void liberarMemoria(Ip *iprimary, Is *idriver, Isd *idate, Ist *itime, Ir *iroute, idh *idatahora, int nregistros, int ntraj)
{
	free(iprimary);
	free(idriver);
	free(idate);
	free(itime);
	free(idatahora);

	//DESALOCANDO NO POR NO
	//VINICIUS ME MOSTROU COMO FAZER DESALOCACAO CORRETA DOS NOS DA LISTA
	for(int i = 0; i < ntraj; i++)
	{
		ll *node = iroute[i].lista; //JA DELETA QUANDO SAI
		ll *temp;

		while(node != NULL)
		{
		    temp = node;
		    node = node->prox;
		    free(temp);
		}
		iroute[i].lista = NULL;
	}
	free(iroute);
}
