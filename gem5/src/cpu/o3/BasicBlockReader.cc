#include "BasicBlockReader.hh"
#include <sstream>
using namespace std;

BasicBlockReader::BasicBlockReader(Addr mask,std::string filename)
:fileStream(filename.c_str(), ios::in | ios::binary), total_count(0)
{
	fetchBufferMask=mask;
}

BasicBlockReader::~BasicBlockReader()
{
	fileStream.close();
}


void
BasicBlockReader::ReadBasicBlockfile()
{
	string read_string;
	vector<string> temp;
	vector<string>::iterator iter;

	BBinfo info;

	uint64_t id;
	uint64_t addr_start;
	uint64_t addr_second;
	uint64_t insts;
	uint64_t iteration_count;

	int cnt = 0;

	
	while (fileStream >> read_string)
	{
		temp = split(read_string, ':');
		for (iter = temp.begin(); iter != temp.end(); iter++)
		{
			if (*iter != "T" && *iter != "")
			{
				string temp = *iter;
				if (cnt == 0)
				{
					id = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 1){
					addr_start = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 2){
					addr_second = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 3)
				{
					insts = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
				else if (cnt == 4)
				{
					iteration_count = strtoull(temp.c_str(), NULL, 0);
					cnt++;
				}
			}
		}
		info.id = id;
		info.insts = insts;
		info.iteration_count = iteration_count;
		info.range = make_pair(addr_start, addr_second);
		insertMap(id, info);
		cnt = 0;
	}
}

std::vector<std::string> BasicBlockReader::split(const std::string &s, char delim) {
	std::stringstream ss(s); 
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}
void BasicBlockReader::insertMap(uint64_t id,BBinfo count)
{
	bb_list.push_back(count);
	total_count++;
}
/*
void BasicBlockReader::traverse()
{
	list<BBinfo>::iterator i;
	for (i = bb_list.begin(); i != bb_list.end();i++)
	{
		BBinfo block = *i;
	}
}
*/
