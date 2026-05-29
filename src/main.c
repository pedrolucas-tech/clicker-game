#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_SLOTS 3
#define BOTAO_W 350
#define BOTAO_H 120
#define ESPACAMENTO 20

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
    ALLEGRO_BITMAP *img;
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
// SAVE
// =====================
void salvarJogo(Jogo *jogo, Slot slots[]) {

    FILE *arquivo = fopen("save.txt", "w");

    if (!arquivo) {
        printf("Erro ao salvar jogo\n");
        return;
    }

    fprintf(arquivo, "%d\n", jogo->pontos);
    fprintf(arquivo, "%d\n", jogo->pontosPorClique);
    fprintf(arquivo, "%d\n", jogo->opcaoCompra);

    for (int i = 0; i < NUM_SLOTS; i++) {
        fprintf(arquivo, "%d\n", slots[i].nivel);
    }

    fclose(arquivo);

    printf("Jogo salvo!\n");
}

// =====================
// LOAD
// =====================
void carregarJogo(Jogo *jogo, Slot slots[]) {

    FILE *arquivo = fopen("save.txt", "r");

    if (!arquivo) {

    printf("Criando novo save...\n");

    salvarJogo(jogo, slots);

    return;
}

    fscanf(arquivo, "%d", &jogo->pontos);
    fscanf(arquivo, "%d", &jogo->pontosPorClique);
    fscanf(arquivo, "%d", (int*)&jogo->opcaoCompra);

    for (int i = 0; i < NUM_SLOTS; i++) {
        fscanf(arquivo, "%d", &slots[i].nivel);
    }

    fclose(arquivo);

    printf("Save carregado!\n");
}

// =====================
// RECURSIVIDADE
// CALCULAR CUSTO
// =====================
int calcularCustoRecursivo(int custoBase, int nivel) {

    // CASO BASE
    if (nivel == 0)
        return custoBase;

    // CHAMADA RECURSIVA
    return calcularCustoRecursivo(
        custoBase,
        nivel - 1
    ) * 1.6;
}

// =====================
// ORDENAÇÃO BUBBLE SORT
// POR CUSTO
// =====================
void ordenarUpgrades(Slot slots[]) {

    for (int i = 0; i < NUM_SLOTS - 1; i++) {

        for (int j = 0; j < NUM_SLOTS - 1 - i; j++) {

            int custoAtual =
                calcularCustoRecursivo(
                    slots[j].up.custo,
                    slots[j].nivel
                );

            int proximoCusto =
                calcularCustoRecursivo(
                    slots[j + 1].up.custo,
                    slots[j + 1].nivel
                );

            if (custoAtual > proximoCusto) {

                Slot temp = slots[j];

                slots[j] = slots[j + 1];

                slots[j + 1] = temp;
            }
        }
    }
}

// =====================
// BUSCA LINEAR
// MELHOR UPGRADE
// =====================
Slot* buscarMelhorUpgrade(Jogo *jogo, Slot slots[]) {

    Slot *melhor = NULL;

    for (int i = 0; i < NUM_SLOTS; i++) {

        if (jogo->pontos >= slots[i].up.custo) {

            if (melhor == NULL ||
                slots[i].up.bonus > melhor->up.bonus) {

                melhor = &slots[i];
            }
        }
    }

    return melhor;
}

// =====================
// DESENHAR
// =====================
void desenhar(Jogo *jogo,
              ALLEGRO_BITMAP *pc,
              ALLEGRO_BITMAP *menu,
              ALLEGRO_BITMAP *cpu_img,
              ALLEGRO_BITMAP *gpu_img,
              ALLEGRO_BITMAP *ram_img,
              ALLEGRO_FONT *font,
              Slot slots[],
              int multiplicador[3][1],
              int largura_original,
              int altura_original) {

    al_clear_to_color(al_map_rgb(0, 0, 0));

    // FUNDO
    al_draw_scaled_bitmap(
        menu,
        0, 0,
        al_get_bitmap_width(menu),
        al_get_bitmap_height(menu),
        0, 0,
        jogo->larguraTela,
        jogo->alturaTela,
        0
    );

    // CENTRALIZA PC
    float area_util_x = jogo->larguraTela - 350;

    jogo->img_x = (area_util_x / 2) - (jogo->img_w / 2) + 90;
    jogo->img_y = (jogo->alturaTela / 2) - (jogo->img_h / 2);

    // DESENHA PC
    al_draw_scaled_bitmap(
        pc,
        0, 0,
        largura_original,
        altura_original,
        jogo->img_x,
        jogo->img_y,
        jogo->img_w,
        jogo->img_h,
        0
    );

    int mult = multiplicador[jogo->opcaoCompra][0];

    Slot *recomendado =
    buscarMelhorUpgrade(jogo, slots);

    // HUD
    float centro_pc_x = jogo->img_x + (jogo->img_w / 2) - 180;
    float base_pc_y = jogo->img_y + jogo->img_h - 60;

    al_draw_textf(
        font,
        al_map_rgb(255,255,255),
        centro_pc_x,
        base_pc_y,
        ALLEGRO_ALIGN_CENTER,
        "Pontos: %d",
        jogo->pontos
    );

    al_draw_textf(
        font,
        al_map_rgb(255,255,0),
        centro_pc_x,
        base_pc_y + 30,
        ALLEGRO_ALIGN_CENTER,
        "Modo Compra: %dx",
        mult
    );

    if (recomendado != NULL) {

    al_draw_textf(
        font,
        al_map_rgb(0,255,255),
        centro_pc_x,
        base_pc_y + 60,
        ALLEGRO_ALIGN_CENTER,
        "Melhor Upgrade: %s",
        recomendado->up.nome
    );
}

    // PAINEL UPGRADES
    int painel_x = jogo->larguraTela - (BOTAO_W + 90);
    int start_y = 150;

    for (int i = 0; i < NUM_SLOTS; i++) {

        int by = start_y + (i * (BOTAO_H + ESPACAMENTO));

        int custoAtual =
            calcularCustoRecursivo(
                slots[i].up.custo,
                slots[i].nivel
            );

        int custoTotal = custoAtual * mult;

        // IMAGEM
        al_draw_scaled_bitmap(
            slots[i].img,
            0, 0,
            al_get_bitmap_width(slots[i].img),
            al_get_bitmap_height(slots[i].img),
            painel_x,
            by,
            BOTAO_W,
            BOTAO_H,
            0
        );

        // BORDA
        al_draw_rectangle(
            painel_x,
            by,
            painel_x + BOTAO_W,
            by + BOTAO_H,
            al_map_rgb(255,255,255),
            1
        );

        // TEXTO
        al_draw_textf(
            font,
            al_map_rgb(255,255,255),
            painel_x + 200,
            by + 35,
            0,
            "%s (Lv %d)",
            slots[i].up.nome,
            slots[i].nivel
        );

        ALLEGRO_COLOR corTexto =
            (jogo->pontos >= custoTotal)
            ? al_map_rgb(0,255,0)
            : al_map_rgb(255,50,50);

        al_draw_textf(
            font,
            corTexto,
            painel_x + 200,
            by + 65,
            0,
            "Custo: $%d",
            custoTotal
        );
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

    // CLICK PC
    int margem = jogo->img_w * 0.15;

    if (x >= jogo->img_x + margem &&
        x <= jogo->img_x + jogo->img_w - margem &&
        y >= jogo->img_y + margem &&
        y <= jogo->img_y + jogo->img_h - margem) {

        jogo->pontos += jogo->pontosPorClique;
        return;
    }

    // UPGRADES
    int painel_x = jogo->larguraTela - 400;
    int start_y = 150;

    for (int i = 0; i < NUM_SLOTS; i++) {

        int by = start_y + (i * (BOTAO_H + ESPACAMENTO));

        if (x >= painel_x &&
            x <= painel_x + BOTAO_W &&
            y >= by &&
            y <= by + BOTAO_H) {

            for (int j = 0; j < mult; j++) {

    int custoAtual =
        calcularCustoRecursivo(
            slots[i].up.custo,
            slots[i].nivel
        );

    if (jogo->pontos >= custoAtual) {

        jogo->pontos -= custoAtual;

        jogo->pontosPorClique +=
            slots[i].up.bonus;

        slots[i].nivel++;

        ordenarUpgrades(slots);
    }
}

            return;
        }
    }

    // TROCAR MODO DE COMPRA
    float centro_pc_x = jogo->img_x + (jogo->img_w / 2) - 180;
    float base_pc_y = jogo->img_y + jogo->img_h - 60;

    float modo_y = base_pc_y + 30;

    if (x >= centro_pc_x - 100 &&
        x <= centro_pc_x + 100 &&
        y >= modo_y - 15 &&
        y <= modo_y + 25) {

        jogo->opcaoCompra++;

        if (jogo->opcaoCompra > MULT_10X)
            jogo->opcaoCompra = MULT_1X;
    }
}

void atribuirImagens(Slot slots[],
                     ALLEGRO_BITMAP *cpu_img,
                     ALLEGRO_BITMAP *gpu_img,
                     ALLEGRO_BITMAP *ram_img) {

    slots[0].img = cpu_img;
    slots[1].img = gpu_img;
    slots[2].img = ram_img;
    
}                    
                     

// =====================
// MAIN
// =====================
int main() {

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *queue = NULL;
    ALLEGRO_EVENT event;

    ALLEGRO_TIMER *timerSave = NULL;

    ALLEGRO_BITMAP *pc = NULL;
    ALLEGRO_BITMAP *menu = NULL;

    ALLEGRO_BITMAP *cpu_img = NULL;
    ALLEGRO_BITMAP *gpu_img = NULL;
    ALLEGRO_BITMAP *ram_img = NULL;

    ALLEGRO_FONT *font = NULL;

    Jogo *jogo = malloc(sizeof(Jogo));

    if (!jogo)
        return -1;

    inicializarJogo(jogo);

    Slot slots[NUM_SLOTS];

    // CPU
    strcpy(slots[0].up.nome, "CPU");
    slots[0].up.custo = 10;
    slots[0].up.bonus = 1;
    slots[0].nivel = 0;
    slots[0].img = NULL;

    // GPU
    strcpy(slots[1].up.nome, "GPU");
    slots[1].up.custo = 50;
    slots[1].up.bonus = 5;
    slots[1].nivel = 0;
    slots[1].img = NULL;

    // RAM
    strcpy(slots[2].up.nome, "RAM");
    slots[2].up.custo = 100;
    slots[2].up.bonus = 10;
    slots[2].nivel = 0;
    slots[2].img = NULL;

    // LOAD SAVE
    carregarJogo(jogo, slots);

    // MATRIZ
    int multiplicador[3][1] = {
        {1},
        {5},
        {10}
    };

    // ALLEGRO
    if (!al_init())
        return -1;

    al_install_mouse();

    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    timerSave = al_create_timer(10.0);

if (!timerSave) {
    printf("Erro timer save\n");
    return -1;
    }
    
    display = al_create_display(
        jogo->larguraTela,
        jogo->alturaTela
    );

    if (!display) {
        printf("Erro display\n");
        return -1;
    }

    // FONT
    font = al_load_ttf_font(
        "../assets/fonts/ari-w9500.ttf",
        20,
        0
    );

    if (!font) {
        printf("Erro fonte\n");
        return -1;
    }

    // IMAGENS
    pc = al_load_bitmap("../assets/images/pc.png");
    menu = al_load_bitmap("../assets/images/menu.png");

    cpu_img = al_load_bitmap("../assets/images/upgrade_CPU.png");
    gpu_img = al_load_bitmap("../assets/images/upgrade_GPU.png");
    ram_img = al_load_bitmap("../assets/images/upgrade_RAM.png");

    atribuirImagens(
    slots,
    cpu_img,
    gpu_img,
    ram_img
);

    if (!pc || !menu ||
        !cpu_img || !gpu_img || !ram_img) {

        printf("Erro imagens\n");
        return -1;
    }

    // TAMANHO PC
    int largura_original = al_get_bitmap_width(pc);
    int altura_original = al_get_bitmap_height(pc);

    jogo->img_w = 780;

    jogo->img_h =
        (altura_original * jogo->img_w)
        / largura_original;

    jogo->img_x =
        (jogo->larguraTela / 2)
        - (jogo->img_w / 2);

    jogo->img_y = 40;

    // EVENTOS
    queue = al_create_event_queue();

    al_register_event_source(
        queue,
        al_get_mouse_event_source()
    );

    al_register_event_source(
        queue,
        al_get_display_event_source(display)
    );

    al_register_event_source(
    queue,
    al_get_timer_event_source(timerSave)
    );

    al_start_timer(timerSave);

    bool rodando = true;

    // LOOP
    while (rodando) {

        desenhar(
            jogo,
            pc,
            menu,
            cpu_img,
            gpu_img,
            ram_img,
            font,
            slots,
            multiplicador,
            largura_original,
            altura_original
        );

        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

            salvarJogo(jogo, slots);

            rodando = false;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {

            salvarJogo(jogo, slots);

            printf("Auto save!\n");
}

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {

            tratarClique(
                jogo,
                slots,
                multiplicador,
                event.mouse.x,
                event.mouse.y
            );
        }
    }

    // LIMPEZA
    al_destroy_bitmap(pc);
    al_destroy_bitmap(menu);

    al_destroy_bitmap(cpu_img);
    al_destroy_bitmap(gpu_img);
    al_destroy_bitmap(ram_img);

    al_destroy_font(font);

    al_destroy_event_queue(queue);
    al_destroy_timer(timerSave);
    al_destroy_display(display);

    free(jogo);

    return 0;
}