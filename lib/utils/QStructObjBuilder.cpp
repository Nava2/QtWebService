/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
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

#include "utils/QStructObjBuilder.h"

#include <QDebug>
#include <QVariant>
#include <QVariantMap>

static const int DEFAULT_PARENT_STACK_SIZE = 10;

///////////////////////////////////////////////////////////////////////////////
// BaseData inner-class:
///////////////////////////////////////////////////////////////////////////////

QStructObj::Builder::BaseData::BaseData()
    : m_baseData(QVariantMap()),
      m_level(&m_baseData),
      m_parentLevel()
{
    m_parentLevel.reserve(DEFAULT_PARENT_STACK_SIZE);
}

QStructObj::Builder::BaseData::BaseData(const QStructObj::Builder::BaseData& other)
    : m_baseData(other.m_baseData),
      m_level(&m_baseData),
      m_parentLevel(other.m_parentLevel)
{
    m_parentLevel.reserve(DEFAULT_PARENT_STACK_SIZE);
}

QStructObj::Builder::BaseData::~BaseData() {

}

void QStructObj::Builder::BaseData::mapInsert(const QString& name, const QVariant value)
{
    if (name.isNull() || name.isEmpty()) {
        throw std::logic_error("name must not be empty or null");
    }
    
    if (m_level->type() != QVariant::Map) {
        throw std::logic_error("Tried to insert key-value pair into non-map type {key=" + name.toStdString() + 
                ", value=" + value.toString().toStdString() + "}");
    }
    
    m_level->toMap().insert(name, value);
}

void QStructObj::Builder::BaseData::arrInsert(const QVariant& value)
{
    
    if (m_level->type() != QVariant::List) {
        throw std::logic_error("Tried to insert non-named value into map type {value=" + 
                value.toString().toStdString() + "}");
    }
    
    m_level->toList().append(value);
}

void QStructObj::Builder::BaseData::mapNewObj(const QString& name)
{
    QVariantMap obj;
    mapInsert(name, obj);
    
    QVariant &ref = m_level->toMap()[name];
    
    m_parentLevel.push(m_level);
    m_level = &ref;   
}

void QStructObj::Builder::BaseData::mapNewArr(const QString& name)
{
    QVariantList arr;
    mapInsert(name, arr);
    
    QVariant &ref = m_level->toMap()[name];
    
    m_parentLevel.push(m_level);
    m_level = &ref;  
}

void QStructObj::Builder::BaseData::arrNewArr()
{
    QVariantList arr;
    arrInsert(arr);
    
    QVariant &ref = m_level->toList().last();
    
    m_parentLevel.push(m_level);
    m_level = &ref; 
}

void QStructObj::Builder::BaseData::arrNewObj()
{
    QVariantMap obj;
    arrInsert(obj);
    
    QVariant &ref = m_level->toList().last();
    
    m_parentLevel.push(m_level);
    m_level = &ref; 
}

void QStructObj::Builder::BaseData::climbLevel()
{
    if (!m_parentLevel.isEmpty()) {
        // it was a sub-child:
        m_level = m_parentLevel.pop();
        
    } else {
        // called extra times?
        qWarning() << "Close called on top-level builder.";
        
    }
}

///////////////////////////////////////////////////////////////////////////////
// Builder inner-class:
///////////////////////////////////////////////////////////////////////////////

QStructObj::Builder QStructObj::Builder::create() {
    
    return Builder();
}


QStructObj::Builder::Builder()
    : m_impl(new BaseData) {

}

QStructObj::Builder::Builder(const QStructObj::Builder& other) 
    : m_impl(other.m_impl) {
        
}

QStructObj::Builder::~Builder()
{
    // no destruction needed
}

QStructObj::Builder& QStructObj::Builder::operator=(const QStructObj::Builder& other)
{
    m_impl = QSharedDataPointer<BaseData>(new BaseData());
    m_impl->m_baseData = *(other.m_impl->m_level);
    m_impl->m_level = &m_impl->m_baseData;
    
    return *this;
}

bool QStructObj::Builder::operator==(const QStructObj::Builder& other) const
{
    return (this == &other || *m_impl->m_level == *other.m_impl->m_level);
}



QStructObj::Builder& QStructObj::Builder::boolean(const QString& name, const bool value)
{
    m_impl->mapInsert(name, value);
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::boolean(const bool value)
{
    m_impl->arrInsert(value);
    
    return *this;
}


QStructObj::Builder& QStructObj::Builder::string(const QString& name, const QString& value)
{
    m_impl->mapInsert(name, value);
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::string(const QString& value)
{
    m_impl->arrInsert(value);
    
    return *this;
}


QStructObj::Builder& QStructObj::Builder::object(const QString& name)
{
    m_impl->mapNewObj(name);
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::object()
{
    m_impl->arrNewObj();
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::object(const QStructObj::Builder& obj)
{
    m_impl->arrNewObj();
    
    m_impl->m_level->setValue(*(obj.m_impl->m_level));
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::object(const QString& name, const QStructObj::Builder& obj)
{
    m_impl->mapNewObj(name);
    
    m_impl->m_level->setValue(*(obj.m_impl->m_level));
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::array()
{
    m_impl->arrNewArr();
    
    return *this;
}

QStructObj::Builder& QStructObj::Builder::array(const QString& name)
{
    m_impl->mapNewArr(name);
    
    return *this;
}


QStructObj::Builder& QStructObj::Builder::close()
{
    m_impl->climbLevel();
    
    return *this;
}



