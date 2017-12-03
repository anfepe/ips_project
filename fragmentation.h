#pragma once
#include "box.h"
#include <vector>

typedef std::pair< std::vector<double>, std::vector<double> > min_max_vectors;
typedef std::pair<Box, Box> boxes_pair;

class low_level_fragmentation
{
protected:
	Box current_box;
	void VerticalSplitter( const Box& box, boxes_pair& vertical_splitter_pair );
	void HorizontalSplitter( const Box& box, boxes_pair& horizontal_splitter_pair );
	void GetNewBoxes( const Box& box, boxes_pair& new_pair_of_boxes );
	unsigned int FindTreeDepth();
	int ClasifyBox( const min_max_vectors& vects );
	void GetBoxType( const Box& box );
	virtual void GetMinMax( const Box& box, min_max_vectors& min_max_vector ) = 0;

public:
	low_level_fragmentation() {}

	low_level_fragmentation( double& min_x, double& min_y, double& x_width, double& y_height );

	low_level_fragmentation( const Box& box );

	virtual ~low_level_fragmentation() {}
};


class high_level_analysis : public low_level_fragmentation
{
protected:
	void GetMinMax(const Box& box, min_max_vectors& min_max_vector) override;

public:
	high_level_analysis() {}

	high_level_analysis( double& min_x, double& min_y, double& x_width, double& y_height );

	high_level_analysis( Box& box );

	void GetSolution();
};

void WriteResults( const char* file_names[] );