#include "Rules.h"

using namespace Ipponboard;

AbstractRules::AbstractRules()
{
}

bool AbstractRules::IsScoreLess(const Score& lhs, const Score& rhs) const
{
    using Point = Score::Point;

    if (lhs.Value(Point::Hansokumake) != rhs.Value(Point::Hansokumake))
    {
        return lhs.Value(Point::Hansokumake) > rhs.Value(Point::Hansokumake);
    }
    else
    {
        if (lhs.Value(Point::Ippon) != rhs.Value(Point::Ippon))
        {
            return lhs.Value(Point::Ippon) < rhs.Value(Point::Ippon);
        }
        else
        {
            if (lhs.Value(Point::Wazaari) != rhs.Value(Point::Wazaari))
            {
                return lhs.Value(Point::Wazaari) < rhs.Value(Point::Wazaari);
            }
            else
            {
                if (lhs.Value(Point::Yuko) != rhs.Value(Point::Yuko))
                {
                    return lhs.Value(Point::Yuko) < rhs.Value(Point::Yuko);
                }
                else
                {
                    // shidos are not compared as they result in concrete points
                    if (!IsOption_ShidoAddsPoint() && IsOption_ShidoScoreCounts())
                    {
                        return lhs.Value(Point::Shido) > rhs.Value(Point::Shido);
                    }
                }
            }
        }
    }

    return false;
}
