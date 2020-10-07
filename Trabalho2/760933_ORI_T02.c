/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Organização e Recuperação da Informação
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
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
#define TAM_STRING_INDICE (30 + 6 + 4 + 1)

#define TAM_REGISTRO 256
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS "Busca por %s.\nNos percorridos:\n"

/* Registro da Carona */
typedef struct
{
	int rrn;
	char string[TAM_STRING_INDICE];
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

/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_ip *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;  /* numero de chaves armazenadas*/
	Chave_is *chave; /* vetor das chaves e rrns [m-1]*/
	int *desc;		 /* ponteiros para os descendentes, *desc[m]*/
	char folha;		 /* flag folha da arvore*/
} node_Btree_is;

typedef struct
{
	int raiz;
} Indice;

//CRIANDO NOVAS STRUCT PARA CONSEGUIR FAZER A PROMOCAO E DIVISAO DOS NOS
typedef struct
{
	Chave_ip chave; //LOCALIZANDO PK
	int filho;

} ExtraIP;

typedef struct
{
	Chave_is chave; //LOCALIZANDO PK E STRING
	int filho;

} ExtraIS;


/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
char ARQUIVO_IP[2000 * sizeof(Chave_ip)];
char ARQUIVO_IS[2000 * sizeof(Chave_is)];
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
int tamanho_registro_ip;
int tamanho_registro_is;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de Caronas  */
void criar_iride(Indice *iride);

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca dinamicamente os vetores de chaves e descendentes */
void *criar_no(char ip);

/* Percorre a arvore e retorna o RRN da chave informada.  Retorna -1, caso não
 * encontrada. */
int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho);

/* Percorre a arvore e retorna a pk da string destino/data-hora informada. Retorna -1, caso não
 * encontrada. */
char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho);

/* Realiza percurso em-ordem imprimindo todas as caronas na ordem lexicografica do destino e data/hora e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_is(int noderrn, int nivel);

/*Gera Chave da struct Carona*/
void gerarChave(Carona *novo);

/* Função auxiliar que ordena as chaves em esq + a chave a ser inserida e divide
 * entre os nós esq e dir. Retorna o novo nó à direita, a chave promovida e seu
 * descendente direito, que pode ser nulo, caso a nó seja folha. */
// int divide_no_ip(int rrnesq, Chave_ip *chave, int desc_dir_rrn);
// ExtraIP *divide_no_ip(Indice *iprimary, Carona k, int rrn, int filho);
ExtraIP *divide_no_ip(int xrrn, Carona k, int filho);

ExtraIS *divide_no_is(int norrn, Carona k, int filho);

// ExtraIS *divide_no_is(Indice *iprimary, Carona k, int rrn, int filho);
// int divide_no_is(int rrnesq, Chave_is *chave, int desc_dir_rrn);

/* Lista todos os registros não marcados para remoção */
void listar(Indice iprimary, Indice iride);

/* Realiza percurso pré-ordem imprimindo as chaves primárias dos registros e o
 * nível de cada nó (raiz = nível 1) */
int imprimir_arvore_ip(int noderrn, int nivel);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
Carona recuperar_registro(int rrn);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice *iprimary, Indice *iride);

int alterar(Indice iprimary);

void buscar(Indice iprimary, Indice iride);

void listar(Indice iprimary, Indice iride);

/*******************************************************/

void libera_no(void *node, char ip);

/*Realiza os scanfs na struct Carona*/
void ler_entrada(char *registro, Carona *novo);

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j);

/* Insere um novo registro na Árvore B */
void insere_chave_ip(Indice *iprimary, Chave_ip chave);

/* Função auxiliar para ser chamada recursivamente, inserir as novas chaves nas
 * folhas e tratar overflow no retorno da recursão. */
// int insere_aux_ip(int noderrn, Chave_ip *chave);
void *insere_aux_ip(int rrn, Carona k);
// int insere_aux_is(int noderrn, Chave_is *chave);
// void *insere_aux_is(Indice *irider, Carona k);
void *insere_aux_is(int norrn, Carona k);

/* VOCÊS NÃO NECESSARIAMENTE PRECISAM USAR TODAS ESSAS FUNÇÕES, É MAIS PARA TEREM UMA BASE MESMO,
 * PODEM CRIAR SUAS PRÓPRIAS FUNÇÕES SE PREFERIREM */

void gerarChave(Carona *novo);
Carona inserirCarona();
void colocarNoArquivo(Carona j);
void emOrdem(int rrn);
void preOrdem(int rrn, int nivel);
void cadastrarComArquivo(Indice *iprimary, Indice *iride);

// ExtraIP insere_aux_ip(int rrn, ExtraIP noIP);

int main()
{
	char *p; /* # */
			 /* Arquivo */
	int carregarArquivo = 0;
	nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 10);

	/* Índice primário */
	Indice iprimary;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Caronas */
	Indice iride;
	criar_iride(&iride);

	if(carregarArquivo)
		cadastrarComArquivo(&iprimary, &iride); //SE QUALQUER COISA DER ERRADO, TIRAR ISSO s

	/* Execução do programa */
	int opcao = 0;
	while (1)
	{
		scanf("%d%*c", &opcao);
		switch (opcao)
		{
		case 1: /* Cadastrar uma nova Carona */
			cadastrar(&iprimary, &iride);
			break;
		case 2: /* Alterar a qtd de vagas de uma Carona */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar uma Carona */
			printf(INICIO_BUSCA);
			buscar(iprimary, iride);
			break;
		case 4: /* Listar todos as Caronas */
			printf(INICIO_LISTAGEM);
			listar(iprimary, iride);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO != '\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if (!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IP; *p != '\0'; p += tamanho_registro_ip)
				{
					fwrite(p, 1, tamanho_registro_ip, stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if (!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for (p = ARQUIVO_IS; *p != '\0'; p += tamanho_registro_is)
				{
					fwrite(p, 1, tamanho_registro_is, stdout);
					puts("");
				}
			//printf("%s\n", ARQUIVO_IS);
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]%*c", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe a Carona */
int exibir_registro(int rrn)
{
	if (rrn < 0)
		return 0;

	Carona j = recuperar_registro(rrn);
	char *traj, trajeto[TAM_TRAJETO + 10];

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

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Carona */
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

//-----------------------------------------------------------------//

void cadastrar(Indice *iprimary, Indice *irider)
{
	Carona j;
	char *traj;
	char *traj2;
	char trajeto[TAM_TRAJETO + 10];

	j = inserirCarona();
	j.rrn = nregistros;

	strcpy(trajeto, j.trajeto);
	traj = strtok(trajeto, "|");
	while (traj != NULL)
	{
		traj2 = traj;
		traj = strtok(NULL, "|");
	}

	sprintf(j.string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, j.data[6], j.data[7], j.data[3], j.data[4], j.data[0], j.data[1], j.hora[0], j.hora[1], j.hora[3], j.hora[4]);

	//BUSCA VAI VIR AQUI ONDE VAI CHECAR POR REPETIÇÃO

	// if(buscar(iprimary, *j, 0))
	// {
	// 	printf(ERRO_PK_REPETIDA);
	// 	return;
	// }

	//else
	inserir_registro_indices(iprimary, irider, j);

	nregistros++;

	colocarNoArquivo(j);

}

void cadastrarComArquivo(Indice *iprimary, Indice *irider)
{
	Carona k;
	int extra;
	char *traj;
	char *traj2;
	char trajeto[TAM_TRAJETO + 10];


	//BUSCA VAI VIR AQUI ONDE VAI CHECAR POR REPETIÇÃO

	// if(buscar(iprimary, *j, 0))
	// {
	// 	printf(ERRO_PK_REPETIDA);
	// 	return;
	// }

	extra = nregistros;
	for(nregistros = 0 ; nregistros < extra ; nregistros++)
	{
		k = recuperar_registro(nregistros);

		strcpy(trajeto, k.trajeto);
		traj = strtok(trajeto, "|");
		while (traj != NULL)
		{
			traj2 = traj;
			traj = strtok(NULL, "|");
		}
		sprintf(k.string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[6], k.data[7], k.data[3], k.data[4], k.data[0], k.data[1], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
		k.rrn = nregistros;

		inserir_registro_indices(iprimary, irider, k);
	}
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

//INICIALIZANDO O VALOR DO IPRIMARY
void criar_iprimary(Indice *iprimary)
{
	iprimary->raiz = (-1); //INICIALIZANDO O VALOR DO IPRIMARY
}

//INICIALIZANDO O VALOR DO IRIDE
void criar_iride(Indice *iride)
{
	iride->raiz = (-1);
}

//-----------------------------------------------------------------//

//CRIANDO OS NOS DA ÁRVORE
void *criar_no(char ip)
{
	if(ip == 0) //É PRIMARIO
	{
		node_Btree_ip *noPrimary;

		noPrimary = (node_Btree_ip*)calloc(1, sizeof(node_Btree_ip));
		noPrimary->chave =  (Chave_ip*)calloc(ordem_ip - 1, sizeof(Chave_ip)); //NUMERO MAX DE CHAVES : ORDEM - 1
		noPrimary->desc = (int*)calloc(ordem_ip, sizeof(int));

		noPrimary->num_chaves = 0;
		noPrimary->folha = 'T';

		return(noPrimary);
	}

	else if(ip == 1) //É SECUNDARIO
	{
		node_Btree_is *noRider;

		noRider = (node_Btree_is*)calloc(1, sizeof(node_Btree_is));
		noRider->chave =  (Chave_is*)calloc((ordem_is - 1), sizeof(Chave_is)); //NUMERO MAX DE CHAVES: ORDEM - 1
		noRider->desc = (int*)calloc(ordem_is, sizeof(int));

		noRider->num_chaves = 0;
		noRider->folha = 'T';

		return(noRider);
	}

	return (NULL);
}


//PSEUDOCÓDIGO DA ARVORE
//INSERIR(T, k)
void inserir_registro_indices(Indice *iprimary, Indice *iride, Carona j)
{
	//INSERINDO IPRIMARY
	node_Btree_ip *noPrimary;
	node_Btree_is *noRide;

	ExtraIP *chave_promovidaIP;
	ExtraIS *chave_promovidaIS;

	chave_promovidaIP = (ExtraIP*)calloc(1, sizeof(ExtraIP));
	chave_promovidaIS = (ExtraIS*)calloc(1, sizeof(ExtraIS));

	if(iprimary->raiz == (-1)) //ARVORE VAZIA, VAMOS INSERIR NA PRIMEIRA POSIÇÃO
	{

		// printf("OI 000000000000\n");

		noPrimary = (node_Btree_ip*)criar_no(0);
		noPrimary->folha = 'T';
		noPrimary->num_chaves = 1;
		strcpy(noPrimary->chave[0].pk, j.pk);
		noPrimary->chave[0].rrn = nregistros;

		noPrimary->desc[0] = (-1);
		noPrimary->desc[1] = (-1);

		iprimary->raiz = 0;

		write_btree(noPrimary, iprimary->raiz, 0);

		libera_no(noPrimary, 0);

		nregistrosip++;
	}
	else
	{
		chave_promovidaIP = (ExtraIP*)insere_aux_ip(iprimary->raiz, j);

		// printf("OI 2\n");

		if(chave_promovidaIP != NULL)
		{
			// printf("OI 3.0\n");

			noPrimary = (node_Btree_ip*)criar_no(0);
			noPrimary->folha = 'F';
			noPrimary->num_chaves = 1;
			strcpy(noPrimary->chave[0].pk, chave_promovidaIP->chave.pk);
			noPrimary->chave[0].rrn = chave_promovidaIP->chave.rrn;

			noPrimary->desc[0] = iprimary->raiz;
			noPrimary->desc[1] = chave_promovidaIP->filho;

			iprimary->raiz = nregistrosip;

			write_btree(noPrimary, nregistrosip, 0);

			libera_no(noPrimary, 0);

			nregistrosip++;
		}
	}

	if(iride->raiz == (-1)) //ARVORE VAZIA, INSERE NA PRIMEIRA POSIÇÃO
	{
		noRide = (node_Btree_is*)criar_no(1);
		noRide->num_chaves = 1;
		noRide->folha = 'T';
		strcpy(noRide->chave[0].pk, j.pk);
		// sprintf(noRide->chave[0].string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, j.data[0], j.data[1], j.data[3], j.data[4], j.data[6], j.data[7], j.hora[0], j.hora[1], j.hora[3], j.hora[4]);
		strcpy(noRide->chave[0].string, j.string);

		iride->raiz = 0;

		noRide->desc[1] = (-1);
		noRide->desc[0] = (-1);

		write_btree(noRide, iride->raiz, 1);

		libera_no(noRide, 1);

		nregistrosis++;
	}
	else
	{
		chave_promovidaIS = (ExtraIS*)insere_aux_is(iride->raiz, j);

		if(chave_promovidaIS != NULL)
		{
			noRide = (node_Btree_is*)criar_no(1);

			noRide->folha = 'F';
			noRide->num_chaves = 1;
			strcpy(noRide->chave[0].pk, chave_promovidaIS->chave.pk);
			strcpy(noRide->chave[0].string, chave_promovidaIS->chave.string);
			// sprintf(noRide->chave[0].string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, j.data[0], j.data[1], j.data[3], j.data[4], j.data[6], j.data[7], j.hora[0], j.hora[1], j.hora[3], j.hora[4]);

			noRide->desc[0] = iride->raiz;
			noRide->desc[1] = chave_promovidaIS->filho;

			iride->raiz = nregistrosis;

			write_btree(noRide, nregistrosis, 1);

			libera_no(noRide, 1);

			nregistrosis++;
		}
	}

	// free(chave_promovidaIP);
	// free(chave_promovidaIS);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//LIBERANDO OS NOS

void libera_no(void *no, char ip)
{
	if(ip == 0)
	{
		// free(((node_Btree_ip*)no)->chave);
		// free(((node_Btree_ip*)no)->desc);
		// free(((node_Btree_ip*)no));
	}
	else if(ip == 1)
	{
		// free(((node_Btree_is*)no)->chave);
		// free(((node_Btree_is*)no)->desc);
		// free(((node_Btree_is*)no));
	}
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//LEITURA NO ARQUIVO DE INDICES PARA CONSTRUIR NO

//PARA O B READ FORAM USADAS AS ESPECIFICACOES DE TAMANHO PRESENTES NO DOCUMENTO
//IPRIMARY
//ORDEM - 1 * 10 BYTES DA CHAVE PRIMARIA + 4 RRN DO ARQUIVO DE DADOS

//IRIDE
//ORDEM - 1 * 10 BYTES DA CHAVE PRIMARIA + 41 RRN DO ARQUIVO DE DADOS + 51 #

//PEDI AJUDA PARA VINICIUS PARA AJUDAR A ANDAR OS INDICES DE MANEIRA CORRETA
void *read_btree(int rrn, char ip)
{
	char extraIP[tamanho_registro_ip];
	char extraIS[tamanho_registro_is];

	// int extra;
	int posicao = 3;

	if(ip == 0)
	{
		// posicao = 3;

		node_Btree_ip *extraNo;
		extraNo = (node_Btree_ip*)criar_no(0);

		extraIP[0] = '\0'; //VER SE PRECISA FAZER ISSO
		strncat(extraIP, &ARQUIVO_IP[tamanho_registro_ip * rrn], tamanho_registro_ip);

		sscanf(extraIP, "%3d", &extraNo->num_chaves);

		for(int i = 0 ; i < extraNo->num_chaves ; i++, posicao = posicao + 4) //SALVANDO A PK NO EXTRANO
		{
			sscanf(&extraIP[posicao], "%10s", extraNo->chave[i].pk);

			posicao = posicao + 10; //PULANDO OS 10 PRIMEIROS REGISTROS E INDO PARA OS RRN DO ARQUIVO QUE É ONDE ELE ESTA NO ARQUIVO DE DADOS
			sscanf(&extraIP[posicao], "%4d", &extraNo->chave[i].rrn);

		}

		//GARANTINDO QUE O BUFFER ESTA NA POSICAO CERTA, MESMO QUE SÓ TENHA UMA FOLHA
		posicao = posicao + (((ordem_ip - 1) - extraNo->num_chaves) * 14);

		extraNo->folha = extraIP[posicao];

		// printf("EXTRAIP: %c\n", extraIP[posicao]);
		// printf("POSICAO: %d\n", posicao);

		if(extraNo->folha == 'F') //NÃO É FOLHA
		{
			posicao++; //	PEGANDO O PRIMEIRO NÚMERO

			//ESTAREMOS VERIFICANDO TODOS OS NOS LIGADOS A ELE, PERCORRENDO TUDO ATÉ ACHAR TODOS
			for(int i = 0 ; (i < ordem_ip) && (extraIP[i] != '\0') && (extraIP[i] != '*') ; i++, posicao = posicao + 3)
				sscanf(&extraIP[posicao], "%3d", &extraNo->desc[i]);
		}

		return((void*)extraNo);
	}

	else if(ip == 1) //FAZENDO A MESMA COISA DO CIMA SÓ QUE PARA OS INDICES DO IRIDE
	{
		node_Btree_is *extraNo;
		// char *extraIS = ARQUIVO_IS + (rrn * tamanho_registro_is);
		extraNo = (node_Btree_is*)criar_no(1);

		// extraIS[0] = '\0'; //VER SE PRECISA FAZER ISSO
		// strncat(extraIS, &ARQUIVO_IS[tamanho_registro_is * rrn], tamanho_registro_is);
		int i;

		for (i = 0; i < tamanho_registro_is; i++) {
			extraIS[i] = ARQUIVO_IS[tamanho_registro_is*rrn + i];
		}
		extraIS[i] = '\0';

		sscanf(extraIS, "%03d", &extraNo->num_chaves);

		for(int i = 0 ; i < extraNo->num_chaves ; i++, posicao = posicao + 41)
		{
			// printf("NUM CHAVES: %d\n", extraNo->num_chaves);
			// printf("PK: %c\n", extraIS[posicao]);

			strncpy(extraNo->chave[i].pk, &extraIS[posicao], 10);
			extraNo->chave[i].pk[10] = '\0';

			posicao = posicao + 10;

			sscanf(&extraIS[posicao], "%[^#\n]", extraNo->chave[i].string);
		}

		posicao = posicao + (((ordem_is - 1) - extraNo->num_chaves) * 51);

		extraNo->folha = extraIS[posicao]; //PRECISO DISSO AQUI

		if(extraNo->folha == 'F')
		{
			posicao++;
			// printf("POSICAO: %d\n", posicao);

			for(int i = 0 ; (i < ordem_is) ; i++, posicao = posicao + 3)
				sscanf(&extraIS[posicao], "%3d", &extraNo->desc[i]);

			// printf("SAINDO DAQUI\n");
		}

		return((void*)extraNo);
	}

	return (NULL);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//ESCRITA NO ARQUIVO DE INDICES
//PEDI PARA O ANTONIO ME EXPLICAR COMO FAZER A ESRITA DIRETO NO ARQUIVO
void write_btree(void *salvar, int rrn, char ip)
{
	int posicao = 3;
	int posicao2 = 0;

	// char extraIP[tamanho_registro_ip];
	// char extraIS[tamanho_registro_is];

	char extra;

	if(ip == 0)
	{
		// printf("ESCREVENDO Ip b ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n");
		//SALVANDO QUANTIDADE DE NOS
		sprintf(ARQUIVO_IP + tamanho_registro_ip * rrn, "%03d", ((node_Btree_ip*)salvar)->num_chaves);
		// sprintf(extraIP, "%03d", ((node_Btree_ip*)salvar)->num_chaves);
		// strncpy(&ARQUIVO_IP[tamanho_registro_ip * rrn], extraIP, strlen(extraIP));

		//14 POSICOES DA PK E DO RRN
		for(int i = 0 ; i < ((node_Btree_ip*)salvar)->num_chaves ; i++, posicao = posicao + 4)
		{
			sprintf(&ARQUIVO_IP[tamanho_registro_ip * rrn + posicao], "%s%04d", ((node_Btree_ip*)salvar)->chave[i].pk, ((node_Btree_ip*)salvar)->chave[i].rrn);

			posicao = posicao + 10;

			sprintf(&ARQUIVO_IP[tamanho_registro_ip * rrn + posicao], "%04d", ((node_Btree_ip*)salvar)->chave[i].rrn);
		}

		//GUARDANDO OS # NO ARQUIVO
		for( ; posicao < (ordem_ip - 1) * 14 + 3 ; posicao++)
			ARQUIVO_IP[tamanho_registro_ip * rrn + posicao] = '#';

		//SALVANDO SE O NÓ É FOLHA OU NÃO
		ARQUIVO_IP[tamanho_registro_ip * rrn + posicao] = ((node_Btree_ip*)salvar)->folha;
		// printf("foia %c\n", ((node_Btree_ip*)salvar)->folha);

		//SALVANDO MAIS UM POSICAO DO CARACTER
		posicao++;

		if(((node_Btree_ip*)salvar)->folha == 'F')
		{
			extra = ARQUIVO_IP[(rrn + 1) * tamanho_registro_ip];

			for(int i = 0 ; i < ((node_Btree_ip*)salvar)->num_chaves + 1 ; i++, posicao = posicao + 3)
				sprintf(ARQUIVO_IP + tamanho_registro_ip * rrn + posicao, "%03d", ((node_Btree_ip*)salvar)->desc[i]);

			ARQUIVO_IP[tamanho_registro_ip * (rrn + 1)] = extra;
		}

		//COMPLETANDO O REGISTRO
		for(int i = posicao ; i < tamanho_registro_ip ; i++)
			ARQUIVO_IP[tamanho_registro_ip * rrn + i] = '*';
	}

	else if(ip == 1)
	{
		//SALVANDO QUANTIDADE DE NOS
		sprintf(ARQUIVO_IS + tamanho_registro_is * rrn, "%03d", ((node_Btree_is*)salvar)->num_chaves);
		// sprintf(extraIS, "%03d", ((node_Btree_is*)salvar)->num_chaves);
		// strncpy(&ARQUIVO_IS[tamanho_registro_is * rrn], extraIS, strlen(extraIS));

		//14 POSICOES DA PK E DO RRN
		for(int i = 0 ; i < ((node_Btree_is*)salvar)->num_chaves ; i++, posicao = posicao + 51) //VERIFICAR ESSE VALOR AQUI
		{
			sprintf(ARQUIVO_IS + tamanho_registro_is * rrn + posicao, "%s%s", ((node_Btree_is*)salvar)->chave[i].pk, ((node_Btree_is*)salvar)->chave[i].string);
			posicao2 = strlen(ARQUIVO_IS + tamanho_registro_is * rrn + posicao);
			// printf("AQUI ESTA A CHAVE: %s\n", ((node_Btree_is*)salvar)->chave[i].string);

			for( ; posicao2 < 51 ; posicao2++)
				ARQUIVO_IS[tamanho_registro_is * rrn + posicao2 + posicao] = '#';

		}

		for( ; posicao < ((ordem_is - 1) * 51) + 3 ; posicao++) {
			ARQUIVO_IS[tamanho_registro_is * rrn + posicao] = '#';
		}
		//GUARDANDO OS # NO ARQUIVO

		// strncpy(&ARQUIVO_IS[tamanho_registro_is * rrn], extraIS, strlen(extraIS)); //COPIANDO

		//SALVANDO SE O NÓ É FOLHA OU NÃO
		ARQUIVO_IS[tamanho_registro_is * rrn + posicao] = ((node_Btree_is*)salvar)->folha;
		// printf("foia %c\n", ((node_Btree_is*)salvar)->folha);

		//SALVANDO MAIS UM POSICAO DO CARACTER
		posicao++;

		if(((node_Btree_is*)salvar)->folha == 'F')
		{
			extra = ARQUIVO_IS[tamanho_registro_is * (rrn + 1)];

			for(int i = 0 ; i < ((node_Btree_is*)salvar)->num_chaves + 1 ; i++, posicao = posicao + 3)
				sprintf(ARQUIVO_IS + (tamanho_registro_is * rrn) + posicao, "%03d", ((node_Btree_is*)salvar)->desc[i]);

			ARQUIVO_IS[tamanho_registro_is * (rrn + 1)] = extra;
		}

		//COMPLETANDO O REGISTRO
		for( ; posicao < tamanho_registro_is ; posicao++)
			ARQUIVO_IS[tamanho_registro_is * rrn + posicao] = '*';

	}
	return;
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//INSERINDO NO USANDO O PSEUDOCÓDIGO

void *insere_aux_ip(int norrn, Carona k)
{
	node_Btree_ip *x;
	int i; //POSICAO

	Indice *extra = (Indice*)calloc(1, sizeof(Indice));
	ExtraIP *chave_promovida = (ExtraIP*)calloc(1, sizeof(ExtraIP));

	x = (node_Btree_ip*)read_btree(norrn, 0);

	if(x->folha == 'T')
	{
		if(x->num_chaves < (ordem_ip - 1))
		{
			i = (x->num_chaves - 1);

			while((i >= 0) && (strcmp(k.pk, x->chave[i].pk)) < 0)
			{
				// x->chave[i + 1] = x->chave[i];
				strcpy(x->chave[i + 1].pk, x->chave[i].pk);
				x->chave[i + 1].rrn = x->chave[i].rrn;
				i = i - 1;
			}

			strcpy(x->chave[i + 1].pk, k.pk);
			x->chave[i + 1].rrn = k.rrn;
			x->num_chaves = x->num_chaves + 1;

			write_btree(x, norrn, 0);

			libera_no(x, 0);
			// free(chave_promovida);
			// free(extra);

			return(NULL);
		}
		else
		{
			libera_no(x, 0);
			// free(chave_promovida);
			// free(extra);
			return(divide_no_ip(norrn, k, -1));
		}
	}

	else
	{
		i = x->num_chaves - 1;

		while((i >= 0) && (strcmp(k.pk, x->chave[i].pk)) < 0)
			i = i - 1;

		i = i + 1;

		chave_promovida = (ExtraIP*)insere_aux_ip(x->desc[i], k);

		if(chave_promovida != NULL)
		{
			strcpy(k.pk, chave_promovida->chave.pk);
			k.rrn = chave_promovida->chave.rrn;

			if(x->num_chaves < (ordem_ip - 1))
			{
				i = x->num_chaves - 1; //PORQUE É UM VECTOR

				while((i >= 0) && (strcmp(k.pk, x->chave[i].pk)) < 0)
				{
					// x->chave[i + 1] = x->chave[i];
					strcpy(x->chave[i + 1].pk, x->chave[i].pk);
					x->chave[i + 1].rrn = x->chave[i].rrn;
					x->desc[i + 2] = x->desc[i + 1];
					i = i - 1;
				}

				// strcpy(x->chave[i + 1].pk, chave_promovida->chave.pk);
				strcpy(x->chave[i + 1].pk, k.pk); //MUDEI ISSO AQUI VERIFIFCAR
				x->chave[i + 1].rrn = chave_promovida->chave.rrn; //VER SE FUNCIONA ASSIM, SENAO chave_promovida->rrn
				x->desc[i + 2] = chave_promovida->filho;
				x->num_chaves = x->num_chaves + 1;

				//COISAS A MAIS
				write_btree(x, norrn, 0);

				libera_no(x, 0);


				return(NULL);
			}
			else
			{
				libera_no(x, 0);
				// free(chave_promovida);
				// free(extra);

				return(divide_no_ip(norrn, k, chave_promovida->filho));
			}
		}
		else
		{
			// write_btree(x, norrn, 0);

			libera_no(x, 0);
			// free(chave_promovida);
			free(extra);
		}
	}

	return(NULL);
}

//-----------------------------------------------------------------//

void *insere_aux_is(int norrn, Carona k)
{
	node_Btree_is *x;
	int i; //POSICAO
	// int extra2 = 0;
	char string[TAM_STRING_INDICE];
	// char *traj;
	// char* traj2;
	// char trajeto[TAM_TRAJETO];

	ExtraIS *chave_promovida = (ExtraIS*)calloc(1, sizeof(ExtraIS));

	// sprintf(string, "%s$%s%s", k.trajeto, k.data, k.hora);
	// sprintf(string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[0], k.data[1], k.data[3], k.data[4], k.data[6], k.data[7], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
	strcpy(string, k.string);

	x = (node_Btree_is*)read_btree(norrn, 1);

	if(!x)
	{
		libera_no(x, 1);
		return (NULL);
	}

	if(x->folha == 'T')
	{
		if(x->num_chaves < (ordem_is - 1))
		{
			i = (x->num_chaves - 1);

			while((i >= 0) && (strcmp(string, x->chave[i].string)) < 0)
			{
				x->chave[i + 1] = x->chave[i];
				i = i - 1;

			}

			// sprintf(x->chave[i + 1].string, "%s$%s%s", k.trajeto, k.data, k.hora);
			// sprintf(x->chave[i + 1].string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[0], k.data[1], k.data[3], k.data[4], k.data[6], k.data[7], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
			strcpy(x->chave[i + 1].string, k.string);

			strcpy(x->chave[i + 1].pk, k.pk);
			x->num_chaves = x->num_chaves + 1;

			write_btree(x, norrn, 1);

			libera_no(x, 1);
			// free(chave_promovida);
			// free(extra);

			return(NULL);
		}
		else
		{
			libera_no(x, 1);
			// free(chave_promovida);
			// free(extra);

			return(divide_no_is(norrn, k, (-1)));
		}
	}

	else if(x->folha == 'F')
	{
		i = x->num_chaves - 1;

		while((i >= 0) && (strcmp(string, x->chave[i].string)) < 0)
			i = i - 1;

		i = i + 1;

		chave_promovida = (ExtraIS*)insere_aux_is(x->desc[i], k);

		if(chave_promovida != NULL)
		{
			strcpy(k.pk, chave_promovida->chave.pk);

			strcpy(k.string, chave_promovida->chave.string); //CRIEI

			// strcpy(x->chave[i + 1].string, chave_promovida->chave.string); //MUDEI
			// strcpy(x->chave[i + 1].string, k.string); //COMENTEI

			if(x->num_chaves < (ordem_is - 1))
			{
				i = x->num_chaves - 1; //PORQUE É UM VECTOR

				while((i >= 0) && (strcmp(k.string, x->chave[i].string)) < 0)
				{
					x->chave[i + 1] = x->chave[i];
					x->desc[i + 2] = x->desc[i + 1];

					i = i - 1;
				}

				// sprintf(x->chave[i + 1].string, "%s$%s%s", k.trajeto, k.data, k.hora);
				strcpy(x->chave[i + 1].string, k.string); //COLOQUEI
				strcpy(x->chave[i + 1].pk, k.pk);
				x->desc[i + 2] = chave_promovida->filho;
				// printf("DESC 6: %d\n", x->desc[i+2]);
				x->num_chaves = x->num_chaves + 1;

				//COISAS A MAIS
				write_btree(x, norrn, 1);

				libera_no(x, 1);
				// free(chave_promovida);
				// free(extra);

				return(NULL);
			}
			else
			{
				libera_no(x, 0);
				// free(chave_promovida);
				// free(extra);

				return(divide_no_is(norrn, k, chave_promovida->filho));
			}
		}
		else
		{
			write_btree(x, norrn, 1);

			libera_no(x, 1);
			// free(chave_promovida);
			// free(extra);

			return(NULL);
		}
	}

	return(NULL);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//

ExtraIP *divide_no_ip(int xrrn, Carona k, int filho)
{
	int i;
	char chave_alocada;

	node_Btree_ip *x;
	node_Btree_ip *y;
	ExtraIP *chave_promovida;

	// printf("ENTROU NO DIVIDE IP\n");

	x = (node_Btree_ip*)read_btree(xrrn, 0);
	chave_promovida = (ExtraIP*)calloc(1, sizeof(ExtraIP));

	i = x->num_chaves - 1;
	chave_alocada = 0;

	y = (node_Btree_ip*) criar_no(0);
	y->folha = x->folha;
	y->num_chaves = (ordem_ip - 1) / 2;
	y->desc[0] = (-1);

	// printf("PK K NO DIVIDE IS: %s\n", k.pk);
	// printf("PK X NO DIVIDE IS: %s\n", x->chave[i].pk);

	for(int j = y->num_chaves - 1 ; j >= 0 ; j--)
	{
		if(!(chave_alocada) && strcmp(k.pk, x->chave[i].pk) > 0)
		{
			strcpy(y->chave[j].pk, k.pk);
			y->chave[j].rrn = k.rrn;
			y->desc[j + 1] = filho;
			chave_alocada = 1;
		}
		else
		{
			y->chave[j] = x->chave[i];
			y->desc[j + 1] = x->desc[i + 1];
			i = i - 1;
		}
	}

	if(!chave_alocada)
	{
		while((i >= 0) && strcmp(k.pk, x->chave[i].pk) < 0)
		{
			// x->chave[i + 1] = x->chave[i];
			strcpy(x->chave[i + 1].pk, x->chave[i].pk);
			x->chave[i + 1].rrn = x->chave[i].rrn;
			x->desc[i + 2] = x->desc[i + 1];
			i = i - 1;
		}

		strcpy(x->chave[i + 1].pk, k.pk);
		x->chave[i + 1].rrn = k.rrn;
		x->desc[i + 2] = filho;
	}

	strcpy(chave_promovida->chave.pk, x->chave[(ordem_ip) / 2].pk); //MUDEI AQUI
	chave_promovida->chave.rrn = x->chave[(ordem_ip / 2)].rrn;
	y->desc[0] = x->desc[(ordem_ip / 2) + 1];
	x->num_chaves = ordem_ip / 2;

	chave_promovida->filho = nregistrosip	;

	write_btree(x, xrrn, 0);
	write_btree(y, nregistrosip, 0);

	nregistrosip++;

	libera_no(x, 0);
	libera_no(y, 0);

	return(chave_promovida);
}

//-----------------------------------------------------------------//

ExtraIS *divide_no_is(int norrn, Carona k, int filho)
{
	int i;
	char chave_alocada;

	node_Btree_is *x;
	node_Btree_is *y;
	ExtraIS *chave_promovida;


	// sprintf(extra, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[0], k.data[1], k.data[3], k.data[4], k.data[6], k.data[7], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
	// strcpy(extra, k.string);

	x = (node_Btree_is*)read_btree(norrn, 1);
	// chave_promovida = (ExtraIS*)malloc(sizeof(ExtraIS));
	chave_promovida = (ExtraIS*)calloc(1, sizeof(ExtraIS));

	i = x->num_chaves - 1;
	chave_alocada = 0;

	y = (node_Btree_is*) criar_no(1);

	y->folha = x->folha;
	y->num_chaves = (ordem_is - 1) / 2;
	y->desc[0] = (-1);

	for(int j = y->num_chaves - 1 ; j >= 0 ; j--)
	{
		if(!(chave_alocada) && strcmp(k.string, x->chave[i].string) > 0)
		{
			strcpy(y->chave[j].pk, k.pk);
			strcpy(y->chave[j].string, k.string);
			y->desc[j + 1] = filho;
			chave_alocada = 1;

			// sprintf(y->chave[j].string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[0], k.data[1], k.data[3], k.data[4], k.data[6], k.data[7], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
		}
		else
		{
			y->chave[j] = x->chave[i];
			// sprintf(y->chave[j].string, "%s", extra);
			// sprintf(y->chave[j].pk, "%s", x->chave[i].pk);
			y->desc[j + 1] = x->desc[i + 1];

			i = i - 1;
		}
	}

	if(!chave_alocada)
	{
		while((i >= 0) && strcmp(k.string, x->chave[i].string) < 0)
		{
			x->chave[i + 1] = x->chave[i];
			x->desc[i + 2] = x->desc[i + 1];

			i = i - 1;
		}

		strcpy(x->chave[i + 1].pk, k.pk);
		strcpy(x->chave[i + 1].string, k.string);
		x->desc[i + 2] = filho;

		// strcpy(x->chave[i + 1].string, extra);

		// sprintf(y->chave[i + 1].string, "%s$%s%s", k.trajeto, k.data, k.hora);
		// sprintf(x->chave[i + 1].string, "%s$%c%c%c%c%c%c%c%c%c%c", traj2, k.data[0], k.data[1], k.data[3], k.data[4], k.data[6], k.data[7], k.hora[0], k.hora[1], k.hora[3], k.hora[4]);
		// x->chave[i + 1].rrn = rrn;
	}

	strcpy(chave_promovida->chave.pk, x->chave[(ordem_is) / 2].pk);
	strcpy(chave_promovida->chave.string, x->chave[(ordem_is) / 2].string);
	y->desc[0] = x->desc[(ordem_is / 2) + 1];
	x->num_chaves = ordem_is / 2;

	// chave_promovida->rrn = x->chave[(ordem_is / 2)].rrn;
	chave_promovida->filho = nregistrosis;
	// chave_promovida->chave.rrn = norrn;

	write_btree(x, norrn, 1);
	write_btree(y, nregistrosis, 1);

	nregistrosis++;

	libera_no(x, 1);
	libera_no(y, 1);

	// printf("SAIU DO DIVIDE IS\n");

	return(chave_promovida);
}

void buscar(Indice iprimary, Indice iride)
{
	int operacao;
	char pk[TAM_PRIMARY_KEY];
	char titulo[TAM_STRING_INDICE]; //TROQUEI AQUI
	int encontrouPK;
	char *encontrouTitulo;
	char trajeto[30];
	char data[TAM_DATA];
	char hora[TAM_HORA];

	scanf("%d%*c", &operacao);

	if(operacao == 1) //POR PK
	{
		scanf("%[^\n]%*c", pk);

		printf(NOS_PERCORRIDOS_IP, pk);
		// printf("\n");

		encontrouPK = buscar_chave_ip(iprimary.raiz, pk, 1);

		if(encontrouPK > -1)
		{
			printf("\n");
			exibir_registro(encontrouPK);
		}
		else
		{
			printf("\n");
			printf(REGISTRO_N_ENCONTRADO);
		}

	}
	else if(operacao == 2) //POR TRAJETO DATA E HORA
	{
		// scanf("%[^\n]%*c", titulo);
		scanf("%[^\n]%*c", trajeto);
		scanf("%[^\n]%*c", data);
		scanf("%[^\n]%*c", hora);

		sprintf(titulo, "%s$%c%c%c%c%c%c%c%c%c%c", trajeto, data[6], data[7], data[3], data[4], data[0], data[1], hora[0], hora[1], hora[3], hora[4]);

		printf(NOS_PERCORRIDOS_IS, titulo);
		// printf("\n");

		encontrouTitulo = buscar_chave_is(iride.raiz, titulo, 1);

		if(encontrouTitulo)
		{
			encontrouPK = (buscar_chave_ip(iprimary.raiz, encontrouTitulo, 0));
			printf("\n");
			exibir_registro(encontrouPK);
		}
		else
		{
			printf("\n");
			printf(REGISTRO_N_ENCONTRADO);
		}
	}
	else
		printf(OPCAO_INVALIDA);

}

//-----------------------------------------------------------------//

int buscar_chave_ip(int noderrn, char *pk, int exibir_caminho)
{
	//INT NODE É O RRN DA RAIZ
	node_Btree_ip *x;
	int i;
	int extra = 0;

	x = (node_Btree_ip*)read_btree(noderrn, 0);
	i = 0;

	// pk[10] = '\0';
	// x->chave[0].pk[10] = '\0';

	if(noderrn == (-1))
		return(-1);

	if(exibir_caminho) //CASO A GENTE USE O BUSCAR PARA IMPRIMIR AS COISAS
	{
		for(int j = 0 ; j < x->num_chaves ; j++)
		{
			printf("%s", x->chave[j].pk);

			if(j < x->num_chaves - 1) //CASO TENHA MAIS DE UMA CHVAVE NO NÓ
				printf(", ");
		}
		printf("\n");
	}

	while((i <= x->num_chaves - 1) && (strcmp(pk, x->chave[i].pk) > 0))
		i = i + 1;

	if((i <= x->num_chaves - 1) && (strcmp(pk, x->chave[i].pk) == 0))
		return(x->chave[i].rrn);

	if(x->folha == 'T')
	{
		libera_no(x, 0);
		return(-1);
	}
	else
	{
		extra = x->desc[i];
		libera_no(x, 0);
		return(buscar_chave_ip(extra, pk, exibir_caminho));
	}

	return(-1);
}

//-----------------------------------------------------------------//

char *buscar_chave_is(const int noderrn, const char *titulo, const int exibir_caminho)
{
	node_Btree_is *x;
	char extraString[TAM_STRING_INDICE];
	int i;
	int extra = 0;

	x = (node_Btree_is*)read_btree(noderrn, 1);
	strcpy(extraString, titulo);
	i = 0;

	if(noderrn == (-1))
		return(NULL);

	if(exibir_caminho) //CASO A GENTE USE O BUSCAR PARA IMPRIMIR AS COISAS
	{
		for(int j = 0 ; j < x->num_chaves ; j++)
		{
			printf("%s", x->chave[j].string);

			if(j < x->num_chaves - 1) //CASO TENHA MAIS DE UMA CHVAVE NO NÓ
				printf(", ");
		}
		printf("\n");
	}

	while((i <= x->num_chaves - 1) && strcmp(extraString, x->chave[i].string) > 0)
		i = i + 1;

	if((i <= x->num_chaves - 1) && strcmp(extraString, x->chave[i].string) == 0)
		return(x->chave[i].pk);

	if(x->folha == 'T')
	{
		libera_no(x, 1);
		return(NULL);
	}
	else
	{
		extra = x->desc[i]; //ATRIBUINDO PARA PODER LIBERAR O NO
		libera_no(x, 1);
		return(buscar_chave_is(extra, titulo, exibir_caminho));
	}

	return(NULL);
}

//-----------------------------------------------------------------//
//-----------------------------------------------------------------//
//FUNCAO PARA ALTERAR AS VAGAS DO NO

int alterar(Indice iprimary)
{
	char indicePrimario[TAM_PRIMARY_KEY];
	char novasVagas;
	int localizacao;
	int m;
	int extra2 = 0;
	int encontrouPK;

	scanf("\n%[^\n]s", indicePrimario); //RECEBENDO PRIMARIO QUE QUEREMOS

	encontrouPK = buscar_chave_ip(iprimary.raiz, indicePrimario, 0);

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
//LISTAR
void listar(Indice iprimary, Indice iride)
{
	// int inicio = 1;
	int operacao;

	scanf("%d%*c", &operacao);

	if(operacao == 1) //PRE ORDEM
	{
		if(iprimary.raiz == (-1))
		{
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}

		preOrdem(iprimary.raiz, 1);
	}
	else if(operacao == 2) //EM ORDEM
	{
		if(iride.raiz == (-1))
		{
			printf(REGISTRO_N_ENCONTRADO);
			return;
		}

		emOrdem(iride.raiz);
	}

	return;
}

void preOrdem(int rrn, int nivel)
{
	node_Btree_ip *extraIP;

	extraIP = (node_Btree_ip*)read_btree(rrn, 0);

	if(extraIP == NULL)
		return;

	printf("%d - ", nivel);

	for(int j = 0 ; j < extraIP->num_chaves ; j++)
	{
		printf("%s", extraIP->chave[j].pk);

		if(j < extraIP->num_chaves - 1) //CASO TENHA MAIS DE UMA CHVAVE NO NÓ
			printf(", ");
	}
	printf("\n");

	if(extraIP->folha == 'F')
		for(int i = 0 ; i <= extraIP->num_chaves ; i++)
			preOrdem(extraIP->desc[i], nivel + 1);

	libera_no(extraIP, 0);
	return;
}

void emOrdem(int rrn)
{
	node_Btree_is *extraIS;
	char *trajeto;
	char extraString[TAM_STRING_INDICE + 50];
	char *resto;
	int extra;
	int i;

	extraIS = (node_Btree_is*)read_btree(rrn, 1);

		for(i = 0 ; i < extraIS->num_chaves ; i++)
		{
			if(extraIS->folha == 'F')
				emOrdem(extraIS->desc[i]);

			strcpy(extraString, extraIS->chave[i].string);

			//CORTANDO OS CAMPOS QUE PRECISAMOS
			trajeto = strtok(extraString, "$");
			extra = strlen(trajeto);
			resto = strtok(NULL, "$");

			printf("%s", trajeto);

			for(int j = 0 ; j < (30 - extra) ; j++)
				printf("-");

			// printf("RESTO: %s", resto);
			// aammddhhmm
			// 0123456789
			printf(" %.2s/%.2s/%.2s - %.2s:%2s-", &resto[4], &resto[2], &resto[0], &resto[6], &resto[8]);

			// printf("-");
			// printf(" %2s:%2s-", &resto[], &resto[]);
			printf("\n");
		}
		if(extraIS->folha == 'F')
			emOrdem(extraIS->desc[i]);

		libera_no(extraIS, 1);
}
