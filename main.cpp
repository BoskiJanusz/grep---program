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

/*todo
change program name
add directories and files for program 
*/


/*todo
result txt file 
*/

/*todo
result log file 
*/


/* todo 
parameter handling
 -d || --dir - start directory where program needs to look for files (also in subfolders)
l || --log_file - name of the log file
-r || --result_file - name of the file where result is given
-t || --threads - number of threads in the pool

no parameter handling
*/
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