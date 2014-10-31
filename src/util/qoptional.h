#pragma once

#include <QSharedPointer>

///**
// * Simple class to return a pointer value or None if failed. The type is
// * immutable, but the underlying pointer's data is mutable if accessed so.
// */
//template <typename T>
//class QOptional
//{

//public:

//    //!< Shared pointer type returned
//    typedef QSharedPointer<T> Pointer;

//private:

//    template <typename U>
//    class None : public QOptional<U> {

//        None() { }
//        ~None() { }

//        inline
//        bool isPresent() const {
//            return false;
//        }

//        inline
//        U *value() const {
//            return nullptr;
//        }
//    };

//    template <typename U>
//    class Some : public QOptional<U> {

//        Some(U * const value)
//            : m_value(value) {
//        }

//        Some(const Some &other)
//            : m_value(other.m_value) {

//        }

//        ~Some() { }

//        inline
//        bool isPresent() const {
//            return true;
//        }

//        inline
//        Pointer value() const {
//            return m_value;
//        }

//    private:
//        Pointer const m_value;
//    };

//public:

//    template <typename U>
//    static
//    QOptional<U> none() {
//        return None<U>();
//    }

//    template <typename U>
//    static
//    QOptional<U> some(U * const ptr) {
//        return Some<U>(ptr);
//    }

//    virtual
//    bool isPresent() const = 0;
<<<<<<< HEAD

//    virtual
//    const Pointer value() const = 0;

//    virtual
//    Pointer value() = 0;

//    inline
//    T &operator *() const {
//        return *(value());
//    }


//};


=======

//    virtual
//    const Pointer value() const = 0;

//    virtual
//    Pointer value() = 0;

//    inline
//    T &operator *() const {
//        return *(value());
//    }


//};
>>>>>>> 838be6b... Fixes most compiling issues, adds qt-maybe, might remove it lator (it's not as useful as I thought..). Need to reimplement proper normal routing, regex should work.
