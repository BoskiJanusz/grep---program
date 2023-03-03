#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

void wordsFinder(const std::string filePath, const std::string  word)
{
    std::ifstream istrm(filePath, std::ios::binary);
    std::string line;
    while(getline(istrm, line))
    {
        if (line.find(word) != std::string::npos)
            std::cout << filePath << std::endl;
    }
}


int main()
{
    fs::path defultPath = fs::current_path();
    std::string word = "words";
    for (auto const& directories : fs::recursive_directory_iterator{defultPath}) 
    {
       wordsFinder(directories.path(), word); 
    }
 
   
    return 0;
}