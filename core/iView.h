#ifndef CORE__IVIEW_H_
#define CORE__IVIEW_H_

#include <QString>
#include "enums.h"

namespace Ipponboard
{
class IController;

class IView
{
public:
	virtual void UpdateView() = 0;
	virtual void Reset() = 0;
	virtual void SetShowInfoHeader(bool show) = 0;
};

}
#endif  // CORE__IVIEW_H_
