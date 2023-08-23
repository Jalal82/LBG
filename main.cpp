#include <iostream>
#include <SDL2/SDL.h>
#include <cstring>

class Grid
{
private:
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    int _w, _h;

public:
    uint8_t cell_size = 14;   // 14 px
    uint8_t digit_width = 6;  // 6 rows
    uint8_t digit_height = 8; // 8 cols

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    bool **grid = nullptr;

    SDL_Event event;
    bool quit = false;

    Grid(int h, int w)
    {
        _w = w;
        _h = h;
        SCREEN_WIDTH = cell_size * w;
        SCREEN_HEIGHT = cell_size * h;
        SDL_Init(SDL_INIT_VIDEO);

        window = SDL_CreateWindow("Bitmap Generator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Dynamically allocate memory for the grid
        grid = new bool *[_w];
       
            for (int i = 0; i < _w; i++)
            {
                grid[i] = new bool[_h];
                if (grid != NULL)
                    std::memset(grid[i], false, _h * sizeof(bool));
            }
    }

    ~Grid()
    {
        // Cleanup
        for (int i = 0; i < _w; i++)
        {
            delete[] grid[i];
        }
        delete[] grid;

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void renderGrid()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // Set black background
        SDL_RenderClear(renderer);

        // Draw grid lines
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE); // Set gray color
        for (int x = 0; x < _w; x++)
        {
            SDL_RenderDrawLine(renderer, x * cell_size, 0, x * cell_size, SCREEN_HEIGHT);
        }
        for (int y = 0; y < _h; y++)
        {
            SDL_RenderDrawLine(renderer, 0, y * cell_size, SCREEN_WIDTH, y * cell_size);
        }

        // Render filled cells
        for (int x = 0; x < _w; x++)
        {
            for (int y = 0; y < _h; y++)
            {
                if (grid[x][y])
                {
                    SDL_Rect cellRect = {x * cell_size, y * cell_size, cell_size, cell_size};
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE); // Green color for clicked cells
                    SDL_RenderFillRect(renderer, &cellRect);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    void handleEvents()
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                int clickedX = mouseX / cell_size;
                int clickedY = mouseY / cell_size;

                if (clickedX >= 0 && clickedX < _w && clickedY >= 0 && clickedY < _h)
                {
                    grid[clickedX][clickedY] = !grid[clickedX][clickedY]; // Toggle clicked cell
                    std::cout << "Clicked cell: (" << clickedX << ", " << clickedY << ")" << std::endl;
                }
            }
        }
    }
};

int WinMain()
{
    const int WIDTH = 6;
    const int HEIGHT = 8;

    Grid grid(HEIGHT, WIDTH);

    while (!grid.quit)
    {
        grid.handleEvents();
        grid.renderGrid();
    }

    return 0;
}