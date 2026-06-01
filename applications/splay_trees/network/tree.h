#include <iostream>
#include <string>
#include <vector>

struct RouteInfo {
    std::string ip_subnet;
    std::string output_interface;
};

struct SplayNode {
    RouteInfo route;
    SplayNode *left = nullptr, *right = nullptr;
    SplayNode(RouteInfo r) : route(r) {}
};

class OSPFRouteCache {
private:
    SplayNode* root = nullptr;

    SplayNode* rotateRight(SplayNode* x) {
        SplayNode* y = x->left;
        x->left = y->right;
        y->right = x;
        return y;
    }

    SplayNode* rotateLeft(SplayNode* x) {
        SplayNode* y = x->right;
        x->right = y->left;
        y->left = x;
        return y;
    }

    SplayNode* splay(SplayNode* h, std::string ip) {
        if (!h || h->route.ip_subnet == ip) return h;

        if (ip < h->route.ip_subnet) {
            if (!h->left) return h;
            if (ip < h->left->route.ip_subnet) {
                h->left->left = splay(h->left->left, ip);
                h = rotateRight(h);
            } else if (ip > h->left->route.ip_subnet) {
                h->left->right = splay(h->left->right, ip);
                if (h->left->right) h->left = rotateLeft(h->left);
            }
            if (!h->left) return h;
            return rotateRight(h);
        } else {
            if (!h->right) return h;
            if (ip < h->right->route.ip_subnet) {
                h->right->left = splay(h->right->left, ip);
                if (h->right->left) h->right = rotateRight(h->right);
            } else if (ip > h->right->route.ip_subnet) {
                h->right->right = splay(h->right->right, ip);
                h = rotateLeft(h);
            }
            if (!h->right) return h;
            return rotateLeft(h);
        }
    }

public:
    OSPFRouteCache() = default;

    void addStaticRoute(std::string ip, std::string port) {
        RouteInfo r{ip, port};
        if (!root) {
            root = new SplayNode(r);
            return;
        }
        root = splay(root, ip);
        if (root->route.ip_subnet == ip) return;

        SplayNode* new_node = new SplayNode(r);
        if (ip < root->route.ip_subnet) {
            new_node->right = root;
            new_node->left = root->left;
            root->left = nullptr;
        } else {
            new_node->left = root;
            new_node->right = root->right;
            root->right = nullptr;
        }
        root = new_node;
    }

    std::string route_packet(std::string destination_ip) {
        if (!root) return "DROP_PACKET";
        root = splay(root, destination_ip);
        if (root->route.ip_subnet == destination_ip) {
            return root->route.output_interface;
        }
        return "DROP_DEFAULT_ROUTE";
    }
};