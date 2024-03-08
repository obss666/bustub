#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  auto now = root_;
  if (now == nullptr) return nullptr;
  for (decltype(key.size()) i = 0; i < key.size(); i++) {
    auto it = now->children_.find(key[i]);
    if (it != now->children_.end()) {
      now = it->second;
    } else {
      return nullptr;
    }
  }

  const TrieNodeWithValue<T> *valueNode = dynamic_cast<const TrieNodeWithValue<T> *>(now.get());
  if (!valueNode) return nullptr;
  return valueNode->value_.get();
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  if (key.empty()) {
    if (root_)
      return Trie(std::make_shared<TrieNodeWithValue<T>>(root_->children_, std::make_shared<T>(std::move(value))));

    return Trie(std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value))));
  }

  std::shared_ptr<TrieNode> newroot;
  if (root_)
    newroot = std::move(root_->Clone());
  else
    newroot = std::make_shared<TrieNode>();

  auto now = newroot;
  for (decltype(key.size()) i = 0; i < key.size(); i++) {
    auto it = now->children_.find(key[i]);
    auto newnode = std::make_shared<TrieNode>();
    // auto newnode = std::shared_ptr<TrieNode>(); why not? 与上一行的区别? 没有分配内存
    if (i == key.size() - 1) {
      if (it != now->children_.end()) {
        newnode = std::make_shared<TrieNodeWithValue<T>>(it->second->children_, std::make_shared<T>(std::move(value)));
      } else {
        newnode = std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
      }
    } else {
      if (it != now->children_.end()) {
        newnode = it->second->Clone();
      }
    }
    now->children_[key[i]] = newnode;
    now = newnode;
  }
  return Trie(newroot);
}

auto Trie::Remove(std::string_view key) const -> Trie {
  if (root_ == nullptr) return Trie();

  if (key.empty()) {
    if (root_->children_.empty())
      return Trie();
    else
      return Trie(std::make_shared<TrieNode>(root_->children_));
  }

  std::shared_ptr<TrieNode> newroot = std::move(root_->Clone());

  auto now = newroot;

  std::vector<std::shared_ptr<TrieNode>> path{now};
  std::vector<char> word;

  for (decltype(key.size()) i = 0; i < key.size(); i++) {
    auto it = now->children_.find(key[i]);
    auto newnode = std::make_shared<TrieNode>();
    if (it == now->children_.end()) {
      return Trie(newroot);
    }
    if (i == key.size() - 1) {
      newnode = std::make_shared<TrieNode>(it->second->children_);
    } else {
      newnode = it->second->Clone();
    }
    now->children_[key[i]] = newnode;
    now = newnode;

    path.push_back(now);
    word.push_back(key[i]);
  }

  auto son = path.back();
  path.pop_back();

  while (path.size()) {
    auto father = path.back();
    path.pop_back();

    if (son->children_.empty() && !son->is_value_node_) {
      father->children_.erase(word.back());
      word.pop_back();
    } else {
      break;
    }
    son = father;
  }

  if (newroot->children_.empty() && !newroot->is_value_node_) {
    return Trie();
  }

  return Trie(newroot);
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;
}  // namespace bustub
