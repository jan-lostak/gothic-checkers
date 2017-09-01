#include "TreeNode.h"

TreeNode::TreeNode()
{

}

TreeNode::~TreeNode()
{
    for (TreeNode* node : _childs)
        delete node;
}

Turns TreeNode::GenerateTurns()
{
    Turns turns;
    Turn turn;

    WalkNodes(this, turn, turns);

    size_t longest = 0;

    for (Turn turn : turns)
        if (turn.size() > longest)
            longest = turn.size();

    Turns filteredTurns;

    for (Turn turn : turns)
        if (turn.size() == longest)
            filteredTurns.push_back(turn);

    return filteredTurns;
}

void TreeNode::WalkNodes(TreeNode* node, Turn turn, Turns& turns)
{
    for (TreeNode* childNode : node->GetChilds())
    {
        if (turn.size() == 0)
            turn.push_back(node->GetPosition());
        else if (!turn.back().CompareTo(node->GetPosition()))
            turn.push_back(node->GetPosition());

        Turn turnCopy(turn);
        turnCopy.push_back(childNode->GetPosition());
        turns.push_back(turnCopy);

        WalkNodes(childNode, turn, turns);
    }
}
