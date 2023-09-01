# LBG LCD Bitmap Generator
Bitmap to Hex Tool
Lcd Bitmap Generator tested with nokia 5110 lcd

This tool is designed to help developers design custom bitmaps using a GUI. It allows the user to draw the bitmap using the mouse and offers two modes:

The first mode codes each pixel in one byte.
The second mode allows 8 pixels in a row to be coded in one byte.
The tool automatically copies the hex code for the bitmap into the clipboard so that the user can use it in their code.


Requirements
SDL2 library
TTF library
Installation
To install the requirements, you can run the following commands:

pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf


## Usage

To compile and run the code, you can use the following commands:
g++ main.cpp -o bitmap_to_hex.exe -lSDL2 -lSDL2_ttf -lSDL2_gfx

output: bitmap_to_hex.exe

## Contributing

This project is open source. You can contribute by submitting pull requests or by reporting issues.

## License

This project is licensed under the MIT License.
