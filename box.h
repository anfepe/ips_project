#pragma once

struct Box
{
protected:
	double x_min;
	double y_min;
	double width;
	double height;
public:
	Box() {}

	Box( double min_x, double min_y, double x_width, double y_height );

	~Box() {}

	void GetParameters( double& min_x, double& min_y, double& x_width, double& y_height ) const;

	void GetWidhtHeight(double& x_width, double& y_height) const;

	double GetDiagonal() const;

	bool operator==(const Box& rhs) const;
};