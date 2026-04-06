#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

int main() {
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_EVENT event;
    ALLEGRO_BITMAP *pc = NULL;

    int pontos = 0;

    // Inicia os Processos
    if (!al_init()) return -1;

    al_install_mouse();
    al_init_image_addon();
    al_init_primitives_addon();

    display = al_create_display(800, 600);
    if (!display) return -1;

    // carregar imagem
    pc = al_load_bitmap("../assets/images/PcTeste.png");

    if (!pc) {
        printf("ERRO: nao carregou a imagem!\n");
        al_rest(5.0);
        return -1;
    }

    // pega tamanho original
    int largura_original = al_get_bitmap_width(pc);
    int altura_original = al_get_bitmap_height(pc);

    // define largura desejada
    int img_w = 200;

    // Calcula para ficar proporcional
    int img_h = (altura_original * img_w) / largura_original;

    // Centralias a Imagem
    int img_x = 800/2 - img_w/2;
    int img_y = 600/2 - img_h/2;

    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    // loop principal
    while (1) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        // desenha imagem proporcional
        al_draw_scaled_bitmap(
            pc,
            0, 0,
            largura_original,
            altura_original,
            img_x, img_y,
            img_w, img_h,
            0
        );

        al_flip_display();

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int x = event.mouse.x;
            int y = event.mouse.y;

            // verifica clique na imagem
            if (x >= img_x && x <= img_x + img_w &&
                y >= img_y && y <= img_y + img_h) {

                pontos++;
                printf("Pontos: %d\n", pontos);
            }
        }
    }

    // liberar memória
    al_destroy_bitmap(pc);
    al_destroy_display(display);
    al_destroy_event_queue(queue);

    return 0;
}