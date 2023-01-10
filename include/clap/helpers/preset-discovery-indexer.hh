#pragma once

#include <clap/clap.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {

   /// @brief C++ glue and checks
   template <MisbehaviourHandler h, CheckingLevel l>
   class PresetDiscoveryIndexer {
   public:
      const clap_preset_discovery_indexer *indexer() const noexcept { return &_indexer; }

   protected:
      PresetDiscoveryIndexer(const char *name,
                             const char *vendor,
                             const char *url,
                             const char *version);
      virtual ~PresetDiscoveryIndexer() = default;

      // not copyable, not moveable
      PresetDiscoveryIndexer(const PresetDiscoveryIndexer &) = delete;
      PresetDiscoveryIndexer(PresetDiscoveryIndexer &&) = delete;
      PresetDiscoveryIndexer &operator=(const PresetDiscoveryIndexer &) = delete;
      PresetDiscoveryIndexer &operator=(PresetDiscoveryIndexer &&) = delete;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      //--------------------------------//
      // clap_preset_discovery_provider //
      //--------------------------------//
      virtual bool declareFiletype(const clap_preset_discovery_filetype_t *filetype) noexcept {
         return true;
      }

      virtual bool declareLocation(const clap_preset_discovery_location_t *location) noexcept {
         return true;
      }

      virtual bool
      declareCollection(const clap_preset_discovery_collection_t *collection) noexcept {
         return true;
      }

      virtual bool setInvalidationWatchFile(const char *path) noexcept { return true; }

      virtual const void *extension(const char *id) noexcept { return nullptr; }

      ///////////////
      // Utilities //
      ///////////////
      static PresetDiscoveryIndexer &from(const clap_preset_discovery_indexer *indexer) noexcept;

   private:
      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      const clap_preset_discovery_indexer _indexer;

      // clap_plugin
      static bool indexerDeclareFiletype(const struct clap_preset_discovery_indexer *indexer,
                                         const clap_preset_discovery_filetype_t *filetype);

      static bool indexerDeclareLocation(const struct clap_preset_discovery_indexer *indexer,
                                         const clap_preset_discovery_location_t *location);

      static bool indexerDeclareCollection(const struct clap_preset_discovery_indexer *indexer,
                                           const clap_preset_discovery_collection_t *collection);

      static bool
      indexerSetInvalidationWatchFile(const struct clap_preset_discovery_indexer *indexer,
                                      const char *path);

      static const void *indexerGetExtension(const clap_preset_discovery_indexer *indexer,
                                             const char *id) noexcept;
   };
}} // namespace clap::helpers
