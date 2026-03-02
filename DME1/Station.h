#pragma once
#include "Point.h"

#define MaxName 256
class Station
{
	Point coordinates;
	char* name;

public:
	Station();
	~Station();
	Station(const Station&);
	Station(Point p, const char*);
	Station(double x1, double y1, const char* c);

	void setName(const char* n);
	const char* getName() const;  // ИСПРАВЛЕНО: добавлен const

	void setCoordinates(const Point& p);
	Point getCoordinates() const;

	void print() const;
	void scan();

	Station& operator=(const Station& s);
};

