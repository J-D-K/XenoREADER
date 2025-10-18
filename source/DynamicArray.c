#include "DynamicArray.h"

#include <stdio.h>
#include <stdlib.h>

// This is the capacity extended with when the array is getting full.
#define EXTEND_CAPACITY 64

// clang-format off
struct DynamicArray
{
    /// @brief This is the internal array.
    unsigned char *array;

    /// @brief This is the size of the elements in the array.
    size_t elementSize;

    /// @brief Current length of the array.
    size_t length;

    /// @brief This is the actual capacity.
    size_t capacity;
};
// clang-format on

DynamicArray *DynamicArray_Create(size_t elementSize, size_t initialCapacity)
{
    DynamicArray *dynamicArray = malloc(sizeof(DynamicArray));
    if (!dynamicArray) { return NULL; }

    dynamicArray->array       = malloc(elementSize * initialCapacity);
    dynamicArray->elementSize = elementSize;
    dynamicArray->length      = 0;
    dynamicArray->capacity    = initialCapacity;

    return dynamicArray;
}

// To do: Revise this.
void DynamicArray_Free(DynamicArray *array)
{
    if (!array) { return; }

    if (array->array) { free(array->array); }
    free(array);
}

void *DynamicArray_New(DynamicArray *array)
{
    if (!array) { return NULL; }

    if (array->capacity - array->length <= 2)
    {
        array->capacity += EXTEND_CAPACITY;
        void *newArray = realloc(array->array, array->elementSize * array->capacity);
        if (!newArray) { return NULL; }

        array->array = newArray;
    }

    return &array->array[array->length++ * array->elementSize];
}

void *DynamicArray_GetElementAt(const DynamicArray *array, int index)
{
    if (!array || !array->array || index < 0 || index >= (int)array->length) { return NULL; }

    return &array->array[index * array->elementSize];
}

size_t DynamicArray_GetLength(const DynamicArray *array) { return array->length; }
