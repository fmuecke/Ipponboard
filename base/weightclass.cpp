#include "weightclass.h"

using namespace Ipponboard;

void WeightClass::RemoveWeight(QString const& weight)
{
	weights.erase(
		std::remove(
			weights.begin(),
			weights.end(),
			weight.toStdString() ),
		weights.end() );
}

const QStringList WeightClass::GetWeightList() const
{
	QStringList list;
	for( size_t i(0); i != weights.size(); ++i )
	{
		list.push_back(QString::fromStdString(weights[i]));
	}
	return list;
}

