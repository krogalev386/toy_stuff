#include <iostream>
#include <stdio.h>
#include <string.h>

template<typename T>
class Query {

    int firstItem;
    int lastItem;
    int capacity;
    int size;
    T* queryArray;

public:

    Query(){
        capacity = 8;
        queryArray = new T[capacity];
        firstItem = 0;
        lastItem = -1; // for correctness pushing of first item
        size = 0;
    }
    
    ~Query(){
        delete [] queryArray;
    }

    size_t getSize(){
        return size;
    }

    void pushback(const T & item){
        if (size == capacity){
            T* extendedQueryArray = new T[2*capacity];
            if (lastItem > firstItem)
                memcpy(extendedQueryArray, queryArray, size*sizeof(T));
            else {
                //coping first part of query
                int sizeOfFirstPart = (size - firstItem) + 1;
                memcpy(extendedQueryArray, &queryArray[firstItem], sizeOfFirstPart*sizeof(T));
                //coping last part
                int sizeOfLastPart = lastItem + 1;
                memcpy(&extendedQueryArray[firstItem], queryArray, sizeOfLastPart*sizeof(T));
                firstItem = 0;
                lastItem = size-1;
            }    
            delete [] queryArray;
            queryArray = extendedQueryArray;  
            capacity *= 2;
        }
        size++;
        lastItem++;
        if (lastItem == capacity)
            lastItem = 0;
        queryArray[lastItem] = item;
    }

    T popforward(){
        T result;
        if (size == 0){
            std::cout << "Empty query" << std::endl;
            result = 0;
        } else {
            result = queryArray[firstItem];
            size--;
            firstItem++;
            if (firstItem == capacity)
                firstItem = 0;
        }
        return result;
    }

    T & front(){
        if (size > 0)
            return queryArray[firstItem];
        else
            return NULL;
    }

    T & back(){
        if (size > 0)
            return queryArray[lastItem];
        else
            return NULL;
    }

};

int main(){
    Query<int> iQuery = Query<int>();
    Query<double> dQuery = Query<double>();
    for (int i = 0; i < 160; ++i){
        iQuery.pushback(i+0.5);
        dQuery.pushback(i+0.5);
    }

    for (int i = 0; i < 64; ++i){
    //    std::cout << iQuery.popforward() << " " << dQuery.popforward() << std::endl;
        iQuery.popforward();
        dQuery.popforward();
    }

    for (int i = 0; i < 160; ++i){
        iQuery.pushback(i+0.5);
        dQuery.pushback(i+0.5);
    }

    size_t size = iQuery.getSize();
    for (int i = 0; i < size; ++i){
        std::cout << iQuery.popforward() << " " << dQuery.popforward() << std::endl;
    }

    iQuery.pushback(11324);
    dQuery.pushback(23423.234);
    std::cout << iQuery.popforward() << " " << dQuery.popforward() << std::endl;
    std::cout << iQuery.popforward() << " " << dQuery.popforward() << std::endl;
    

}