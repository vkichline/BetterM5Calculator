#pragma once
#include "CoreCalculator.h"

#define CLEAR_OPERATOR   (uint8_t('A'))

// This template wraps CoreCalculator and provides memories of type T
// There is a simple memory: memory
// There is an array of memories: memories
// And there is a memory stack: memory_stack
//
template <typename T, uint8_t M>
class MemoryCalculator : public CoreCalculator<T> {
  public:
    MemoryCalculator();
    bool            set_memory(T value);                        // Set the simple memory
    T               get_memory();                               // Get the simple memory
    bool            set_memory(uint8_t index, T value);         // Set one of the indexed memories
    T               get_memory(uint8_t index);                  // Get one of the indexed memories
    void            push_memory(T value);                       // Push a value onto the memory stack
    T               pop_memory();                               // Pop a value from the stack and return it
    T               peek_memory();                              // Return the value at the top of the memory stack idempotently
    bool            memory_operation(Op_ID id);                 // Operation between Val and M -> M
    bool            memory_operation(Op_ID id, uint8_t index);  // Operation between Val and M[index] -> M[index]
    uint16_t        get_memory_depth();                         // Get the number of items on the memory stack
    void            clear_memory_stack();                       // Clear the memory stack
    void            clear_all_memory();                         // Clear simple, indexed and stack memory
    uint8_t         get_mem_array_size();                       // The value of M
protected:
    T               memory;                                     // The simplest to access memory
    T               memories[M];                                // The array of indexed memory
    std::vector<T>  memory_stack;                               // A memory stack. It would be nice if <stack> compiled.
};


template <typename T, uint8_t M>MemoryCalculator<T, M>::MemoryCalculator() : CoreCalculator<T>() {
  clear_all_memory();
}

template <typename T, uint8_t M> bool MemoryCalculator<T, M>::set_memory(T value) {
  memory = value;
  return true;
}

template <typename T, uint8_t M> T MemoryCalculator<T, M>::get_memory() {
  return memory;
}

template <typename T, uint8_t M> bool MemoryCalculator<T, M>::set_memory(uint8_t index, T value) {
  if(M <= index) return false;  // Out of range
  memories[index] = value;
  return true;
}

template <typename T, uint8_t M> T MemoryCalculator<T, M>::get_memory(uint8_t index) {
  if(M <= index) return T(0);  // Out of range
  return memories[index];
}

template <typename T, uint8_t M> void MemoryCalculator<T, M>::push_memory(T value) {
  memory_stack.push_back(value);
}

template <typename T, uint8_t M> T MemoryCalculator<T, M>::pop_memory() {
  T value = memory_stack.back();
  memory_stack.pop_back();
  return value;
}

template <typename T, uint8_t M> T MemoryCalculator<T, M>::peek_memory() {
  return memory_stack.back();
}

template <typename T, uint8_t M> uint16_t MemoryCalculator<T, M>::get_memory_depth() {
  return memory_stack.size();
}

template <typename T, uint8_t M> void MemoryCalculator<T, M>::clear_memory_stack() {
  memory_stack.clear();
}

template <typename T, uint8_t M> void MemoryCalculator<T, M>::clear_all_memory() {
  memory = T(0);
  for(uint8_t i = 0; i < M; i++) memories[i] = T(0);
  clear_memory_stack();
}

template <typename T, uint8_t M> uint8_t MemoryCalculator<T, M>::get_mem_array_size() {
  return M;
}

// Operation between Val and M -> M
//  =   recall
//  +   M = M + Value
//
template <typename T, uint8_t M> bool  MemoryCalculator<T, M>::memory_operation(Op_ID id) {
  switch(id) {
    case EVALUATE_OPERATOR:
      // M= means store Value in M
      return set_memory(CoreCalculator<T>::get_value());
    case CLEAR_OPERATOR:
      // MA means clear M
      return set_memory(0.0);
    case ADDITION_OPERATOR:
      return set_memory(get_memory() + CoreCalculator<T>::get_value());
    case SUBTRACTION_OPERATOR:
      return set_memory(get_memory() - CoreCalculator<T>::get_value());
    case MULTIPLICATION_OPERATOR:
      return set_memory(get_memory() * CoreCalculator<T>::get_value());
    case DIVISION_OPERATOR:
      return set_memory(get_memory() / CoreCalculator<T>::get_value());
    case PRECENT_OPERATOR:
      // TBD BUGBUG
      // return set_memory(get_memory() % CoreCalculator<T>::get_value());
    default: Serial.printf("Error in memory_operation(%c): %c unknown\n", id, id);
             return false;
  }
  return false;
}

// Operation between Val and M[index] -> M[index]
//
template <typename T, uint8_t M> bool  MemoryCalculator<T, M>::memory_operation(Op_ID id, uint8_t index) {
  switch(id) {
    case EVALUATE_OPERATOR:
      // M= means store Value in M
      return set_memory(index, CoreCalculator<T>::get_value());
    case CLEAR_OPERATOR:
      // MA means clear M
      return set_memory(index, 0.0);
    case ADDITION_OPERATOR:
      return set_memory(index, get_memory() + CoreCalculator<T>::get_value());
    case SUBTRACTION_OPERATOR:
      return set_memory(index, get_memory() - CoreCalculator<T>::get_value());
    case MULTIPLICATION_OPERATOR:
      return set_memory(index, get_memory() * CoreCalculator<T>::get_value());
    case DIVISION_OPERATOR:
      return set_memory(index, get_memory() / CoreCalculator<T>::get_value());
    case PRECENT_OPERATOR:
      // TBD BUGBUG
      // return set_memory(index, get_memory() % CoreCalculator<T>::get_value());
    default: Serial.printf("Error in memory_operation(%c): %c unknown\n", id, id);
             return false;
  }
  return false;
}
