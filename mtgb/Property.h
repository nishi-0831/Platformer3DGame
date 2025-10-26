#pragma once

template<class T> struct Property
{
	T& value;
	operator T() { return value; }
	void operator =(const T v) { r = v; }
};
