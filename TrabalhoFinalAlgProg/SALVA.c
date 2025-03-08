/*#include "raylib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#define WIDTH 1200//largura da tela do jogo

#define HEIGHT 608//altura da tela do jogo
#define TAMANHO_MUNDO 3200//200*16 pixeis máximos + boss
#define MIN_ITENS 15
#define MAX_ITENS 20
#define MAX_LETRAS 10
#define MAX_JOGADOR 5


//estruturas
typedef struct espaco
{
	char nome[MAX_LETRAS + 1];
	int pontuacao;
	bool on;
	int ganhou;
}ESPACO;

typedef struct tiro {
	int x;
	int y;
	int vx;
	bool atira;
	float tempotiro;

}TIRO;

typedef struct jogador
{
	char nome[MAX_LETRAS + 1];
	int pontuacao;
}JOGADOR;


typedef struct inimigo {
	int x;
	int y;
	int vx;
	int vy;
	Texture2D inimigotexture;
	char direcao;
	int xoriginal;
	int yoriginal;
}INIMIGO;

typedef struct personagem {//personagem
	int x;
	int y;
	int altura;
	int largura;
	int vy;
	int vx;
	Texture2D textureD;
	Texture2D textureE;
	Color cor;
	int vidas;
	char direcao;

}PERSONAGEM;

typedef struct bloco {
	int altura;
	int largura;
	int x;
	int y;
	Color cor;
	char tipo;
	Texture2D spike;
	Texture2D bloconormal;
	Texture2D item;
	Texture2D gosma;
	char direcao;
}BLOCO;


void atira(PERSONAGEM personagem, TIRO* atirar) {

	atirar->x = personagem.x; //tiro recebe a posicao que o personagem ta
	atirar->y = personagem.y + 10;
	atirar->atira = true;  //caso o boneco atire, o tiro fica verdadeiro


}

void atualizatiro(TIRO* atirar, float time) {

	float tempoPraAtira = GetTime() - time; //peganos o tempo no qual o tiro ainda pode ficar na tela, sendo o get time de agora com o get time quando o tiro é disparado, diminui um do outro e temos o tempo que pode atirar

	if (tempoPraAtira > atirar->tempotiro) {//aqui vemos se o tempo do tiro ativo é maior que o tempo do tiro (0,3) se for o tiro nao é mais verdadeiro e nao aparece mais na tela
		atirar->atira = false;
	}

	if (atirar->atira == true) //se o tiro for verdadeiro, ele recebe uma velocidade (15);
		atirar->x += atirar->vx;

}

void colisaotiro(TIRO* atirar, INIMIGO inimigos[], int ninimigos, ESPACO* espaco, BLOCO mundo[38][200]) {

	Vector2 t = { atirar->x, atirar->y };//posicao do tiro

	for (int i = 0; i < ninimigos; i++) {//percorremos a array de inimigos e checamos se ouve colisao do tiro com algum inimigo da array
		Rectangle m = {//criamos o retangulo do inimigo para usar na funcao checkcollision
			.height = 20,
			.width = 16,
			.x = inimigos[i].x,
			.y = inimigos[i].y,
		};

		if (CheckCollisionPointRec(t, m)) {//caso tenha colisao, o inimigo some da tela e a pontuacao do personagem aumenta
			inimigos[i].x = -100;
			inimigos[i].vx = 0;
			inimigos[i].vy = 0;
			inimigos[i].direcao = 'o';

			espaco->pontuacao += 100;
		}
	}

	int x = atirar->x / 16; //caso o x ou o y do tiro passar de ate onde tem blobo ( 0 ate 199 / 0 ate 37) esse x é corrigido para os pontos max e min de bloco
	if (x > 199) {
		x = 199;
	}
	if (x < 0) {
		x = 0;
	}
	int y = atirar->y / 16;
	if (y > 37) {
		y = 37;
	}
	if (y < 0) {
		y = 0;
	}

	if (mundo[y][x].tipo == 'B' || mundo[y][x].tipo == 'O' || mundo[y][x].tipo == 'W') { //caso o tiro x e y do tiro esteja na posicao de um bloco, o tiro some
		atirar->atira = false;
	}
}


void le_arq(JOGADOR top5[MAX_JOGADOR], char nomearq[]) {

	JOGADOR a;
	FILE* arq;
	int erro = false;

	//le  o arquivo 

	if (!(arq = fopen(nomearq, "rb"))) {
		printf("ERRO NA ABERTURA(LEITURA)\n");
	}
	else {
		for (int i = 0; i < MAX_JOGADOR; i++) {
			if (fread(&a, sizeof(JOGADOR), 1, arq) == 1) {
				top5[i] = a;
				erro = false;
			}
			else {
				erro = true;
			}
		}
		fclose(arq);
		if (erro == true) {
			printf("ERRO NA LEITURA");
		}

	}
}

void ordena_escreve_arq(ESPACO espaco, JOGADOR top5[MAX_JOGADOR], char nomearq[]) {

	FILE* arq;
	JOGADOR novojogador = { " ", espaco.pontuacao };
	int erro = false;


	strcpy(novojogador.nome, espaco.nome);


	//adiciona o novo jogador na lista se ele for maior que o menor de todos(isso é para nao colocar no ranking alguem que teve a pontuacao menor que o top5)
	if (novojogador.pontuacao > top5[MAX_JOGADOR - 1].pontuacao && espaco.ganhou == 1) {
		top5[MAX_JOGADOR - 1] = novojogador;
	}


	//ordena a array
	JOGADOR temp = { 0 };

	for (int i = 0; i < MAX_JOGADOR - 1; i++) {
		for (int j = i + 1; j < MAX_JOGADOR; j++) {
			if (top5[j].pontuacao > top5[i].pontuacao) {
				temp = top5[i];
				top5[i] = top5[j];
				top5[j] = temp;
			}
		}
	}

	//salva no bin

	if (!(arq = fopen(nomearq, "wb"))) {
		printf("ERRO NA ABERTURA (ESCRITA)");
	}
	else {
		for (int i = 0; i < MAX_JOGADOR; i++) {
			if (fwrite(&top5[i], sizeof(JOGADOR), 1, arq) != 1) {
				erro = true;
			}
		}
		fclose(arq);
		if (erro == true) {
			printf("ERRO NA ESCRITA");
		}


	}


}


int EscreveTela(ESPACO* espaco, Texture2D digitenome) {//tela antes do jogo realmente comecar, em que ele salva o nome do jogador

	static int contletras = 0;
	int tecla = 0;


	if (IsKeyPressed(KEY_BACKSPACE) && contletras > 0) {//se clicar na tecla backspace a letra sai e o nome fica do tamanho desejado
		contletras--;
		espaco->nome[contletras] = '\0';
	}
	else if (contletras < MAX_LETRAS) {//caso nao seja backspace entao escreve a letra
		tecla = GetKeyPressed();//pega a tecla pressionada
	}

	if ((tecla >= 32) && (tecla <= 125)) {//so deixa escrever as teclas validas da raylib
		espaco->nome[contletras] = (char)tecla;//o nome do personagem vai receber a letra digitada
		contletras++;
		espaco->nome[contletras] = '\0';//dx o /0 no ultimo carater;
	}

	DrawTexture(digitenome, 280, 20, RAYWHITE);
	DrawText("Clique em ENTER para jogar", 450, 460, 20, DARKPURPLE);
	DrawText("Clique em TAB para volar", 460, 500, 20, DARKPURPLE);
	DrawText(espaco->nome, 510, 300, 30, BLACK);//desenha as letras

	if (contletras != 0) {//caso tenha um numero no contador (tem letra na tela) retorna 1
		return 1;
	}

	return 0;//se nao tiver nada na tela (contletras == 0) retorna 0

}



void colisao(PERSONAGEM personagem, BLOCO mundo[38][200], BLOCO blococolisao[3][3], BLOCO padrao) {
	for (int i = 0;i < 3;i++) {
		for (int j = 0;j < 3;j++) {
			blococolisao[i][j] = padrao;
			blococolisao[i][j].tipo = 'U';
		}
	}

	int maxl = ((personagem.y) / 16) + 2;  // Possíveis tiles para colisão
	int minl = ((personagem.y) / 16);
	if (minl < 0) {
		minl = 0;
	}
	if (maxl > 37) {  // Garante acessos seguros
		maxl = 37;
	}

	int maxc = ((personagem.x) / 16) + 2;
	int minc = ((personagem.x) / 16);
	if (minc < 0) {
		minc = 0;
	}
	if (maxc > 199) {
		maxc = 199;
	}

	Rectangle p = {
		.height = personagem.altura,
		.width = personagem.largura,
		.x = personagem.x,
		.y = personagem.y,
	};

	int l = 0;


	for (int linha = minl; linha <= maxl; linha++) {
		int c = 0;
		for (int coluna = minc; coluna <= maxc; coluna++) {
			// Verificar se o bloco atual é válido para colisão
			if (mundo[linha][coluna].tipo != ' ') {
				Rectangle b = {
					.height = 16,
					.width = 16,
					.x = mundo[linha][coluna].x,
					.y = mundo[linha][coluna].y,
				};

				if (CheckCollisionRecs(p, b)) {

					blococolisao[l][c] = mundo[linha][coluna];  // Copia o bloco com colisão
				}
				//printf("%d ", blococolisao[l][c].x);

			}
			c++;
		}
		//printf("\n");
		l++;

	}
}

void random_item(BLOCO mundo[38][200]) {
	srand(time(NULL));
	int linha;
	int coluna;
	int qitens = (int)MIN_ITENS + (rand() % (MAX_ITENS - MIN_ITENS + 1));
	for (int l = 0;l < qitens;l++) {
		do {
			linha = (int)22 + (rand() % (31 - 22 + 1));
			coluna = (int)0 + (rand() % (199 - 0 + 1));

		} while (mundo[linha][coluna].tipo != ' ');

		mundo[linha][coluna].x = coluna * 16;
		mundo[linha][coluna].y = linha * 16;
		mundo[linha][coluna].altura = 16;
		mundo[linha][coluna].largura = 16;
		mundo[linha][coluna].cor = RED;
		mundo[linha][coluna].tipo = 'I';
	}
}


void achouitem(BLOCO item, ESPACO* espaco, BLOCO mundo[38][200], Sound itemsom) {
	mundo[item.y / 16][item.x / 16].x = -70;
	PlaySound(itemsom);
	mundo[item.y / 16][item.x / 16].tipo = 'i';
	espaco->pontuacao += 50;
}



void inimigos_movimentacao(int ninimigos, INIMIGO inimigos[], BLOCO mundo[38][200]) {
	int tile;
	int y;

	for (int i = 0;i < ninimigos;i++) {
		tile = inimigos[i].x / 16;
		y = inimigos[i].y / 16;
		if (!(tile + 1 >= 200 || tile - 1 < 0 || y + 1 >= 38 || y - 1 < 0)) {
			if (i % 2 == 0) {
				inimigos[i].x += inimigos[i].vx;

				if (inimigos[i].direcao == 'd') {

					if (mundo[y][tile + 1].tipo == 'B' || mundo[y][tile + 1].tipo == 'O' || mundo[y][tile + 1].tipo == 'P') {
						inimigos[i].x = mundo[y][tile + 1].x - 16;
						inimigos[i].vx = -1;
						inimigos[i].direcao = 'e';
					}
					if (inimigos[i].x > inimigos[i].xoriginal + 100) {
						inimigos[i].x = inimigos[i].xoriginal + 100;
						inimigos[i].vx = -1;
						inimigos[i].direcao = 'e';
					}




				}
				else if (inimigos[i].direcao == 'e') {
					if (mundo[y][tile].tipo == 'B' || mundo[y][tile].tipo == 'O' || mundo[y][tile].tipo == 'P') {
						inimigos[i].x = mundo[y][tile].x + 16;
						inimigos[i].vx = 1;
						inimigos[i].direcao = 'd';
					}
					if (inimigos[i].x < inimigos[i].xoriginal - 100) {
						inimigos[i].x = inimigos[i].xoriginal - 100;
						inimigos[i].vx = 1;
						inimigos[i].direcao = 'd';
					}




				}
			}
			else {
				tile = inimigos[i].x / 16;
				y = inimigos[i].y / 16;

				inimigos[i].y += inimigos[i].vy;

				if (inimigos[i].direcao == 'c') {

					if (mundo[y][tile].tipo == 'B' || mundo[y][tile].tipo == 'O' || mundo[y][tile].tipo == 'P') {
						inimigos[i].y = mundo[y][tile].y + 16;
						inimigos[i].vy = 1;
						inimigos[i].direcao = 'b';
					}
					if (inimigos[i].y < inimigos[i].yoriginal - 100) {
						inimigos[i].y = inimigos[i].yoriginal - 100;
						inimigos[i].vy = 1;
						inimigos[i].direcao = 'b';
					}




				}
				else if (inimigos[i].direcao == 'b') {
					if (mundo[y + 1][tile].tipo == 'B' || mundo[y + 1][tile].tipo == 'O' || mundo[y + 1][tile].tipo == 'P') {
						inimigos[i].y = mundo[y + 1][tile].y - 16;
						inimigos[i].vy = -1;
						inimigos[i].direcao = 'c';
					}
					if (inimigos[i].y > inimigos[i].yoriginal + 100) {
						inimigos[i].y = inimigos[i].yoriginal + 100;
						inimigos[i].vy = -1;
						inimigos[i].direcao = 'c';
					}
				}
			}
		}
	}
}

int inimigos_colisao(PERSONAGEM* personagem, INIMIGO inimigos[], int ninimigos) {
	Rectangle p = {
		.height = personagem->altura,
		.width = personagem->largura,
		.x = personagem->x,
		.y = personagem->y,
	};

	for (int i = 0; i < ninimigos; i++) {//percorremos a array de inimgios e vemos se teve colisao com o persinagem, caso tenha a vida do personagem perde 3 e ele morre
		Rectangle m = {
			.height = 20,
			.width = 16,
			.x = inimigos[i].x,
			.y = inimigos[i].y,
		};

		if (CheckCollisionRecs(p, m)) {
			personagem->vidas += -3;
		}
	}

}

void personagemy(PERSONAGEM* personagem, BLOCO blococolisao[3][3], ESPACO* espaco, BLOCO mundo[38][200], Sound itemsom) {

	for (int y = 0;y < 3;y++) {
		for (int x = 0;x < 3;x++) {
			if (blococolisao[y][x].y != 0) {
				switch (blococolisao[y][x].tipo) {
				case 'O':
					if (personagem->vy >= 0) { // Verifica se o personagem está caindo ou parado
						personagem->y = blococolisao[y][x].y + blococolisao[y][x].altura - personagem->altura;
						personagem->vy = 0; // Reseta a velocidade vertical

					}
					else {
						// Caso o personagem esteja subindo, ajusta conforme necessário
						personagem->y = blococolisao[y][x].y + blococolisao[y][x].altura;
						personagem->vy = 0; // Inverte a velocidade

					}
					break;
				case 'B':
					if ((*personagem).y <= blococolisao[y][x].y && personagem->vy > 0) {//significa que está em cima e colisão veio de cima
						(*personagem).y = blococolisao[y][x].y - personagem->altura;
						personagem->vy = 0;
					}
					else if ((*personagem).y >= blococolisao[y][x].y && personagem->vy < 0) {//personagem está em baixo do bloco veio de um pulo
						personagem->y = blococolisao[y][x].y + blococolisao[y][x].altura;
						personagem->vy *= -1;
					}
					break;
				case 'I':
					if (personagem->y - personagem->altura > blococolisao[y][x].y)
						achouitem(blococolisao[y][x], espaco, mundo, itemsom);
					break;
				case'W':
					if ((*personagem).y <= blococolisao[y][x].y && personagem->vy > 0) {//significa que est� em cima e colis�o veio de cima
						(*personagem).y = blococolisao[y][x].y - personagem->altura;
						personagem->vy = 0;
					}
					else if ((*personagem).y >= blococolisao[y][x].y && personagem->vy < 0) {//personagem est� em baixo do bloco veio de um pulo
						personagem->y = blococolisao[y][x].y + blococolisao[y][x].altura;
						personagem->vy = 0;
					}
					break;

				}
			}
		}
	}

	if (personagem->y < 288) {
		personagem->y = 288;
		personagem->vy = 1;

	}

}

int personagemx(PERSONAGEM* personagem, BLOCO blococolisao[3][3], ESPACO* espaco, BLOCO mundo[38][200], INIMIGO inimigos[], int ninimigos, Sound itemsom) {
	int vida = 0;
	if (personagem->vx >= 0 && personagem->direcao == 'd') {//indo para a direita, então tem que conferir se colide com algum bloco e colocar ele no bloco mais a esquerda que colidir
		for (int coluna = 2;coluna >= 0;coluna--) {
			for (int linha = 0; linha < 2; linha++) {
				switch (blococolisao[linha][coluna].tipo) {
				case 'O':
					vida = -1;
					personagem->x = blococolisao[linha][coluna].x - personagem->largura - 16;
					break;
				case 'B':
					personagem->x = blococolisao[linha][coluna].x - personagem->largura;
					personagem->vx = 0;
					break;
				case 'I':
					achouitem(blococolisao[linha][coluna], espaco, mundo, itemsom);
					break;
				case 'W':
					personagem->x = blococolisao[linha][coluna].x - personagem->largura;
					personagem->vx = 0;
					break;
				}
			}
		}

	}
	else if (personagem->vx <= 0 && personagem->direcao == 'e') {
		for (int coluna = 0;coluna < 2;coluna++) {
			for (int linha = 0; linha < 2; linha++) {
				switch (blococolisao[linha][coluna].tipo) {
				case 'O':
					vida = -1;
					personagem->x = blococolisao[linha][coluna].x + blococolisao[linha][coluna].largura + 32;
					break;
				case 'B':
					personagem->x = blococolisao[linha][coluna].x + blococolisao[linha][coluna].largura;
					personagem->vx = 0;
					break;
				case 'I':
					achouitem(blococolisao[linha][coluna], espaco, mundo, itemsom);
					break;
				case 'W':
					personagem->x = blococolisao[linha][coluna].x + blococolisao[linha][coluna].largura;
					personagem->vx = 0;
					break;
				}
			}
		}
	}


	if (personagem->x < 0) {
		personagem->x = 0;
	}
	return vida;

}

void RandonStars(int numStars, int seed) {
	srand(seed); // Garante que os números aleatórios sejam sempre os mesmos

	for (int i = 0; i < numStars; i++) {
		int x = rand() % TAMANHO_MUNDO; // o pixel vai ser denenhado num ponto aleatorio dentro do tamanho do mundo 
		int y = rand() % HEIGHT;
		DrawPixel(x, y, WHITE);
	}
}




void desenhar_jogo(BLOCO mundo[38][200], PERSONAGEM personagem, ESPACO espaco, int ninimigos, INIMIGO inimigos[]) {
	int quantestrelas = 200;
	int seed = 763286;
	ClearBackground(BLACK);
	RandonStars(quantestrelas, seed);

	for (int linha = 0; linha < 38; linha++) {
		for (int coluna = 0; coluna < 200; coluna++) {
			switch (mundo[linha][coluna].tipo) {
			case 'B':
				DrawTexture(mundo[linha][coluna].bloconormal, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'O':
				DrawTexture(mundo[linha][coluna].spike, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'I':
				DrawTexture(mundo[linha][coluna].item, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'W':
				DrawTexture(mundo[linha][coluna].gosma, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			}
		}
	}

	for (int inimigo = 0;inimigo < ninimigos;inimigo++) {
		DrawTexture(inimigos[inimigo].inimigotexture, inimigos[inimigo].x, inimigos[inimigo].y, RAYWHITE);
	}



	if (personagem.direcao == 'd') {
		DrawTexture(personagem.textureD, personagem.x, personagem.y, personagem.cor);
	}
	else if (personagem.direcao == 'e') {
		DrawTexture(personagem.textureE, personagem.x, personagem.y, personagem.cor);
	}



}


void desenhar_jogo2(BLOCO mundo[38][200], PERSONAGEM personagem, PERSONAGEM personagem2, ESPACO espaco, int ninimigos, INIMIGO inimigos[]) {
	int quantestrelas = 200;
	int seed = 763286;
	ClearBackground(BLACK);
	RandonStars(quantestrelas, seed);

	for (int linha = 0; linha < 38; linha++) {
		for (int coluna = 0; coluna < 200; coluna++) {
			switch (mundo[linha][coluna].tipo) {
			case 'B':
				DrawTexture(mundo[linha][coluna].bloconormal, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'O':
				DrawTexture(mundo[linha][coluna].spike, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'I':
				DrawTexture(mundo[linha][coluna].item, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			case 'W':
				DrawTexture(mundo[linha][coluna].gosma, mundo[linha][coluna].x, mundo[linha][coluna].y, RAYWHITE);
				break;
			}
		}
	}

	for (int inimigo = 0;inimigo < ninimigos;inimigo++) {
		DrawTexture(inimigos[inimigo].inimigotexture, inimigos[inimigo].x, inimigos[inimigo].y, RAYWHITE);
	}


	if (personagem.direcao == 'd') {
		DrawTexture(personagem.textureD, personagem.x, personagem.y, personagem.cor);
	}
	else if (personagem.direcao == 'e') {
		DrawTexture(personagem.textureE, personagem.x, personagem.y, personagem.cor);
	}
	if (personagem2.direcao == 'd') {
		DrawTexture(personagem2.textureD, personagem2.x, personagem2.y, personagem2.cor);
	}
	else if (personagem2.direcao == 'e') {
		DrawTexture(personagem2.textureE, personagem2.x, personagem2.y, personagem2.cor);
	}





}


void camera_movimentacao(PERSONAGEM personagem, Camera2D* camera) {

	camera->target.x = personagem.x;

	camera->offset.x = WIDTH / 2;
	camera->target.y = 528;
	camera->offset.y = 528;

	if (camera->target.x - camera->offset.x < 0) {
		camera->offset.x = camera->target.x;
	}

	if (personagem.x + 1200 / 4 > 3200) {
		camera->target.x = 2900; // Limitaao final do mapa
	}

}



int ler_mundo(BLOCO mundo[38][200], INIMIGO inimigos[], int* ninimigos, PERSONAGEM* personagem, PERSONAGEM* personagem2, Texture2D inimigomacho, Texture2D inimigofemea) {
	*ninimigos = 0;
	FILE* filetxt = fopen("mapa.txt", "r+");
	if (filetxt == NULL) {
		return -1;
	}
	else {
		//Leitura do mapa linha a linha
		char c;
		char l[200 + 2];
		int linha = 23;
		while (!feof(filetxt)) {
			if (fgets(l, 200 + 2, filetxt) != NULL) {
				l[201] = '\0';
				for (int coluna = 0;coluna < 200;coluna++) {
					c = l[coluna];
					mundo[linha][coluna].altura = 16;
					mundo[linha][coluna].largura = 16;
					mundo[linha][coluna].x = coluna * 16;
					mundo[linha][coluna].y = linha * 16;
					mundo[linha][coluna].tipo = c;
					switch (c) {
					case 'B':
						mundo[linha][coluna].cor = BLUE;
						break;

					case 'O':
						mundo[linha][coluna].cor = GREEN;
						break;

					case 'M':
						inimigos[*ninimigos].x = coluna * 16;
						inimigos[*ninimigos].y = linha * 16;
						inimigos[*ninimigos].xoriginal = coluna * 16;
						inimigos[*ninimigos].yoriginal = linha * 16;
						inimigos[*ninimigos].vx = 0;
						inimigos[*ninimigos].vy = 0;
						if ((*ninimigos) % 2 == 0) {
							inimigos[*ninimigos].inimigotexture = inimigomacho;
							inimigos[*ninimigos].vx = 1;
							inimigos[*ninimigos].direcao = 'd';
						}
						else {
							inimigos[*ninimigos].inimigotexture = inimigofemea;
							inimigos[*ninimigos].vy = 1;
							inimigos[*ninimigos].direcao = 'b';
						}
						(*ninimigos)++;
						break;

					case 'P':
						(*personagem).x = coluna * 16;
						(*personagem).y = linha * 16;
						(*personagem2).x = coluna * 16;
						(*personagem2).y = linha * 16;
						break;
					case'W':
						mundo[linha][coluna].cor = DARKGREEN;
						break;
					}
				}
			}linha++;
		}


		fclose(filetxt);

		//itens aleatorios

		random_item(mundo);

		return 1;
	}
}


void menu(Texture2D buttonexit, Texture2D buttonleaderboard, Texture2D buttonstart, Texture2D buttonmultiplayer, int* estadobutton) {

	const float buttonWidth = 204.0;//dimensoes dos botoes
	const float buttonHeight = 115.0;

	float colunaX = 490.0;//onde os botoes vao ficar na tela
	float linha1X = 200.0;
	float linha2X = 300.0;
	float linha3X = 400.0;
	float linha4X = 500.0;


	//dimensoes dos botoes e seus lugares para checara colisao
	Rectangle buttonstartbounds = { colunaX, linha1X, buttonWidth, buttonHeight };
	Rectangle buttonmultiplayerbounds = { colunaX, linha2X, buttonWidth, buttonHeight };
	Rectangle buttonleaderboardbounds = { colunaX, linha3X, buttonWidth, buttonHeight };
	Rectangle buttonexitbounds = { colunaX, linha4X, buttonWidth, buttonHeight };



	Vector2 mousePosition = GetMousePosition(); //onde o mouse esta

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {//caso a pessoa clique com o botao esquerdo do mouse
		if (CheckCollisionPointRec(mousePosition, buttonexitbounds)) {//o estado do botao sera 1, assim fechando o jogo
			*estadobutton = 1;
		}
		else if (CheckCollisionPointRec(mousePosition, buttonleaderboardbounds)) {//o estado do botao sera 2, indo para a leaderboard
			*estadobutton = 2;
		}
		else if (CheckCollisionPointRec(mousePosition, buttonstartbounds)) {//estado 3, para o jogo normal
			*estadobutton = 3;
		}
		else if (CheckCollisionPointRec(mousePosition, buttonmultiplayerbounds)) {//estado 4, para o multiplayer
			*estadobutton = 4;
		}
	}

	DrawTexture(buttonstart, colunaX, linha1X, RAYWHITE); // Botão Start
	DrawTexture(buttonmultiplayer, colunaX, linha2X, RAYWHITE); //Botão Multiplayer
	DrawTexture(buttonleaderboard, colunaX, linha3X, RAYWHITE);//Botão Leaderboard
	DrawTexture(buttonexit, colunaX, linha4X, RAYWHITE);  // Botão Exit






}



int main() {
	//Declaracoes
	SetTargetFPS(60);
	InitWindow(WIDTH, HEIGHT, "INF-WOMAN");
	//música
	InitAudioDevice();

	Music music = LoadMusicStream("musica.mp3");
	SetMusicVolume(music, 0.5f);

	PlayMusicStream(music);
	float timePlayed = 0.0f;
	int super = -1;





	BLOCO padrao = { .altura = 16,.largura = 16,.x = 0,.y = 0,.tipo = ' ' };
	int vida;
	int vida2;
	BLOCO blococolisao[3][3];
	BLOCO mundo[38][200];//blocos normais
	int tilepy;
	char escrita[1000];


	for (int i = 0; i < 38; i++) {//inicia os blobos da matriz mudno como padrao
		for (int j = 0; j < 200; j++) {
			mundo[i][j] = padrao;
		}
	}


	//Jogo
	ESPACO espaco = { " ", 0, true };
	ESPACO novoespaco = { " ", 0, true };

	//pngs
	Texture2D characterDD = LoadTexture("dudaparadadisparadireita.png");
	Texture2D characterD = LoadTexture("dudaparadadireita.png");
	Texture2D characterE = LoadTexture("dudaparadaesquerda.png");
	Texture2D character2D = LoadTexture("manuparadadireita.png");
	Texture2D character2E = LoadTexture("manuparadaesquerda.png");
	Texture2D gameover = LoadTexture("gameover.png");
	Texture2D fundoinicial = LoadTexture("fundotelainicial.png");
	Texture2D buttonexit = LoadTexture("exitbutton.png");
	Texture2D buttonstart = LoadTexture("startbutton.png");
	Texture2D buttonleaderboard = LoadTexture("leaderboardbutton.png");
	Texture2D buttonmultiplayer = LoadTexture("multiplayerbutton.png");
	Texture2D infwoman = LoadTexture("infwomandiva.png");
	Texture2D spike = LoadTexture("spikegreen.png");
	Texture2D bloconormal = LoadTexture("bloconormal.png");
	Texture2D item = LoadTexture("itemestrela.png");
	Texture2D rankleaderboard = LoadTexture("leaderboard.png");
	Texture2D digitenome = LoadTexture("digitenome.png");
	Texture2D inimigomacho = LoadTexture("inimigomacho.png");
	Texture2D inimigofemea = LoadTexture("inimigofemea.png");
	Texture2D gosma = LoadTexture("blocoslime.png");
	Sound tirosom = LoadSound("tiro.wav");
	Sound itemsom = LoadSound("item.wav");

	//personagens
	PERSONAGEM personagem = { WIDTH / 2,HEIGHT / 2, 24, 24, 0, 0, characterD, characterE, RAYWHITE, 3, 'd' };
	PERSONAGEM personagem2 = { WIDTH / 2,HEIGHT / 2, 24, 24, 0, 0, character2D, character2E, RAYWHITE, 3, 'd' };

	//jogador para o bin
	JOGADOR top5[MAX_JOGADOR] = { 0 };

	//nome arq bin
	char nomearq[500] = "jogadores.bin";

	//tiros
	TIRO atirar = { 0 , 0, 0, false, 0.3f };
	float time;
	TIRO atirar2 = { 0 , 0, 0, false, 0.3f };
	float time2;



	for (int i = 0; i < 38; i++) {//coloca os pngs nos campos texture do mundo tipo BLOCO
		for (int j = 0; j < 200; j++) {
			mundo[i][j].spike = spike;
			mundo[i][j].bloconormal = bloconormal;
			mundo[i][j].item = item;
			mundo[i][j].gosma = gosma;

		}
	}


	INIMIGO inimigos[2000];//inimigos
	int ninimigos = 0;

	//camera
	Vector2 camera_offset = { .x = 0,.y = 0 };
	Vector2 camera_target = { .x = 0,.y = 0 };
	Camera2D camera = { .offset = camera_offset,.target = camera_target,.rotation = 0,.zoom = 2 };


	int estadobutton = 0;


	ler_mundo(mundo, inimigos, &ninimigos, &personagem, &personagem2, inimigomacho, inimigofemea);



	while (espaco.on && !WindowShouldClose()) {


		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTexture(fundoinicial, 0, 0, RAYWHITE);//desenha o fundo

		switch (estadobutton) {//verifica o estado do botao para cada botao

		case 0:
			DrawTexture(infwoman, 250, 20, RAYWHITE);
			menu(buttonexit, buttonleaderboard, buttonstart, buttonmultiplayer, &estadobutton);//escreve o menu e modifica o estado do botao
			break;
		case 1:
			espaco.on = false;//o aqui encerra o loop, fechando a janela
			break;
		case 2:
			DrawTexture(rankleaderboard, 300, 50, RAYWHITE);//leaderboard
			le_arq(top5, nomearq);//lemos o binario para conseguir colocar o que tem no bin na array top5 e escrever na tela essa array



			int x = 400;
			int y = 280;

			DrawText("NOME", 390, 200, 45, BLACK);
			DrawText("PONTOS", 600, 200, 45, BLACK);

			for (int i = 0; i < MAX_JOGADOR; i++) {//escreve na tela a pontuacao e o nome de cada pessoa/dupla do bin
				DrawText(top5[i].nome, x, y, 30, DARKPURPLE);
				sprintf(escrita, "%d", top5[i].pontuacao);
				DrawText(escrita, x + 300, y, 30, DARKPURPLE);

				y += 50;
			}

			DrawText("PRESSIONE TAB PARA VOLTAR", 450, 550, 18, BLACK);//caso clique em TAB o estado fica 0 e volta para o menu
			if (IsKeyPressed(KEY_TAB)) {
				estadobutton = 0;
			}
			break;

		case 3:

			EscreveTela(&espaco, digitenome);//entra a tela paradigitar o nome do jogador
			if (IsKeyPressed(KEY_TAB)) {//caso TAB for clicado o estado fica 0 e volta para o menu
				estadobutton = 0;
			}
			if (IsKeyPressed(KEY_ENTER) && EscreveTela(&espaco, digitenome)) { //caso a tecla enter for precisonada e a funcao retornar 1 (tem algo escrito na tela) o estado fica 5, comecando o jogo
				estadobutton = 5;
			}
			break;
		case 4:

			EscreveTela(&espaco, digitenome);//tela para dogotar nome da dupla
			if (IsKeyPressed(KEY_TAB)) {//volta menu cado TAB
				estadobutton = 0;
			}
			if (IsKeyPressed(KEY_ENTER) && EscreveTela(&espaco, digitenome)) {//caso a tecla enter for pressionada e a funcao retornar 1(tem algo na tela) o estado fica 6, chamando o jogo multiplayer
				estadobutton = 6;
			}
			break;
		case 5://jogo
			UpdateMusicStream(music);
			timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
			if (timePlayed > 1.0f) timePlayed = 1.0f;
			inimigos_movimentacao(ninimigos, inimigos, mundo);
			if (IsKeyPressed(KEY_Z)) {
				super *= -1;
			}

			if (personagem.x > 3200) {//caso o jogador passou dos 3200 pixeis (terminou o mapa) a tela ganhou aparece

				ClearBackground(RAYWHITE);
				DrawTexture(fundoinicial, 0, 0, RAYWHITE);
				DrawText("VOCE GANHOU", 300, 200, 80, DARKPURPLE);
				const float buttonWidth = 204.0;
				const float buttonHeight = 115.0;
				//dimensoes dos botoes
				Rectangle buttonexitbounds = { 500.0, 300.0, buttonWidth, buttonHeight };
				Vector2 mousePosition = GetMousePosition();

				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					if (CheckCollisionPointRec(mousePosition, buttonexitbounds)) {
						estadobutton = 1;
					}
				}

				DrawTexture(buttonexit, 500, 300, RAYWHITE);


				if (WindowShouldClose()) {
					estadobutton = 1;
				}

				espaco.ganhou = 1; //ele ganhou entao o bin da escrita seja feito


			}
			else {
				vida = 0;

				BeginMode2D(camera);
				camera_movimentacao(personagem, &camera);





				//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
				tilepy = ((personagem.y) / 16);
				bool podepular = false;
				if (personagem.vy == 0 && personagem.y > 376 && (mundo[tilepy + 2][((personagem.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 2].tipo == 'B')) {//permite que o personagme s� pule quando n�o estiver voando e quando estiver em um bloco;
					podepular = true;
				}
				if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && personagem.y > 376 && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
					podepular = true;
				}
				else {
					podepular = false;
				}

				if (personagem.vy == 0 && podepular == true) {
					if (IsKeyPressed(KEY_UP)) {
						personagem.vy = -19;
					}
				}
				personagem.vy += 1;
				personagem.y += personagem.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


				if (IsKeyPressed(KEY_SPACE) && atirar.atira == false) {//caso o espaco seja clicado e o atira for falso, entao o tiro recebe uma velocida (pos ou neg dependendo da direcao do personagem)
					if (personagem.direcao == 'd') {
						atirar.vx = 12;
					}
					else if (personagem.direcao == 'e') {
						atirar.vx = -12;
					}

					PlaySound(tirosom);//toca o som do tiro
					atira(personagem, &atirar);//chama  a funcao atira
					time = GetTime();//o tempo recebe o tempo do inicio do jogo ate o tempo que a tecla espaco foi acionanda
				}


				if (atirar.atira == true) {//se o tiro for verdadeiro entao o tiro é atualizado e desenhado na tela
					atualizatiro(&atirar, time);
					DrawPixel(atirar.x, atirar.y, PINK);
				}

				colisaotiro(&atirar, inimigos, ninimigos, &espaco, mundo);//testa para ver se tem colisao do tiro com um bloco

				colisao(personagem, mundo, blococolisao, padrao);
				personagemy(&personagem, blococolisao, &espaco, mundo, itemsom);



				personagem.vx = 0;



				if (IsKeyDown(KEY_RIGHT)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
						personagem.vx = 4;
						personagem.direcao = 'd';
					}
					else {
						personagem.vx = 2;
						personagem.direcao = 'd';
					}
				}
				else if (IsKeyDown(KEY_LEFT)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
						personagem.vx = -4;
						personagem.direcao = 'e';
					}
					else {
						personagem.vx = -2;
						personagem.direcao = 'e';
					}
				}

				personagem.x += personagem.vx;


				colisao(personagem, mundo, blococolisao, padrao);
				vida = personagemx(&personagem, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
				inimigos_colisao(&personagem, inimigos, ninimigos);

				if (vida != 0) {
					//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
					tilepy = ((personagem.y) / 16);
					if (personagem.vy == 0 && (mundo[tilepy + 2][((personagem.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 2].tipo == 'B')) {//permite que o personagme só pule quando não estiver voando e quando estiver em um bloco;
						if (IsKeyPressed(KEY_UP)) {
							personagem.vy = -18;
						}
					}
					personagem.vy += 1;
					personagem.y += personagem.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


					colisao(personagem, mundo, blococolisao, padrao);
					personagemy(&personagem, blococolisao, &espaco, mundo, itemsom);


					personagem.vx = 0;
					if (IsKeyDown(KEY_RIGHT)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
							personagem.vx = 4;
							personagem.direcao = 'd';
						}
						else {
							personagem.vx = 2;
							personagem.direcao = 'd';
						}
					}
					else if (IsKeyDown(KEY_LEFT)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
							personagem.vx = -4;
							personagem.direcao = 'e';
						}
						else {
							personagem.vx = -2;
							personagem.direcao = 'e';
						}
					}
					personagem.x += personagem.vx;



					colisao(personagem, mundo, blococolisao, padrao);
					personagemx(&personagem, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
					personagem.vidas--;

				}

				if (super > 0) {
					personagem.vidas = 3;

					// Evita que o personagem fique abaixo da altura limite
					if (personagem.y >= 576) {
						personagem.y = 368;
						// Procura o primeiro bloco sólido ('B') à esquerda

						while (mundo[23][personagem.x / 16].tipo != 'B' && mundo[24][personagem.x / 16].tipo != 'B' && mundo[25][personagem.x / 16].tipo != 'B' && mundo[26][personagem.x / 16].tipo != 'B' && mundo[27][personagem.x / 16].tipo != 'B' && mundo[28][personagem.x / 16].tipo != 'B' && mundo[29][personagem.x / 16].tipo != 'B' && mundo[30][personagem.x / 16].tipo != 'B' && mundo[31][personagem.x / 16].tipo != 'B' && mundo[32][personagem.x / 16].tipo != 'B') {
							if (personagem.direcao == 'd') {
								personagem.x -= 16;
							}
							else if (personagem.direcao == 'e') { personagem.x += 16; }

						}
					}
				}


				if ((personagem.vidas < 1 || personagem.y >= 578) && personagem.largura <= 3200) {
					EndDrawing();
					int denovo = -1;
					while (denovo == -1) {

						BeginDrawing();
						ClearBackground(RAYWHITE);
						DrawTexture(fundoinicial, 0, 0, RAYWHITE);
						DrawTexture(gameover, 280, 100, RAYWHITE);
						DrawText("1 - JOGAR NOVAMENTE", 350, 300, 40, DARKPURPLE);
						DrawText("2 - SAIR", 350, 400, 40, DARKPURPLE);

						EndDrawing();

						// Captura a entrada do jogador
						if (IsKeyPressed(KEY_ONE)) {
							denovo = 1;
						}
						else if (IsKeyPressed(KEY_TWO)) {
							denovo = 2;
						}
						if (WindowShouldClose()) {
							denovo = 2;
						}

					}



					if (denovo == 1) {
						espaco.on = true;
						personagem.vidas = 3;
						personagem.vx = 0;
						personagem.vy = 0;
						espaco.pontuacao = 0;
						ler_mundo(mundo, inimigos, &ninimigos, &personagem, &personagem2, inimigomacho, inimigofemea);
					}
					else {
						espaco.on = false;

					}



				}




				desenhar_jogo(mundo, personagem, espaco, ninimigos, inimigos);

				EndMode2D();//pontuacao fica sempre no mesmo lugar

				//pontuacao e nome

				DrawRectangle(0, 528, TAMANHO_MUNDO, 80, DARKPURPLE);//desenha o chão
				DrawRectangle(0, 0, TAMANHO_MUNDO, 50, DARKPURPLE);
				sprintf(escrita, "%d", espaco.pontuacao);
				DrawText(escrita, 1150, 20, 20, WHITE);
				DrawText(espaco.nome, 950, 20, 20, WHITE);


				if (super > 0) {
					DrawText("SUPER", 50, 20, 20, WHITE);
				}
				else {
					sprintf(escrita, "%d", personagem.vidas);
					DrawText(escrita, 50, 20, 20, WHITE);
				}


			}
			break;
		case 6:
			UpdateMusicStream(music);
			timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
			if (timePlayed > 1.0f) timePlayed = 1.0f;
			inimigos_movimentacao(ninimigos, inimigos, mundo);
			if (IsKeyPressed(KEY_Z)) {
				super *= -1;
			}

			if (personagem.x > 3200 && personagem2.x > 3200) {

				ClearBackground(RAYWHITE);
				DrawTexture(fundoinicial, 0, 0, RAYWHITE);
				DrawText("VOCE GANHOU", 300, 200, 80, DARKPURPLE);
				const float buttonWidth = 204.0;
				const float buttonHeight = 115.0;
				//dimensoes dos botoes
				Rectangle buttonexitbounds = { 500.0, 300.0, buttonWidth, buttonHeight };
				Vector2 mousePosition = GetMousePosition();

				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_TAB)) {
					if (CheckCollisionPointRec(mousePosition, buttonexitbounds)) {
						estadobutton = 1;
					}
				}

				DrawTexture(buttonexit, 500, 300, RAYWHITE);


				if (WindowShouldClose()) { estadobutton = 1; }
				espaco.ganhou = 1;


			}
			else {
				vida = 0;
				vida2 = 0;

				BeginMode2D(camera);
				camera_movimentacao(personagem, &camera);



				//personagem1

				//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
				tilepy = ((personagem.y) / 16);
				bool podepular = false;
				if (personagem.vy == 0 && personagem.y > 376 && (mundo[tilepy + 2][((personagem.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 2].tipo == 'B')) {//permite que o personagme s� pule quando n�o estiver voando e quando estiver em um bloco;
					podepular = true;
				}
				if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && personagem.y > 376 && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
					podepular = true;
				}
				else {
					podepular = false;
				}

				if (personagem.vy == 0 && podepular == true) {
					if (IsKeyPressed(KEY_UP)) {
						personagem.vy = -19;
					}
				}
				personagem.vy += 1;
				personagem.y += personagem.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


				colisao(personagem, mundo, blococolisao, padrao);
				personagemy(&personagem, blococolisao, &espaco, mundo, itemsom);

				if (IsKeyPressed(KEY_SPACE) && atirar.atira == false) {
					if (personagem.direcao == 'd') {
						atirar.vx = 12;
					}
					else if (personagem.direcao == 'e') {
						atirar.vx = -12;
					}
					PlaySound(tirosom);
					atira(personagem, &atirar);
					time = GetTime();
				}


				if (atirar.atira == true) {
					atualizatiro(&atirar, time);
					DrawPixel(atirar.x, atirar.y, PINK);
				}

				colisaotiro(&atirar, inimigos, ninimigos, &espaco, mundo);

				if (IsKeyPressed(KEY_LEFT_ALT) && atirar2.atira == false) {
					if (personagem2.direcao == 'd') {
						atirar2.vx = 12;
					}
					else if (personagem2.direcao == 'e') {
						atirar2.vx = -12;
					}
					PlaySound(tirosom);
					atira(personagem2, &atirar2);
					time2 = GetTime();
				}


				if (atirar2.atira == true) {
					atualizatiro(&atirar2, time2);
					DrawPixel(atirar2.x, atirar2.y, GREEN);
				}

				colisaotiro(&atirar2, inimigos, ninimigos, &espaco, mundo);





				personagem.vx = 0;



				if (IsKeyDown(KEY_RIGHT)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
						personagem.vx = 4;
						personagem.direcao = 'd';
					}
					else {
						personagem.vx = 2;
						personagem.direcao = 'd';
					}
				}
				else if (IsKeyDown(KEY_LEFT)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
						personagem.vx = -4;
						personagem.direcao = 'e';
					}
					else {
						personagem.vx = -2;
						personagem.direcao = 'e';
					}
				}

				personagem.x += personagem.vx;


				colisao(personagem, mundo, blococolisao, padrao);
				vida = personagemx(&personagem, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
				inimigos_colisao(&personagem, inimigos, ninimigos);

				if (vida != 0) {
					//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
					tilepy = ((personagem.y) / 16);
					if (personagem.vy == 0 && (mundo[tilepy + 2][((personagem.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem.x) / 16) + 2].tipo == 'B')) {//permite que o personagme só pule quando não estiver voando e quando estiver em um bloco;
						if (IsKeyPressed(KEY_UP)) {
							personagem.vy = -18;
						}
					}
					personagem.vy += 1;
					personagem.y += personagem.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


					colisao(personagem, mundo, blococolisao, padrao);
					personagemy(&personagem, blococolisao, &espaco, mundo, itemsom);


					personagem.vx = 0;
					if (IsKeyDown(KEY_RIGHT)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
							personagem.vx = 4;
							personagem.direcao = 'd';
						}
						else {
							personagem.vx = 2;
							personagem.direcao = 'd';
						}
					}
					else if (IsKeyDown(KEY_LEFT)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem.x) / 16) + 1].tipo != 'W') {
							personagem.vx = -4;
							personagem.direcao = 'e';
						}
						else {
							personagem.vx = -2;
							personagem.direcao = 'e';
						}
					}
					personagem.x += personagem.vx;



					colisao(personagem, mundo, blococolisao, padrao);
					personagemx(&personagem, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
					personagem.vidas--;

				}

				if (super > 0) {
					personagem.vidas = 3;

					// Evita que o personagem fique abaixo da altura limite
					if (personagem.y >= 576) {
						personagem.y = 368;
						// Procura o primeiro bloco sólido ('B') à esquerda

						while (mundo[23][personagem.x / 16].tipo != 'B' && mundo[24][personagem.x / 16].tipo != 'B' && mundo[25][personagem.x / 16].tipo != 'B' && mundo[26][personagem.x / 16].tipo != 'B' && mundo[27][personagem.x / 16].tipo != 'B' && mundo[28][personagem.x / 16].tipo != 'B' && mundo[29][personagem.x / 16].tipo != 'B' && mundo[30][personagem.x / 16].tipo != 'B' && mundo[31][personagem.x / 16].tipo != 'B' && mundo[32][personagem.x / 16].tipo != 'B') {
							if (personagem.direcao == 'd') {
								personagem.x -= 16;
							}
							else if (personagem.direcao == 'e') { personagem.x += 16; }

						}
					}
				}
				//personagem2


				//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
				tilepy = ((personagem2.y) / 16);
				podepular = false;
				if (personagem2.vy == 0 && personagem2.y > 376 && (mundo[tilepy + 2][((personagem2.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem2.x) / 16) + 2].tipo == 'B')) {//permite que o personagme s� pule quando n�o estiver voando e quando estiver em um bloco;
					podepular = true;
				}
				if (mundo[tilepy + 2][((personagem2.x) / 16)].tipo != 'W' && personagem2.y > 376 && mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo != 'W') {
					podepular = true;
				}
				else {
					podepular = false;
				}

				if (personagem2.vy == 0 && podepular == true) {
					if (IsKeyPressed(KEY_W)) {
						personagem2.vy = -19;
					}
				}
				personagem2.vy += 1;
				personagem2.y += personagem2.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


				colisao(personagem2, mundo, blococolisao, padrao);
				personagemy(&personagem2, blococolisao, &espaco, mundo, itemsom);



				personagem2.vx = 0;



				if (IsKeyDown(KEY_D)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem2, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem2 por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem2.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo != 'W') {
						personagem2.vx = 4;
						personagem2.direcao = 'd';
					}
					else {
						personagem2.vx = 2;
						personagem2.direcao = 'd';
					}
				}
				else if (IsKeyDown(KEY_A)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem2, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem2 por 240 pixels a direita
					if (mundo[tilepy + 2][((personagem2.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo != 'W') {
						personagem2.vx = -4;
						personagem2.direcao = 'e';
					}
					else {
						personagem2.vx = -2;
						personagem2.direcao = 'e';
					}
				}

				personagem2.x += personagem2.vx;


				colisao(personagem2, mundo, blococolisao, padrao);
				vida2 = personagemx(&personagem2, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
				inimigos_colisao(&personagem2, inimigos, ninimigos);

				if (vida2 != 0) {
					//muda antes de desenhar porque caso tenha colisão as funcões arrumam os lugares
					tilepy = ((personagem2.y) / 16);
					if (personagem2.vy == 0 && (mundo[tilepy + 2][((personagem2.x) / 16)].tipo == 'B' || mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo == 'B' || mundo[tilepy + 2][((personagem2.x) / 16) + 2].tipo == 'B')) {//permite que o personagme só pule quando não estiver voando e quando estiver em um bloco;
						if (IsKeyPressed(KEY_UP)) {
							personagem2.vy = -18;
						}
					}
					personagem2.vy += 1;
					personagem2.y += personagem2.vy;//fica mudando o y, a cada passada o y recebe a velocidade que foi estipulada na passada anterior, se for positiva ele fica "caindo", se for negativa, ele recebe pixeis a mais


					colisao(personagem2, mundo, blococolisao, padrao);
					personagemy(&personagem2, blococolisao, &espaco, mundo, itemsom);


					personagem2.vx = 0;
					if (IsKeyDown(KEY_D)) {//se tiver a seta para a direita pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem2.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo != 'W') {
							personagem2.vx = 4;
							personagem2.direcao = 'd';
						}
						else {
							personagem2.vx = 2;
							personagem2.direcao = 'd';
						}
					}
					else if (IsKeyDown(KEY_A)) {//se tiver a seta para a esquerda pressionada, aumenta o x da personagem, desenhando ele 4 pixels a direita da "passada" anterior, ent�o se apertar por um segundo, muda a personagem por 240 pixels a direita
						if (mundo[tilepy + 2][((personagem2.x) / 16)].tipo != 'W' && mundo[tilepy + 2][((personagem2.x) / 16) + 1].tipo != 'W') {
							personagem2.vx = -4;
							personagem2.direcao = 'e';
						}
						else {
							personagem2.vx = -2;
							personagem2.direcao = 'e';
						}
					}
					personagem2.x += personagem2.vx;



					colisao(personagem2, mundo, blococolisao, padrao);
					personagemx(&personagem2, blococolisao, &espaco, mundo, inimigos, ninimigos, itemsom);
					personagem2.vidas--;

				}

				if (super > 0) {
					personagem2.vidas = 3;

					// Evita que o personagem2 fique abaixo da altura limite
					if (personagem2.y >= 576) {
						personagem2.y = 368;
						// Procura o primeiro bloco sólido ('B') à esquerda

						while (mundo[23][personagem2.x / 16].tipo != 'B' && mundo[24][personagem2.x / 16].tipo != 'B' && mundo[25][personagem2.x / 16].tipo != 'B' && mundo[26][personagem2.x / 16].tipo != 'B' && mundo[27][personagem2.x / 16].tipo != 'B' && mundo[28][personagem2.x / 16].tipo != 'B' && mundo[29][personagem2.x / 16].tipo != 'B' && mundo[30][personagem2.x / 16].tipo != 'B' && mundo[31][personagem2.x / 16].tipo != 'B' && mundo[32][personagem2.x / 16].tipo != 'B') {
							if (personagem2.direcao == 'd') {
								personagem2.x -= 16;
							}
							else if (personagem2.direcao == 'e') { personagem2.x += 16; }

						}
					}
				}



				if (((personagem.vidas < 1 || personagem.y >= 578)) || (personagem2.vidas < 1 || personagem2.y >= 578)) {
					EndDrawing();
					int denovo = -1;
					while (denovo == -1) {

						BeginDrawing();
						ClearBackground(RAYWHITE);
						DrawTexture(fundoinicial, 0, 0, RAYWHITE);
						DrawTexture(gameover, 280, 100, RAYWHITE);
						DrawText("1 - JOGAR NOVAMENTE", 350, 300, 40, DARKPURPLE);
						DrawText("2 - SAIR", 350, 400, 40, DARKPURPLE);

						EndDrawing();

						// Captura a entrada do jogador
						if (IsKeyPressed(KEY_ONE)) {
							denovo = 1;
						}
						else if (IsKeyPressed(KEY_TWO)) {
							denovo = 2;
						}
						if (WindowShouldClose()) {
							denovo = 2;
						}

					}



					if (denovo == 1) {
						espaco.on = true;

						personagem.vidas = 3;
						personagem.vx = 0;
						personagem.vy = 0;
						personagem2.vidas = 3;
						personagem2.vx = 0;
						personagem2.vy = 0;
						espaco.pontuacao = 0;
						ler_mundo(mundo, inimigos, &ninimigos, &personagem, &personagem2, inimigomacho, inimigofemea);
					}
					else {
						espaco.on = false;

					}



				}




				desenhar_jogo2(mundo, personagem, personagem2, espaco, ninimigos, inimigos);

				EndMode2D();//pontuacao fica sempre no mesmo lugar

				//pontuacao

				DrawRectangle(0, 528, TAMANHO_MUNDO, 80, DARKPURPLE);//desenha o chão
				DrawRectangle(0, 0, TAMANHO_MUNDO, 50, DARKPURPLE);
				sprintf(escrita, "%d", espaco.pontuacao);
				DrawText(escrita, 1150, 20, 20, WHITE);
				DrawText(espaco.nome, 950, 20, 20, WHITE);


				if (super > 0) {
					DrawText("SUPER", 50, 20, 20, WHITE);
				}
				else {
					sprintf(escrita, "P1: %d", personagem.vidas);
					DrawText(escrita, 50, 20, 20, WHITE);

					sprintf(escrita, "P2: %d", personagem2.vidas);
					DrawText(escrita, 100, 20, 20, WHITE);
				}
			}
			break;


		default:
			printf("erro");


		}

		EndDrawing();


	}

	le_arq(top5, nomearq);
	ordena_escreve_arq(espaco, top5, nomearq);

	UnloadMusicStream(music);

	CloseAudioDevice();

	UnloadSound(tirosom);
	UnloadSound(itemsom);
	UnloadTexture(buttonmultiplayer);
	UnloadTexture(fundoinicial);
	UnloadTexture(gameover);
	UnloadTexture(characterD);
	UnloadTexture(characterE);
	UnloadTexture(character2D);
	UnloadTexture(character2E);
	UnloadTexture(buttonexit);
	UnloadTexture(buttonstart);
	UnloadTexture(buttonleaderboard);
	UnloadTexture(infwoman);
	UnloadTexture(spike);
	UnloadTexture(bloconormal);
	UnloadTexture(item);
	UnloadTexture(rankleaderboard);
	UnloadTexture(digitenome);

	CloseWindow();
	return 0;



}*/