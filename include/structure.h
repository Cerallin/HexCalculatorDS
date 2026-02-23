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
    CircularQueue() : head(0), tail(0), size(0) {}

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
    Empty() const {
        return size == 0;
    }

    /**
     * @brief Check if the queue is full.
     *
     * @return true if the queue is full, otherwise returns false.
     */
    bool
    Full() const {
        return size == N;
    }

    /**
     * @brief Get the current size of the queue.
     *
     * @return The number of elements currently in the queue.
     */
    size_t
    Size() const {
        return size;
    }

  private:
    T data[N];
    size_t head;
    size_t tail;
    size_t size;
};

/**
 * @brief A simple binary tree node.
 *
 * @tparam DataType The type of the value stored in the tree node.
 */
template <typename Class, typename DataType>
class TreeNode {
  public:
    TreeNode(void) : value(DataType(0)), left(nullptr), right(nullptr) {}
    TreeNode(const DataType &val) : value(val), left(nullptr), right(nullptr) {}

    void
    Assign(const DataType &val) {
        value = val;
    }

    DataType
    Get() const {
        return value;
    }

    Class *
    Left() const {
        return left;
    }

    Class *
    Right() const {
        return right;
    }

    Class *
    Parent() const {
        return parent;
    }

    int
    ChildCount() const {
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
    ConnectLeft(Class &child) {
        left = &child;
        child.parent = static_cast<Class *>(this);
    }

    void
    ConnectRight(Class &child) {
        right = &child;
        child.parent = static_cast<Class *>(this);
    }

    void
    Reset() {
        value = DataType(0);
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }

    /**
     * @brief Performs a post-order traversal (left -> right -> root) of a
     * binary tree.
     *
     * This implementation uses an explicit stack instead of recursion.
     *
     * @tparam T Node value type.
     * @tparam N Maximum stack depth.
     * @param visit Callback invoked for each visited node.
     */
    template <size_t N, typename Func>
    void
    PostOrderTraversal(Func visit) {
        Stack<Class *, N> stack;

        Class *current = this;
        Class *lastVisited = nullptr;

        while (current != nullptr || !stack.Empty()) {
            if (current != nullptr) {
                stack.Push(current);
                current = current->Left();
            } else {
                Class *peek;
                stack.Pop(peek); // temporary pop to inspect

                auto *right = peek->Right();
                if (right != nullptr && lastVisited != right) {
                    // Right subtree not visited yet
                    stack.Push(peek); // push back
                    current = right;  // traverse right
                } else {
                    // Visit node
                    visit(*peek);
                    lastVisited = peek;
                }
            }
        }
    }

  protected:
    DataType value;
    Class *left;
    Class *right;
    Class *parent;
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
    Stack() : top(0) {}

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
    Empty() const {
        return top == 0;
    }

    /**
     * @brief Checks whether the stack is full.
     *
     * @return true  If the stack has reached its capacity.
     * @return false Otherwise.
     */
    bool
    Full() const {
        return top == N;
    }

  private:
    T data[N];  // Internal storage buffer.
    size_t top; // Index of the next insertion position (also current size).
};
