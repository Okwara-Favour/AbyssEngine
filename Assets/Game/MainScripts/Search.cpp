#include "Search.h"

SearchConfig::SearchConfig(const std::string& type, const std::string& strategy, const std::string& distanceType, Vec2 size)
	:m_type(type), m_strategy(strategy), m_distanceType(distanceType), m_size(size)
{
    if (type == "UDLR")
    {
        m_action = UDLR;
        m_actionCost = UDLRCost;
    }
    else if (type == "DIAG")
    {
        m_action = DIAG;
        m_actionCost = DIAGCost;
    }
    else
    {
        throw std::runtime_error("Unknown type: " + type);
    }
    m_size.x = round(m_size.x + 0.5);
    m_size.y = round(m_size.y + 0.5);
    //std::cout << m_size.toString() << std::endl;
}

Node::Node(const Vec2& cellPos, std::shared_ptr<Node> parent, const Vec2& action, const float cost, const float hcost)
    :m_cellPos(cellPos), m_parent(parent), m_action(action), m_cost(cost), m_hcost(hcost), m_fcost(cost + hcost)
{}

std::shared_ptr<Node> Node::nullNode()
{
    std::shared_ptr<Node> nNode = std::make_shared<Node>(Vec2(-1, -1), nullptr, Vec2(-1, -1), -1, -1);
    nNode->m_fcost = -1;
    return nNode;
}
const bool Node::isNull()
{
    return m_cellPos == Vec2(-1, -1) && m_parent == nullptr && m_action == Vec2(-1, -1) && m_cost == -1 && m_hcost == -1 && m_fcost == -1;
}

int PriorityQueue::parentIndex(int index){ return floor((index - 1) / 2); }
int PriorityQueue::leftChildIndex(int index){ return 2 * index + 1; }
int PriorityQueue::rightChildIndex(int index){ return 2 * index + 2; }
void PriorityQueue::swap(std::deque<std::shared_ptr<Node>>& heap, int index1, int index2)
{
    std::shared_ptr<Node> temp = heap[index1];
    heap[index1] = heap[index2];
    heap[index2] = temp;
}
void PriorityQueue::enqueue(std::deque<std::shared_ptr<Node>>& heap, const std::shared_ptr<Node>& node)
{
    heap.push_back(node);
    heapifyUp(heap);
}
void PriorityQueue::heapifyUp(std::deque<std::shared_ptr<Node>>& heap)
{
    int index = heap.size() - 1;
    while (index > 0 && heap[parentIndex(index)]->m_fcost > heap[index]->m_fcost)
    {
        swap(heap, parentIndex(index), index);
        index = parentIndex(index);
    }
}
std::shared_ptr<Node> PriorityQueue::dequeue(std::deque<std::shared_ptr<Node>>& heap)
{
    std::shared_ptr<Node> nNode;
    if (heap.size() == 0) { return nNode->nullNode(); }
    if (heap.size() == 1) {
        std::shared_ptr<Node> n = heap[0];
        heap.pop_back();
        return n;
    }
    std::shared_ptr<Node> root = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();
    heapifyDown(heap);
    return root;
}
void PriorityQueue::heapifyDown(std::deque<std::shared_ptr<Node>>& heap)
{
    int index = 0;
    while (leftChildIndex(index) < heap.size())
    {
        int minIndex = leftChildIndex(index);
        if (rightChildIndex(index) < heap.size() && heap[rightChildIndex(index)]->m_fcost < heap[minIndex]->m_fcost)
        {
            minIndex = rightChildIndex(index);
        }
        if (heap[index]->m_fcost <= heap[minIndex]->m_fcost) break;
        swap(heap, index, minIndex);
        index = minIndex;
    }
}


Search::Search(const Grid& grid, const SearchConfig& config)
    :m_grid(grid), m_config(config)
{}

Search::Search(const Search& other)
    : m_grid(other.m_grid)
    , m_config(other.m_config)
    , m_start(other.m_start)
    , m_end(other.m_end)
    , m_open(other.m_open)
    , m_closed(other.m_closed)
    , m_path(other.m_path)
{}

Search& Search::operator=(const Search& other)
{
    if (this != &other) // Check for self-assignment
    {
        m_grid = other.m_grid;
        m_config = other.m_config;
        m_start = other.m_start;
        m_end = other.m_end;
        m_open = other.m_open;
        m_closed = other.m_closed;
        m_path = other.m_path;
    }
    return *this;
}

void Search::setSearchPoints(Vec2 start, Vec2 end)
{
    start.makeWholeNumber();
    end.makeWholeNumber();
    //std::cout << start.toString() << std::endl;
    //std::cout << end.toString() << std::endl;
    m_start = start, m_end = end;
    m_path = {};
    m_open = { std::make_shared<Node>(start) };
    m_closed = {};
}
bool Search::canMove(const Vec2& a, const Vec2& b, const Vec2& size)
{
    int extentX = floor(size.x / 2.0);
    int extentY = floor(size.y / 2.0);
    for (int i = -extentX; i <= extentX; i++)
    {
        for (int j = -extentY; j <= extentY; j++)
        {
            int col = a.x + i;
            int row = a.y + j;
            if (!m_grid.notInBounds(col, row))
            {
                if (!m_grid.get(col, row).color.equals(cellConfig["path"])) return false;
            }
            else { return false; }
        }
    }
    return true;
}
bool Search::isLegalAction(const Vec2& cellPos, const Vec2& scaledActionType, const std::pair<std::string, Vec2>& action)
{
    Vec2 nAction = cellPos + scaledActionType;
    
    auto& actionList = m_config.m_action;
    auto& size = m_config.m_size;
    Vec2 Up = cellPos + Vec2(actionList["UU"].x * size.x, actionList["UU"].y * size.y);
    Vec2 Down = cellPos + Vec2(actionList["DD"].x * size.x, actionList["DD"].y * size.y);
    Vec2 Left = cellPos + Vec2(actionList["LL"].x * size.x, actionList["LL"].y * size.y);
    Vec2 Right = cellPos + Vec2(actionList["RR"].x * size.x, actionList["RR"].y * size.y);
    if (action.first == "UL")
    {
        return  canMove(nAction, cellPos, m_config.m_size) &&
            canMove(Up, cellPos, m_config.m_size) &&
            canMove(Left, cellPos, m_config.m_size);
    }
    if (action.first == "UR")
    {
        return  canMove(nAction, cellPos, m_config.m_size) &&
            canMove(Up, cellPos, m_config.m_size) &&
            canMove(Right, cellPos, m_config.m_size);
    }
    if (action.first == "DL")
    {
        return  canMove(nAction, cellPos, m_config.m_size) &&
            canMove(Down, cellPos, m_config.m_size) &&
            canMove(Left, cellPos, m_config.m_size);
    }
    if (action.first == "DR")
    {
        return  canMove(nAction, cellPos, m_config.m_size) &&
            canMove(Down, cellPos, m_config.m_size) &&
            canMove(Right, cellPos, m_config.m_size);
    }
    return canMove(nAction, cellPos, m_config.m_size);
}
void Search::search()
{
    if(!m_grid.get(m_end.x, m_end.y).color.equals(cellConfig["path"]) && !m_grid.get(m_end.x, m_end.y).color.equals(cellConfig["entity"]))
    {
        return;
    }
    PriorityQueue pq;
    while (!m_open.empty())
    {
        std::shared_ptr<Node> node;
        if (m_config.m_strategy == "bfs") { node = m_open[0]; m_open.pop_front(); }
        else if (m_config.m_strategy == "dfs") { node = m_open[m_open.size() - 1]; m_open.pop_back(); }
        else if (m_config.m_strategy == "dijikstra") { node = pq.dequeue(m_open); }
        else if (m_config.m_strategy == "astar") { node = pq.dequeue(m_open); }
        else if (m_config.m_strategy == "gbefs") { node = pq.dequeue(m_open); }
        else { throw std::runtime_error("invalid strategy"); }

        if (node->m_cellPos == m_end)
        {
            solution(node);
            break;
        }

        auto it = std::find(m_closed.begin(), m_closed.end(), node->m_cellPos);
        if (it == m_closed.end()) {
            m_closed.push_back(node->m_cellPos);
            expand(m_open, node);
        }
    }
}
void Search::solution(std::shared_ptr<Node> node)
{
    while (node->m_parent != nullptr)
    {
        //std::cout << node->m_cellPos.toString() << std::endl;
        m_path.push_front(node->m_cellPos);
        node = node->m_parent;
    }
}
void Search::expand(std::deque<std::shared_ptr<Node>>& fringe, std::shared_ptr<Node>& node)
{
    PriorityQueue pq;
    for (auto& elem : m_config.m_action)
    {
        Vec2 action = elem.second;
        int actionCost = m_config.m_actionCost[elem.first];
        Vec2 scaledAction(action.x * m_config.m_size.x, action.y * m_config.m_size.y);
        if (isLegalAction(node->m_cellPos, scaledAction, elem))
        {
            Vec2 nVec = node->m_cellPos + scaledAction;
            float gCost = node->m_cost + actionCost;
            float hCost = estimateCost(nVec, m_end) * 100;
            std::shared_ptr<Node> newNode = std::make_shared<Node>(nVec, node, scaledAction, gCost, hCost);
            if (m_config.m_strategy == "bfs" || m_config.m_strategy == "dfs") { fringe.push_back(newNode); }
            else if (m_config.m_strategy == "dijikstra") {
                pq.enqueue(fringe, std::make_shared<Node>(nVec, node, scaledAction, gCost, 0));
            }
            else if (m_config.m_strategy == "gbefs") {
                pq.enqueue(fringe, std::make_shared<Node>(nVec, node, scaledAction, 0, hCost));
            }
            else { pq.enqueue(fringe, newNode); }
        }
    }
}
float Search::estimateCost(const Vec2& a, const Vec2& b)
{
    if (m_config.m_distanceType == "CMC") { return estimateCardManhattanCost(a, b); }
    else if (m_config.m_distanceType == "EC") { return estimateEuclideanCost(a, b); }
    else if (m_config.m_distanceType == "DMC") { return estimateDiagManhattanCost(a, b); }
    else { return 0; }
}
float Search::estimateEuclideanCost(const Vec2& a, const Vec2& b)
{
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}
float Search::estimateDiagManhattanCost(const Vec2& a, const Vec2& b)
{
    return abs(b.x - a.x) + abs(b.y - a.y);
}
float Search::estimateCardManhattanCost(const Vec2& a, const Vec2& b)
{
    return std::max(abs(b.x - a.x), abs(b.y - a.y));
}

std::deque<Vec2>& Search::path()
{
    return m_path;
}

void Search::smoothPath() {
    std::deque<Vec2> smoothedPath;
    if (m_path.size() < 3) return;

    
    auto isCollinear = [](const Vec2& a, const Vec2& b, const Vec2& c) -> bool {
        // Collinearity check (simplified for 2D)
        return (b.y - a.y) * (c.x - b.x) == (c.y - b.y) * (b.x - a.x);
    };

    smoothedPath.push_back(m_path.front());
    for (size_t i = 1; i < m_path.size() - 1; ++i) {
        Vec2 prev = m_path[i - 1];
        Vec2 curr = m_path[i];
        Vec2 next = m_path[i + 1];

        // Check if the current point is in line with the previous and next points
        if (!isCollinear(prev, curr, next)) {
            smoothedPath.push_back(curr);
        }
    }
    smoothedPath.push_back(m_path.back());
    m_path = smoothedPath;
}