#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

void wordsFinder(const std::string &filePath, const std::string  &word, const std::string  &resultFileName, const std::string  &logFileName);
void saveResultFile(const std::string  &resultFileName, const std::string &pathFileFound, const int &lineNumber, const std::string &lineContent);


/*todo
add directories and files for program 
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
else if(vm.count("input"))
{
    if(vm.count("dir"))
    {
        for (auto const& directories : fs::recursive_directory_iterator{directionPath}) 
        {
            if(directories.path().filename() != resultsFileName)
            {
                wordsFinder(directories.path(), word, resultsFileName, logFileName); 
            }
        }
    }
    if(vm.count("threads"))
    {
        /*todo*/
    }
}
else if(!vm.count("input"))
{
    std::cout << "please input characters to find\n";
}

    return 0;
}

void wordsFinder(const std::string &filePath, const std::string  &word, const std::string  &resultFileName, const std::string  &logFileName)
{
    std::ifstream istrm(filePath, std::ios::binary);
    std::string line;
    int lineNumber = 1;
    while(getline(istrm, line))
    {
        if (line.find(word) != std::string::npos)
        {
            std::cout << filePath << std::endl;
            saveResultFile(resultFileName, filePath, lineNumber, line);
        }
        lineNumber++;
    }
}

void saveResultFile(const std::string  &resultFileName, const std::string &pathFileFound, const int &lineNumber, const std::string &lineContent)
{
    std::ofstream ostrm(resultFileName, std::ios_base::app);
    ostrm  << pathFileFound << ":" << lineNumber << ":" << lineContent << "\n";
}
