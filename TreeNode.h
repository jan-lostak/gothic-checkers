#ifndef TREENODE_H
#define TREENODE_H

#include <vector>
#include "Game.h"

class TreeNode
{
    public:
        TreeNode();
        ~TreeNode();

        void AddChild(TreeNode* node) { _childs.push_back(node); }
        std::vector<TreeNode*>& GetChilds() { return _childs; }
        Position& GetPosition() { return _position; }
        void SetPosition(Position position) { _position = position; }
        Turns GenerateTurns();

    private:
        void WalkNodes(TreeNode* node, Turn turn, Turns& turns);

        std::vector<TreeNode*> _childs;
        Position _position;
};

#endif // TREENODE_H
