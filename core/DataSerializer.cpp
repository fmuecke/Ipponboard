// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "DataSerializer.h"
#include "fighter.h"
#include <QString>
#include <QFile>
#include <vector>

DataSerializer::DataSerializer()
{
}

DataSerializer::EErrorCode DataSerializer::ReadFighters(
    const QString &fileName,
    std::vector<Ipponboard::Fighter> &fighters)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return eFile_open_error;
    }

    fighters.clear();

    QTextStream in(&file);
    for (QString line = in.readLine(); !line.isNull();)
    {
        QStringList splittedLine = line.split(';');
        if (splittedLine.length() < 3) // lastname, firstname, weight-class
        {
            return eInvalid_file_format;
        }

        QString club = splittedLine.length() > 3 ? splittedLine[3] : "";
        Ipponboard::Fighter fighter(
                    splittedLine[1].trimmed(),
                    splittedLine[0].trimmed(),
                    splittedLine[2].trimmed(),
                    club);

        fighters.push_back(fighter);

        line = in.readLine();
    }
    file.close();

    return eOk;
}

DataSerializer::EErrorCode DataSerializer::WriteFighters(
    const QString& fileName,
    const std::vector<Ipponboard::Fighter>& fighters)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return eFile_open_error;
    }

    if (fighters.empty())
    {
        return eNo_data;
    }

    Q_FOREACH(Ipponboard::Fighter const& f, fighters)
    {
        const QString line("%1; %2; %3; %4\n");
        if (-1 == file.write(line.arg(f.last_name, f.first_name, f.weight_class, f.club).toLatin1()))
        {
            return eUnknown_error;
        }
    }

    return eOk;
}
