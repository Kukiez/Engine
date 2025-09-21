#pragma once

template <typename T>
class DynamicAABBTree {
    struct Node {
        AABB box;
        int parent = -1;
        int left = -1;
        int right = -1;

        T data{};

        bool isLeaf() const {
            return left == -1;
        }
    };
    std::vector<Node> nodes;
    int root = -1;

public:
    void build(const std::vector<AABB>& objectBoxes) {
        nodes.clear();
        root = -1;

        for (int i = 0; i < objectBoxes.size(); ++i) {
            insert(i, objectBoxes[i]);
        }
    }

    void insert(T data, const AABB& box) {
        Node leaf;
        leaf.box = box;
        leaf.objectID = data;
        leaf.left = leaf.right = -1;
        int leafIndex = nodes.size();
        nodes.push_back(leaf);

        if (root == -1) {
            root = leafIndex;
            return;
        }

        int sibling = findBestSibling(leaf.box, root);

        Node parent;
        parent.left = sibling;
        parent.right = leafIndex;
        parent.box = geom::merge(leaf.box, nodes[sibling].box);
        parent.parent = nodes[sibling].parent;

        int parentIndex = nodes.size();
        nodes.push_back(parent);

        if (nodes[sibling].parent != -1) {
            if (nodes[nodes[sibling].parent].left == sibling)
                nodes[nodes[sibling].parent].left = parentIndex;
            else
                nodes[nodes[sibling].parent].right = parentIndex;
        } else {
            root = parentIndex;
        }

        nodes[sibling].parent = parentIndex;
        nodes[leafIndex].parent = parentIndex;

        refit(nodes[parentIndex].parent);
    }

private:
    int findBestSibling(const AABB& box, int start) {
        if (nodes[start].isLeaf()) return start;

        const float leftCost = geom::surface_area(geom::merge(box, nodes[nodes[start].left].box));
        const float rightCost = geom::surface_area(geom::merge(box, nodes[nodes[start].right].box));
        if (leftCost < rightCost)
            return findBestSibling(box, nodes[start].left);
        return findBestSibling(box, nodes[start].right);
    }

    void refit(int nodeIndex) {
        while (nodeIndex != -1) {
            Node& n = nodes[nodeIndex];
            n.box = geom::merge(nodes[n.left].box, nodes[n.right].box);
            nodeIndex = n.parent;
        }
    }
};