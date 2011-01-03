#ifndef I_SCORE_VIEW_INCLUDED
#define I_SCORE_VIEW_INCLUDED

#include <QString>
#include "enums.h"

namespace Ipponboard
{
class IController;

class IView
{
public:
	virtual void UpdateView() = 0;
	virtual void SetController( IController* ) = 0;
	virtual void Reset() = 0;
	virtual void SetShowInfoHeader(bool show) = 0;
};

}
#endif // I_SCORE_VIEW_INCLUDED
