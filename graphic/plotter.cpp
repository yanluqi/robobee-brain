#include "include/plotter.h"

#include <iostream>

Plotter::Plotter(double *data_max, double *data_min, int num_plot)
{
	num_window = num_plot;
	max = data_max; 
	min = data_min;

	max_time = 10;
	index = 0;

	WIN_MAX = float(1)/float(num_window); 
	WIN_MIN = float(-1)/float(num_window);

	GraphShader.SetShader("../graphic/shaders/vertex_plot.glsl", "../graphic/shaders/fragment_plot.glsl");

	back_coord.push_back(glm::vec3(-1,-1, 0));
	back_coord.push_back(glm::vec3( 1,-1, 0));

	double y;
	

	for (int i = 1; i < num_plot; ++i)
	{
		back_coord.push_back( glm::vec3(-1, i*float(2)/float(num_plot) - 1.0, 0));
		back_coord.push_back( glm::vec3( 1, i*float(2)/float(num_plot) - 1.0, 0));	
	}

	back_coord.push_back( glm::vec3(-1, 1, 0));
	back_coord.push_back( glm::vec3( 1, 1, 0));

	for (int i = 0; i < back_coord.size(); ++i)
	{
		back_color.push_back( glm::vec3( 1, 1, 1) );
	}

	glGenVertexArrays(MAX, m_vertexArrayObject);
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	
}


Plotter::~Plotter()
{

}

void Plotter::SetData(double *data, double time)
{
	if (time - max_time*index > max_time)
	{
		index++;
		coords1.clear();
		colors1.clear();
		coords2.clear();
		colors2.clear();
		coords3.clear();
		colors3.clear();
	}

	double x = Resize(time - max_time*index, max_time, 0, 1, -1);

	double y = Resize(data[0], max[0], min[0], WIN_MAX, WIN_MIN);

	if(coords1.empty())
	{
		coords1.push_back( glm::vec3(-1, 0, 0) );
		colors1.push_back( glm::vec3(1, 0, 0) );

		coords1.push_back( glm::vec3(x, y, 0) );
		colors1.push_back( glm::vec3(1, 0, 0) );
	}
	else
	{
		coords1.push_back( coords1.back() );
		colors1.push_back( glm::vec3(1, 0, 0) );

		coords1.push_back( glm::vec3(x, y, 0) );
		colors1.push_back( glm::vec3(1, 0, 0) );
	}

	if (num_window > 1)
	{
		double y = Resize(data[1], max[1], min[1], WIN_MAX, WIN_MIN);

		if(coords2.empty())
		{
			coords2.push_back( glm::vec3(-1, 0, 0) );
			colors2.push_back( glm::vec3(0, 1, 0) );

			coords2.push_back( glm::vec3(x, y, 0) );
			colors2.push_back( glm::vec3(0, 1, 0) );
		}
		else
		{
			coords2.push_back( coords2.back() );
			colors2.push_back( glm::vec3(0, 1, 0) );

			coords2.push_back( glm::vec3(x, y, 0) );
			colors2.push_back( glm::vec3(0, 1, 0) );
		}
	}	

	if (num_window > 2)
	{
		double y = Resize(data[2], max[2], min[2], WIN_MAX, WIN_MIN);

		if(coords3.empty())
		{
			coords3.push_back( glm::vec3(-1, 0, 0) );
			colors3.push_back( glm::vec3(0, 0, 1) );

			coords3.push_back( glm::vec3(x, y, 0) );
			colors3.push_back( glm::vec3(0, 0, 1) );
		}
		else
		{
			coords3.push_back( coords3.back() );
			colors3.push_back( glm::vec3(0, 0, 1) );

			coords3.push_back( glm::vec3(x, y, 0) );
			colors3.push_back( glm::vec3(0, 0, 1) );
		}
	}


}


void Plotter::Plot()
{
	GraphShader.Bind();

	glBindVertexArray(m_vertexArrayObject[BACKGROUND]);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, back_coord.size()*sizeof(glm::vec3), &back_coord[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// COLOR
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, back_color.size()*sizeof(glm::vec3), &back_color[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	posPlot = glm::mat4(1.0f);
	glUniformMatrix4fv(GraphShader.GetUniform(2), 1, GL_FALSE, &posPlot[0][0]);

	glDrawArrays(GL_LINES, 0, back_coord.size());

	glBindVertexArray(0);


	
	glBindVertexArray(m_vertexArrayObject[DATA1]);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION]);
	glBufferData(GL_ARRAY_BUFFER, coords1.size()*sizeof(glm::vec3), &coords1[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// COLOR
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR]);
	glBufferData(GL_ARRAY_BUFFER, colors1.size()*sizeof(glm::vec3), &colors1[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	posPlot = glm::translate( glm::vec3(0, float(2)/float(3), 0) );
	glUniformMatrix4fv(GraphShader.GetUniform(2), 1, GL_FALSE, &posPlot[0][0]);

	glDrawArrays(GL_LINES, 0, coords1.size());

	glBindVertexArray(0);


	if (num_window > 1)
	{
		glBindVertexArray(m_vertexArrayObject[DATA2]);

		// Vertex
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION]);
		glBufferData(GL_ARRAY_BUFFER, coords2.size()*sizeof(glm::vec3), &coords2[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// COLOR
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR]);
		glBufferData(GL_ARRAY_BUFFER, colors2.size()*sizeof(glm::vec3), &colors2[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		posPlot = glm::mat4(1.0f);
		glUniformMatrix4fv(GraphShader.GetUniform(2), 1, GL_FALSE, &posPlot[0][0]);
	
		glDrawArrays(GL_LINES, 0, coords2.size());

		glBindVertexArray(0);
	}


	if (num_window > 2)
	{
		glBindVertexArray(m_vertexArrayObject[DATA3]);

		// Vertex
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION]);
		glBufferData(GL_ARRAY_BUFFER, coords3.size()*sizeof(glm::vec3), &coords3[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// COLOR
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR]);
		glBufferData(GL_ARRAY_BUFFER, colors3.size()*sizeof(glm::vec3), &colors3[0], GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		posPlot = glm::translate( glm::vec3(0, -float(2)/float(3), 0) );
		glUniformMatrix4fv(GraphShader.GetUniform(2), 1, GL_FALSE, &posPlot[0][0]);

		glDrawArrays(GL_LINES, 0, coords3.size());

		glBindVertexArray(0);
	}
}

double Plotter::Resize(double current_data, double max_data, double min_data, double max_win, double min_win)
{
	double Q = std::abs(max_data - min_data)/std::abs(max_win - min_win);

	return std::abs(current_data - min_data)/Q + min_win;
}