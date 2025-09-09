#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

class Button {
private:
SDL_FRect button;

public:
    Button() = default;
    ~Button() = default;

    Button(int x, int y, int w, int h) {
        this->button.x = x;
        this->button.y = y;
        this->button.w = w;
        this->button.h = h;
    }

    void draw(SDL_Renderer * render) {
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        SDL_RenderRect(render, &this->button);
    }

    void handle(SDL_Event * event) {
        // if(event->type == )
    }
};