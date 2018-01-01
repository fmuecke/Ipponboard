#ifndef __UTIL_TOML_H_
#define __UTIL_TOML_H_

#include "../3rdParty/tinytoml/include/toml/toml.h"
#include <ostream>

namespace fm
{

static void WriteToml(const char* filePath, toml::Value const& root)
{
	std::ofstream of(filePath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	if (of.is_open())
	{
//			if (writeBom)
//			{
//				t << (char)0xEF << (char)0xBB << (char)0xBF;
//			}

		root.writeFormatted(&of, toml::FORMAT_INDENT);
		//of << root;

		of.close();
	}
}
	
}

#endif __UTIL_TOML_H_
