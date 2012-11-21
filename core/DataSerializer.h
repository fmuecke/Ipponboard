// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef _DATA_SERIALIZER_H_INCLUDED_
#define _DATA_SERIALIZER_H_INCLUDED_

// forwards
class QString;
namespace Ipponboard
{
    struct Fighter;
}

class DataSerializer
{
public:
    enum EErrorCode
    {
        eOk = 0,
        eFile_open_error,
        eInvalid_file_format,
        eNo_data,
        eUnknown_error
    };

    DataSerializer();

    static EErrorCode ReadFighters(QString const& fileName, std::vector<Ipponboard::Fighter>& fighters);
    static EErrorCode WriteFighters(QString const& fileName, std::vector<Ipponboard::Fighter> const& fighters);
};

#endif // _DATA_SERIALIZER_H_INCLUDED_
