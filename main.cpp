#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

#include <boost/program_options.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

template <typename T>
class TQueue {
private:
    std::queue<T> m_queue;
  
    mutable std::mutex m_mutex;
  
    std::condition_variable m_cond;
  
public:
    void push(T item)
    {
        m_queue.push(item);
    }
  
    bool empty() const
    {
      std::lock_guard<std::mutex> lock (m_mutex);
      return m_queue.empty();
    }

    int size() const
    {
        return m_queue.size();
    }

    T pop()
    {
  
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });
        T item = m_queue.front();
        m_queue.pop();
  
        return item;
    }
};
  
struct Data
{
    fs::path directionPath;
    std::string word, logFileName, resultsFileName, lineOfWord;
    int numberOfThreads, numberOfLine = 1, numberOfFiles = 0;
};

int filesInMapCounter(const std::map<std::thread::id, std::string> &mapOfLogs)
{
    int sum = mapOfLogs.size();

    for(auto &elem : mapOfLogs)
    {
        int occurences = std::count(elem.second.begin(), elem.second.end(), ',');
        sum += occurences;
    }
    return sum;
}

void printer(const int &SearchedFiles, const Data &data, const std::vector<Data> &vectorOfDataToResultFile, const std::map<std::thread::id, std::string> &mapOfLogs, const std::chrono::milliseconds &timer )
{
    std::cout <<"Searched files:" << SearchedFiles << "\n";
    std::cout <<"Files with pattern:" << filesInMapCounter(mapOfLogs) << "\n";
    std::cout <<"Patterns number:" << vectorOfDataToResultFile.size() << "\n";
    std::cout <<"Result file:" << fs::current_path().generic_string() + "/" + data.resultsFileName  << "\n";
    std::cout <<"Log file:" << fs::current_path().generic_string() + "/" + data.logFileName  << "\n";
    std::cout <<"Used threads:" << mapOfLogs.size() << "\n";
    std::cout <<"Elapsed time(miliseconds):" << timer.count() << "\n";
}

bool compareByOccurrences(const Data &a, const Data &b)
{
    size_t countA = std::count_if(a.word.begin(), a.word.end(), [](char c){ return std::isalpha(c); });
    size_t countB = std::count_if(b.word.begin(), b.word.end(), [](char c){ return std::isalpha(c); });
    return countA > countB;
}

bool compareSign(const std::pair<std::thread::id, std::string>& a, const std::pair<std::thread::id, std::string>& b) 
{
    int countA = std::count(a.second.begin(), a.second.end(), ',');
    int countB = std::count(b.second.begin(), b.second.end(), ',');
    return countA > countB;
}

void saveResultFile(const std::string &resultFileName, std::vector<Data> &vectorOfDataToResultFile)
{
    std::ofstream ostrm(resultFileName, std::ios_base::app);
    std::sort(vectorOfDataToResultFile.begin(), vectorOfDataToResultFile.end(), compareByOccurrences);

    for(auto &elem : vectorOfDataToResultFile)
    {
        ostrm << elem.directionPath << ":" << elem.numberOfLine << ":" << elem.lineOfWord << "\n";
    }
    ostrm.close();
}

void saveLogFile(const std::string &logFileName, const std::map<std::thread::id, std::string> &mapOfLogs)
{
    std::vector<std::pair<std::thread::id, std::string>> logsVec(mapOfLogs.begin(), mapOfLogs.end());

    std::sort(logsVec.begin(), logsVec.end(), compareSign);

    std::ofstream ostrm(logFileName, std::ios_base::app);
    for(auto &elem : logsVec)
    {
        if(elem.second.empty())
        {
            ostrm << elem.first << ":" << "\n";
        }
        else
        {
            ostrm << elem.first << ":" << elem.second << "\n";
        }
    }
}

void wordsFinder(const fs::path &filePath, const Data &data, std::map<std::thread::id, std::string> &mapOfLogs, std::vector<Data> &vectorOfDataToResultFile, std::shared_mutex &m)
{
    std::vector<Data> tempVect = {data};
    bool flag = false;
    int lineNumber = 1;
    std::string line;
    std::ifstream istrm(filePath, std::ios::binary);

    while(getline(istrm, line))
    {
        if (line.find(data.word) != std::string::npos)
        {
            std::unique_lock<std::shared_mutex> lock(m);
            tempVect.back().numberOfLine = lineNumber;
            tempVect.back().lineOfWord = line;
            tempVect.back().directionPath = filePath;
            std::copy(tempVect.begin(), tempVect.end(), std::back_inserter(vectorOfDataToResultFile));
            if(!flag)
            {
                auto iter = mapOfLogs.find(std::this_thread::get_id());
                if (iter != mapOfLogs.end())
                {
                    iter->second += "," + filePath.filename().generic_string();
                }
                else
                {
                    mapOfLogs.emplace(std::this_thread::get_id(), filePath.filename().generic_string());
                }
            }
            flag = true;
        }
        ++lineNumber;
    }
}

int main(int argc, char **argv)
{
    auto start = std::chrono::steady_clock::now();
    Data data;
    TQueue<std::string>queueOfPaths;
    std::vector<Data> vectorOfDataToResultFile; 
    std::vector<std::string> vectorOfPaths;
    std::vector<std::thread> threads;
    std::map<std::thread::id, std::string> mapOfLogs;
    std::shared_mutex mutex;

    po::options_description desc ("Allowed options");
    desc.add_options ()
        ("help,h", "print usage message")
        ("input", po::value(&data.word), "input characters")
        ("dir,d", po::value(&data.directionPath)->default_value(fs::current_path()),"start directory where program needs to look for files")
        ("log_file,l", po::value(&data.logFileName)->default_value("specific_grep.log"), "name of the log file")
        ("result_file,r", po::value(&data.resultsFileName)->default_value("specific_grep.txt"), "name of the file where result is given")
        ("threads,t", po::value(&data.numberOfThreads)->default_value(4), "number of threads in the pool");

    po::positional_options_description positionals;
    positionals.add("input", -1 );
    po::variables_map vm;
    po::store (po::command_line_parser (argc, argv)
        .positional(positionals)
        .options (desc).run (), vm);
    po::notify (vm);

if (vm.count ("help"))
{
    std::cerr << desc << "\n";
}
else if(vm.count("input"))
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(data.directionPath, std::filesystem::directory_options::skip_permission_denied))
    {
        if (!fs::is_regular_file(entry.path()))
        {
            continue;
        }
        else if(entry.path().generic_string().find("CMakeFiles") != std::string::npos)
        {
            continue;
        }
        else if(entry.path().generic_string().find("specific_grep") != std::string::npos)
        {
            continue;
        }
        else if (entry.path().filename() != data.resultsFileName && entry.path().filename() != data.logFileName)
        {
            queueOfPaths.push(entry.path());
        }
    }
    const int sizeOfQueue = queueOfPaths.size();
    for(int i=0; i<data.numberOfThreads; ++i)
    {
        threads.emplace_back([&]()
        {
            while(!queueOfPaths.empty())
            {
                wordsFinder(queueOfPaths.pop(), data, mapOfLogs, vectorOfDataToResultFile, mutex);
            }
        });
    }

    for(auto &t : threads)
    {
        if(t.joinable())
        {
            t.join();
        }
    }
    saveLogFile(data.logFileName, mapOfLogs);
    saveResultFile(data.resultsFileName, vectorOfDataToResultFile);
    auto end = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printer(sizeOfQueue, data, vectorOfDataToResultFile, mapOfLogs, time);
}
else if(!vm.count("input"))
{
    std::cout << "Please input characters to find\n";
}

    return 0;
}

