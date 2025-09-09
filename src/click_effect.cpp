#include <SDL3/SDL.h>
#include <vector>
#include <cmath>
#include <iostream>

class Effect
{
  public:
    float x, y, radius, alpha;
    Uint32 start;

    Effect(float x, float y)
      : x(x)
      , y(y)
      , radius(0)
      , alpha(255)
      , start(SDL_GetTicks())
    {
    }

    bool update()
    {
        Uint32 elapsed = SDL_GetTicks() - start;
        if (elapsed >= 500)
            return false;

        float t = elapsed / 500.0f;
        radius = t * t * 50.0f; // Eased growth

        // Smooth fade out - chậm ở đầu, nhanh ở cuối
        float fade = 1.0f - t * t * t; // Cubic fade out
        alpha = 255 * fade;
        return true;
    }

    void render(SDL_Renderer* r)
    {
        SDL_SetRenderDrawColor(r, 255, 255, 255, (Uint8)alpha);
        int rad = (int)radius;
        for (int dy = -rad; dy <= rad; dy++) {
            int dx = (int)sqrt(rad * rad - dy * dy);
            for (int i = -dx; i <= dx; i++)
                SDL_RenderPoint(r, (int)x + i, (int)y + dy);
        }
    }
};

int
main()
{
    SDL_Init(SDL_INIT_VIDEO);
    auto window = SDL_CreateWindow("Blue Archive Click", 800, 600, 0);
    auto renderer = SDL_CreateRenderer(window, nullptr);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    std::vector<Effect> effects;
    SDL_Event e;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT ||
                (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE))
                running = false;
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                e.button.button == SDL_BUTTON_LEFT)
                effects.emplace_back(e.button.x, e.button.y);
        }

        // Update và cleanup
        for (auto it = effects.begin(); it != effects.end();) {
            it = it->update() ? ++it : effects.erase(it);
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 25, 35, 55, 255);
        SDL_RenderClear(renderer);
        for (auto& effect : effects)
            effect.render(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}