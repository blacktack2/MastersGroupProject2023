/**
 * @file   BufferObjectBase.cpp
 * @brief  See BufferObjectBase.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "BufferObjectBase.h"

using namespace NCL::Rendering;

BufferObjectBase::BufferObjectBase(size_t size, unsigned int binding) : size(size), binding(binding) {
}

BufferObjectBase::~BufferObjectBase() {
}
