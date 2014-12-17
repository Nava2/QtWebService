/*
 * Copyright 2014 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef QSTRUCTOBJBUILDER_H
#define QSTRUCTOBJBUILDER_H

#include <type_traits>

#include <QStack>
#include <QVariant>

#include <QSharedData>

class QStructObj {
    
public:
    
protected:
    
    
    
public:
    
    class Builder
    {

    public:
        
        static Builder create();
        
        
        ~Builder();
        
        
        Builder(const Builder& other);
        
        /**
        * Creates a copy of the current level, checking if they are the same at the current point. This is because of the 
        * heirarchy nature of this builder. Parent relationships are <strong>not</strong> maintained in the copy.
        * @param other Other builder
        */
        Builder& operator=(const Builder& other);
        
        /**
        * Compares the entire structure of the builder, not the current place in the structure. 
        * @param other builder
        * @return `true` if both are the same structure. 
        */
        bool operator==(const Builder& other) const;
        
        
        Builder &boolean(const QString &name, const bool value);
        Builder &boolean(const bool value);  
        
        Builder &string(const QString &name, const QString &value);
        Builder &string(const QString &value);
            
        /**
         * Adds a Number type to the builder, with `name`. This will fail to compile with anything that is not a "Number"
         * @param name Name of the Number
         * @param value Value of the number
         * @returns `this` reference.
         * @throws std::logic_error Thrown if name is null or empty.
         */
        template <class T>
        Builder &number(const QString &name, const T value) {
            static_assert(std::is_arithmetic<T>::value, "Must be arithmetic and integral");
            
            m_impl->mapInsert(name, QVariant::fromValue<T>(value));
            
            return *this;
        }
        
        /**
         * Adds a Number type to the builder, with `name`. This will fail to compile with anything that is not a "Number"
         * @param name Name of the Number
         * @param value Value of the number
         * @returns `this` reference.
         * @throws std::logic_error Thrown if name is null or empty.
         */
        template <class T>
        Builder &number(const T value) {
            static_assert(std::is_arithmetic<T>::value, "Must be arithmetic and integral");
            
            const QVariant vv(value);
            m_impl->arrInsert(vv);
            
            return *this;
        }
        
        /**
         * Creates a new QStructObjBuilder with `this` as parent as a child object of `this`. 
         * @param name Name of the array.
         */
        Builder &array(const QString &name); 
        
        /**
         * Creates a new QStructObjBuilder with `this` as parent as a child object of `this`. 
         * @param name Name of the array.
         */
        Builder &array(); 
        
        
        Builder &object(const QString &name);       
        Builder &object();
        
        Builder &object(const QString &name, const Builder &obj);
        Builder &object(const Builder &obj);
        
        /**
         * Close the current object scope, return the parent. 
         */
        Builder &close();
        
        template <class T>
        T build();
        
    private:
        
        class BaseData : public QSharedData {
        
        public:
            BaseData();
            
            BaseData(const BaseData &other);
            
            ~BaseData();
            
            void mapInsert(const QString &name, const QVariant value);
            
            void arrInsert(const QVariant &value);
            
            /**
             * Setup the current object to be a sub object with name.
             */
            void mapNewObj(const QString &name);
            
            /**
             * Setup the current object to be a sub array with name.
             */
            void mapNewArr(const QString &name);
            
            /**
             * Setup the current array to have a sub object
             */
            void arrNewObj();
            
            /**
             * Setup the current array to have a sub array.
             */
            void arrNewArr();
            
            void climbLevel();

            QVariant m_baseData;
            
            QVariant* m_level;
            
            QStack<QVariant *> m_parentLevel;
            
        };
        
        Builder();
        
        QSharedDataPointer<BaseData> m_impl;
    };
    
};



#endif // QSTRUCTOBJBUILDER_H
