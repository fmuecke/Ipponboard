#ifndef BASE__IVIEW_H_
#define BASE__IVIEW_H_

#include <QString>
#include "Enums.h"

namespace Ipponboard
{
class IController;

class IView
{
public:
	virtual void UpdateView() = 0;
	virtual void SetController(IController*) = 0;
	virtual void Reset() = 0;
	virtual void SetShowInfoHeader(bool show) = 0;
};

}
#endif  // BASE__IVIEW_H_
