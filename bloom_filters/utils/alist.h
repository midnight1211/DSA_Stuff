#pragma once
#ifndef ALIST_H
#define ALIST_H

#include <string>
#include <stdexcept>
#include "list.h"
#include "array.h"

// Inherit from List<T> instead of List
template <typename T>
class AList : public List<T> {
private:
    Array<T> listArray;
    static const int DEFAULT_SIZE = 10;
    int maxSize;
    int listSize;
    int curr;

public:
    // Fixed: properly initialize Array<T> using constructor initialization list
    AList(int size = DEFAULT_SIZE) : listArray(size), maxSize(size), listSize(0), curr(0) {}

    // Fixed: listArray is a class object; its own destructor handles memory management automatically
    ~AList() override = default; 

    void clear() override { listSize = curr = 0; }

    bool insert(const T& item) override {
        if (listSize >= maxSize) return false;
        for (int i = listSize; i > curr; i--)
            listArray[i] = listArray[i - 1];
        listArray[curr] = item;
        listSize++;
        return true;
    }

    bool append(const T& item) override {
        if (listSize >= maxSize) return false;
        listArray[listSize++] = item;
        return true;
    }

    T remove() override {
        if ((curr < 0) || (curr >= listSize))
            throw std::out_of_range("remove() in AList has current of " + std::to_string(curr) + " and size of " + std::to_string(listSize) + " that is not a valid element");
        T item = listArray[curr];
        for (int i = curr; i < listSize - 1; i++)
            listArray[i] = listArray[i + 1];
        listSize--;
        return item;
    }

    void moveToStart() override { curr = 0; }
    void moveToEnd() override { curr = listSize; }
    void prev() override { if (curr != 0) curr--; }
    void next() override { if (curr < listSize) curr++; }
    int length() override { return listSize; }
    int currPos() override { return curr; }   

    bool moveToPos(int pos) override {
        if ((pos < 0) || (pos > listSize)) return false;
        curr = pos;
        return true;
    }

    bool isAtEnd() override { return curr == listSize; }

    T getValue() override {
        if ((curr < 0) || (curr >= listSize))
            throw std::out_of_range("getValue() in AList has current of " + std::to_string(curr) + " and size of " + std::to_string(listSize) + " that is not a valid element");
        return listArray[curr];
    }

    bool isEmpty() override { return listSize == 0; }
};
#endif