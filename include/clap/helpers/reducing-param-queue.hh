#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <unordered_map>

namespace clap { namespace helpers {
   template <typename K, typename V>
   class ReducingParamQueue {
   public:
      using key_type = K;
      using value_type = V;
      using queue_type = std::unordered_map<key_type, value_type>;

      ReducingParamQueue();

      void setCapacity(size_t capacity);

      void set(const key_type &key, const value_type &value);
      void producerDone();

      void consume(const std::function<void(const key_type &key, const value_type &value)> consumer);

      void reset();

   private:
      queue_type _queues[2];
      std::atomic<queue_type *> _free = nullptr;
      std::atomic<queue_type *> _producer = nullptr;
      std::atomic<queue_type *> _consumer = nullptr;
   };
}} // namespace clap::helpers