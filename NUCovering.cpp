#include "fragmentation.h"
#include "defines.h"

#include <locale.h>
#include <chrono>
#include <iostream>

using namespace std;

int main()
{
	setlocale(LC_ALL,"Rus");
	for (int i = 0; i < 100; i++)
	{	
		high_level_analysis main_object;
		main_object.GetSolution();
	}
	const char* out_files[3] = { "solutions.txt", "no_solutions.txt", "boundaries.txt" };
	// WriteResults( out_files );

	return 0;
}