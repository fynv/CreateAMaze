#pragma once

#include <vector>

class Maze
{
public:
	int m_width;
	int m_height;

	std::vector<bool> x_walls;
	std::vector<bool> y_walls;

	Maze(int w, int h);

	struct CellLocation
	{
		int x;
		int y;
	};

	void print();
	void analyze(std::vector<CellLocation>& farthests);

private:
	struct Wall
	{
		bool dir = false; // x
		int x = 0;
		int y = 0;
	};

	void get_cells(const Wall& wall, int& x0, int& y0, int& x1, int& y1);
	void get_cell_ids(const std::vector<int>& cell_id, const Wall& wall, int& id0, int& id1);
};