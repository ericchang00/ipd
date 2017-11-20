#pragma once

/*
 * A deque (pronounced like "deck") is a double-ended queue. This file
 * This file contains an implementation of a deque represented as a
 * doubly-linked list. It is intended to demonstrate a
 * full-bells-and-whistles generic container. (It doesn't do everything that
 * STL containers do. For example, they allow the use of custom allocators,
 * but the `Deque` class below just uses `new`.)
 */

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace sentinel {

//
// Forward declarations of helper classes
//

template<typename T>
class Deque_iterator;

template<typename T>
class Deque_const_iterator;

//
// The main `Deque` class
//

template<typename T>
class Deque
{
public:
    // Constructs a new, empty deque.
    Deque() noexcept;

    // Constructs a deque with the given elements;
    Deque(std::initializer_list<T>);

    // Copy constructor.
    Deque(const Deque&);
    // Copy-assignment operator.
    Deque& operator=(const Deque&);

    // Move constructor.
    Deque(Deque&&) noexcept;
    // Move assignment operator.
    Deque& operator=(Deque&&) noexcept;

    // Returns true if the deque is empty.
    bool empty() const noexcept;
    // Returns the number of elements in the deque.
    size_t size() const noexcept;

    // Returns a reference to the first element of the deque. If the deque is
    // empty then the behavior is undefined.
    const T& front() const;
    T& front();

    // Returns a reference to the last element of the deque. If the deque is
    // empty then the behavior is undefined.
    const T& back() const;
    T& back();

    // Inserts a new element at the front of the deque.
    void push_front(const T&);
    void push_front(T&&);

    // Constructs a new element at the front of the queue.
    template<typename... Args>
    void emplace_front(Args&& ...);

    // Inserts a new element at the back of the deque.
    void push_back(const T&);
    void push_back(T&&);

    // Constructs a new element at the back of the queue.
    template<typename... Args>
    void emplace_back(Args&& ...);

    // Removes the first element of the deque. Undefined if the
    // deque is empty.
    void pop_front();

    // Removes the last element of the deque. Undefined if the
    // deque is empty.
    void pop_back();

    // Removes all elements from the deque.
    void clear() noexcept;

    // Exchanges the contents of two deques without copying.
    void swap(Deque&) noexcept;

    // Iterators over Deques. There are four kinds, all combinations of
    // const-ness and reverse-ness.
    using iterator = Deque_iterator<T>;
    using const_iterator = Deque_const_iterator<T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // The destructor.
    ~Deque();

private:
    struct node_;

    struct node_base_
    {
        node_base_* prev = nullptr;
        node_base_* next = nullptr;

        node_* as_node() { return static_cast<node_*>(this); }

        const node_* as_node() const { return static_cast<const node_*>(this); }
    };

    // The linked list is made out of nodes, each of which contains a data
    // element and pointers to next and previous nodes.
    struct node_ : node_base_
    {
        T data;

        // Constructs a new node, forwarding the arguments to construct the
        // data element. The prev and next pointers are initialized to nullptr.
        template<typename... Args>
        explicit node_(Args&& ... args)
        noexcept(noexcept(T(std::forward<Args>(args)...)))
                : data(std::forward<Args>(args)...) {}
    };

    // Private member variables:
    node_base_ sentinel_;
    size_t     size_;

    // Attaches a new node to the front of the list.
    void push_front_(node_*) noexcept;
    // Attaches a new node to the back of the list.
    void push_back_(node_*) noexcept;

    // Sets this deque to empty without deleting any nodes. Will leak of the
    // nodes haven't been adopted by another deque.
    void set_empty_() noexcept;
    // Moves the elements of the other deque to this deque, setting the other
    // deque to empty.
    void move_from_(Deque&&) noexcept;

    // Gets a reference to the first node (next of sentinel_), downcast to
    // node_. Undefined if empty.
    node_* head_() const noexcept;
    // Gets a reference to the last node (prev of sentinel_), downcast to
    // node_. Undefined if empty.
    node_* tail_() const noexcept;

    friend class Deque_iterator<T>;

    friend class Deque_const_iterator<T>;
};

// Helper class for Deque<T>::iterator.
template<typename T>
class Deque_iterator : public std::iterator<
        std::bidirectional_iterator_tag,
        T>
{
public:
    // Compares two iterators.
    bool operator==(Deque_iterator) noexcept;

    // Gets the value of the current node.
    T& operator*() const noexcept;
    // Gets a pointer to the value of the current node.
    T* operator->() const noexcept;

    // Advances to the next node.
    Deque_iterator& operator++() noexcept;
    // Advances to the next node.
    Deque_iterator operator++(int) noexcept;
    // Retreats to the previous node.
    Deque_iterator& operator--() noexcept;
    // Retreats to the previous node.
    Deque_iterator operator--(int) noexcept;

private:
    using node_base_ = typename Deque<T>::node_base_;

    // We represent an iterator as a pointer to the current node and a
    // pointer to the deque as a whole. The reason we need the pointer to the
    // deque is so that we can go back to the tail be decrementing the end
    // iterator, which points past the end. That is, in the result of the
    // end() function (and friends) the current_ node is nullptr, but the
    // owner_ can be used to find the tail when autodecremented.
    node_base_* current_;

    Deque_iterator(node_base_* current) noexcept
            : current_(current) {}

    friend class Deque<T>;

    friend class Deque_const_iterator<T>;
};

template<typename T>
bool operator!=(Deque_iterator<T>, Deque_iterator<T>) noexcept;

// Helper class for Deque<T>::const_iterator.
template<typename T>
class Deque_const_iterator : public std::iterator<
        std::bidirectional_iterator_tag,
        const T>
{
public:
    // Coercion from Deque<T>::iterator to Deque<T>::const_iterator.
    Deque_const_iterator(Deque_iterator<T> other) noexcept
            : current_(other.current_) {}

    bool operator==(Deque_const_iterator) noexcept;

    const T& operator*() const noexcept;
    const T* operator->() const noexcept;

    Deque_const_iterator& operator++() noexcept;
    Deque_const_iterator operator++(int) noexcept;
    Deque_const_iterator& operator--() noexcept;
    Deque_const_iterator operator--(int) noexcept;

private:
    using node_base_  = typename Deque<T>::node_base_;

    const node_base_* current_;

    Deque_const_iterator(const node_base_* current) noexcept
            : current_(current) {}

    friend class Deque<T>;
};

template<typename T>
bool operator!=(Deque_const_iterator<T>, Deque_const_iterator<T>) noexcept;

///
/// IMPLEMENTATIONS
///

template<typename T>
auto Deque<T>::head_() const noexcept -> node_*
{
    return sentinel_.next->as_node();
}

template<typename T>
auto Deque<T>::tail_() const noexcept -> node_*
{
    return sentinel_.prev->as_node();
}

template<typename T>
void Deque<T>::set_empty_() noexcept
{
    size_ = 0;
    sentinel_.next = sentinel_.prev = &sentinel_;
}

template<typename T>
void Deque<T>::move_from_(Deque&& other) noexcept
{
    if (other.empty()) {
        set_empty_();
    } else {
        sentinel_ = other.sentinel_;
        size_     = other.size_;
        other.set_empty_();
    }
}

template<typename T>
Deque<T>::Deque() noexcept
{
    set_empty_();
}

template<typename T>
Deque<T>::Deque(std::initializer_list<T> args) : Deque()
{
    for (const auto& arg : args)
        push_back(arg);
}

template<typename T>
Deque<T>::Deque(const Deque& other) : Deque()
{
    for (const auto& elt : other)
        push_back(elt);
}

template<typename T>
Deque<T>& Deque<T>::operator=(const Deque& other)
{
    clear();

    for (const auto& elt : other)
        push_back(elt);

    return *this;
}

template<typename T>
Deque<T>::Deque(Deque&& other) noexcept
{
    move_from_(other);
}

template<typename T>
Deque<T>& Deque<T>::operator=(Deque&& other) noexcept
{
    clear();
    move_from_(other);
    return *this;
}

template<typename T>
bool Deque<T>::empty() const noexcept
{
    return size_ == 0;
}

template<typename T>
size_t Deque<T>::size() const noexcept
{
    return size_;
}

template<typename T>
const T& Deque<T>::front() const
{
    return head_()->data;
}

template<typename T>
T& Deque<T>::front()
{
    return head_()->data;
}

template<typename T>
const T& Deque<T>::back() const
{
    return tail_()->data;
}

template<typename T>
T& Deque<T>::back()
{
    return tail_()->data;
}

template<typename T>
void Deque<T>::push_front_(node_* new_node) noexcept
{
    new_node->next       = sentinel_.next;
    new_node->prev       = &sentinel_;
    new_node->next->prev = new_node;
    new_node->prev->next = new_node;
    ++size_;
}

template<typename T>
void Deque<T>::push_front(const T& value)
{
    push_front_(new node_(value));
}

template<typename T>
void Deque<T>::push_front(T&& value)
{
    push_front_(new node_(std::move(value)));
}

template<typename T>
template<typename... Args>
void Deque<T>::emplace_front(Args&& ... args)
{
    push_front_(new node_(std::forward(args)...));
}

template<typename T>
void Deque<T>::push_back_(node_* new_node) noexcept
{
    new_node->prev       = sentinel_.prev;
    new_node->next       = &sentinel_;
    new_node->next->prev = new_node;
    new_node->prev->next = new_node;
    ++size_;
}

template<typename T>
void Deque<T>::push_back(const T& value)
{
    push_back_(new node_(value));
}

template<typename T>
void Deque<T>::push_back(T&& value)
{
    push_back_(new node_(std::move(value)));
}

template<typename T>
template<typename... Args>
void Deque<T>::emplace_back(Args&& ... args)
{
    push_back_(new node_(std::forward<Args>(args)...));
}

template<typename T>
void Deque<T>::pop_front()
{
    node_     * head     = head_();
    node_base_* new_head = head->next;
    delete head;

    sentinel_.next = new_head;
    new_head->prev = &sentinel_;

    --size_;
}

template<typename T>
void Deque<T>::pop_back()
{
    node_     * tail     = tail_();
    node_base_* new_tail = tail->prev;
    delete tail;

    sentinel_.prev = new_tail;
    new_tail->next = &sentinel_;

    --size_;
}

template<typename T>
void Deque<T>::clear() noexcept
{
    while (!empty()) pop_front();
}

// The sentinel node makes other operations easier, but it makes swap harder,
// because we have pointers to the internal sentinel_ node that need to be
// adjusted.
template<typename T>
void Deque<T>::swap(Deque& other) noexcept
{
    if (empty())
        move_from_(std::move(other));
    else if (other.empty())
        other.move_from_(std::move(*this));
    else {
        std::swap(sentinel_, other.sentinel_);
        std::swap(size_, other.size_);
        sentinel_.next->prev = &sentinel_;
        sentinel_.prev->next = &sentinel_;
        other.sentinel_.next->prev = &other.sentinel_;
        other.sentinel_.prev->next = &other.sentinel_;
    }
}

template<typename T>
Deque<T>::~Deque()
{
    clear();
}

template<typename T>
auto Deque<T>::begin() noexcept -> iterator
{
    return iterator(sentinel_.next);
}

template<typename T>
auto Deque<T>::begin() const noexcept -> const_iterator
{
    return const_iterator(sentinel_.next);
}

template<typename T>
auto Deque<T>::end() noexcept -> iterator
{
    return iterator(&sentinel_);
}

template<typename T>
auto Deque<T>::end() const noexcept -> const_iterator
{
    return const_iterator(&sentinel_);
}

template<typename T>
auto Deque<T>::rbegin() noexcept -> reverse_iterator
{
    return reverse_iterator(end());
}

template<typename T>
auto Deque<T>::rbegin() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(end());
}

template<typename T>
auto Deque<T>::rend() noexcept -> reverse_iterator
{
    return reverse_iterator(begin());
}

template<typename T>
auto Deque<T>::rend() const noexcept -> const_reverse_iterator
{
    return const_reverse_iterator(begin());
}

template<typename T>
auto Deque<T>::cbegin() const noexcept -> const_iterator
{
    return begin();
}

template<typename T>
auto Deque<T>::cend() const noexcept -> const_iterator
{
    return end();
}

template<typename T>
auto Deque<T>::crbegin() const noexcept -> const_reverse_iterator
{
    return rbegin();
}

template<typename T>
auto Deque<T>::crend() const noexcept -> const_reverse_iterator
{
    return rend();
}

template<typename T>
bool Deque_iterator<T>::operator==(Deque_iterator other) noexcept
{
    return current_ == other.current_;
}

template<typename T>
T& Deque_iterator<T>::operator*() const noexcept
{
    return current_->as_node()->data;
}

template<typename T>
T* Deque_iterator<T>::operator->() const noexcept
{
    return &current_->as_node()->data;
}

template<typename T>
Deque_iterator<T>& Deque_iterator<T>::operator++() noexcept
{
    current_ = current_->next;
    return *this;
}

template<typename T>
Deque_iterator<T> Deque_iterator<T>::operator++(int) noexcept
{
    Deque_iterator result(*this);
    ++*this;
    return result;
}

template<typename T>
Deque_iterator<T>& Deque_iterator<T>::operator--() noexcept
{
    current_ = current_->prev;
    return *this;
}

template<typename T>
Deque_iterator<T> Deque_iterator<T>::operator--(int) noexcept
{
    Deque_iterator result(*this);
    --*this;
    return result;
}

template<typename T>
bool operator!=(Deque_iterator<T> i, Deque_iterator<T> j) noexcept
{
    return !(i == j);
}

template<typename T>
bool Deque_const_iterator<T>::operator==(Deque_const_iterator other) noexcept
{
    return current_ == other.current_;
}

template<typename T>
const T& Deque_const_iterator<T>::operator*() const noexcept
{
    return current_->as_node()->data;
}

template<typename T>
const T* Deque_const_iterator<T>::operator->() const noexcept
{
    return &current_->as_node()->data;
}

template<typename T>
Deque_const_iterator<T>& Deque_const_iterator<T>::operator++() noexcept
{
    current_ = current_->next;
    return *this;
}

template<typename T>
Deque_const_iterator<T> Deque_const_iterator<T>::operator++(int) noexcept
{
    Deque_const_iterator result(*this);
    ++*this;
    return result;
}

template<typename T>
Deque_const_iterator<T>& Deque_const_iterator<T>::operator--() noexcept
{
    current_ = current_->prev;
    return *this;
}

template<typename T>
Deque_const_iterator<T> Deque_const_iterator<T>::operator--(int) noexcept
{
    Deque_const_iterator result(*this);
    --*this;
    return result;
}

template<typename T>
bool operator!=(Deque_const_iterator<T> i, Deque_const_iterator<T> j) noexcept
{
    return !(i == j);
}

}

namespace std {

template<typename T>
void swap(sentinel::Deque<T>& a, sentinel::Deque<T>& b) noexcept
{
    a.swap(b);
}

}
