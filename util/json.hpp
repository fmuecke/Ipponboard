// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "debug.h"
#include "jsoncpp/json.h"
//#include "jsoncpp\json.cpp"

//#include <filesystem>
#include <fstream>
#include <sstream>
#include <QFileInfo>

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
                TRACE(2, "Exception::Exception()");
            }

            virtual const char * what() const noexcept override
			{
                TRACE(2, "Exception::what()");
                return m_message.c_str();
			}

		private:
			std::string m_message;
		};

		class FileNotFoundException : public Exception
		{
		public:
			explicit FileNotFoundException(const char * const & file) : Exception(std::string("File not found: ") + file)
            {
                TRACE(2, "FileNotFoundException::FileNotFoundException()");
            }
		};

		class FileAccessException : public Exception
		{
		public:
			explicit FileAccessException(const char * const & file) : Exception(std::string("Access denied: ") + file)
            {
                TRACE(2, "FileAccessException::FileAccessException()");
            }
		};

        static Json::Value ReadString(const std::string& str)
        {
            TRACE(2, "ReadString(str=%s)", str.c_str());
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
            TRACE(2, "ReadFile(file=%s)", file);
            QFileInfo fileInfo((QFile(file)));
            if (!fileInfo.exists())
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

        static void WriteFile(const char * const & file, std::string const& str, bool writeBom = true)
		{
            TRACE(2, "WriteFile(file=%s, str=%s, writeBom=%d)", file, str.c_str(), writeBom);
            try
			{
				std::ofstream t(file, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
				if (t.is_open())
				{
					if (writeBom)
					{
						t << (char)0xEF << (char)0xBB << (char)0xBF;
					}

                    t << str;
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
            TRACE(2, "WriteFile(file=%s, writeBom=%d)", file, writeBom);
            return WriteFile(file, json.toStyledString(), writeBom);
		}

		static bool VerifyFile(const char * const & file, std::string& result)
		{
            TRACE(2, "VerifyFile(file=%s)", file);
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

