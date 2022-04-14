#include <cstddef>
#include <cstdint>
#include <cassert>
#include <unordered_set>

#include <clap/clap.h>

namespace clap { namespace helpers {
   class NoteEndQueue {
   public:
      explicit NoteEndQueue(std::size_t initialBucketSize = 19)
         : _voicesToKill(initialBucketSize) {}

      void onNoteOn(int16_t port, int16_t channel, int16_t key) {
         assert(checkValidEntry(port, channel, key));
         _voicesToKill.erase(Entry{port, channel, key});
      }

      void onNoteEnd(int16_t port, int16_t channel, int16_t key) {
         assert(checkValidEntry(port, channel, key));
         _voicesToKill.insert(Entry{port, channel, key});
      }

      void flush(clap_output_events *out) {
         clap_event_note ev;
         ev.velocity = 0;
         ev.header.flags = 0;
         ev.header.size = sizeof(ev);
         ev.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
         ev.header.time = 0; // time is irrelevant here
         ev.header.type = CLAP_EVENT_NOTE_END;

         for (auto &e : _voicesToKill) {
            ev.port_index = e.port;
            ev.channel = e.channel;
            ev.key = e.key;
            out->try_push(out, &ev.header);
         }

         _voicesToKill.clear();
      }

   private:
      bool checkValidEntry()
      {
         assert(port >= 0);
         assert(channel >= 0 && channel < 16);
         assert(key >= 0 && key < 127);
         return true;
      }

      struct Entry {
         int16_t port;
         int16_t channel;
         int16_t key;
      };

      struct EntryHash {
         std::size_t operator()(const Entry &e) noexcept {
            return e.key ^ (e.channel << 8LL) ^ (e.port << 16LL);
         }
      };

      std::unordered_set<Entry, EntryHash> _voicesToKill;
   };
}} // namespace clap::helpers
