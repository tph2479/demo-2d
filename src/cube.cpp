#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

struct Triangle
{
    Vertex v1, v2, v3;
    float depth; // Để depth sorting
};

class SimpleCube
{
  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* frameBuffer;
    int screenWidth, screenHeight;

    std::vector<Vertex> cubeVertices;
    std::vector<Triangle> triangles;

    // Định nghĩa các đỉnh của khối lập phương
    void InitializeCube()
    {
        cubeVertices = {
            // Front face (đỏ)
            { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } }, // 0
            { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },  // 1
            { { 0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },   // 2
            { { -0.5f, 0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },  // 3

            // Back face (xanh lá)
            { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }, // 4
            { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },  // 5
            { { 0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },   // 6
            { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } }   // 7
        };
    }

    // Chuyển đổi tọa độ 3D sang 2D với perspective projection
    glm::vec2 ProjectTo2D(const glm::vec3& point3D, const glm::mat4& mvp)
    {
        glm::vec4 clipSpace = mvp * glm::vec4(point3D, 1.0f);

        // Perspective divide
        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

        // Viewport transform
        float x = (ndc.x + 1.0f) * 0.5f * screenWidth;
        float y = (1.0f - ndc.y) * 0.5f * screenHeight;

        return glm::vec2(x, y);
    }

    // Vẽ tam giác với màu đồng nhất
    void DrawTriangle(const glm::vec2& p1,
                      const glm::vec2& p2,
                      const glm::vec2& p3,
                      const glm::vec3& color)
    {
        // Đơn giản hóa: chỉ vẽ outline
        Uint8 r = (Uint8)(color.r * 255);
        Uint8 g = (Uint8)(color.g * 255);
        Uint8 b = (Uint8)(color.b * 255);

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        // Vẽ 3 cạnh của tam giác
        SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
    }

    // Tạo các tam giác cho khối lập phương
    void GenerateTriangles(const glm::mat4& mvp)
    {
        triangles.clear();

        // Định nghĩa các mặt của cube (mỗi mặt = 2 tam giác)
        int faces[12][3] = { // Front face
                             { 0, 1, 2 },
                             { 2, 3, 0 },
                             // Back face
                             { 4, 5, 6 },
                             { 6, 7, 4 },
                             // Left face
                             { 7, 3, 0 },
                             { 0, 4, 7 },
                             // Right face
                             { 1, 5, 6 },
                             { 6, 2, 1 },
                             // Top face
                             { 3, 2, 6 },
                             { 6, 7, 3 },
                             // Bottom face
                             { 0, 1, 5 },
                             { 5, 4, 0 }
        };

        for (int i = 0; i < 12; i++) {
            Vertex v1 = cubeVertices[faces[i][0]];
            Vertex v2 = cubeVertices[faces[i][1]];
            Vertex v3 = cubeVertices[faces[i][2]];

            // Transform vertices
            glm::vec4 transformedV1 = mvp * glm::vec4(v1.position, 1.0f);
            glm::vec4 transformedV2 = mvp * glm::vec4(v2.position, 1.0f);
            glm::vec4 transformedV3 = mvp * glm::vec4(v3.position, 1.0f);

            // Calculate average depth for sorting
            float depth =
              (transformedV1.z + transformedV2.z + transformedV3.z) / 3.0f;

            Triangle tri;
            tri.v1 = v1;
            tri.v2 = v2;
            tri.v3 = v3;
            tri.depth = depth;

            triangles.push_back(tri);
        }

        // Sort by depth (front to back)
        std::sort(triangles.begin(),
                  triangles.end(),
                  [](const Triangle& a, const Triangle& b) {
                      return a.depth < b.depth;
                  });
    }

  public:
    SimpleCube(int width = 800, int height = 600)
      : screenWidth(width)
      , screenHeight(height)
    {
        Initialize();
    }

    bool Initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("3D Cube - SDL3 + GLM Software Rendering",
                                  screenWidth,
                                  screenHeight,
                                  SDL_WINDOW_RESIZABLE);
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError()
                      << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError()
                      << std::endl;
            return false;
        }

        InitializeCube();
        return true;
    }

    void Render()
    {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        // Tạo transformation matrices với GLM
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // Xoay cube theo thời gian
        float time = SDL_GetTicks() / 1000.0f;
        model = glm::rotate(model, time * 0.5f, glm::vec3(1.0f, 1.0f, 0.8f));

        // Đặt camera
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // Tạo perspective projection
        projection = glm::perspective(glm::radians(40.0f),
                                      (float)screenWidth / (float)screenHeight,
                                      0.1f,
                                      50.0f);

        // Model-View-Projection matrix
        glm::mat4 mvp = projection * view * model;

        // Generate và sort triangles
        GenerateTriangles(mvp);

        // Vẽ tất cả triangles
        for (const auto& triangle : triangles) {
            glm::vec2 p1 = ProjectTo2D(triangle.v1.position, mvp);
            glm::vec2 p2 = ProjectTo2D(triangle.v2.position, mvp);
            glm::vec2 p3 = ProjectTo2D(triangle.v3.position, mvp);

            // Sử dụng màu trung bình của 3 đỉnh
            glm::vec3 avgColor =
              (triangle.v1.color + triangle.v2.color + triangle.v3.color) /
              3.0f;

            DrawTriangle(p1, p2, p3, avgColor);
        }

        // Vẽ wireframe với độ mờ dần theo depth
        for (int i = 0; i < triangles.size(); i++) {
            const auto& triangle = triangles[i];

            // Cạnh ở gần (đầu danh sách) sẽ sáng, cạnh xa (cuối danh sách) sẽ
            // tối
            float depthFactor = (float)i / (float)triangles.size(); // Đảo ngược
            Uint8 alpha = (Uint8)((1.0f - depthFactor) * 120 +
                                  30); // Gần = mờ ít, xa = mờ nhiều
            Uint8 brightness =
              (Uint8)((1.0f - depthFactor) * 180 + 60); // Gần = sáng, xa = tối

            SDL_SetRenderDrawColor(
              renderer, brightness, brightness, brightness, alpha);

            glm::vec2 p1 = ProjectTo2D(triangle.v1.position, mvp);
            glm::vec2 p2 = ProjectTo2D(triangle.v2.position, mvp);
            glm::vec2 p3 = ProjectTo2D(triangle.v3.position, mvp);

            SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
            SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
            SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
        }

        SDL_RenderPresent(renderer);
    }

    void HandleResize(int width, int height)
    {
        screenWidth = width;
        screenHeight = height;
    }

    void Cleanup()
    {
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    ~SimpleCube() { Cleanup(); }
};

int
main(int argc, char* argv[])
{
    SimpleCube cube(800, 600);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }

                    break;

                case SDL_EVENT_WINDOW_RESIZED:
                    cube.HandleResize(event.window.data1, event.window.data2);
                    break;
            }
        }

        cube.Render();
        // SDL_Delay(16); // ~60 FPS
    }

    return 0;
}