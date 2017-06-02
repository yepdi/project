#ifndef __CPU_FILE_READER
#define __CPU_FILE_READER

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <sstream>

class FileReader
{
private :
	std::ifstream filestream;

	struct SequentialCache
	{
		uint64_t start_count;
		uint64_t size;
		uint64_t start_addr;
	};

	std::unordered_map<uint64_t, SequentialCache> cache_map;
public :
	FileReader();
	~FileReader();
	void FileRead();
	std::vector<std::string> split(const std::string &s, char delim);
	void insertMap(SequentialCache count,uint64_t addr);
	uint64_t GetCorrespondingCacheLine(uint64_t addr,uint64_t count)
	{
		auto map_itr=cache_map.find(addr);
		if(map_itr==cache_map.end())
		{
			return 0;
		}
		else
		{
			SequentialCache info=map_itr->second;
			if(count==info.start_count)
				return info.size;
			else
				return 0;
		}
	}
};

#endif