/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "traits.h"

/**
 * @brief A simple circular queue implementation.
 *
 * @tparam T The type of elements stored in the queue.
 * @tparam N The maximum number of elements the queue can hold.
 */
template <typename T, size_t N>
class CircularQueue : public NonCopyable {
  public:
    CircularQueue(void) : data{}, head(0), tail(0), size(0) {}

    /**
     * @brief Enqueue an element into the queue.
     *
     * @param value The value to be enqueued.
     * @return true if the element was successfully enqueued, false if the queue
     * is full.
     */
    bool
    Enqueue(const T &value) {
        if (size == N) {
            return false; // queue is full
        }
        data[tail] = value;
        tail = (tail + 1) % N;
        ++size;
        return true;
    }

    /**
     * @brief Dequeue an element from the queue.
     *
     * @param value A reference to store the dequeued value.
     * @return true if the element was successfully dequeued, false if the queue
     * is empty.
     */
    bool
    Dequeue(T &value) {
        if (size == 0) {
            return false; // queue is empty
        }
        value = data[head];
        head = (head + 1) % N;
        --size;
        return true;
    }

    /**
     * @brief Check if the queue is empty.
     *
     * @return true if the queue is empty, otherwise returns false.
     */
    bool
    Empty(void) const {
        return size == 0;
    }

    /**
     * @brief Check if the queue is full.
     *
     * @return true if the queue is full, otherwise returns false.
     */
    bool
    Full(void) const {
        return size == N;
    }

    /**
     * @brief Get the current size of the queue.
     *
     * @return The number of elements currently in the queue.
     */
    size_t
    Size(void) const {
        return size;
    }

    /**
     * @brief Clear the queue, removing all elements.
     */
    void
    Clear(void) {
        head = 0;
        tail = 0;
        size = 0;
    }

  private:
    T data[N];
    size_t head;
    size_t tail;
    size_t size;
};

/**
 * @brief A fixed-capacity stack container.
 *
 * This stack stores elements in a contiguous internal buffer with
 * compile-time capacity. No dynamic memory allocation is performed.
 *
 * @tparam T The element type stored in the stack.
 * @tparam N The maximum number of elements the stack can hold.
 */
template <typename T, size_t N>
class Stack : public NonCopyable {
  public:
    /**
     * @brief Constructs an empty stack.
     */
    Stack(void) : top(0) {}

    /**
     * @brief Pushes a value onto the stack.
     *
     * @param value The value to be pushed.
     * @return true  If the value was successfully pushed.
     * @return false If the stack is already full.
     */
    bool
    Push(const T &value) {
        if (top == N) {
            return false; // stack is full
        }
        data[top++] = value;
        return true;
    }

    /**
     * @brief Pops the top value from the stack.
     *
     * @param value Output parameter that receives the popped value.
     * @return true if a value was successfully popped, false if the stack is
     * empty.
     */
    bool
    Pop(T &value) {
        if (top == 0) {
            return false; // stack is empty
        }
        value = data[--top];
        return true;
    }

    /**
     * @brief Checks whether the stack is empty.
     *
     * @return true if the stack contains no elements, false otherwise.
     */
    bool
    Empty(void) const {
        return top == 0;
    }

    /**
     * @brief Checks whether the stack is full.
     *
     * @return true  If the stack has reached its capacity.
     * @return false Otherwise.
     */
    bool
    Full(void) const {
        return top == N;
    }

  private:
    T data[N];  // Internal storage buffer.
    size_t top; // Index of the next insertion position (also current size).
};

/**
 * @brief A simple binary tree node.
 *
 * @tparam DataType The type of the value stored in the tree node.
 */
template <class Derived, typename DataType>
class TreeNode {
  public:
    TreeNode(void)
        : value(DataType(0)), left(nullptr), right(nullptr), parent(nullptr) {}
    explicit TreeNode(const DataType &val)
        : value(val), left(nullptr), right(nullptr), parent(nullptr) {}

    void
    Assign(const DataType &val) {
        value = val;
    }

    DataType
    Get(void) const {
        return value;
    }

    Derived *
    Left(void) const {
        return left;
    }

    Derived *
    Right(void) const {
        return right;
    }

    Derived *
    Parent(void) const {
        return parent;
    }

    int
    ChildCount(void) const {
        int count = 0;

        if (left != nullptr) {
            count++;
        }
        if (right != nullptr) {
            count++;
        }

        return count;
    }

    void
    ConnectLeft(Derived &child) {
        left = &child;
        child.parent = static_cast<Derived *>(this);
    }

    void
    ConnectRight(Derived &child) {
        right = &child;
        child.parent = static_cast<Derived *>(this);
    }

    void
    Reset(void) {
        value = DataType(0);
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }

    /**
     * @brief Performs a post-order traversal (left -> right -> root) of a
     * binary tree.
     *
     * @tparam T Node value type.
     * @tparam N Maximum stack depth.
     * @param visit Callback invoked for each visited node.
     */
    template <size_t N, typename Func>
    void
    PostOrderTraversal(Func visit) {
        Stack<Derived *, N> stack;

        Derived *current = static_cast<Derived *>(this);
        Derived *lastVisited = nullptr;

        while (current != nullptr || !stack.Empty()) {
            if (current != nullptr) {
                stack.Push(current);
                current = current->Left();
            } else {
                Derived *peek;
                stack.Pop(peek); // temporary pop to inspect

                auto *rightChild = peek->Right();
                if (rightChild != nullptr && lastVisited != rightChild) {
                    // Right subtree not visited yet
                    stack.Push(peek);     // push back
                    current = rightChild; // traverse right
                } else {
                    // Visit node
                    visit(*peek);
                    lastVisited = peek;
                }
            }
        }
    }

    /**
     * @brief Performs an in-order traversal (left -> root -> right) of a binary
     * tree.
     *
     * @tparam N Maximum stack depth.
     * @tparam Func Callable type of the visit callback, which should be
     * invocable with a single argument of type `Derived&` (the node being
     * visited).
     * @param visit Callback invoked for each visited node.
     */
    template <size_t N, typename Func>
    void
    InOrderTraversal(Func visit) {
        Stack<Derived *, N> stack;

        Derived *current = static_cast<Derived *>(this);

        while (current != nullptr || !stack.Empty()) {
            if (current != nullptr) {
                stack.Push(current);
                current = current->Left();
            } else {
                Derived *peek;
                stack.Pop(peek); // temporary pop to inspect

                // Visit node
                visit(*peek);

                current = peek->Right(); // traverse right
            }
        }
    }

  protected:
    DataType value;
    Derived *left;
    Derived *right;
    Derived *parent;
};
