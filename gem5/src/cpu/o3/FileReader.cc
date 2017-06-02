#include "FileReader.hh"

FileReader::FileReader()
:filestream("output.txt",std::ios::in | std::ios::binary)
{

}

FileReader::~FileReader()
{
	filestream.close();
}

void FileReader::FileRead()
{
	std::string read_string;
	std::vector<std::string> temp;
	std::vector<std::string>::iterator iter;

	SequentialCache cacheblock;
	uint64_t addr;

	bool start = false;
	bool useful_data = false;

	int cnt = 0;

	while (filestream >> read_string)
	{
		temp = split(read_string, ':');
		for (iter = temp.begin(); iter != temp.end(); iter++)
		{
			std::string temp = *iter;
			if (start)
			{
				if (cnt == 0)
				{
					cacheblock.start_count = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 1){
					cacheblock.size = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 2)
				{
					cacheblock.start_addr = strtoull(temp.c_str(), NULL, 0);
					addr = cacheblock.start_addr;
					cnt++;
					start = false;
					useful_data = true;
				}
			}
			if (*iter == "T")
				start = true;
		}
		if (useful_data)
			insertMap(cacheblock,addr);
		start = false;
		useful_data = false;
		cnt = 0;
	}
}

std::vector<std::string> FileReader::split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
void FileReader::insertMap(SequentialCache count, uint64_t addr)
{
	cache_map.insert(std::make_pair(addr, count));
}
/*
void FileReader::traverse()
{
	for (auto map_itr = cache_map.begin(); map_itr != cache_map.end(); map_itr++)
	{
		SequentialCache c_block = map_itr->second;
		printf("start addr %llx start position %d \n", c_block.start_addr, c_block.start_count);
		std::cout << "size of inst" << c_block.size << std::endl;
	}
}
*/