#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_FRect health_bar;

#define WIDTH 960
#define HEIGHT 540

SDL_AppResult
SDL_AppInit(void** appstate, int argc, char** argv)
{
    // create a window
    if (!SDL_CreateWindowAndRenderer("Hello, Triangle!",
                                     WIDTH,
                                     HEIGHT,
                                     SDL_WINDOW_RESIZABLE,
                                     &window,
                                     &renderer)) {
        SDL_Log("error: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void* appstate)
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    health_bar.w = WIDTH / 3.0;
    health_bar.h = HEIGHT / 20.0;
    health_bar.x = (WIDTH - health_bar.w) / 2.0;
    health_bar.y = (HEIGHT - health_bar.h) / 2.0;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &health_bar);

    long scale = SDL_GetTicks() / 100 % 101;
    health_bar.w = health_bar.w / 100 * scale;
    SDL_RenderFillRect(renderer, &health_bar);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event)
{
    // close the window on request
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_DestroyWindow(window);
}