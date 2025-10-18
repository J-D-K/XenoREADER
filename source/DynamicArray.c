#include "DynamicArray.h"

#include <malloc.h>

// clang-format off
struct DynamicArray
{
    /// @brief This is the internal array.
    void **array;

    /// @brief This is the size of the elements in the array.
    size_t elementSize;

    /// @brief Current length of the array.
    size_t length;

    /// @brief Function used to free the elements.
    ElementFreeFunction freeFunction;
};
// clang-format on

DynamicArray *dynamic_array_create(size_t elementSize, ElementFreeFunction freeFunction)
{
    DynamicArray *dynamicArray = malloc(sizeof(DynamicArray));
    if (!dynamicArray) { return NULL; }

    dynamicArray->array        = NULL;
    dynamicArray->elementSize  = elementSize;
    dynamicArray->length       = 0;
    dynamicArray->freeFunction = freeFunction;

    return dynamicArray;
}

// To do: Revise this.
void dynamic_array_free(DynamicArray *array)
{
    if (!array || !array->array || array->length <= 0) { return; }

    for (size_t i = 0; i < array->length; i++)
    {
        void *pointer = array->array[i];
        if (!pointer) { continue; }

        if (array->freeFunction) { (array->freeFunction)(pointer); }
        else { free(pointer); }
    }

    free(array->array);
    free(array);
}

void *dynamic_array_new(DynamicArray *array)
{
    if (!array) { return NULL; }

    // Reallocate array to fit new element.
    array->array = realloc(array->array, sizeof(void *) * ++array->length);

    // Allocate new element.
    array->array[array->length - 1] = malloc(array->elementSize);

    // Return the element at the "back"
    return array->array[array->length - 1];
}

void *dynamic_array_get_element_at(const DynamicArray *array, int index)
{
    if (!array || !array->array || index < 0 || index >= (int)array->length) { return NULL; }

    return array->array[index];
}

size_t dynamic_array_get_length(const DynamicArray *array) { return array->length; }