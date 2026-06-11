#pragma once
#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <cstddef> // For std::size_t

template <typename T>
class Array {
    private:
        T *a;
        int length;

    public:
        // 1. Default Constructor (Good practice)
        Array() : a(nullptr), length(0) {}

        // 2. Parameterized Constructor
        Array(int len) {
            assert(len >= 0);
            length = len;
            a = (length > 0) ? new T[length]() : nullptr; // () initializes primitives to 0/false
        }

        // 3. Destructor (Crucial to prevent memory leaks)
        ~Array() {
            delete[] a;
        }

        // 4. Copy Constructor (Deep Copy)
        Array(const Array<T> &other) {
            length = other.length;
            if (length > 0) {
                a = new T[length];
                for (int i = 0; i < length; ++i) {
                    a[i] = other.a[i];
                }
            } else {
                a = nullptr;
            }
        }

        // 5. Copy Assignment Operator (Deep Copy)
        Array<T>& operator=(const Array<T> &other) {
            if (this != &other) { // Guard against self-assignment
                delete[] a;       // Free existing resource

                length = other.length;
                if (length > 0) {
                    a = new T[length];
                    for (int i = 0; i < length; ++i) {
                        a[i] = other.a[i];
                    }
                } else {
                    a = nullptr;
                }
            }
            return *this;
        }

        // 6. Move Constructor (Resource Theft)
        Array(Array<T> &&other) noexcept {
            a = other.a;
            length = other.length;

            other.a = nullptr;
            other.length = 0;
        }

        // 7. Move Assignment Operator
        Array<T>& operator=(Array<T> &&other) noexcept {
            if (this != &other) {
                delete[] a; // Clean up current memory

                a = other.a; // Steal the resource
                length = other.length;

                other.a = nullptr; // Leave source in valid empty state
                other.length = 0;
            }
            return *this;
        }

        // 8. Subscript Operator (Non-const: for writing)
        T& operator[](int i) {
            assert(i >= 0 && i < length);
            return a[i];
        }

        // 9. Subscript Operator (Const: for reading const arrays)
        const T& operator[](int i) const {
            assert(i >= 0 && i < length);
            return a[i];
        }

        // 10. Getter for Length
        int getLength() const {
            return length;
        }
};

#endif