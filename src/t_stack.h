/**
 * @file t_stack.h
 * @author Naufal Ahmad <na867@drexel.edu>
 * @date June 11, 2025
 * @brief A stack data structure with thread safety built-in.
 * 
 * @section Description
 * A stack data structure with thread safety built-in.
 */

#ifndef _T_STACK_H_
#define _T_STACK_H_

#include <mutex>
#include <stack>

template <class T>
class t_stack {
    private:
        std::stack<T> data; /**Stack data structure to store elements */
        std::mutex m;/**Mutex to prevent multiple threads from accessing stack at once */
    public:
	void push(T &elem);/**Push element to stack */
        T pop();/**Pop element from stack, returning popped element */
        bool empty();/**Check if stack is empty */
};

template <class T>
void t_stack<T>::push(T &elem) {
    const std::lock_guard lk(m);
    data.push(elem);
}

template <class T>
T t_stack<T>::pop() {
    const std::lock_guard lk(m);
    T elem = data.top();
    data.pop();
    return elem;
}

template <class T>
bool t_stack<T>::empty() {
    return data.empty();
}

#endif
