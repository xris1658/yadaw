#ifndef YADAW_TEST_FILETREEMODELTEST_TREENODE
#define YADAW_TEST_FILETREEMODELTEST_TREENODE

#include <algorithm>
#include <optional>
#include <vector>

namespace YADAW::Model::Impl
{
template<typename T>
class TreeNode
{
public:
    TreeNode(const T& data, TreeNode* parent = nullptr):
        parent_(parent), children_(), data_(data)
    {}
    TreeNode(T&& data, TreeNode* parent = nullptr):
        parent_(parent), children_(), data_(std::move(data))
    {}
    template<typename... Args>
    static TreeNode<T>* create(TreeNode* parent, Args&& ... args)
    {
        return new TreeNode<T>(T(std::forward<Args>(args)...), parent);
    }
    ~TreeNode()
    {
        clearChildren();
    }
public:
    T& data()
    {
        return data_;
    }
    const T& data() const
    {
        return data_;
    }
    TreeNode<T>* parent() const
    {
        return parent_;
    }
    std::size_t childCount() const
    {
        return children_->size();
    }
    auto childBegin() const
    {
        return children_->cbegin();
    }
    auto childBegin()
    {
        return children_->begin();
    }
    auto childCBegin() const
    {
        return children_->cbegin();
    }
    auto childEnd() const
    {
        return children_->cend();
    }
    auto childEnd()
    {
        return children_->end();
    }
    auto childCEnd() const
    {
        return children_->cend();
    }
    bool childrenInitialized() const
    {
        auto ret = static_cast<bool>(children_);
        return ret;
    }
public:
    void initializeChildren()
    {
        children_.emplace();
    }
    void addChild(TreeNode<T>* node)
    {
        children_->emplace_back(node);
    }
    void clearChildren()
    {
        if(children_.has_value())
        {
            std::for_each(children_->begin(), children_->end(), [](TreeNode<T>* child)
            {
                delete child;
            });
            children_->clear();
            children_.reset();
        }
    }

private:
    TreeNode<T>* parent_ = nullptr;
    std::optional<std::vector<TreeNode<T>*>> children_;
    T data_;
};
}

#endif // YADAW_TEST_FILETREEMODELTEST_TREENODE
