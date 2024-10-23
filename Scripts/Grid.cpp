#include "Grid.h"
Cell::Cell(const Vec2& p, const Vec2& s): size(s), pos(Vec2(p.x* s.x, p.y* s.y)) {}
void Cell::draw(sf::RenderWindow& window)
{
	sf::RectangleShape rect(sf::Vector2f(size.x, size.y));
	rect.setPosition(pos.x, pos.y);
	rect.setFillColor(color.getColor());
	rect.setOutlineColor((Color(255,255,255)).getColor());
	//rect.setOutlineColor(color.getColor());
	rect.setOutlineThickness(1);
	window.draw(rect);
}

const Vec2 Cell::getSize() const
{
	return size;
}
const Vec2 Cell::getPosition() const
{
	return pos;
}

Grid::Grid() :m_size(Vec2()), m_cellSize(Vec2()) {}
Grid::Grid(const Vec2& size, const Vec2& cellsize) : m_size(size), m_cellSize(cellsize) { init(); }
void Grid::init()
{
	//std::cout << m_size.toString() << std::endl;
	for (int row = 0; row < m_size.y; row++)
	{
		std::vector<Cell> temp;
		for (int col = 0; col < m_size.x; col++)
		{
			temp.push_back(Cell(Vec2(col, row), Vec2(m_cellSize.x, m_cellSize.y)));
		}
		m_grid.push_back(temp);
	}
}

Cell& Grid::get(int col, int row) { return m_grid[row][col]; }

const bool Grid::notInBounds(int col, int row) const
{
	return col < 0 || row < 0 || col >= m_size.x || row >= m_size.y;
}

const Vec2& Grid::size() { return m_size; }
const Vec2& Grid::cellSize() { return m_cellSize; }

bool Grid::outOfViewBounds(const sf::View& view, const Vec2& targetPos)
{
	float viewLeft = view.getCenter().x - (view.getSize().x / 2);
	float viewTop = view.getCenter().y - (view.getSize().y / 2);
	float viewRight = viewLeft + view.getSize().x;
	float viewBottom = viewTop + view.getSize().y;
	return targetPos.x < viewLeft || targetPos.y < viewTop || targetPos.x > viewRight || targetPos.y > viewBottom;
}

bool Grid::insideViewBounds(const Cell& cell, const sf::View& view)
{
	auto& eP = cell.getPosition();
	auto& eS = cell.getSize();
	Vec2 UL = Vec2(eP.x		  , eP.y	   );
	Vec2 UR = Vec2(eP.x + eS.x, eP.y	   );
	Vec2 DL = Vec2(eP.x		  , eP.y + eS.y);
	Vec2 DR = Vec2(eP.x + eS.x, eP.y + eS.y);
	return !(outOfViewBounds(view, UL)) || !(outOfViewBounds(view, UR)) || !(outOfViewBounds(view, DL)) || !(outOfViewBounds(view, DR));
}

void Grid::render(sf::RenderWindow& window, bool fullScreen)
{
	if (fullScreen)
	{
		for (int row = 0; row < m_size.y; row++)
		{
			for (int col = 0; col < m_size.x; col++)
			{
				m_grid[row][col].draw(window);
			}
		}
	}
	else {
		for (int row = 0; row < m_size.y; row++)
		{
			for (int col = 0; col < m_size.x; col++)
			{
				auto& cell = m_grid[row][col];
				if(insideViewBounds(cell, window.getView())) m_grid[row][col].draw(window);
			}
		}
	}
}