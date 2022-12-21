#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WINDOW_W 800
#define WINDOW_H 800

// --------------------------------------
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;
} Game;
// --------------------------------------

void InitGame(Game* g) {
    g->running = 1;
    g->window = SDL_CreateWindow(
        "Hello, World!", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE
    );

    g->renderer = SDL_CreateRenderer(
        g->window,
        -1,
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );
}

void DestroyGame(Game* game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
}

void HandleGameEvents(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                game->running = 0;
                break;
            }
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                game->running = 0;
                break;
            }
        }
    }
}

void CreateNoiseTextures(SDL_Renderer* renderer, SDL_Texture** tex_noise) {
    // noise texture function - not made by me

    // texture size: 20
    // noise texture (20 in array, randomly sort in loop)
    unsigned char buff[WINDOW_W * WINDOW_H * 3];
    int x, y;
    for (x = 0; x < 20; x++) // create 20 noise textures to draw over screen
    {
        for (y = 0; y < WINDOW_W * WINDOW_H; y++)
        {
            unsigned char c = rand() % 255;
            buff[y * 3] = c;     // red
            buff[y * 3 + 1] = c; // green
            buff[y * 3 + 2] = c; // blue
        }
        // create texture and set its blending properties
        tex_noise[x] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STATIC, WINDOW_W, WINDOW_H);
        SDL_UpdateTexture(tex_noise[x], 0, buff, WINDOW_W * 3);
        SDL_SetTextureBlendMode(tex_noise[x], SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(tex_noise[x], 150); // set strength of texture blend from 0-255
    }
}

int main() {

    Game g;
    InitGame(&g);

    // init textures

    SDL_Texture *background_layer = IMG_LoadTexture(g.renderer, "pyramid.png");

    SDL_Texture *shadow_layer = SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, 800, 800);

    SDL_Texture *light_texture_1 = IMG_LoadTexture(g.renderer, "circle2.png");
    SDL_Texture *light_texture_2 = IMG_LoadTexture(g.renderer, "light-solid.png");

    SDL_Texture *noise_texture[20];
    CreateNoiseTextures(g.renderer, noise_texture);
    
    // ----------

    while (g.running) {
        HandleGameEvents(&g);
      
        // create shadow layer ------------
        SDL_SetRenderTarget(g.renderer, shadow_layer);
        SDL_RenderClear(g.renderer);

        SDL_SetRenderDrawColor(g.renderer, 100, 100, 100, 255);
        SDL_RenderDrawRect(g.renderer, NULL);

        // copy noise texture
        SDL_RenderCopy(g.renderer, noise_texture[rand()%20], NULL, NULL);

        // copy light textures
        SDL_SetTextureBlendMode(light_texture_1, SDL_BLENDMODE_ADD); 
        SDL_SetTextureBlendMode(light_texture_2, SDL_BLENDMODE_ADD); 

        int light_size = 300;
        
        // first light point
        SDL_Rect light_rect_1 = (SDL_Rect){200,200,light_size,light_size};
        SDL_RenderCopy(g.renderer, light_texture_1, NULL, &light_rect_1);

        // second light point
        int mx = 0; int my = 0;
        SDL_GetMouseState(&mx, &my);
        SDL_Rect light_rect_2 = (SDL_Rect){mx-(light_size/2),my-(light_size/2),light_size,light_size};
        SDL_RenderCopy(g.renderer, light_texture_2, NULL, &light_rect_2);

        // ---------------

        // reset render target
        SDL_SetRenderTarget(g.renderer, NULL);
        SDL_RenderClear(g.renderer);

        // copy background layer to game renderer
        SDL_RenderCopy(g.renderer, background_layer, NULL, NULL);

        // copy shadow layer to game renderer
        SDL_SetTextureBlendMode(shadow_layer, SDL_BLENDMODE_MOD);
        SDL_RenderCopy(g.renderer, shadow_layer, NULL, NULL);


        //SDL_RenderCopy(g.renderer, noise_texture[rand()%20], NULL, NULL);
        SDL_RenderPresent(g.renderer);
    }

    // cleanup textures
    SDL_DestroyTexture(background_layer);
    SDL_DestroyTexture(shadow_layer);

    SDL_DestroyTexture(light_texture_1);
    SDL_DestroyTexture(light_texture_2);

    SDL_DestroyTexture(*noise_texture);
    
    // cleanup game

    DestroyGame(&g);
    return 0;
}