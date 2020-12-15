#include <memory>
#include <vector>
#include <utility>
#include <algorithm>

#pragma once

// @brief B+ tree implementation.
// @param Key: type of key contained in each node
// @param Value: type of value stored in leaves
template<typename Key, typename Value>
class bplus_tree
{
public:

    bplus_tree(size_t n, size_t m) : N(n), M(m), root(new_leaf(m)), depth(0) {}

    bplus_tree(size_t n) : N(n), M(n), root(new_leaf(n)), depth(0) {}
    
    ~bplus_tree()
    {
        // Empty. Memory deallocation is done through the smart pointers
    }

    // Bulk load constructor
    bplus_tree(const std::vector<std::pair<Key, Value>>& data);

    // Inserts a key and its value
    // @param key: The key
    // @param value: The value
    void insert(Key key, Value value)
    {
        insertion_result result;
        bool split = false;
        if (depth == 0)
        {
            auto current_node = std::reinterpret_pointer_cast<leaf_node>(root);
            split = leaf_insert(current_node, key, value, result);
        }
        else 
        {
            auto current_node = std::reinterpret_pointer_cast<inner_node>(root);
            split = inner_insert(current_node, key, value, result, depth);
        }
        if (split)
        {
            depth++;
            root = new_inner(N);
            auto root_proxy = std::reinterpret_pointer_cast<inner_node>(root);
            root_proxy->keys.push_back(std::move(result.key));
            root_proxy->children.push_back(result.left);
            root_proxy->children.push_back(result.right);
        }
    }

    // returns a reference if key is contained in the B+ tree
    const Value& at(const Key& key) const
    {
        std::shared_ptr<inner_node> inner;
        std::shared_ptr<node> current_node = root;
        int32_t d = depth;
        int32_t index = 0;

        while (d != 0)
        {
            inner = std::reinterpret_pointer_cast<inner_node>(current_node);
            index = inner_index(key, inner->keys);
            current_node = inner->children[index];
            d--;
        }

        auto leaf = std::reinterpret_pointer_cast<leaf_node>(current_node);
        if (leaf->keys.size() != 0)
        {
            index = leaf_index(key, leaf->keys);
        }

        return leaf->values[index];
    }

    // returns a const reference if key is contained in the B+ tree
    Value& at(const Key& key)
    {
        std::shared_ptr<inner_node> inner;
        std::shared_ptr<node> current_node = root;
        int32_t d = depth;
        int32_t index = 0;

        while (d != 0)
        {
            inner = std::reinterpret_pointer_cast<inner_node>(current_node);
            index = inner_index(key, inner->keys);
            current_node = inner->children[index];
            d--;
        }

        auto leaf = std::reinterpret_pointer_cast<leaf_node>(current_node);
        if (leaf->keys.size() != 0)
        {
            index = leaf_index(key, leaf->keys);
        }

        return leaf->values[index];
    }

    bool contains(const Key& key)
    {
        std::shared_ptr<inner_node> inner;
        std::shared_ptr<node> current_node = root;
        int32_t d = depth;
        int32_t index = 0;

        while (d != 0)
        {
            inner = std::reinterpret_pointer_cast<inner_node>(current_node);
            index = inner_index(key, inner->keys);
            current_node = inner->children[index];
            d--;
        }

        auto leaf = std::reinterpret_pointer_cast<leaf_node>(current_node);
        if (leaf->keys.size() != 0)
        {
            index = leaf_index(key, leaf->keys);
            bool result = leaf->keys[index] == key;
            return result;
        }
        return false;
    }

private:  

    enum node_type { LEAF, INNER };

    struct node
    {
        std::vector<Key> keys;
        std::vector<uint8_t> pad; // template this?
        node_type type;

        node(node_type t) : type(t) {}
    };

    struct leaf_node : node
    {
        leaf_node(size_t m) : node(LEAF) 
        {
            pad.reserve(0);
            keys.reserve(m);
            values.reserve(m);
        };

        std::vector<Value> values;
    };

    struct inner_node : node 
    {
        inner_node(size_t n) : node(INNER)
        {
            pad.reserve(0);
            keys.reserve(n);
            children.reserve(n + 1);
        };

        std::vector<std::shared_ptr<node>> children;
    };

    struct insertion_result
    {
        Key key; // copy, might be unnecessary 
        std::shared_ptr<node> left;
        std::shared_ptr<node> right;
    };

    std::shared_ptr<leaf_node> new_leaf(size_t m)
    {
        return std::shared_ptr<leaf_node>(new leaf_node(m));
    }

    std::shared_ptr<inner_node> new_inner(size_t n)
    {
        return std::shared_ptr<inner_node>(new inner_node(n));
    }
    
    static int32_t leaf_index(const Key&  key, const std::vector<Key>& keys)
    {
        // auto ptr = std::find(keys.begin(), keys.end(), key); // linear search
        // binary search
        auto ptr = std::lower_bound(keys.begin(), keys.end(), key);
        int32_t index = ptr - keys.begin();
        return index;
    }
    
    static int32_t inner_index(const Key& key, const std::vector<Key>& keys)
    {  
        auto ptr = std::upper_bound(keys.begin(), keys.end(), key); 
        int32_t index = ptr - keys.begin();
        return index;
    }

    bool inner_insert(
        std::shared_ptr<inner_node>& node,
        const Key& key,
        const Value& value,
        insertion_result& result,
        int depth)
    {
        bool split = false;
        if (node->keys.size() != node->keys.capacity())
        {
            inner_insert_available(node, key, value, depth);
        }
        else 
        {
            int32_t threshold = (N + 1) / 2;
            auto sibling = new_inner(N);

            for (int32_t i = threshold; i < node->keys.size(); i++)
            {
                sibling->keys.push_back(std::move(node->keys[i]));
                sibling->children.push_back(std::move(node->children[i]));
                
            }
            int last = node->children.size() - 1;
            sibling->children.push_back(std::move(node->children[last]));

            int count = node->keys.size() - threshold;
            for (int32_t i = 0; i < count; i++)
            {
                node->keys.pop_back();
                node->children.pop_back();
            }
            node->children.pop_back();

            split = true;
            result.key = node->keys[threshold - 1];
            result.left = node;
            result.right = sibling;

            if (key < result.key)
            {
                inner_insert_available(node, key, value, depth);
            }
            else 
            {
                inner_insert_available(sibling, key, value, depth);
            }
        }
        return split;
    }

    void inner_insert_available(
        std::shared_ptr<inner_node>& node,
        const Key& key,
        const Value& value,
        int depth)
    {
        uint32_t index = inner_index(key, node->keys);
        insertion_result result;
        bool split = false;

        if (depth - 1 == 0)
        {
            auto leaf = std::reinterpret_pointer_cast<leaf_node>(node->children[index]);
            split = leaf_insert(leaf, key, value, result);
        }
        else 
        {
            auto child = std::reinterpret_pointer_cast<inner_node>(node->children[index]);
            split = inner_insert(child, key, value, result, depth - 1);
        }

        if (split)
        {
            if (index == node->keys.size())
            {   
                node->keys.push_back(result.key);
                node->children[index] = result.left;
                node->children.push_back(result.right);
            }
            else 
            {
                node->keys.push_back(result.key);
                auto begin = node->children.begin();
                node->children.insert(begin + index + 1, result.right);
                node->children[index] = result.left;
            }
        }
    }

    bool leaf_insert(
        std::shared_ptr<leaf_node> leaf,
        const Key& key,
        const Value& value,
        insertion_result& result)
    {
        bool split = false;
        int32_t index = leaf_index(key, leaf->keys);
        if (leaf->keys.size() != leaf->keys.capacity())
        {
            leaf_insert_available(leaf, key, value, index);
        }
        else
        {
            int32_t threshold = (M + 1) / 2;
            auto sibling = new_leaf(M);

            // moves keys and values into their new leaf rather than copying
            // requires move constructor for the Key and Value!
            for (int32_t i = threshold; i < leaf->keys.size(); i++)
            {
                sibling->keys.push_back(std::move(leaf->keys[i]));
                sibling->values.push_back(std::move(leaf->values[i]));
            }

            int count = leaf->keys.size() - threshold;
            for (int32_t i = 0; i < count; i++)
            {
                leaf->keys.pop_back();
                leaf->values.pop_back();
            }

            if (index < threshold)
            {
                leaf_insert_available(leaf, key, value, index);
            }
            else 
            {
                leaf_insert_available(sibling, key, value, index - threshold);
            }

            split = true;
            result.key = sibling->keys[0];
            result.left = leaf;
            result.right = sibling;
        }
        return split;
    }

    void leaf_insert_available(
        std::shared_ptr<leaf_node> leaf,
        const Key& key,
        const Value& value,
        int32_t index)
    {
        if (index < leaf->keys.size() && leaf->keys.size() != 0 && leaf->keys[index] == key)
        {
            // duplicate key
            leaf->values[index] = value;
        }
        else
        {
            std::vector<Key>& keys = leaf->keys;
            std::vector<Value>& values = leaf->values;
            keys.insert(keys.begin() + index, key);
            values.insert(values.begin() + index, value);
        }
    }

    std::shared_ptr<node> root;
    const size_t N;
    const size_t M;
    int32_t depth;
};
