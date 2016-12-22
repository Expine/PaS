#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

/*********************************************************/
namespace util
{
	inline std::string loadText(const std::string &file)
	{
		return FileUtils::getInstance()->getStringFromFile(file);
	}

	std::vector<std::string> splitString(const std::string& input, char delim)
	{
		std::string item;
		std::vector<std::string> result;
		for (char ch : input) 
		{
			if (ch == delim) 
			{
				result.push_back(item);
				item.clear();
			}
			else 
			{
				item += ch;
			}
		}
		result.push_back(item);
		return result;
	}

	std::vector<std::string> splitFile(const std::string &file)
	{
		// Result
		std::string item;
		std::vector<std::string> result;
		// for BOM
		bool first = true;
		bool bom = false;
		int bom_count = 0;

		for (char ch : loadText(file))
		{
			// BOM Check
			if (first) 
			{
				if ((unsigned char)ch == 0xEF)
					bom = true;
				first = false;
			}
			if (bom && bom_count++ < 3)
				continue;

			// '\r' is skip
			if (ch == '\r')
				continue;

			if (ch == '\n')
			{
				result.push_back(item);
				item.clear();
			}
			else
			{
				item += ch;
			}
		}

		result.push_back(item);
		return result;
	}

}

#endif // __STAGE_H__