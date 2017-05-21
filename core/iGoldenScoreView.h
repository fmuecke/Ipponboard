#ifndef CORE__IGOLDENSCOREVIEW_H_
#define CORE__IGOLDENSCOREVIEW_H_

#include <QString>
#include "Enums.h"

namespace Ipponboard
{
class IController;

class IGoldenScoreView
{
public:
	virtual void UpdateGoldenScoreView() = 0;
};

}
#endif  // CORE__IGOLDENSCOREVIEW_H_
