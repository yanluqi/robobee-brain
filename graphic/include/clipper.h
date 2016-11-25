#ifndef CLIPPER_H
#define CLIPPER_H

class Clipper
{
public:
	Clipper();

	Clipper(double max_data, double min_data, double max_win, double min_win);

	virtual ~Clipper();

	double resize(current_data);

protected:
	double WIN_MAX;
	double WIN_MIN;
	double DATA_MAX;
	double DATA_MIN;

	double Q;
	
};

#endif // CLIPPER_H