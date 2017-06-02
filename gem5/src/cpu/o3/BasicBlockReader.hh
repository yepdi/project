#ifndef __CPU_BASICBLOCK_READER
#define __CPU_BASICBLOCK_READER

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdint>
#include <vector>
#include <list>
using namespace std;

typedef uint64_t Addr;
typedef std::pair<Addr,Addr> BasicBlockRange;
namespace std {
template <>
struct hash<BasicBlockRange>
{
  public:
    size_t operator()(const BasicBlockRange &bb) const {
        return hash<Addr>()(bb.first + bb.second);
    }
};
}

class BasicBlockReader
{
	private:
		std::ifstream fileStream;
		struct BBinfo{
			uint64_t id;
			uint64_t insts;
			uint64_t count;
			uint64_t iteration_count;
			BasicBlockRange range;
		};

		uint64_t total_count;
		Addr fetchBufferMask;
		std::list<BBinfo> bb_list;

	public:
		BasicBlockReader(Addr mask,std::string filename);
		~BasicBlockReader();
		void ReadBasicBlockfile();
		std::vector<std::string> split(const std::string &s, char delim);
		void insertMap(uint64_t id, BBinfo count);
		/*void traverse();*/
		bool GetcorrespondingBB(uint64_t add1)
		{
			list<BBinfo>::iterator i;
			if (!bb_list.empty())
			{
				for(i=bb_list.begin();i!=bb_list.end();i++)
				{
					BBinfo block=*i;
					if(fetchBufferAlignPC(block.range.first)==add1)
						return true;
				}
			}
			return false;
		}
		Addr fetchBufferAlignPC(Addr addr)
		{
			return (addr & ~(fetchBufferMask));
		}
		bool readfile()
		{
			return fileStream.is_open();
		}
};

#endif