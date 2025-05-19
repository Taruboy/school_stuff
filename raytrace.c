#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_GRAY 0xefefefef
#define COLOR_RAY 0xfffa78
#define RAYS_NUMBER 400
#define RAY_THICKNESS 1

struct Circle {
    double x;
    double y;
    double radius;

    int x_velocity;
    int y_velocity;
};

struct Ray {
    double x_start, y_start;
    double angle;
};

// 원 그리기 픽셀마다 피타고라스 해서 반지름내에 검사
static void FillCircle(SDL_Surface* surface, struct Circle circle, Uint32 color)
{
    double radius_squared = pow(circle.radius, 2);
    for (double x = circle.x - circle.radius; x <= circle.x + circle.radius; x++)
    {
        for (double y = circle.y - circle.radius; y <= circle.y + circle.radius; y++)
        {
            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
            if (distance_squared <= radius_squared)
            {
                SDL_Rect pixel = (SDL_Rect){ x, y, 1, 1 };
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

static void generate_rays(struct Circle circle, struct Ray rays[RAYS_NUMBER]) {
    for (int i = 0; i < RAYS_NUMBER; i++) {
        double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
        double surface_x_start = circle.x + cos(angle) * circle.radius;
        double surface_y_start = circle.y + sin(angle) * circle.radius;

        struct Ray ray = { surface_x_start, surface_y_start, angle };
        rays[i] = ray;
    } 
}

static void FillRays(SDL_Surface* surface, struct Ray rays[RAYS_NUMBER], Uint32 color, struct Circle object) 
{
    double radius_squared = pow(object.radius, 2);
    for (int i = 0; i < RAYS_NUMBER; i++) {
        struct Ray ray = rays[i];

        int end_of_screen = 0;
        int object_hit = 0;

        double x_draw = ray.x_start;
        double y_draw = ray.y_start;
        while (!end_of_screen && !object_hit) {
            x_draw += cos(ray.angle);
            y_draw += sin(ray.angle);

            SDL_Rect ray_dot = (SDL_Rect){ x_draw, y_draw, RAY_THICKNESS, RAY_THICKNESS };
            SDL_FillRect(surface, &ray_dot, color);

            if (x_draw < 0 || x_draw > WIDTH) end_of_screen = 1;
            if (y_draw < 0 || y_draw > HEIGHT) end_of_screen = 1;
            double distance_squared = pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);
            if (distance_squared < radius_squared) object_hit = 1;
        }
    }
}

static void move_obstacle_circle(struct Circle* obstacle) {
    if (obstacle->x - obstacle->radius < 0 || obstacle->x + obstacle->radius > WIDTH) {
        obstacle->x_velocity *= -1;
    }
    if (obstacle->y - obstacle->radius < 0 || obstacle->y + obstacle->radius > HEIGHT) {
        obstacle->y_velocity *= -1;
    }
    obstacle->y += obstacle->y_velocity;
    obstacle->x += obstacle->x_velocity;
}

// 1. 사각형 만들기
// 2. 첫번째 동그라미 만들기
// 3. 종료 기능 만들기
// 4. 마우스 따라다니느 첫번째 원 만들기
// 5. 두번째 원 만들기
// 6. Ray 구조체 만들기
// 7. generate_rays 함수 만들기
// 8. FillRays (중요부분)


int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    // 종이 가져오기
    SDL_Surface* surface = SDL_GetWindowSurface(window); 

    // 사각형 만들기
    //SDL_Rect rect = (SDL_Rect){ 200, 200, 200, 200 };
    //SDL_FillRect(surface, &rect, COLOR_WHITE);

    struct Circle circle = { 300, 300, 40, 0, 0 };
    struct Circle obstacle_circle = { 550, 300, 140, 1, 1 };
    SDL_Rect erase_rect = (SDL_Rect){ 0, 0, WIDTH, HEIGHT };
    
    struct Ray rays[RAYS_NUMBER];
    generate_rays(circle, rays);

    int simulation_running = 1;
    SDL_Event event;
    while (simulation_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) simulation_running = 0;
            if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
                circle.x = event.motion.x;
                circle.y = event.motion.y;
                generate_rays(circle, rays);
            }
        }
        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        FillCircle(surface, circle, COLOR_WHITE);
        FillCircle(surface, obstacle_circle, COLOR_WHITE);
        FillRays(surface, rays, COLOR_RAY, obstacle_circle);

        move_obstacle_circle(&obstacle_circle);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);
    }
}
