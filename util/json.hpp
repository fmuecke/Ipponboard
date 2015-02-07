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

        static Json::Value ReadString(const std::string& str)
        {
            // skip byte-order-mark
            auto content = str;
            if (str[0] == (char)0xEF && str[1] == (char)0xBB && str[2] == (char)0xBF)
            {
                content = str.substr(3);
            }

            ::Json::Reader reader;
            Json::Value json;

            if (!reader.parse(content, json))
            {
                std::string message = "Invalid JSON:\n";
                message.append(reader.getFormattedErrorMessages());
                throw Exception(message);
            }

            return json;
        }

		static Json::Value ReadFile(const char * const & file)
		{
			if (!std::tr2::sys::exists(std::tr2::sys::path(file)))
			{
				throw FileNotFoundException(file);
			}

			std::ifstream t(file, std::ifstream::binary);
			std::stringstream buffer;
			buffer << t.rdbuf();

			// skip byte-order-mark
			auto content = buffer.str();
			buffer.clear();

			if (content[0] == (char)0xEF && content[1] == (char)0xBB && content[2] == (char)0xBF)
			{
				content = content.substr(3);
			}

			try
			{
				return ReadString(content);
			}
			catch (Exception const& e)
			{
				std::string message = "Error parsing file '";
				message.append(file);
				message.append("':\n");
				message.append(e.what());
				throw Exception(message);
			}
		}

		static void WriteFile(const char * const & file, std::string const& jsonStr, bool writeBom = true)
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

					t << jsonStr;
					t.close();
				}

			}
			catch (std::ofstream::failure& e)
			{
				throw FileAccessException(e.what());
			}
		}

		static void WriteFile(const char * const & file, Json::Value const& json, bool writeBom = true)
		{
			return WriteFile(file, json.toStyledString(), writeBom);
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

