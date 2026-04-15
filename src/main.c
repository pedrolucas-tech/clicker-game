#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// =====================
// STRUCT DO JOGO
// =====================
typedef struct {
    int pontos;
    int pontosPorClique;

    int larguraTela;
    int alturaTela;

    float img_x;
    float img_y;
    int img_w;
    int img_h;

} Jogo;

// =====================
// STRUCT UPGRADE
// =====================
typedef struct {
    char nome[50];
    int custo;
    int bonus;
} Upgrade;

// =====================
// FUNÇÃO DE INICIALIZAÇÃO
// =====================
void inicializarJogo(Jogo *jogo) {
    jogo->pontos = 0;
    jogo->pontosPorClique = 1;

    jogo->larguraTela = 800;
    jogo->alturaTela = 600;

    jogo->img_x = 0;
    jogo->img_y = 0;
    jogo->img_w = 0;
    jogo->img_h = 0;
}

// =====================
// MAIN
// =====================
int main() {

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP *pc = NULL;
    ALLEGRO_FONT *font = NULL;

    Jogo *jogo = malloc(sizeof(Jogo));
    if (!jogo) {
        printf("Erro de memoria\n");
        return -1;
    }

    inicializarJogo(jogo);

    // =====================
    // UPGRADES
    // =====================
    Upgrade upgrades[3];

    strcpy(upgrades[0].nome, "Melhoria de CPU");
    upgrades[0].custo = 10;
    upgrades[0].bonus = 1;

    strcpy(upgrades[1].nome, "Melhoria de GPU");
    upgrades[1].custo = 50;
    upgrades[1].bonus = 5;

    strcpy(upgrades[2].nome, "Melhoria de RAM");
    upgrades[2].custo = 100;
    upgrades[2].bonus = 10;

    // =====================
    // INICIALIZA ALLEGRO
    // =====================
    if (!al_init()) return -1;

    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(jogo->larguraTela, jogo->alturaTela);
    if (!display) {
        printf("Erro ao criar tela\n");
        return -1;
    }

    // =====================
    // CARREGAR FONTE
    // =====================
    font = al_load_ttf_font("../assets/fonts/ari-w9500.ttf", 20, 0);
    if (!font) {
        printf("Erro ao carregar fonte\n");
        al_rest(5.0);
        return -1;
    }

    // =====================
    // CARREGAR IMAGEM
    // =====================
    pc = al_load_bitmap("../assets/images/PcTeste.png");
    if (!pc) {
        printf("Erro ao carregar imagem\n");
        return -1;
    }

    int largura_original = al_get_bitmap_width(pc);
    int altura_original = al_get_bitmap_height(pc);

    jogo->img_w = 400;
    jogo->img_h = (altura_original * jogo->img_w) / largura_original;

    jogo->img_x = jogo->larguraTela/2 - jogo->img_w/2;
    jogo->img_y = jogo->alturaTela/2 - jogo->img_h/2;

    // =====================
    // EVENTOS
    // =====================
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    bool rodando = true;

    // =====================
    // LOOP PRINCIPAL
    // =====================
    while (rodando) {

        al_clear_to_color(al_map_rgb(0, 0, 0));

        // DESENHAR PC
        al_draw_scaled_bitmap(
            pc,
            0, 0,
            largura_original,
            altura_original,
            jogo->img_x, jogo->img_y,
            jogo->img_w, jogo->img_h,
            0
        );

        // PONTOS
        char texto_pontos[50];
        sprintf(texto_pontos, "Pontos: %d", jogo->pontos);

        al_draw_text(font, al_map_rgb(255,255,255), 50, 10, 0, texto_pontos);

        // UPGRADES
        for (int i = 0; i < 3; i++) {

            char texto[100];
            sprintf(texto, "%s - Custo: %d", upgrades[i].nome, upgrades[i].custo);

            ALLEGRO_COLOR cor;
            if (jogo->pontos >= upgrades[i].custo) {
                cor = al_map_rgb(0,255,0);
            } else {
                cor = al_map_rgb(255,0,0);
            }

            al_draw_text(font, cor, 50, 100 + i * 40, 0, texto);
        }

        al_flip_display();

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }

        // CLIQUE
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {

            int x = event.mouse.x;
            int y = event.mouse.y;

            // CLIQUE NO PC
            int margemX = jogo->img_w * 0.25; // corta 25% dos lados
            int margemY = jogo->img_h * 0.25;

            int area_x1 = jogo->img_x + margemX;
            int area_y1 = jogo->img_y + margemY;
            int area_x2 = jogo->img_x + jogo->img_w - margemX;
            int area_y2 = jogo->img_y + jogo->img_h - margemY;

            if (x >= area_x1 && x <= area_x2 &&
            y >= area_y1 && y <= area_y2) {

                jogo->pontos += jogo->pontosPorClique;
            }

            // CLIQUE NOS UPGRADES
            for (int i = 0; i < 3; i++) {

                int y_up = 100 + i * 40;

                if (x >= 50 && x <= 300 &&
                    y >= y_up && y <= y_up + 30) {

                    if (jogo->pontos >= upgrades[i].custo) {
                        jogo->pontos -= upgrades[i].custo;
                        jogo->pontosPorClique += upgrades[i].bonus;

                        printf("Comprou: %s\n", upgrades[i].nome);
                    }
                }
            }
        }
    }

    // =====================
    // LIMPEZA
    // =====================
    al_destroy_bitmap(pc);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    free(jogo);

    return 0;
}