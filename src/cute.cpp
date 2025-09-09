#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static int texture_width = 0;
static int texture_height = 0;

#define WINDOW_WIDTH 1360
#define WINDOW_HEIGHT 768

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_Surface* surface = NULL;
    char* bmp_path = NULL;

    SDL_SetAppMetadata(
      "Example Renderer Textures", "1.0", "com.example.renderer-textures");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/textures",
                                     WINDOW_WIDTH,
                                     WINDOW_HEIGHT,
                                     0,
                                     &window,
                                     &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Textures are pixel data that we upload to the video hardware for fast
       drawing. Lots of 2D engines refer to these as "sprites." We'll do a
       static texture (upload once, draw many times) with data from a bitmap
       file. */

    /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data
       the GPU can access. Load a .bmp into a surface, move it to a texture from
       there. */
    SDL_asprintf(
      &bmp_path,
      "%simages/sample.bmp",
      SDL_GetBasePath()); /* allocate a string of the full file path */

    SDL_Log("base path: %s", SDL_GetBasePath());
    surface = SDL_LoadBMP(bmp_path);
    if (!surface) {
        SDL_Log("Couldn't load bitmap: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(bmp_path); /* done with this, the file is loaded. */

    texture_width = surface->w;
    texture_height = surface->h;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(
      surface); /* done with this, the texture has a copy of the pixels now. */

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS.
                                 */
    }
    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate)
{
    SDL_FRect dst_rect;
    const Uint64 now = SDL_GetTicks();

    /* we'll have some textures move around over a few seconds. */
    const float direction = ((now % 4000) >= 2000) ? 1.0f : -1.0f;
    const float scale =
      ((float)(((int)(now % 2000)) - 1000) / 999.0f) * direction;
    /* as you can see from this, rendering draws over whatever was drawn before
     * it. */
    SDL_SetRenderDrawColor(
      renderer, 189, 229, 252, SDL_ALPHA_OPAQUE); /* black, full alpha */
    SDL_RenderClear(renderer);              /* start with a blank canvas. */

    SDL_SetRenderDrawColor(renderer, 99, 185, 219, SDL_ALPHA_OPAQUE);
    dst_rect.x = 0;
    dst_rect.y = WINDOW_HEIGHT - WINDOW_HEIGHT * 2.0 / 3.0;
    dst_rect.w = WINDOW_WIDTH;
    dst_rect.h = WINDOW_HEIGHT * 2.0 / 3.0;
    SDL_RenderFillRect(renderer, &dst_rect);

    /* Just draw the static texture a few times. You can think of
       it like a stamp, there isn't a limit to the number of
       times you can draw with it.
     */

    /* top left */
    dst_rect.x = 0.0f;
    dst_rect.y = 100 * scale + 100;
    dst_rect.w = (float)texture_width;
    dst_rect.h = (float)texture_height;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    /* center this one. */
    dst_rect.x =
      (WINDOW_WIDTH - texture_width) / 2.0f + 100.0f * SDL_sin(now / 1000.0f);
    dst_rect.y =
      (WINDOW_HEIGHT - texture_height) / 2.0f + 100.0f * SDL_cos(now / 1000.0f);
    dst_rect.w = (float)texture_width / 2;
    dst_rect.h = (float)texture_height / 2;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    dst_rect.x = (WINDOW_WIDTH - texture_width) / 2.0f +
                 100.0f * SDL_sin(now / 1000.0f + SDL_PI_F);
    dst_rect.y = (WINDOW_HEIGHT - texture_height) / 2.0f +
                 100.0f * SDL_cos(now / 1000.0f + SDL_PI_F);
    dst_rect.w = (float)texture_width / 2;
    dst_rect.h = (float)texture_height / 2;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    /* bottom right. */
    dst_rect.x =
      (float)(WINDOW_WIDTH - texture_width) - (100.0f * scale) - 100.0;
    dst_rect.y = (float)(WINDOW_HEIGHT - texture_height);
    dst_rect.w = (float)texture_width;
    dst_rect.h = (float)texture_height;
    SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

    SDL_RenderPresent(renderer); /* put it all on the screen! */

    return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
    /* SDL will clean up the window/renderer for us. */
}