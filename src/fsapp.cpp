#include <iostream>
#include <chrono>

#include "filesearcher.h"


int main(int argc, char const *argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage:" << std::endl;
		std::cout << "Pass one argument which is the name of the file to search (including its extension)" << std::endl;
		return 0;
	}
	std::string fileToSearch = std::string(argv[1]);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

	auto timeSearchStart = high_resolution_clock::now();
    FileSearcher fs(fileToSearch);
	fs.search();
	if (fs.getPath().empty() == false)
    {
		std::cout << "File is found!" << std::endl;
		std::cout << "The path is:" << std::endl;
		std::cout << fs.getPath() << std::endl;
    }
    else
    {
    	std::cout << "File not found" << std::endl;
    }
	auto timeSearchEnd = high_resolution_clock::now();
	duration<double, std::milli> searchTime = timeSearchEnd - timeSearchStart;
	std::cout << "Search was done in: " << searchTime.count() << " ms\n";
	return 0;
}
