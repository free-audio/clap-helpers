#pragma once

#include <cstring>

#include <clap/events.h>

#include "items-heap.hh"

namespace clap { namespace helpers {

   class EventList {
   public:
      EventList(uint32_t maxEventSize = 1024) : _maxEventSize(maxEventSize) {}

      EventList(const EventList &) = delete;
      EventList(EventList &&) = delete;
      EventList &operator=(const EventList &) = delete;
      EventList &operator=(EventList &&) = delete;

      void reserveEvents(size_t capacity) { _heap.reserveItems(capacity); }

      void reserveHeap(size_t size) { _heap.reserveHeap(size); }

      template <typename T>
      T *allocate() {
         return _heap.allocate(CLAP_ALIGN, sizeof(T));
      }

      void push(const clap_event_header *h) {
         if (h->size > _maxEventSize)
            return;

         auto ptr = _heap.allocate(CLAP_ALIGN, h->size);
         std::memcpy(ptr, h, h->size);
      }

      bool tryPush(const clap_event_header *h) {
         if (h->size > _maxEventSize)
            return false;

         auto ptr = _heap.tryAllocate(CLAP_ALIGN, h->size);
         if (!ptr)
            return false;

         std::memcpy(ptr, h, h->size);
         return true;
      }

      const clap_event_header *get(uint32_t index) const {
         return static_cast<const clap_event_header *>(_heap.get(index));
      }

      size_t size() const { return _heap.size(); }

      bool empty() const { return _heap.empty(); }

      void clear() { _heap.clear(); }

      const clap_input_events *clapInputEvents() const noexcept { return &_inputEvents; }

      const clap_output_events *clapOutputEvents() const noexcept { return &_outputEvents; }

      const clap_output_events *clapBoundedOutputEvents() const noexcept {
         return &_boundedOutputEvents;
      }

   private:
      static uint32_t clapSize(const struct clap_input_events *list) {
         auto *self = static_cast<const EventList *>(list->ctx);
         return self->size();
      }

      static const clap_event_header_t *clapGet(const struct clap_input_events *list,
                                                uint32_t index) {
         auto *self = static_cast<const EventList *>(list->ctx);
         return self->get(index);
      }

      static void clapPushBack(const struct clap_output_events *list,
                               const clap_event_header *event) {
         auto *self = static_cast<EventList *>(list->ctx);
         self->push(event);
      }

      static void clapBoundedPushBack(const struct clap_output_events *list,
                                      const clap_event_header_t *event) {
         auto *self = static_cast<EventList *>(list->ctx);
         self->tryPush(event);
      }

      const clap_input_events _inputEvents = {this, &clapSize, &clapGet};
      const clap_output_events _outputEvents = {this, &clapPushBack};
      const clap_output_events _boundedOutputEvents = {this, &clapBoundedPushBack};

      const uint32_t _maxEventSize;

      ItemsHeap _heap;
   };

}} // namespace clap::helpers