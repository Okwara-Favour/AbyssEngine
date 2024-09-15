#pragma once
#include <deque>
#include "Grid.h"

const std::map<std::string, Vec2> UDLR = {
    {"UU", Vec2(0, -1)},
    {"DD", Vec2(0, 1)},
    {"LL", Vec2(-1, 0)},
    {"RR", Vec2(1, 0)}
};

const std::map<std::string, int> UDLRCost = {
    {"UU", 100},
    {"DD", 100},
    {"LL", 100},
    {"RR", 100}
};

const std::map<std::string, Vec2> DIAG = {
    {"UU", Vec2(0, -1)},
    {"DD", Vec2(0,  1)},
    {"LL", Vec2(-1, 0)},
    {"RR", Vec2(1,  0)},
    {"UL", Vec2(-1,-1)},
    {"DL", Vec2(-1, 1)},
    {"UR", Vec2(1, -1)},
    {"DR", Vec2(1,  1)}
};

const std::map<std::string, int> DIAGCost = {
    {"UU", 100},
    {"DD", 100},
    {"LL", 100},
    {"RR", 100},
    {"UL", 141},
    {"DL", 141},
    {"UR", 141},
    {"DR", 141}
};

struct SearchConfig
{
    std::string m_type;
    std::string m_strategy;
    std::string m_distanceType;
    Vec2 m_size;
    std::map<std::string, Vec2> m_action;
    std::map<std::string, int> m_actionCost;
    SearchConfig(const std::string& type = "UDLR", const std::string& strategy = "bfs", const std::string& distanceType = "None", Vec2 size = Vec2(0,0));
};


struct Node
{
    Vec2                      m_cellPos;                         
    std::shared_ptr<Node>     m_parent;       
    Vec2                      m_action;
    float                     m_cost;
    float                     m_hcost;
    float                     m_fcost;
    Node(const Vec2& cellPos = Vec2(0,0), std::shared_ptr<Node> parent = nullptr, const Vec2& action = Vec2(0, 0), const float cost = 0, const float hcost = 0);
    std::shared_ptr<Node> nullNode();
    const bool isNull();
};

struct PriorityQueue
{
    int parentIndex(int index);
    int leftChildIndex(int index);
    int rightChildIndex(int index);
    void swap(std::deque<std::shared_ptr<Node>>& heap, int index1, int index2);
    void enqueue(std::deque<std::shared_ptr<Node>>& heap, const std::shared_ptr<Node>& node);
    void heapifyUp(std::deque<std::shared_ptr<Node>>& heap);
    std::shared_ptr<Node> dequeue(std::deque<std::shared_ptr<Node>>& heap);
    void heapifyDown(std::deque<std::shared_ptr<Node>>& heap);
};

class Search
{
    std::map<std::string, Color> cellConfig = { {"wall", Color(0,0,0)}, {"entity", Color(255,255,10)}, {"path", Color(255,25,255)} };
    const Vec2 nullVec = Vec2(-1, -1);
	Grid                m_grid;
    SearchConfig        m_config;
    Vec2                m_start = nullVec;
    Vec2                m_end = nullVec;
    std::deque<std::shared_ptr<Node>>     m_open;
    std::deque<Vec2>     m_closed;
    std::deque<Vec2>     m_path;
public:
    Search(const Grid& grid = Grid(), const SearchConfig& config = SearchConfig());
    Search(const Search& other);

    // Copy assignment operator
    Search& operator=(const Search& other);
    void setSearchPoints(Vec2 start, Vec2 end);
    bool canMove(const Vec2& a, const Vec2& b, const Vec2& size);
    bool isLegalAction(const Vec2& cellPos, const Vec2& scaledActionType, const std::pair<std::string, Vec2>& action);
    void search();
    void solution(std::shared_ptr<Node> node);
    void expand(std::deque<std::shared_ptr<Node>>& fringe, std::shared_ptr<Node>& node);
    float estimateCost(const Vec2& a, const Vec2& b);
    float estimateEuclideanCost(const Vec2& a, const Vec2& b);
    float estimateDiagManhattanCost(const Vec2& a, const Vec2& b);
    float estimateCardManhattanCost(const Vec2& a, const Vec2& b);
    std::deque<Vec2>& path();
    void smoothPath();
};

