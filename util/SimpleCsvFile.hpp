// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _UTIL_SIMPLECSVFILE_HPP
#define _UTIL_SIMPLECSVFILE_HPP

#include <QFile>
#include <QObject>  // needed for tr()
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <vector>

namespace fm
{

// Read and write data to and from a CSV (comma separated values) file 
// - has multiple lines of separated values
// - (?) has separator escaping
// - has a standard header specifying the format
// - is utf8 formatted
// - values can not have line breaks (unless they are escaped)
namespace SimpleCsvFile
{
    static bool ParseData(QByteArray const& data, QString separator, std::vector<QStringList>& result, QString& errorMsg)
    {
        if (data.isEmpty())
        {
            errorMsg = QObject::tr("no data");
            return false;
        }

        if (separator.isNull() || separator.isEmpty())
        {
            errorMsg = QObject::tr("invalid separator");
            return false;
        }

        QTextStream in(data, QIODevice::ReadOnly);

        in.setCodec("UTF-8");

        int lineNo = 1; // 1-based :)
        int requiredValuesPerLine = 0;

        for (QString line = in.readLine(); !line.isNull(); ++lineNo)
        {
            QStringList splittedLine = line.split(separator);

            if (requiredValuesPerLine == 0)
            {
                requiredValuesPerLine = splittedLine.length();
            }

            if (splittedLine.length() != requiredValuesPerLine)
            {
                errorMsg = splittedLine.length() < requiredValuesPerLine ?
                    QObject::tr("line %1 has fewer values as required by the first line").arg(QString::number(lineNo)) :
                    QObject::tr("line %1 has more values as required by the first line").arg(QString::number(lineNo));
                result.clear();
                return false;
            }

            result.push_back(splittedLine);
            line = in.readLine();
        }
            
        return true;
    }

    static bool FormatValues(std::vector<QStringList> const& values, QString const& separator, QByteArray& result, QString& errorMsg)
    {
        if (values.empty() || values[0].empty() || values[0].at(0).isEmpty())
        {
            errorMsg = "no values";
            return false;
        }

        result.clear();

        auto const requiredValuesPerLine = values[0].length();

        for (size_t index = 0; index < values.size(); ++index)
        {
            auto const& item = values[index];
            if (item.length() != requiredValuesPerLine)
            {
                errorMsg = item.length() < requiredValuesPerLine ?
                    QObject::tr("item at index %1 has fewer values as required by the first item").arg(QString::number(index)) :
                    QObject::tr("item at index %1 has more values as required by the first item").arg(QString::number(index));

                return false;
            }

            // check if item values contain separator
            for (auto const& value : item)
            {
                if (value.contains(separator))
                {
                    // TODO: implement escaping
                    errorMsg = QObject::tr("not supported: item at index %1 contains separator").arg(QString::number(index));
                    return false;
                }
            }

            result.append(item.join(separator));
            result.append("\n");
        }
        
        return true;
    }

    static bool Read(QString const& fileName, QString const& separator, std::vector<QStringList>& result, QString& errorMsg)
    {
        errorMsg.clear();

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            errorMsg = QObject::tr("Unable to open file %1").arg(fileName);
            return false;
        }

        result.clear();

		QTextStream in(&file);
		in.setCodec("UTF-8");

        int lineNo = 0;
        int itemsPerLine = 0;

        for (QString line = in.readLine(); !line.isNull(); ++lineNo)
        {
            QStringList splittedLine = line.split(separator);

            if (itemsPerLine == 0)
            {
                itemsPerLine = splittedLine.length();
            }

            if (splittedLine.length() != itemsPerLine)
            {
                errorMsg = QObject::tr("Error parsing file %1:\n"
                              "line %2 has %3 items instead of %4 like in the previous lines")
                        .arg(fileName)
                        .arg(QString::number(lineNo+1))
                        .arg(QString::number(itemsPerLine))
                        .arg(QString::number(splittedLine.length()));

                return false;
            }

            result.push_back(splittedLine);

            line = in.readLine();
        }

        file.close();

        return true;
    }

	static bool Write(QString const& fileName, QStringList const& data, QString& errorMsg)
    {
        errorMsg.clear();

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            errorMsg = QObject::tr("Unable to open file %1 for writing").arg(fileName);
            return false;
        }

        if (data.empty())
        {
        //    errorMsg = QObject::tr("Error writing to file %1:\nno data to write").arg(fileName);
        //    return false;
        }
		else
		{
			QTextStream outStream(&file);
			outStream.setCodec("UTF-8");
			outStream.setGenerateByteOrderMark(true);
			Q_FOREACH(QString const& line, data)
			{
				outStream << line << endl;
			}
		}

        return true;
    }
};
}

#endif // _UTIL_SIMPLECSVFILE_HPP
