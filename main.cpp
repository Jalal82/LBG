#include <iostream>
#include <SDL2/SDL.h>
#include <cstring>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <Windows.h>

class Grid
{
private:
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    int _w, _h, _mode;

    struct Button
    {
        SDL_Rect rect;
        SDL_Color color;
        bool clicked;
        int radius;
    };

public:
    uint8_t cell_size = 14;   // 14 px
    uint8_t digit_width = 6;  // 6 rows
    uint8_t digit_height = 8; // 8 cols

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *font = nullptr;
    TTF_Font *font2 = nullptr;

    bool checkboxChecked = false;
    std::string textViewText = "Get Hex";
    std::string output_matrix = "";
    std::string output_prefex = "";
    std::string output_hex = "";
    bool **grid = nullptr;

    Button button;
    SDL_Rect textViewRect, out_box;

    SDL_Event event;
    bool quit = false;

    uint16_t width_offset = 0;
    uint16_t height_offset = 0;
    Grid(int h, int w, int mode)
    {
        _w = w;
        _h = h;
        _mode = mode;
        SCREEN_WIDTH = cell_size * w;
        SCREEN_HEIGHT = cell_size * h;
        width_offset = 35 * _w;
        height_offset = 5 * _h;
        if (width_offset <= 200)
            width_offset = 250;
        if (height_offset <= 60)
            height_offset = 100;
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();

        window = SDL_CreateWindow("Bitmap Generator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH + width_offset, SCREEN_HEIGHT + height_offset, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        font = TTF_OpenFont("font.ttf", 12);
        font2 = TTF_OpenFont("font.ttf", 18);
        if (!font)
        {
            std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        }

        button.rect = {((SCREEN_WIDTH)-70) / 2, SCREEN_HEIGHT + ((height_offset - 30) / 2), 70, 30}; // Change the button position and size as per your requirements
        button.color = {255, 130, 138, 255};                                                         // Green color for button
        button.radius = 3;                                                                           // Set the radius to 10 pixels
        textViewRect = {button.rect.x + 5, button.rect.y, 60, 30};                                   // Change the text

        out_box = {SCREEN_WIDTH + 10, 5, width_offset - 15, height_offset - 10};

        output_prefex = "//output bitmap \nuint8_t out[" + std::to_string(_w) + "][" + std::to_string(_h) + "]={";

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

    void render_btn()
    {
        // SDL_SetRenderDrawColor(renderer, button.color.r, button.color.g, button.color.b, button.color.a);
        // SDL_RenderFillRect(renderer, &button.rect);
        roundedBoxRGBA(renderer, button.rect.x, button.rect.y, button.rect.x + button.rect.w, button.rect.y + button.rect.h, button.radius, button.color.r, button.color.g, button.color.b, button.color.a);
    }

    bool isPointInside_btn(int x, int y)
    {
        return (x >= button.rect.x && x <= button.rect.x + button.rect.w && y >= button.rect.y && y <= button.rect.y + button.rect.h);
    }

    void render_out_matrix()
    {
        output_hex = output_prefex + output_matrix;
        SDL_Color textColor = {214, 214, 214}; // White color for text
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, output_hex.c_str(), textColor, out_box.w);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        out_box.x = SCREEN_WIDTH + 5;
        out_box.y = 0;
        out_box.w = textSurface->w;
        out_box.h = textSurface->h;

        SDL_RenderCopy(renderer, textTexture, nullptr, &out_box);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void renderButtonAndTextView()
    {
        render_btn();

        SDL_Color textColor = {31, 31, 31}; // White color for text
        SDL_Surface *textSurface = TTF_RenderText_Solid(font2, textViewText.c_str(), textColor);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_RenderCopy(renderer, textTexture, nullptr, &textViewRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    void copyToClipboard(const std::string &text)
    {
        if (OpenClipboard(nullptr))
        {
            EmptyClipboard();

            HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
            if (hClipboardData)
            {
                char *pchData = static_cast<char *>(GlobalLock(hClipboardData));
                if (pchData)
                {
                    strcpy_s(pchData, text.size() + 1, text.c_str());
                    GlobalUnlock(hClipboardData);
                    SetClipboardData(CF_TEXT, hClipboardData);
                }
            }

            CloseClipboard();
        }
    }

    void renderGrid()
    {
        SDL_SetRenderDrawColor(renderer, 31, 31, 31, SDL_ALPHA_OPAQUE); // Set black background
        SDL_RenderClear(renderer);

        // Draw grid lines
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE); // Set gray color
        for (int x = 0; x <= _w; x++)
        {
            SDL_RenderDrawLine(renderer, x * cell_size, 0, x * cell_size, SCREEN_HEIGHT);
        }
        for (int y = 0; y <= _h; y++)
        {
            SDL_RenderDrawLine(renderer, 0, y * cell_size, SCREEN_WIDTH, y * cell_size);
        }

        // Render filled cells and get bitmap output
        output_matrix = "";
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
                if (_mode == 1)
                    output_matrix += (grid[x][y]) ? "0x80," : "0x00,";
            }
        }
        if (_mode == 1)
            output_matrix += "};";

        if (_mode == 2)
        {
            for (int col = 0; col < _w; col++)
            {
                uint8_t byte = 0;
                for (int row = 0; row < 8; row++)
                {
                    byte |= (grid[col][row] << (7 - (row)));
                }
                output_matrix += std::to_string(byte);
                output_matrix += ',';
                byte = 0;
            }
            output_matrix += "};";
        }

        render_out_matrix();

        // Render Btn
        renderButtonAndTextView();
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
                    // std::cout << "Clicked cell: (" << clickedX << ", " << clickedY << ")" << std::endl;
                }

                if (isPointInside_btn(mouseX, mouseY))
                {
                    button.clicked = true;
                    copyToClipboard(output_hex);
                    std::cout << "Bitmap copied to Clipboard!" << std::endl;
                }
            }
            else if (event.type == SDL_MOUSEMOTION && event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                int clickedX = mouseX / cell_size;
                int clickedY = mouseY / cell_size;

                if (clickedX >= 0 && clickedX < _w && clickedY >= 0 && clickedY < _h)
                {
                    grid[clickedX][clickedY] = true; // Toggle clicked cell
                    // std::cout << "Clicked cell: (" << clickedX << ", " << clickedY << ")" << std::endl;
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_s)
                {
                    for (size_t i = 0; i < _w; i++)
                    {
                        for (size_t j = 0; j < _h; j++)
                        {
                            grid[i][j] = false;
                        }
                    }
                }
            }
        }
    }
};

int WinMain()
{

    Uint32 frameStart;
    int frameTime;
    const int FPS = 15;

    int terminal_width = 100; // desired width
    int terminal_height = 18; // desired height

    std::string command = "mode con: cols=" + std::to_string(terminal_width) + " lines=" + std::to_string(terminal_height);
    system(command.c_str());

    int width, height;
    int mode = 1;

    std::cout << "[HINT]: Press 's' to erase the grid." << std::endl;
    std::cout << "Select the ouput mode, 1 for buffer (1 Pixel in byte) and 2 for row-wise writing (8 Pixel in byte)" << std::endl;
    std::cout << "Enter the mode: ";
    std::cin >> mode;

    while (mode < 1 || mode > 2)
    {
        std::cout << "Error: mode must be equal to 1 or 2." << std::endl;
        std::cout << "Select the ouput mode, 1 for buffer (1 Pixel in byte) and 2 for row-wise writing (8 Pixel in byte)" << std::endl;
        std::cout << "Enter the mode: ";
        std::cin >> mode;
    }

    // Buffer mode 1 ==> Pixel in byte
    if (mode == 1)
    {
        std::cout << "Enter the height: ";
        std::cin >> height;
        std::cout << "Enter the width: ";
        std::cin >> width;

        while (width <= 2 || height <= 2)
        {
            std::cout << "Error: Width and height must be greater than 2." << std::endl;
            std::cout << "Enter the height: ";
            std::cin >> height;
            std::cout << "Enter the width: ";
            std::cin >> width;
        }
    }
    // row-wise writing mode ==> 8 pexel in byte (row in lcd)
    else if (mode == 2)
    {
        std::cout << std::endl;
        std::cout << "[HINT]: The height should be a multiple of 8, such as 8, 16, 24, and so on." << std::endl;
        std::cout << "Enter the height: ";
        std::cin >> height;
        std::cout << "Enter the width: ";
        std::cin >> width;

        while (width <= 2 || height <= 2 || height % 8 != 0)
        {
            std::cout << "Error: Width and height  must be greater than 2, height should be a multiple of 8." << std::endl;
            std::cout << "Enter the height: ";
            std::cin >> height;
            std::cout << "Enter the width: ";
            std::cin >> width;
        }
    }

    Grid grid(height, width, mode);
    while (!grid.quit)
    {
        frameStart = SDL_GetTicks();

        grid.handleEvents();
        grid.renderGrid();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < 1000 / FPS)
        {
            SDL_Delay((1000 / FPS) - frameTime);
        }
    }

    return 0;
}
// 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
// g++ main.cpp -o bitmap_to_hex.exe -lSDL2 -lSDL2_ttf -lSDL2_gfx
