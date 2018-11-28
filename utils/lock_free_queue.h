//
// Created by Сергей Ефимочкин on 06.05.17.
//

#ifndef LOCK_FREE_QUEUE_SOLUTION_H
#define LOCK_FREE_QUEUE_SOLUTION_H
#include <thread>
#include <atomic>
#include <vector>
#include <stack>


template <typename T, template <typename U> class Atomic = std::atomic>
class LockFreeQueue {
    struct Node {
        T element_{};
        Atomic<Node *> next_{nullptr};

        explicit Node(T element, Node *next = nullptr)
                : element_(std::move(element)), next_(next) {
        }

        explicit Node() {
        }
    };

public:
    explicit LockFreeQueue() {
        Node *dummy = new Node{};
        head_ = dummy;
        tail_ = dummy;
        counter = 0;
       // go = true;
    }

    ~LockFreeQueue() {
        Node *cur = head_.load();
        Node *next = cur->next_.load();
        while (true) {
            delete cur;
            cur = next;
            if (cur == nullptr) {
                break;
            }
            next = cur->next_.load();
        }
        while (garbage.size() > 0) {
            delete garbage.top();
            garbage.pop();
        }

    }


    void Enqueue(T element) {
        counter++;
        //while(!go){}
        Node * node = new Node();
        node->element_ = std::move(element);
        while (true) {
            Node *last = tail_.load();
            Node *next = last->next_.load();
            if (last == tail_.load()) {
                if (next == nullptr) {
                    if (last->next_.compare_exchange_strong(next, node)) {
                        tail_.compare_exchange_strong(last, node);
                        counter--;
                        return;
                    }
                } else {
                    tail_.compare_exchange_strong(last, next);
                }
            }

        }
    }

    bool Dequeue(T& element) {
        counter++;
       // while(!go){}
        while (true) {
            Node *first = head_.load();
            Node *last = tail_.load();
            Node *next = first->next_.load();
            if (first == head_.load()) {
                if (first == last) {
                    if (next == nullptr) {
                        counter--;
                        if(!counter) {
                            std::unique_lock<std::mutex> lock(mtx);
                            if (garbage.size() > 0) {
                                cleanUp();
                            }
                        }
                        return false;
                    }
                    tail_.compare_exchange_strong(last, next);
                } else {
                    T value = next->element_;
                    if(head_.compare_exchange_strong(first, next)) {
                        element = value;
                        counter--;

                        if(!counter) {
                            delete first;
                            std::unique_lock<std::mutex> lock(mtx);
                            if (garbage.size() > 0) {
                                cleanUp();
                            }
                        }
                        else {
                            std::unique_lock<std::mutex> lock(mtx);
                            garbage.push(first);
                        }
                        return true;
                    }
                }
            }
        }
    }

private:
    void cleanUp(){
       // go.store(false);
        while(garbage.size() > 0) {
            delete garbage.top();
            garbage.pop();
        }
      //  go.store(true);
    }
    std::mutex mtx;
    std::stack<Node*> garbage;
    std::atomic<int> counter;
    //std::atomic<bool> go;
    Atomic<Node*> head_{nullptr};
    Atomic<Node*> tail_{nullptr};
};

///////////////////////////////////////////////////////////////////////

#endif //LOCK_FREE_QUEUE_SOLUTION_H
