#include "game.hh"
#include <iostream>
#include <fstream>

int main()
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
