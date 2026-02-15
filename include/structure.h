/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

/**
 * @brief A simple circular queue implementation.
 *
 * @tparam T The type of elements stored in the queue.
 * @tparam N The maximum number of elements the queue can hold.
 */
template <typename T, size_t N>
class CircularQueue {
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
 * @tparam T The type of the value stored in the tree node.
 */
template <typename T>
class TreeNode {
  public:
    TreeNode(void) : value(T(0)), left(nullptr), right(nullptr) {}
    TreeNode(const T &val) : value(val), left(nullptr), right(nullptr) {}

    void
    Assign(const T &val) {
        value = val;
    }

    T
    Get() const {
        return value;
    }

    TreeNode *
    Left() const {
        return left;
    }

    TreeNode *
    Right() const {
        return right;
    }

    void
    Reset() {
        value = T(0);
        left = nullptr;
        right = nullptr;
    }

  private:
    T value;
    TreeNode *left;
    TreeNode *right;
};
