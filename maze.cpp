#include <queue>
#include <cstdio>
#include <cstdlib>
#include "maze.h"

void Maze::get_cells(const Wall& wall, int& x0, int& y0, int& x1, int& y1)
{
	if (!wall.dir)
	{
		x0 = wall.x;
		x1 = wall.x + 1;
		y0 = wall.y;
		y1 = wall.y;
	}
	else
	{
		x0 = wall.x;
		x1 = wall.x;
		y0 = wall.y;
		y1 = wall.y + 1;
	}

}

void Maze::get_cell_ids(const std::vector<int>& cell_id, const Wall& wall, int& id0, int& id1)
{
	int x0, y0, x1, y1;
	get_cells(wall, x0, y0, x1, y1);

	id0 = cell_id[x0 + y0 * m_width];
	id1 = cell_id[x1 + y1 * m_width];
}

Maze::Maze(int w, int h) : m_width(w), m_height(h)
{
	std::vector<int> cell_id(w * h);
	for (int i = 0; i < w * h; i++)
	{
		cell_id[i] = i;
	}

	x_walls.resize((w - 1) * h, true);
	y_walls.resize(w * (h - 1), true);

	while (true)
	{
		std::vector<Wall> active_walls;
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w - 1; x++)
			{
				Wall wall = { false, x,y };
				int id0, id1;
				get_cell_ids(cell_id, wall, id0, id1);
				if (id0 != id1)
				{
					active_walls.push_back(wall);
				}
			}
		}
		for (int y = 0; y < h - 1; y++)
		{
			for (int x = 0; x < w; x++)
			{
				Wall wall = { true, x,y };
				int id0, id1;
				get_cell_ids(cell_id, wall, id0, id1);
				if (id0 != id1)
				{
					active_walls.push_back(wall);
				}
			}
		}
		if (active_walls.size() < 1) break;

		int num_active_walls = (int)active_walls.size();

		int idx_remove = rand() % num_active_walls;
		Wall& wall_to_remove = active_walls[idx_remove];

		if (!wall_to_remove.dir)
		{
			x_walls[wall_to_remove.x + wall_to_remove.y * (w - 1)] = false;		
		}
		else
		{
			y_walls[wall_to_remove.x + wall_to_remove.y * w] = false;
		}

		int id0, id1;
		get_cell_ids(cell_id, wall_to_remove, id0, id1);		
		if (id0 > id1)
		{
			int tmp = id0;
			id0 = id1;
			id1 = tmp;
		}

		for (int i = 0; i < w * h; i++)
		{
			if (cell_id[i] == id1)
			{
				cell_id[i] = id0;
			}
		}
	}
}

void Maze::print()
{
	{
		printf("©°");
		for (int x = 0; x < m_width-1; x++)
		{
			printf("©¤©¤©Ð");
		}

		printf("©¤©¤©´\n");
	}

	for (int y = 0; y < m_height - 1; y++)
	{
		printf("©¦");
		for (int x = 0; x < m_width-1; x++)
		{
			printf("  ");
			if (x_walls[x + y * (m_width - 1)])
			{
				printf("©¦");
			}
			else
			{
				printf(" ");
			}
		}	
		printf("  ©¦\n");
		
		printf("©À");
		for (int x = 0; x < m_width-1; x++)
		{			
			if (y_walls[x + y * m_width])
			{
				printf("©¤©¤");
			}
			else
			{
				printf("  ");
			}
			printf("©à");
		}
		{
			int x = m_width - 1;
			if (y_walls[x + y * m_width])
			{
				printf("©¤©¤");
			}
			else
			{
				printf("  ");
			}
			printf("©È\n");
		}
		
	}
	{
		int y = m_height - 1;

		printf("©¦");
		for (int x = 0; x < m_width - 1; x++)
		{
			printf("  ");
			if (x_walls[x + y * (m_width - 1)])
			{
				printf("©¦");
			}
			else
			{
				printf(" ");
			}
		}
		printf("  ©¦\n");

		printf("©¸");
		for (int x = 0; x < m_width - 1; x++)
		{
			printf("©¤©¤©Ø");			
		}
		printf("©¤©¤©¼\n");
	}

}

void Maze::analyze(std::vector<CellLocation>& farthests)
{
	struct Node
	{
		int x;
		int y;
		int steps;
	};

	Node end = { m_width - 1, m_height -1, 0 };

	std::vector<int> cell_steps(m_width* m_height, 0x7FFFFFFF);

	std::queue<Node> queue;
	queue.push(end);

	std::vector<std::vector<Node>> classes;
	
	while (queue.size() > 0)
	{
		Node node = queue.front();
		queue.pop();

		if (node.steps >= classes.size())
		{
			classes.resize(node.steps + 1);
		}
		classes[node.steps].push_back(node);		

		if (node.x > 0 && !x_walls[node.x - 1 + node.y * (m_width - 1)])
		{
			int s = cell_steps[node.x - 1 + node.y * m_width];
			if (s > node.steps + 1)
			{
				cell_steps[node.x - 1 + node.y * m_width] = node.steps + 1;
				queue.push({ node.x - 1, node.y, node.steps + 1 });
			}
		}
		
		if (node.x < m_width - 1 && !x_walls[node.x + node.y * (m_width - 1)])
		{
			int s = cell_steps[node.x + 1 +  node.y * m_width];
			if (s > node.steps + 1)
			{
				cell_steps[node.x + 1 + node.y * m_width] = node.steps + 1;
				queue.push({ node.x + 1, node.y, node.steps + 1 });
			}
		}

		if (node.y > 0 && !y_walls[node.x + (node.y - 1) * m_width])
		{
			int s = cell_steps[node.x + (node.y-1) * m_width];
			if (s > node.steps + 1)
			{
				cell_steps[node.x + (node.y - 1) * m_width] = node.steps + 1;
				queue.push({ node.x, node.y-1, node.steps + 1 });
			}
		}
		
		if (node.y < m_height - 1 && !y_walls[node.x + node.y * m_width])
		{

			int s = cell_steps[node.x + (node.y + 1) * m_width];
			if (s > node.steps + 1)
			{
				cell_steps[node.x + (node.y + 1) * m_width] = node.steps + 1;
				queue.push({ node.x, node.y + 1, node.steps + 1 });
			}
		}
	}

	/*int count = 0;
	for (size_t i = 0; i < classes.size(); i++)
	{
		printf("%d %d\n", i, classes[i].size());
		auto& cls = classes[i];
		printf("[");
		for (size_t j = 0; j < cls.size(); j++)
		{
			Node& node = cls[j];
			printf("[%d, %d], ", node.x, node.y);
		}
		printf("]\n");		

		count += classes[i].size();
	}
	printf("total: %d\n", count);*/

	farthests.clear();
	for (size_t i = classes.size()-1; i != (size_t)(-1); i--)
	{
		auto& cls = classes[i];
		for (size_t j = 0; j < cls.size(); j++)
		{
			Node& node = cls[j];
			farthests.push_back({ node.x, node.y });
			if (farthests.size() == 6) break;
		}
		if (farthests.size() == 6) break;
	}

}
