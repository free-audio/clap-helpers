#pragma once

#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <vector>

namespace clap { namespace helpers {

   class ItemsHeap {
   public:
      ItemsHeap(uint32_t initialItemsCapacity = 32, uint32_t initialHeapCapacity = 32 * 48) {
         reserveItems(initialItemsCapacity);
         reserveHeap(initialHeapCapacity);
      }

      ItemsHeap(const ItemsHeap &) = delete;
      ItemsHeap(ItemsHeap &&) = delete;
      ItemsHeap &operator=(const ItemsHeap &) = delete;
      ItemsHeap &operator=(ItemsHeap &&) = delete;

      void reserveHeap(const size_t heapSize) {
         if (heapSize <= _size)
            return;

         auto *const ptr = static_cast<uint8_t *>(std::realloc(_heap, heapSize));
         if (!ptr)
            throw std::bad_alloc();

         _heap = ptr;
         _size = heapSize;
      }

      void reserveItems(size_t eventsCapacity) { _itemsOffset.reserve(eventsCapacity); }

      void clear() {
         _brk = 0;
         _itemsOffset.clear();
      }

      void *tryAllocate(uint32_t align, uint32_t size) {
         if (_itemsOffset.size() == _itemsOffset.capacity())
            return nullptr;

         assert(_brk <= _size);
         void *ptr = _heap + _brk;
         size_t space = _size - _brk;

         if (!std::align(align, size, ptr, space))
            return nullptr;

         auto offset = static_cast<uint8_t *>(ptr) - _heap;
         _itemsOffset.push_back(offset);
         _brk = offset + size;
         return ptr;
      }

      void *allocate(uint32_t align, uint32_t size) {
         if (_itemsOffset.size() == _itemsOffset.capacity())
            _itemsOffset.reserve(_itemsOffset.capacity() * 2);

         assert(_brk <= _size);
         if (size + _brk > _size)
            reserveHeap(_size * 2);

         auto ptr = tryAllocate(align, size);
         assert(ptr);
         return ptr;
      }

      void *get(uint32_t eventIndex) const {
         if (eventIndex >= _itemsOffset.size())
            throw std::out_of_range("out of range");

         auto offset = _itemsOffset[eventIndex];
         assert(offset <= _size);
         return static_cast<uint8_t *>(_heap) + offset;
      }

      size_t size() const { return _itemsOffset.size(); }

      bool empty() const { return _itemsOffset.empty(); }

   private:
      size_t _size = 0;
      size_t _brk = 0;
      uint8_t *_heap = nullptr;

      std::vector<uint32_t> _itemsOffset;
   };

}} // namespace clap::helpers