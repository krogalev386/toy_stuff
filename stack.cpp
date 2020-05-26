#include <iostream>

template <typename T>
class Stack {

    T* stackArray;
    int lastInd;
    int capacity;

public:

    Stack(){
        capacity = 8;
        stackArray = new T[capacity];
        lastInd = -1;
    }

    ~Stack(){
        delete [] stackArray;
    }

    void push(const T & item){
        if (lastInd >= capacity){
            T* extendedStackArray = new T[2*capacity];
            memcpy(extendedStackArray, stackArray, capacity);
            delete [] stackArray;
            stackArray = extendedStackArray;
            capacity *=2;
        }
        lastInd += 1;
        stackArray[lastInd] = item;
    }

    T pop(){
        try{
            if (lastInd == -1)
                throw lastInd;
            lastInd -= 1;
            return stackArray[lastInd+1];
        }
        catch(int i) {
            std::cout << "Stack is empty!" << std::endl;
        }
    }
    
    T & top(){
        try{
            if (lastInd == -1)
                throw lastInd;
            return stackArray[lastInd+1];
        }
        catch(int i) {
            std::cout << "Stack is empty!" << std::endl;
        }
    }
};

int main(){
    Stack<int> iStack = Stack<int>();
    Stack<double> dStack = Stack<double>();
    int i;
    for (i = 0; i < 18; ++i)
        {iStack.push((i+0.5)); dStack.push((i+0.5));}
    for (i = 0; i < 12; ++i)
        std::cout << iStack.pop() << " " << dStack.pop() << std::endl;
    std::cout << iStack.top() << " " << dStack.pop() << std::endl;
    iStack.top() = 10;
    std::cout << iStack.top() << " " << dStack.pop() << std::endl;
}