#include "include/clipper.h"

Clipper::Clipper(double max_data, double min_data, double max_win, double min_win)
{
	WIN_MAX = max_win;
	WIN_MIN = min_win;
	DATA_MAX = max_data;
	DATA_MIN = min_data;

	Q = std::abs(DATA_MAX - DATA_MIN)/std::abs(WIN_MAX - WIN_MIN);

}

Clipper::~Clipper()
{
	
}

double Clipper::Resize(double current_data, double max_data, double min_data, double max_win, double min_win)
{
	Q = std::abs(DATA_MAX - DATA_MIN)/std::abs(WIN_MAX - WIN_MIN);

	return std::abs(current_data - DATA_MIN)/Q + WIN_MIN;
}