#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

const float FPS = 60; //constante para o FPS
const int SCREEN_W = 1360; //tamanho horizonal da tela
const int SCREEN_H = 680; //tamanho vertical da tela

//definição do tipo para uma célula
typedef struct Cell {
  float x; //variável para posição em x
  float y; //variável para posição em y
  float dx; //variável para velocidade horizontal
  float dy; //variável para velocidade vertical
  float raio; //variável para raio da célula
  bool vida; //variavel para saber se a celua esta viva (true)
  ALLEGRO_COLOR cor; //variável para cor da célula
} Cell;

//função para calcular distância entre dois pontos
float dist(float x1, float x2, float y1, float y2) {
	return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

//função para desenhar a celula
void desenhaCell(Cell *C, int i) {
	al_draw_filled_circle(C[i].x, C[i].y, C[i].raio, C[i].cor);
}

//função para gerar celulas inicialmente
void cria_ambiente(Cell *C, int qtd_celulas){
  srand(time(NULL));
  for(int i=0; i<qtd_celulas; i++){
    //Criando o jogador:
    if(i==0){
      C[i].x = SCREEN_W/2;
      C[i].y = SCREEN_H/2;
      C[i].dx = 0;
      C[i].dy = 0;
      C[i].raio = 25;
      C[i].vida = true;
      C[i].cor = al_map_rgb(0,255,0);
    }
    //Criando a celula inimiga
    else if(i==1){
      C[i].x = SCREEN_W/4;
      C[i].y = SCREEN_H/4;
      C[i].dx = rand()%(3) - rand()%(1000)/500.00;
      C[i].dy = rand()%(3) - rand()%(1000)/500.00;
      C[i].raio = 21;
      C[i].vida = true;
      C[i].cor = al_map_rgb(255,36,32);
    }
    else{
      int cor = rand()%(75)+155;
      C[i].x = 20 + rand()%(SCREEN_W - 20);
      C[i].y = 20 + rand()%(SCREEN_H - 20);
      C[i].dx = rand()%(3) - rand()%(1000)/500.00;
      C[i].dy = rand()%(3) - rand()%(1000)/500.00;
      C[i].raio = 4 + rand()%(8);
      C[i].vida = true;
      C[i].cor = al_map_rgb(cor,cor,cor);
    }
  }
}

//Funcao para garantir o ricochete correto ao colidir com a borda da tela
void ricochetear_bordas(Cell *C, int i){

  if(C[i].x + C[i].raio > SCREEN_W || C[i].x - C[i].raio < 0)
     C[i].dx = -C[i].dx;
  if(C[i].y + C[i].raio > SCREEN_H || C[i].y - C[i].raio < 0)
     C[i].dy = -C[i].dy;

  float *teste_esquerda = (float*)calloc(1,sizeof(float));
  float *teste_direita = (float*)calloc(1,sizeof(float));
  float *teste_cima = (float*)calloc(1,sizeof(float));
  float *teste_baixo = (float*)calloc(1,sizeof(float));

  *teste_esquerda = dist(C[i].x, 0, C[i].y, C[i].y);
  if(*teste_esquerda < C[i].raio)
    C[i].x = C[i].x + C[i].raio - *teste_esquerda;

  *teste_direita = dist(C[i].x, SCREEN_W, C[i].y, C[i].y);
  if(*teste_direita < C[i].raio)
    C[i].x = C[i].x - C[i].raio + *teste_direita;

  *teste_cima = dist(C[i].x, C[i].x, C[i].y, 0);
  if(*teste_cima < C[i].raio)
    C[i].y = C[i].y + C[i].raio - *teste_cima;

  *teste_baixo = dist(C[i].x, C[i].x, C[i].y, SCREEN_H);
  if(*teste_baixo < C[i].raio)
    C[i].y = C[i].y - C[i].raio + *teste_baixo;

  free(teste_esquerda);
  free(teste_direita);
  free(teste_cima);
  free(teste_baixo);
}

//Funcao para determinar nova posicao das celulas
void nova_posicao(Cell *C, int qtd_celulas){
  for(int i=0; i<qtd_celulas; i++){
    if(C[i].vida == true){
      ricochetear_bordas(C,i);
      C[i].x += C[i].dx;
      C[i].y += C[i].dy;
      desenhaCell(C,i);
    }
  }
}

//Funcao para dividir celula do jogador ao clicar na qtd_celulas
void divide_celula(Cell *C, int i, ALLEGRO_EVENT ev){
  float *aux_dx = (float*)calloc(1, sizeof(float));
  float *aux_dy = (float*)calloc(1, sizeof(float));
  float *taxa = (float*)calloc(1, sizeof(float));

  *taxa = 0.20;

  //Modificando o jogador
  C[0].raio *= (1-(*taxa * *taxa));

  *aux_dx = (ev.mouse.x - C[0].x)/dist(ev.mouse.x, C[0].x, ev.mouse.y, C[0].y);
  *aux_dy = (ev.mouse.y - C[0].y)/dist(ev.mouse.x, C[0].x, ev.mouse.y, C[0].y);
  C[0].dx += -*aux_dx/(1-(*taxa * *taxa));
  C[0].dy += -*aux_dy/(1-(*taxa * *taxa));

  //Definindo a celula nova
  int cor = rand()%(75)+155;
  C[i].x = C[0].x + (*aux_dx * (C[0].raio + *taxa * C[0].raio));
  C[i].y = C[0].y + (*aux_dy * (C[0].raio + *taxa * C[0].raio));
  C[i].dx = *aux_dx + *aux_dx * sqrt(pow(C[0].dx,2) + pow(C[0].dy,2));
  C[i].dy = *aux_dy + *aux_dy * sqrt(pow(C[0].dx,2) + pow(C[0].dy,2));
  C[i].raio = *taxa*C[0].raio;
  C[i].vida = true;
  C[i].cor = al_map_rgb(cor,cor,cor);

  free(aux_dx);
  free(aux_dy);
  free(taxa);
}

//Funcao para quando jogador clica para ganhar gera_impulso
void aumenta_celulas(Cell **C, int *qtd_celulas){
  *qtd_celulas += 1;
  Cell *temp = (Cell*)realloc(*C, (*qtd_celulas) * sizeof(Cell));
  if(temp == NULL){
    *qtd_celulas -= 1;
    exit(1);
  }
  else *C = temp;
}

//Funcao para verificar se uma celula na posição i colide com as celulas na posicao i+1 até i+n
void verifica_colisao(Cell *C, int qtd_celulas){
  int *maior = (int*)calloc(1,sizeof(int));
  int *menor = (int*)calloc(1,sizeof(int));

  for(int i = 0; i < qtd_celulas; i++){
    for(int j = i + 1; j < qtd_celulas; j++){
      if (dist(C[i].x, C[j].x, C[i].y, C[j].y) <= C[i].raio + C[j].raio){
        if(C[i].raio > C[j].raio){
          *maior = i;
          *menor = j;
        }
        else{
          *maior = j;
          *menor = i;
        }
        C[*maior].raio = sqrt(pow(C[*maior].raio,2) + pow(C[*menor].raio/2,2));
        C[*menor].raio /= 2;

        if (dist(C[i].x, C[j].x, C[i].y, C[j].y) <= C[i].raio || C[*menor].raio < 1){
          C[*maior].raio = sqrt(pow(C[*maior].raio,2) + pow(C[*menor].raio,2));
          C[*menor].raio = 0;
          C[*menor].x = 0;
          C[*menor].y = 0;
          C[*menor].dx = 0;
          C[*menor].dy = 0;
          C[*menor].vida = false;
        }
        free(maior);
        free(menor);
      }
    }
  }
}

//Funao para gerar uma celula aleatoria no lugar de uma celula que morreu
void gera_celula_aleatoria(Cell *C, int qtd_celulas){
  for(int i = 0; i < qtd_celulas; i++){
    if(C[i].vida == false){
      int cor = rand()%(75)+155;
      C[i].x = 20 + rand()%(SCREEN_W - 20);
      C[i].y = 20 + rand()%(SCREEN_H - 20);
      C[i].dx = rand()%(3) - rand()%(1000)/500.00;
      C[i].dy = rand()%(3) - rand()%(1000)/500.00;
      C[i].raio = 4 + rand()%(6);
      C[i].vida = true;
      C[i].cor = al_map_rgb(cor,cor,cor);
    }
  }
}

//Funcao para verificar se houve vitoria ou derrota
void verifica_vitoria(Cell *C, bool *playing, bool *vitoria, bool *derrota){
  if(C[0].vida == false){
    *derrota = true;
    *playing = false;
  }
  if(C[1].vida == false){
    *vitoria = true;
    *playing = false;
  }
}

//Funcao para imprimir na tela o tempo gasto na fase
void exibe_tempo(ALLEGRO_FONT *font_bebas_50, ALLEGRO_TIMER *timer){
  al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 45,
                ALLEGRO_ALIGN_CENTRE, "%.2f",  al_get_timer_count(timer)/FPS);
}

//Funcao para determinar os recordes e salvar no arquivo
void gera_recordes(FILE *file_recordes, float v_rec[6], float tempo_jogo){
  float aux = 0;
  fscanf(file_recordes, "%f %f %f %f %f", &v_rec[1], &v_rec[2], &v_rec[3], &v_rec[4], &v_rec[5]);
  v_rec[0] = tempo_jogo;

  for(int i=0; i<6; i++){
    for(int j=i+1; j<6; j++){
      if(v_rec[i] > v_rec[j] && v_rec[j] != 0){
        aux = v_rec[i];
        v_rec[i] = v_rec[j];
        v_rec[j] = aux;
      }
    }
  }

  fclose(file_recordes);
  file_recordes = fopen("recordes.txt", "w");
  for(int i=0; i<5; i++)
    fprintf(file_recordes, "%f\n", v_rec[i]);
  fclose(file_recordes);
}

//Funcao para ler o arquivo e imprimir na tela os recordes
void exibe_recordes(ALLEGRO_FONT *font_bebas_110, ALLEGRO_FONT *font_bebas_50){
    float vec_aux[5] = {0};
    FILE *file_recordes_aux = fopen("recordes.txt", "r");
    for(int i=0; i<5; i++)
      fscanf(file_recordes_aux, "%f", &vec_aux[i]);
    fclose(file_recordes_aux);

    al_draw_textf(font_bebas_110, al_map_rgb(255, 255, 255), SCREEN_W/2, 150,
                  ALLEGRO_ALIGN_CENTRE, "TEMPO RECORDES");
    al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 280,
                  ALLEGRO_ALIGN_CENTRE, "%.2f S", vec_aux[0]);
    al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 350,
                  ALLEGRO_ALIGN_CENTRE, "%.2f S",vec_aux[1]);
    al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 420,
                  ALLEGRO_ALIGN_CENTRE, "%.2f S",vec_aux[2]);
    al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 490,
                  ALLEGRO_ALIGN_CENTRE, "%.2f S",vec_aux[3]);
    al_draw_textf(font_bebas_50, al_map_rgb(255, 255, 255), SCREEN_W/2, 560,
                  ALLEGRO_ALIGN_CENTRE, "%.2f S",vec_aux[4]);

}


int main (int argc, char **argv){

  ALLEGRO_TIMER *timer = NULL; //variável para temporizador
  ALLEGRO_DISPLAY *display = NULL; //variavel para tela princiapl
  ALLEGRO_EVENT_QUEUE *event_queue = NULL; //variavel para fila de eventos
  ALLEGRO_AUDIO_STREAM *audio_theme = NULL; //variavel para musica tema
  ALLEGRO_AUDIO_STREAM *audio_vitoria = NULL; //variavel para musica vitoria
  ALLEGRO_AUDIO_STREAM *audio_derrota = NULL; //variavel para musica derrota
  ALLEGRO_AUDIO_STREAM *audio_playing = NULL; //variavel para musica de jogo

  ALLEGRO_BITMAP *img_ambiente = NULL;
  ALLEGRO_BITMAP *img_vitoria = NULL;
  ALLEGRO_BITMAP *img_derrota = NULL;
  ALLEGRO_BITMAP *img_main_menu = NULL;
  ALLEGRO_BITMAP *img_recordes = NULL;
  ALLEGRO_BITMAP *img_tutorial = NULL;

  ALLEGRO_FONT *font_bebas_110 = NULL;
  ALLEGRO_FONT *font_bebas_60 = NULL;
  ALLEGRO_FONT *font_bebas_50 = NULL;

  FILE *file_recordes = NULL;
  int qtd_celulas = 60;

  //inicialização da bilbioteca Allegro 5:
  if (!al_init()) {
    fprintf(stderr, "Falha ao inicializar o Allegro!\n");
    return -1;
  }

  //inicialização da bilbioteca Primitives:
  if(!al_init_primitives_addon()){
    fprintf(stderr, "Falha ao inicializar as primitivas!\n");
    return -1;
  }

  //inicialização do mouse
  if(!al_install_mouse()){
    fprintf(stderr, "Falha ao inicializar mouse!\n");
    return -1;
  }

  //inicialização do teclado
  if(!al_install_keyboard()){
    fprintf(stderr, "Falha ao inicializar teclado!\n");
    return -1;
  }

  //inicialização do audio
  if(!al_install_audio()){
    fprintf(stderr, "Falha ao inicializar audio!\n");
    return -1;
  }

  //inicialização do codec de audio
  if(!al_init_acodec_addon()){
    fprintf(stderr, "Falha ao inicializar codec de audio!\n");
    return -1;
  }

  //inicialização do add-on para uso de fontes
  al_init_font_addon();

  // Inicialização do add-on para uso de fontes True Type
  if (!al_init_ttf_addon()){
      fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
      return -1;
  }

  font_bebas_50 = al_load_font("fonts/BebasNeueRegular.ttf", 50, 1);
  if (!font_bebas_50){
    fprintf(stderr, "Falha ao carregar fonte.\n");
    return -1;
  }

  font_bebas_60 = al_load_font("fonts/BebasNeueRegular.ttf", 60, 1);
  if (!font_bebas_60){
    fprintf(stderr, "Falha ao carregar fonte.\n");
    return -1;
  }

  font_bebas_110 = al_load_font("fonts/BebasNeueRegular.ttf", 110, 1);
  if (!font_bebas_110){
    fprintf(stderr, "Falha ao carregar fonte.\n");
    return -1;
  }

  //definição de quantidade de canais que devem ser alocados ao mixer principal
  if (!al_reserve_samples(4)) {
     fprintf(stderr, "Falha ao alocar canais de audio.\n");
     return -1;
  }

  //definição do arquivo de musica tema
  audio_theme = al_load_audio_stream("sounds/main_menu.ogg", 4, 1024);
  if (!audio_theme) {
    fprintf(stderr, "Falha ao inicializar musica tema.\n");
    return -1;
  }

  //definição do arquivo de musica de derrota
  audio_derrota = al_load_audio_stream("sounds/derrota.ogg", 4, 1024);
  if (!audio_derrota) {
    fprintf(stderr, "Falha ao inicializar musica.\n");
    return -1;
  }

  //definição do arquivo de musica de vitoria
  audio_vitoria = al_load_audio_stream("sounds/vitoria.ogg", 4, 1024);
  if (!audio_vitoria) {
    fprintf(stderr, "Falha ao inicializar musica.\n");
    return -1;
  }

  //definição do arquivo de musica de jogo
  audio_playing = al_load_audio_stream("sounds/playing.ogg", 4, 1024);
  if (!audio_playing) {
    fprintf(stderr, "Falha ao inicializar musica.\n");
    return -1;
  }

  //inicialização do temporizador
  timer = al_create_timer(1.0 / FPS);
  if (!timer) {
    fprintf(stderr, "Falha ao inicializar temporizador!\n");
    return -1;
  }

  //inicialização da janela do jogo
  display = al_create_display(SCREEN_W, SCREEN_H);
  if(!display) {
    fprintf(stderr, "Falha ao inicializar tela!\n");
    al_destroy_timer(timer);
    return -1;
  }

  //inicialização da fila de eventos:
  event_queue = al_create_event_queue();
  if(!event_queue) {
     fprintf(stderr, "Falha ao criar fila de eventos!\n");
     al_destroy_display(display);
     al_destroy_timer(timer);
     return -1;
  }
	
  struct Cell *C = (Cell*) calloc(qtd_celulas, sizeof (Cell));

  //registrando a origem dos eventos:
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_mouse_event_source());

  //define a fila de enventos:
  ALLEGRO_EVENT ev;

  bool menu = true;
  bool sair = false;
  bool playing = false;
  bool vitoria = false;
  bool derrota = false;
  bool recordes = false;
  bool tutorial = false;
  float v_rec[6] = {0,0,0,0,0,0};
  float tempo_jogo = 0;

  while(!sair){

    if(menu){
      al_rewind_audio_stream(audio_theme);
      al_attach_audio_stream_to_mixer(audio_theme, al_get_default_mixer());
      al_set_audio_stream_playmode(audio_theme, ALLEGRO_PLAYMODE_LOOP);
      al_set_audio_stream_playing(audio_theme, true);

      al_init_image_addon();
      ALLEGRO_BITMAP *img_main_menu = al_load_bitmap("images/img_main_menu.jpg");
      al_draw_bitmap(img_main_menu, 0, 0, 0);

      while(menu){
        al_draw_bitmap(img_main_menu, 0, 0, 0);
        al_flip_display();

        al_wait_for_event(event_queue, &ev);

    		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          menu = false;
          sair = true;
        }

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
    			switch(ev.keyboard.keycode){
    				case ALLEGRO_KEY_SPACE:
              playing = true;
              menu = false;
    				break;

    				case ALLEGRO_KEY_R:
              recordes = true;
    				break;

            case ALLEGRO_KEY_T:
              tutorial = true;
            break;
    			}
          if(recordes){
            al_init_image_addon();
            ALLEGRO_BITMAP *img_recordes = al_load_bitmap("images/img_recordes.jpg");
            al_draw_bitmap(img_recordes, 0, 0, 0);
            exibe_recordes(font_bebas_110, font_bebas_50);

            while(recordes){
              al_draw_bitmap(img_recordes, 0, 0, 0);
              exibe_recordes(font_bebas_110, font_bebas_50);
              al_flip_display();

              al_wait_for_event(event_queue, &ev);

          		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                menu = false;
                recordes = false;
                sair = true;
              }

              if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
          			switch(ev.keyboard.keycode){
          				case ALLEGRO_KEY_ESCAPE:
                    recordes = false;
          				break;
                }
              }
            }
          }

          if(tutorial){
            al_init_image_addon();
            ALLEGRO_BITMAP *img_tutorial = al_load_bitmap("images/img_tutorial.jpg");
            al_draw_bitmap(img_tutorial, 0, 0, 0);

            while(tutorial){
              al_draw_bitmap(img_tutorial, 0, 0, 0);
              al_flip_display();

              al_wait_for_event(event_queue, &ev);

          		if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                menu = false;
                tutorial = false;
                sair = true;
              }

              if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
          			switch(ev.keyboard.keycode){
          				case ALLEGRO_KEY_ESCAPE:
                    tutorial = false;
          				break;
                }
              }
            }
          }

    		}
    	}
      al_set_audio_stream_playing(audio_theme, false);
      al_detach_audio_stream(audio_theme);
    }

    if(playing){
      float v_rec[6] = {0,0,0,0,0,0};
      tempo_jogo = 0;

      al_rewind_audio_stream(audio_playing);
      al_attach_audio_stream_to_mixer(audio_playing, al_get_default_mixer());
      al_set_audio_stream_playmode(audio_playing, ALLEGRO_PLAYMODE_LOOP);
      al_set_audio_stream_playing(audio_playing, true);

      al_init_image_addon();
      ALLEGRO_BITMAP *img_ambiente = al_load_bitmap("images/img_ambiente.jpg");
      al_draw_bitmap(img_ambiente, 0, 0, 0);
      al_flip_display();

      cria_ambiente(C, qtd_celulas);
      al_start_timer(timer);

      while(playing) {

        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          playing = false;
          sair = true;
        }

        else if(ev.type == ALLEGRO_EVENT_TIMER) {
          al_draw_bitmap(img_ambiente, 0, 0, 0);
          nova_posicao(C, qtd_celulas);
          gera_celula_aleatoria(C, qtd_celulas);
          verifica_colisao(C, qtd_celulas);
          exibe_tempo(font_bebas_50, timer);
          al_flip_display();
          verifica_vitoria(C, &playing, &vitoria, &derrota);
        }

        else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
          aumenta_celulas(&C, &qtd_celulas);
          divide_celula(C, qtd_celulas-1, ev);
        }
      }
      tempo_jogo = al_get_timer_count(timer)/FPS;
      al_stop_timer(timer);
      al_set_timer_count(timer, 0);
      al_set_audio_stream_playing(audio_playing, false);
      al_detach_audio_stream(audio_playing);
    }

    if(vitoria){

      FILE *file_recordes = fopen("recordes.txt", "r");
      gera_recordes(file_recordes, v_rec, tempo_jogo);

      al_init_image_addon();
      ALLEGRO_BITMAP *img_vitoria = al_load_bitmap("images/img_vitoria.jpg");
      al_draw_bitmap(img_vitoria, 0, 0, 0);
      al_draw_textf(font_bebas_60, al_map_rgb(255, 255, 255), SCREEN_W/2, 420,
                    ALLEGRO_ALIGN_CENTRE, "TEMPO: %.2f SEGUNDOS",  tempo_jogo);
      al_flip_display();

      al_rewind_audio_stream(audio_vitoria);
      al_attach_audio_stream_to_mixer(audio_vitoria, al_get_default_mixer());
      al_set_audio_stream_playmode(audio_vitoria, ALLEGRO_PLAYMODE_LOOP);
      al_set_audio_stream_playing(audio_vitoria, true);

      while(vitoria){
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          vitoria = false;
          playing = false;
          sair = true;
        }

        al_draw_bitmap(img_vitoria, 0, 0, 0);
        al_draw_textf(font_bebas_60, al_map_rgb(255, 255, 255), SCREEN_W/2, 420,
                      ALLEGRO_ALIGN_CENTRE, "TEMPO: %.2f SEGUNDOS",  tempo_jogo);
        al_flip_display();

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
          switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_SPACE:
              playing = true;
              vitoria = false;
            break;

            case ALLEGRO_KEY_X:
              menu = true;
              vitoria = false;
            break;
          }
        }
      }
      al_set_audio_stream_playing(audio_vitoria, false);
      al_detach_audio_stream(audio_vitoria);
    }

    if(derrota){

      al_init_image_addon();
      ALLEGRO_BITMAP *img_derrota = al_load_bitmap("images/img_derrota.jpg");
      al_draw_bitmap(img_derrota, 0, 0, 0);
      al_draw_textf(font_bebas_60, al_map_rgb(255, 255, 255), SCREEN_W/2, 420,
                    ALLEGRO_ALIGN_CENTRE, "TEMPO: %.2f SEGUNDOS",  tempo_jogo);
      al_flip_display();

      al_rewind_audio_stream(audio_derrota);
      al_attach_audio_stream_to_mixer(audio_derrota, al_get_default_mixer());
      al_set_audio_stream_playmode(audio_derrota, ALLEGRO_PLAYMODE_LOOP);
      al_set_audio_stream_playing(audio_derrota, true);

      while(derrota){
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
          playing = false;
          derrota = false;
          sair = true;
        }

        al_draw_bitmap(img_derrota, 0, 0, 0);
        al_draw_textf(font_bebas_60, al_map_rgb(255, 255, 255), SCREEN_W/2, 420,
                      ALLEGRO_ALIGN_CENTRE, "TEMPO: %.2f SEGUNDOS",  tempo_jogo);
        al_flip_display();

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
          switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_SPACE:
              playing = true;
              derrota = false;
            break;

            case ALLEGRO_KEY_X:
              menu = true;
              derrota = false;
            break;
          }
        }
      }
      al_set_audio_stream_playing(audio_derrota, false);
      al_detach_audio_stream(audio_derrota);
    }

  }

  free(C);
  al_destroy_timer(timer);
  al_destroy_display(display);
  al_destroy_bitmap(img_derrota);
  al_destroy_bitmap(img_vitoria);
  al_destroy_bitmap(img_ambiente);
  al_destroy_bitmap(img_main_menu);
  al_destroy_event_queue(event_queue);
  al_destroy_audio_stream(audio_theme);
  al_destroy_audio_stream(audio_playing);
  al_destroy_audio_stream(audio_vitoria);
  al_destroy_audio_stream(audio_derrota);

  return 0;
}
