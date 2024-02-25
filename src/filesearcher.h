#ifndef FILE_SEARCHER_H
#define FILE_SEARCHER_H

#include <string>
#include <thread>
#include <mutex>
#include <atomic>

class FileSearcher
{
private:
	std::string fileName;
	std::string pathToFile;

	std::atomic<bool> found = false;
	int workingThreads = 0;
	int maxNumberOfThreads = 8;
	std::mutex mtx;
public:
	FileSearcher(std::string fileName);
	~FileSearcher();

	void search();
	std::string getPath();
	static std::string searchNonConcurrent(const std::string& fileName);
private:
	void findFilePath();
	void threadSearch(std::string startDir);
	void threadEndWork();
};

#endif