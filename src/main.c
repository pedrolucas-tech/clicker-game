#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

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
// FUNÇÃO DE INICIALIZAÇÃO
// =====================
void inicializarJogo(Jogo *jogo) {
    jogo->pontos = 0;
    jogo->pontosPorClique = 1;

    jogo->larguraTela = 1280;
    jogo->alturaTela = 720;
}

// =====================
// MAIN
// =====================
int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP *pc = NULL;

    Jogo jogo;

    // Inicializa struct
    inicializarJogo(&jogo);

    // Inicia Allegro
    if (!al_init()) return -1;

    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();

    display = al_create_display(jogo.larguraTela, jogo.alturaTela);
    if (!display) {
        printf("Erro ao criar a tela\n");
        return -1;
    }

    // Carrega imagem
    pc = al_load_bitmap("C:/clicker-game/assets/images/PcTeste.png");
    if (!pc) {
        printf("Erro ao carregar imagem\n");
        return -1;
    }

    // Tamanho original
    int largura_original = al_get_bitmap_width(pc);
    int altura_original = al_get_bitmap_height(pc);

    // Ajuste proporcional
    jogo.img_w = 400;
    jogo.img_h = (altura_original * jogo.img_w) / largura_original;

    // Centralizar
    jogo.img_x = jogo.larguraTela/2 - jogo.img_w/2;
    jogo.img_y = jogo.alturaTela/2 - jogo.img_h/2;

    // Eventos
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    bool rodando = true;

    while (rodando) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        // Desenha imagem
        al_draw_scaled_bitmap(
            pc,
            0, 0,
            largura_original,
            altura_original,
            jogo.img_x, jogo.img_y,
            jogo.img_w, jogo.img_h,
            0
        );

        al_flip_display();

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int x = event.mouse.x;
            int y = event.mouse.y;

            if (x >= jogo.img_x && x <= jogo.img_x + jogo.img_w &&
                y >= jogo.img_y && y <= jogo.img_y + jogo.img_h) {

                jogo.pontos += jogo.pontosPorClique;
                printf("Pontos: %d\n", jogo.pontos);
            }
        }
    }

    // Limpeza
    al_destroy_bitmap(pc);
    al_destroy_display(display);
    al_destroy_event_queue(queue);

    return 0;
}