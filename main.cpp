#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

void wordsFinder(const std::string filePath, const std::string  word)
{
    std::ifstream istrm(filePath, std::ios::binary);
    std::string line;
    while(getline(istrm, line))
    {
        if (line.find(word) != std::string::npos)
        {
            std::cout << filePath << std::endl;
        }
    }
}

/*todo
add directories and files for program 
*/


/*todo
function result txt file 
*/

/*todo
function result log file 
*/

/*todo
function dir path 
*/

int main(int argc, char **argv)
{
    std::string word, directionPath, logFileName, resultsFileName;
    int numberOfThreads;

    po::options_description desc ("Allowed options");
    desc.add_options ()
        ("help,h", "print usage message")
        ("input", po::value(&word), "input characters")
        ("dir,d", po::value(&directionPath)->default_value(fs::current_path()),"start directory where program needs to look for files")
        ("log_file,l", po::value(&logFileName)->default_value("specific_grep.log"), "name of the log file")
        ("result_file,r", po::value(&resultsFileName)->default_value("specific_grep.txt"), "name of the file where result is given")
        ("threads,t", po::value(&numberOfThreads)->default_value(4), "number of threads in the pool");

    po::positional_options_description positionals;
    positionals.add("input", -1 );
    po::variables_map vm;
    po::store (po::command_line_parser (argc, argv)
        .positional(positionals)
        .options (desc).run (), vm);
    po::notify (vm);

    if (vm.count ("help")){
        std::cerr << desc << "\n";
        return 1;
    }

if(vm.count("input"))
{
    if(vm.count("dir"))
    {
        for (auto const& directories : fs::recursive_directory_iterator{directionPath}) 
        {
            wordsFinder(directories.path(), word); 
        }
    }
    if(vm.count("log_file"))
    {
        /*todo*/
    }
    if(vm.count("result_file"))
    {
        /*todo*/
    }
    if(vm.count("threads"))
    {
        /*todo*/
    }
}
 
   
    return 0;
}