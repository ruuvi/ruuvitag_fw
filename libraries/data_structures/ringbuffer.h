#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdlib.h>
#include <stdint.h>

// Based on https://jlmedina123.wordpress.com/2013/08/19/circular-buffer-queue/

typedef struct{
    size_t element_max;  //Max number of elements
    size_t element_size; //Element size in bytes
    size_t start;        //Index of buffer start
    //int end;  // position of last element
    /* Tracking start and end of buffer would waste
     * one position. A full buffer would always have
     * to leave last position empty or otherwise
     * it would look empty. Instead this buffer uses
     * count to track if buffer is empty or full
     */
    size_t count; // number of elements in buffer
    /* Two ways to make buffer element type opaque
     * First is by using typedef for the element
     * pointer. Second is by using void pointer.
     */
    /* different types of buffer: 
    int *element;   // array of integers
    char *element;  // array of characters 
    void *element;  // array of void type (could cast to int, char, etc)
    char **element; // array of char pointers (array of strings)
    void **element; // array of void pointers
    */
    void* element;

}ringbuffer_t;

//Allocate memory for buffer. TODO: Return malloc status?
void ringbuffer_init(ringbuffer_t* buffer, size_t  element_max, size_t element_size);

//Free resources of ringbuffer
void ringbuffer_uninit(ringbuffer_t *buffer);

// Return true if ringbuffer is initialized
int ringbuffer_is_init(ringbuffer_t *buffer);

//Return true if buffer is full, false otherwise
int ringbuffer_full(ringbuffer_t* buffer);

//Return true if buffer is empty, false otherwise
int ringbuffer_empty(ringbuffer_t* buffer);

// Add element to buffer
void ringbuffer_push(ringbuffer_t* buffer, void* data);

// FIFO pop
void ringbuffer_popqueue(ringbuffer_t* buffer, void* element);

// LIFO pop
void ringbuffer_popstack(ringbuffer_t* buffer, void* element);

// peek to element
void ringbuffer_peek_at(ringbuffer_t* buffer, size_t index, void* element);

//Get max_elements
size_t ringbuffer_get_size(ringbuffer_t* buffer);

//Get number of stored elements
size_t ringbuffer_get_count(ringbuffer_t* buffer);

//Copy values
void ringbuffer_copy_data(void* target, ringbuffer_t* source);

#endif
