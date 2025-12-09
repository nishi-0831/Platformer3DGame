#include "stdafx.h"
#include "MonitorInfo.h"
mtgb::MonitorInfo::MonitorInfo()
	: adapterIndex{ INVALID_INDEX }
	, outputIndex{ INVALID_INDEX }
	, isRequested{false}
	, desc{}
{
}
