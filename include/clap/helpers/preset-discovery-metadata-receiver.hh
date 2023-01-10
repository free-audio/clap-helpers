#pragma once

#include <clap/clap.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {

   /// @brief C++ glue and checks
   template <MisbehaviourHandler h, CheckingLevel l>
   class PresetDiscoveryMetadataReceiver {
   public:
      const clap_preset_discovery_metadata_receiver *receiver() const noexcept {
         return &_receiver;
      }

   protected:
      PresetDiscoveryMetadataReceiver();
      virtual ~PresetDiscoveryMetadataReceiver() = default;

      // not copyable, not moveable
      PresetDiscoveryMetadataReceiver(const PresetDiscoveryMetadataReceiver &) = delete;
      PresetDiscoveryMetadataReceiver(PresetDiscoveryMetadataReceiver &&) = delete;
      PresetDiscoveryMetadataReceiver &operator=(const PresetDiscoveryMetadataReceiver &) = delete;
      PresetDiscoveryMetadataReceiver &operator=(PresetDiscoveryMetadataReceiver &&) = delete;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      //--------------------------------//
      // clap_preset_discovery_provider //
      //--------------------------------//
      virtual void onError(int32_t os_error, const char *error_message) noexcept {}

      virtual void markAsContainerFile() noexcept {}

      virtual void beginContainedPreset(const char *path, const char *load_uri) noexcept {}

      virtual void addPluginId(uint32_t plugin_abi, const char *plugin_id) noexcept {}

      virtual void setCollectionId(const char *collection_id) noexcept {}

      virtual void setFlags(uint32_t flags) noexcept {}

      virtual void setName(const char *name) noexcept {}

      virtual void addCreator(const char *creator) noexcept {}

      virtual void setDescription(const char *description) noexcept {}

      virtual void setTimestamps(uint64_t creation_time, uint64_t modification_time) noexcept {}

      virtual void addFeature(const char *feature) noexcept {}

      virtual void addExtraInfo(const char *key, const char *value) noexcept {}

      ///////////////
      // Utilities //
      ///////////////
      static PresetDiscoveryMetadataReceiver &
      from(const clap_preset_discovery_metadata_receiver *receiver) noexcept;

   private:
      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      const clap_preset_discovery_metadata_receiver _receiver;

      static void receiverOnError(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  int32_t os_error,
                                  const char *error_message) noexcept;

      static void receiverMarkAsContainerFile(
         const struct clap_preset_discovery_metadata_receiver *receiver) noexcept;

      static void
      receiverBeginContainedPreset(const struct clap_preset_discovery_metadata_receiver *receiver,
                                   const char *path,
                                   const char *load_uri) noexcept;

      static void
      receiverAddPluginId(const struct clap_preset_discovery_metadata_receiver *receiver,
                          uint32_t plugin_abi,
                          const char *plugin_id) noexcept;

      static void
      receiverSetCollectionId(const struct clap_preset_discovery_metadata_receiver *receiver,
                              const char *collection_id) noexcept;

      static void receiverSetFlags(const struct clap_preset_discovery_metadata_receiver *receiver,
                                   uint32_t flags) noexcept;

      static void receiverSetName(const struct clap_preset_discovery_metadata_receiver *receiver,
                                  const char *name) noexcept;

      static void receiverAddCreator(const struct clap_preset_discovery_metadata_receiver *receiver,
                                     const char *creator) noexcept;

      static void
      receiverSetDescription(const struct clap_preset_discovery_metadata_receiver *receiver,
                             const char *description) noexcept;

      static void
      receiverSetTimestamps(const struct clap_preset_discovery_metadata_receiver *receiver,
                            uint64_t creation_time,
                            uint64_t modification_time) noexcept;

      static void receiverAddFeature(const struct clap_preset_discovery_metadata_receiver *receiver,
                                     const char *feature) noexcept;

      static void
      receiverAddExtraInfo(const struct clap_preset_discovery_metadata_receiver *receiver,
                           const char *key,
                           const char *value) noexcept;
   };
}} // namespace clap::helpers
