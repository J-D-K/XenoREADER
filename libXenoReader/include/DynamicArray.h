/*
 *      This file is part of the XenoReader library
 *      Copyright (c) 2025 JK
 *
 *      Licensed under the MIT License.
 *      See the included LICENSE file for license and attribution details.
 */
#pragma once
#include <stddef.h>

/// @brief is an extremely simple, probably inefficient DynamicArray in C to help simply the filesystem code.
typedef struct DynamicArray DynamicArray;

/// @brief Allocates a new dynamic array.
/// @param elementSize Size of the elements. Ex: sizeof(ElementStruct);
/// @param freeFunction Function to be used to free the elements. Pass NULL for basic free().
/// @return Pointer to new dynamic array.
DynamicArray *DynamicArray_Create(size_t elementSize, size_t initialCapacity);

/// @brief Frees the array and all elements it contains.
/// @param array Array to free.
void DynamicArray_Free(DynamicArray *array);

/// @brief Creates a new element in the Dynamic array and returns the pointer.
/// @param array Array to create a new element in.
/// @return Pointer to new element.
void *DynamicArray_New(DynamicArray *array);

/// @brief Returns the element at index.
/// @param array Array to get element from.
/// @param index Index of the element to get.
/// @return Pointer to element on success. NULL on failure.
void *DynamicArray_GetElementAt(const DynamicArray *array, int index);

/// @brief Returns the current length of the array passed.
/// @param array Array to return the length of.
size_t DynamicArray_GetLength(const DynamicArray *array);