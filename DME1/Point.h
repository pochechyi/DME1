#include <stdlib.h> // дл€ random
#include <stdio.h> // дл€ ввод и вывода
#include <cmath> // дл€ корн€
#include <cstring>//дл€ strlen

struct Point
{
	double x, y;

	Point() {
		x = 0;
		y = 0;
	}

	Point(double x1, double y1) {
		x = x1;
		y = y1;
	}

	void refresh(double x1, double y1) {
		x = x1; y = y1;
	}

	void scan() {
		printf("¬ведите координату X:");
		scanf_s("%lf ", &x);
		printf("¬ведите координату Y:");
		scanf_s("%lf ", &y);
	}

	void print() const{
		printf("X = %.4lf , Y = %.4lf \n", x, y);
	}

	double get_distance(double x1, double y1) const {
		return sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
	}

	double get_dist_from_center() const {
		return sqrt((x) * (x)+(y) * (y));
	}

	Point& operator = (const Point& p) {
		x = p.x;
		y = p.y;
		return *this;
	}
};