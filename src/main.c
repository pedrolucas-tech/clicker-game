#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// =====================
// ENUM (REQ 6)
// =====================
typedef enum {
    MULT_1X = 0,
    MULT_5X = 1,
    MULT_10X = 2
} ModoCompra;

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

    ModoCompra opcaoCompra;

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
// ANINHAMENTO (REQ 10)
// =====================
typedef struct {
    Upgrade up;
    int nivel;
} Slot;

// =====================
// INICIALIZAÇÃO
// =====================
void inicializarJogo(Jogo *jogo) {
    jogo->pontos = 0;
    jogo->pontosPorClique = 1;

    jogo->larguraTela = 1280;
    jogo->alturaTela = 720;

    jogo->img_x = 0;
    jogo->img_y = 0;
    jogo->img_w = 0;
    jogo->img_h = 0;

    jogo->opcaoCompra = MULT_1X;
}

// =====================
// DESENHAR
// =====================
void desenhar(Jogo *jogo,
              ALLEGRO_BITMAP *pc,
              ALLEGRO_FONT *font,
              Slot slots[],
              int multiplicador[3][1],
              int largura_original,
              int altura_original) {

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_draw_scaled_bitmap(pc,
        0, 0,
        largura_original, altura_original,
        jogo->img_x, jogo->img_y,
        jogo->img_w, jogo->img_h,
        0);

    char texto_pontos[50];
    sprintf(texto_pontos, "Pontos: %d", jogo->pontos);
    al_draw_text(font, al_map_rgb(255,255,255), 50, 10, 0, texto_pontos);

    char textoModo[50];
    sprintf(textoModo, "Compra: %dx", multiplicador[jogo->opcaoCompra][0]);
    al_draw_text(font, al_map_rgb(255,255,0), 50, 40, 0, textoModo);

    for (int i = 0; i < 3; i++) {

        int mult = multiplicador[jogo->opcaoCompra][0];

        char texto[100];
        sprintf(texto, "%s Lv.%d - %d",
                slots[i].up.nome,
                slots[i].nivel,
                slots[i].up.custo * mult);

        ALLEGRO_COLOR cor = (jogo->pontos >= slots[i].up.custo * mult)
                            ? al_map_rgb(0,255,0)
                            : al_map_rgb(255,0,0);

        al_draw_text(font, cor, 50, 100 + i * 40, 0, texto);
    }

    al_flip_display();
}

// =====================
// CLIQUE
// =====================
void tratarClique(Jogo *jogo,
                  Slot slots[],
                  int multiplicador[3][1],
                  int x, int y) {

    int mult = multiplicador[jogo->opcaoCompra][0];

    // trocar modo de compra
    if (x >= 50 && x <= 200 &&
        y >= 40 && y <= 70) {

        jogo->opcaoCompra++;

        if (jogo->opcaoCompra > MULT_10X)
            jogo->opcaoCompra = MULT_1X;

        return;
    }

    // clique no PC
    int margemX = jogo->img_w * 0.25;
    int margemY = jogo->img_h * 0.25;

    int x1 = jogo->img_x + margemX;
    int y1 = jogo->img_y + margemY;
    int x2 = jogo->img_x + jogo->img_w - margemX;
    int y2 = jogo->img_y + jogo->img_h - margemY;

    if (x >= x1 && x <= x2 &&
        y >= y1 && y <= y2) {

        jogo->pontos += jogo->pontosPorClique;
    }

    // upgrades
    for (int i = 0; i < 3; i++) {

        int y_up = 100 + i * 40;

        if (x >= 50 && x <= 300 &&
            y >= y_up && y <= y_up + 30) {

            if (jogo->pontos >= slots[i].up.custo * mult) {

                jogo->pontos -= slots[i].up.custo * mult;
                jogo->pontosPorClique += slots[i].up.bonus * mult;

                slots[i].nivel += mult;
            }
        }
    }
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
    if (!jogo) return -1;

    inicializarJogo(jogo);

    Slot slots[3];

    strcpy(slots[0].up.nome, "CPU");
    slots[0].up.custo = 10;
    slots[0].up.bonus = 1;
    slots[0].nivel = 0;

    strcpy(slots[1].up.nome, "GPU");
    slots[1].up.custo = 50;
    slots[1].up.bonus = 5;
    slots[1].nivel = 0;

    strcpy(slots[2].up.nome, "RAM");
    slots[2].up.custo = 100;
    slots[2].up.bonus = 10;
    slots[2].nivel = 0;

    int multiplicador[3][1] = {
        {1},
        {5},
        {10}
    };

    if (!al_init()) return -1;

    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(jogo->larguraTela, jogo->alturaTela);

    font = al_load_ttf_font("../assets/fonts/ari-w9500.ttf", 20, 0);
    pc = al_load_bitmap("../assets/images/PcTeste.png");

    int largura_original = al_get_bitmap_width(pc);
    int altura_original = al_get_bitmap_height(pc);

    jogo->img_w = 400;
    jogo->img_h = (altura_original * jogo->img_w) / largura_original;

    jogo->img_x = jogo->larguraTela/2 - jogo->img_w/2;
    jogo->img_y = jogo->alturaTela/2 - jogo->img_h/2;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    bool rodando = true;

    while (rodando) {

        desenhar(jogo, pc, font, slots, multiplicador,
                 largura_original, altura_original);

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            rodando = false;

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            tratarClique(jogo, slots, multiplicador,
                         event.mouse.x,
                         event.mouse.y);
    }
   
    // =====================
    // LIMPEZA
    // =====================
    al_destroy_bitmap(pc);
    al_destroy_font(font);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    free(jogo);

    return 0;
}