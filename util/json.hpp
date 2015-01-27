#pragma once

#include "jsoncpp\json.h"
#include "jsoncpp\json.cpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace fm
{
	namespace Json
	{
		using ::Json::Value;
		
		class Exception : public std::exception
		{
		public:
			explicit Exception(std::string const& message) : exception(), m_message(message)
			{
			}

			virtual const char * what() const override
			{
				return m_message.c_str();
			}

		private:
			std::string m_message;
		};

		class FileNotFoundException : public Exception
		{
		public:
			explicit FileNotFoundException(const char * const & file) : Exception(std::string("File not found: ") + file)
			{}
		};

		class FileAccessException : public Exception
		{
		public:
			explicit FileAccessException(const char * const & file) : Exception(std::string("Access denied: ") + file)
			{}
		};

		static Json::Value ReadFile(const char * const & file)
		{
			if (!std::tr2::sys::exists(std::tr2::sys::path(file)))
			{
				throw FileNotFoundException(file);
			}

			std::ifstream t(file, std::ifstream::binary);
			std::stringstream buffer;
			buffer << t.rdbuf();

			::Json::Reader reader;
			Json::Value json;

			// skip byte-order-mark
			auto content = buffer.str();
			buffer.clear();

			if (content[0] == (char)0xEF && content[1] == (char)0xBB && content[2] == (char)0xBF)
			{
				content = content.substr(3);
			}

			if (!reader.parse(content, json))
			{
				std::string message = "Error parsing JSON file '";
				message.append(file);
				message.append("':\n");
				message.append(reader.getFormattedErrorMessages());
				throw Exception(message);
			}

			return json;
		}

		static void WriteFile(const char * const & file, Json::Value const& json, bool writeBom = true)
		{
			try
			{
				std::ofstream t(file, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
				if (t.is_open())
				{
					if (writeBom)
					{
						t << (char)0xEF << (char)0xBB << (char)0xBF;
					}

					t << json.toStyledString();
					t.close();
				}

			}
			catch (std::ofstream::failure& e)
			{
				throw FileAccessException(e.what());
			}
		}

		static bool VerifyFile(const char * const & file, std::string& result)
		{
			try
			{
				auto json = ReadFile(file);
			}
			catch (Exception const& e)
			{
				result = e.what();
				return false;
			}

			result.clear();
			return true;
		}
	}
}

