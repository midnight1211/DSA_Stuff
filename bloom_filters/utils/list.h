#pragma once
#ifndef LIST_H
#define LIST_H

template <typename T>
class List {
public:
    virtual ~List() = default; 
    virtual void clear() = 0;
    virtual bool insert(const T& item) = 0;
    virtual bool append(const T& it) = 0;
    virtual T remove() = 0;
    virtual void moveToStart() = 0;
    virtual void moveToEnd() = 0;
    virtual void prev() = 0;
    virtual void next() = 0;
    virtual int length() = 0;
    virtual int currPos() = 0;
    virtual bool moveToPos(int pos) = 0;
    virtual bool isAtEnd() = 0;
    virtual T getValue() = 0;
    virtual bool isEmpty() = 0;
};

#endif