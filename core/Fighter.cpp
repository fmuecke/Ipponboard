// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "Fighter.h"

using namespace Ipponboard;

Fighter::Fighter(QString const& firstName,
				 QString const& lastName)
	: first_name(firstName)
	, last_name(lastName)
	, club()
	, weight()
	, category()
{
}
