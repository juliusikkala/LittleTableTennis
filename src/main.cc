/*
The MIT License (MIT)

Copyright (c) 2018 Julius Ikkala

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "game.hh"
#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
    try
    {
        game g;

        while(g.update())
        {
            g.render();
        }
    }
    catch(const std::runtime_error& err)
    {
        // Print & save the error message
        std::string what = err.what();

        std::cout << "Runtime error: " << std::endl
                  << what << std::endl;

        std::ofstream error_file("error.txt", std::ios::out);
        error_file << what;
        error_file.close();

        // Show the user that something went awry
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Runtime error",
            "Error report written to error.txt",
            nullptr
        );
        return 1;
    }
    return 0;
}
