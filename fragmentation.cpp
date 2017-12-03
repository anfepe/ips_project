#include "fragmentation.h"

#include "defines.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

std::vector<Box> solution;
std::vector<Box> not_solution;
std::vector<Box> boundary;
std::vector<Box> temporary_boxes;

//------------------------------------------------------------------------------------------
double g1(double x1, double x2)
{
	return (x1*x1 + x2*x2 - g_l1_max*g_l1_max);
}

//------------------------------------------------------------------------------------------
double g2(double x1, double x2)
{
	return (g_l1_min*g_l1_min - x1*x1 - x2*x2);
}

//------------------------------------------------------------------------------------------
double g3(double x1, double x2)
{
	return (x1*x1 + x2*x2 - g_l2_max*g_l2_max);
}

//------------------------------------------------------------------------------------------
double g4(double x1, double x2)
{
	return (g_l2_min*g_l2_min - x1*x1 - x2*x2);
}


//------------------------------------------------------------------------------------------
low_level_fragmentation::low_level_fragmentation(double& min_x, double& min_y, double& x_width, double& y_height )
{
	current_box = Box( min_x, min_y, x_width, y_height );
}

//------------------------------------------------------------------------------------------
low_level_fragmentation::low_level_fragmentation(const Box& box)
{
	current_box = box;
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::VerticalSplitter(const Box& box, boxes_pair& vertical_splitter_pair)
{
	double x, y, w, h;
	box.GetParameters(x, y, w, h);
	Box box_left(x, y, w / 2, h);
	Box box_right(x + w / 2, y, w / 2, h);
	vertical_splitter_pair = std::make_pair(box_left, box_right); 
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::HorizontalSplitter(const Box& box, boxes_pair& horizontal_splitter_pair)
{
	double x, y, w, h;
	box.GetParameters(x, y, w, h);
	Box box_top(x, y, w, h / 2);
	Box box_bottom(x, y + h / 2, w, h / 2);
	horizontal_splitter_pair = std::make_pair(box_top, box_bottom);
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::GetNewBoxes(const Box& box, boxes_pair& new_pair_of_boxes)
{
	double w, h;
	box.GetWidhtHeight(w, h);
	if (w > h)
	{
		VerticalSplitter(box, new_pair_of_boxes);
	}
	else
	{
		HorizontalSplitter(box, new_pair_of_boxes);
	}
}

//------------------------------------------------------------------------------------------
unsigned int low_level_fragmentation::FindTreeDepth()
{
	double box_diagonal = current_box.GetDiagonal();

	if (box_diagonal <= g_precision)
	{
		return 0;
	}
	else
	{
		boxes_pair new_boxes;
		VerticalSplitter(current_box, new_boxes);
		unsigned int tree_depth = 1;

		box_diagonal = new_boxes.first.GetDiagonal();

		if (box_diagonal <= g_precision)
		{
			return tree_depth;
		}
		else
		{
			for (;;)
			{
				GetNewBoxes(new_boxes.first, new_boxes);
				++tree_depth;
				box_diagonal = new_boxes.first.GetDiagonal();

				if (box_diagonal <= g_precision)
				{
					break;
				}
			}
			return tree_depth;
		}
	}
}

//------------------------------------------------------------------------------------------
int low_level_fragmentation::ClasifyBox(const min_max_vectors& vects)
{
	vector<double> minv = vects.first;
	vector<double> maxv = vects.second;

	//eq 5
	for (vector<double>::iterator v = minv.begin(); v != minv.end(); v++)
	{
		if (*v > 0)
		{
			// not solution
			return 1;
		}
	}

	//eq 4
	for (vector<double>::iterator v = maxv.begin(); v != maxv.end(); v++)
	{
		if (*v > 0)
		{
			// check eq 6
			return 2;
		}
	}
	// solution
	return 0;

}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::GetBoxType(const Box& box)
{
	min_max_vectors minmax;
	GetMinMax(box, minmax);
	int clas = ClasifyBox(minmax);
	switch (clas)
	{
		case 0:
		{
			solution.push_back(box);
			break;
		}
		case 1:
		{
			not_solution.push_back(box);
			break;
		}
		case 2:
		{
			// cout << box.GetDiagonal() << "\t" << g_precision << endl;
			if (box.GetDiagonal() <= g_precision)
			{
				boundary.push_back(box);
			}
			else
			{
				boxes_pair new_boxes;
				GetNewBoxes(box, new_boxes);
				temporary_boxes.push_back(new_boxes.first);
				temporary_boxes.push_back(new_boxes.second);
			}
			break;
		}
		default:
			break;
	}
}


//------------------------------------------------------------------------------------------
high_level_analysis::high_level_analysis( double& min_x, double& min_y, double& x_width, double& y_height ) :
					low_level_fragmentation(min_x, min_y, x_width, y_height)
{
}

//------------------------------------------------------------------------------------------
high_level_analysis::high_level_analysis( Box& box ) : low_level_fragmentation( box )
{
}

//------------------------------------------------------------------------------------------
void high_level_analysis::GetMinMax( const Box& box, min_max_vectors& min_max_vecs )
{
	std::vector<double> g_min;
	std::vector<double> g_max;

	double a1min, a2min, a1max, a2max;
	double xmin, xmax, ymin, ymax;

	box.GetParameters(xmin, ymin, xmax, ymax);

	xmax = xmin + xmax;
	ymax = ymin + ymax;

	// double curr_box_diagonal = box.GetDiagonal();

	// if (curr_box_diagonal <= g_precision)
	// {
	// 	g_min.push_back(0);
	// 	g_max.push_back(0);

	// 	min_max_vecs.first = g_min;
	// 	min_max_vecs.second = g_max;

	// 	return;
	// }

	// MIN
	a1min = __min(abs(xmin), abs(xmax));
	a2min = __min(abs(ymin), abs(ymax));
	g_min.push_back(g1(a1min, a2min));

	a1min = __max(abs(xmin), abs(xmax));
	a2min = __max(abs(ymin), abs(ymax));
	g_min.push_back(g2(a1min, a2min));

	a1min = __min(abs(xmin - g_l0), abs(xmax - g_l0));
	a2min = __min(abs(ymin), abs(ymax));
	g_min.push_back(g3(a1min, a2min));

	a1min = __max(abs(xmin - g_l0), abs(xmax - g_l0));
	a2min = __max(abs(ymin), abs(ymax));
	g_min.push_back(g4(a1min, a2min));

	// MAX
	a1max = __max(abs(xmin), abs(xmax));
	a2max = __max(abs(ymin), abs(ymax));
	g_max.push_back(g1(a1max, a2max));

	a1max = __min(abs(xmin), abs(xmax));
	a2max = __min(abs(ymin), abs(ymax));
	g_max.push_back(g2(a1max, a2max));

	a1max = __max(abs(xmin - g_l0), abs(xmax - g_l0));
	a2max = __max(abs(ymin), abs(ymax));
	g_max.push_back(g3(a1max, a2max));

	a1max = __min(abs(xmin - g_l0), abs(xmax - g_l0));
	a2max = __min(abs(ymin), abs(ymax));
	g_max.push_back(g4(a1max, a2max));

	min_max_vecs.first = g_min;
	min_max_vecs.second = g_max;
}

//------------------------------------------------------------------------------------------
void high_level_analysis::GetSolution()
{
	Box prediction(-g_l1_max, 0,
		g_l2_max + g_l1_max + g_l0,
		g_l1_max < g_l2_max ? g_l1_max : g_l2_max);

	temporary_boxes.push_back(prediction);
	while (!temporary_boxes.empty())
	{
		// for (vector<Box>::iterator box = temporary_boxes.begin(); box != temporary_boxes.end(); box++)
		// {
		// 	double x, y, w, h;
		// 	box->GetParameters(x, y, w, h);
		// 	cout << x << ", " << y << ", " << w << ", " << h << endl;
		// }
		// cout << "////////////////////////////////////////////////" << endl;

		std::vector<Box> temp = temporary_boxes;
		for (vector<Box>::const_iterator box = temp.begin(); box != temp.end(); box++)
		{
			temporary_boxes.erase(std::remove(temporary_boxes.begin(), temporary_boxes.end(), *box), temporary_boxes.end());
			GetBoxType(*box);
		}
	}
	
}


//------------------------------------------------------------------------------------------
void WriteResults( const char* file_names[] )
{
	std::ofstream solutions(file_names[0]);
	std::ofstream not_solutions(file_names[1]);
	std::ofstream boundaries(file_names[2]);

	for (vector<Box>::iterator box = solution.begin(); box != solution.end(); box++)
	{
		double x, y, w, h;
		box->GetParameters(x, y, w, h);
		solutions << x << " " << y << " " << w << " " << h << endl;
	}

	for (vector<Box>::iterator box = not_solution.begin(); box != not_solution.end(); box++)
	{
		double x, y, w, h;
		box->GetParameters(x, y, w, h);
		not_solutions << x << " " << y << " " << w << " " << h << endl;
	}

	for (vector<Box>::iterator box = boundary.begin(); box != boundary.end(); box++)
	{
		double x, y, w, h;
		box->GetParameters(x, y, w, h);
		boundaries << x << " " << y << " " << w << " " << h << endl;
	}

	solutions.close();
	not_solutions.close();
	boundaries.close();
}