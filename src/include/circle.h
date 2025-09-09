#include <SDL3/SDL.h>

class Circle
{
  private:
    int centerX, centerY, radius;
    bool filled;

  public:
    Circle(int x, int y, int r, bool fill = false)
      : centerX(x)
      , centerY(y)
      , radius(r)
      , filled(fill)
    {
    }

    void draw(SDL_Renderer* renderer)
    {
        if (filled) {
            fillCircleOptimized(renderer, centerX, centerY, radius);
        } else {
            drawCircle(renderer, centerX, centerY, radius);
        }
    }

    bool isPointInside(int x, int y)
    {
        int dx = x - centerX;
        int dy = y - centerY;
        return (dx * dx + dy * dy) <= (radius * radius);
    }

    // Vẽ đường tròn rỗng
    void drawCircle(SDL_Renderer* renderer,
                    int centerX,
                    int centerY,
                    int radius)
    {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        while (x <= y) {
            // Vẽ 8 điểm đối xứng
            SDL_RenderPoint(renderer, centerX + x, centerY + y);
            SDL_RenderPoint(renderer, centerX - x, centerY + y);
            SDL_RenderPoint(renderer, centerX + x, centerY - y);
            SDL_RenderPoint(renderer, centerX - x, centerY - y);
            SDL_RenderPoint(renderer, centerX + y, centerY + x);
            SDL_RenderPoint(renderer, centerX - y, centerY + x);
            SDL_RenderPoint(renderer, centerX + y, centerY - x);
            SDL_RenderPoint(renderer, centerX - y, centerY - x);

            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }

    // Vẽ hình tròn đặc (filled circle)
    void fillCircle(SDL_Renderer* renderer,
                    int centerX,
                    int centerY,
                    int radius)
    {
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x * x + y * y <= radius * radius) {
                    SDL_RenderPoint(renderer, centerX + x, centerY + y);
                }
            }
        }
    }

    // Cách tối ưu hơn cho filled circle
    void fillCircleOptimized(SDL_Renderer* renderer,
                             int centerX,
                             int centerY,
                             int radius)
    {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        while (x <= y) {
            // Vẽ đường ngang
            for (int i = centerX - x; i <= centerX + x; i++) {
                SDL_RenderPoint(renderer, i, centerY + y);
                SDL_RenderPoint(renderer, i, centerY - y);
            }
            for (int i = centerX - y; i <= centerX + y; i++) {
                SDL_RenderPoint(renderer, i, centerY + x);
                SDL_RenderPoint(renderer, i, centerY - x);
            }

            if (d < 0) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * (x - y) + 10;
                y--;
            }
            x++;
        }
    }
};
