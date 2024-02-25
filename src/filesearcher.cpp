#include "filesearcher.h"

#include <dirent.h>
#include <cstring>
#include <queue>
#include <iostream>

FileSearcher::FileSearcher(std::string fileName)
{
	this->fileName = fileName;
}

FileSearcher::~FileSearcher()
{

}

void FileSearcher::search()
{
	std::queue<std::string> foldersToScan;
	DIR* dir = opendir("/");
	dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_type == DT_DIR)
		{
			// Exclude . and .. folders
			if ( !((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) )
			{
				foldersToScan.push(std::string(entry->d_name) + "/");
			}
		}
		else if (entry->d_type == DT_REG)
		{
			if (strcmp(entry->d_name, fileName.c_str()) == 0)
			{
				pathToFile = std::string("/") + std::string(entry->d_name);
				return;
			}
		}
	}
	closedir(dir);

	do
	{
		mtx.lock();
		if (workingThreads < maxNumberOfThreads)
		{
			workingThreads++;
			std::thread thread(&FileSearcher::threadSearch, this, foldersToScan.front());
			thread.detach();
			foldersToScan.pop();
		}
		mtx.unlock();
	}
	while (!foldersToScan.empty());
	while (workingThreads != 0)
	{
		// wait while detached threads end their work
		// I assume this is bad solution and should be done in different way
	}
}

void FileSearcher::threadSearch(std::string startDir)
{
	std::queue<std::string> foldersToScan;
	foldersToScan.push("/" + startDir);
	DIR* dir;
	dirent* entry;
	while (foldersToScan.empty() == false)
	{
		if (found == true)
		{
			threadEndWork();
			return;
		}
		dir = opendir(foldersToScan.front().c_str());
		if (dir == NULL)
		{
			foldersToScan.pop();
			continue;
		}
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_DIR)
			{
				// Exclude . and .. folders
				if ( !((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) )
				{
					foldersToScan.push(foldersToScan.front() + std::string(entry->d_name) + "/");
				}
			}
			else if (entry->d_type == DT_REG)
			{
				if (strcmp(entry->d_name, fileName.c_str()) == 0)
				{
					if (pathToFile.empty())
					{
						pathToFile = foldersToScan.front() + std::string(entry->d_name);
						found = true;
					}
					// else file was found with a simmilar name but in other directory
					threadEndWork();
					return;
				}
			}
		}
		closedir(dir);
		foldersToScan.pop();
	}
	threadEndWork();
	return;
}

void FileSearcher::threadEndWork()
{
	mtx.lock();
	workingThreads--;
	mtx.unlock();
}

std::string FileSearcher::getPath()
{
	return pathToFile;
}

std::string FileSearcher::searchNonConcurrent(const std::string& fileName)
{
	std::queue<std::string> foldersToScan;
	foldersToScan.push(std::string("/"));
	DIR* dir;
	dirent* entry;
	do
	{
		dir = opendir(foldersToScan.front().c_str());
		if (dir == NULL)
		{
			foldersToScan.pop();
			continue;
		}
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_DIR)
			{
				// Exclude . and .. folders
				if ( !((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)) )
				{
					foldersToScan.push(foldersToScan.front() + std::string(entry->d_name) + "/");
				}
			}
			else if (entry->d_type == DT_REG)
			{
				if (strcmp(entry->d_name, fileName.c_str()) == 0)
				{
					return foldersToScan.front() + std::string(entry->d_name);
				}
			}
		}
		closedir(dir);
		foldersToScan.pop();
	}
	while (foldersToScan.empty() == false);
	throw std::runtime_error("File not found");
}