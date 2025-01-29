#pragma once

#include <clap/all.h>

#include <memory>
#include <mutex>
#include <string>
#include <type_traits>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginEntry {
   public:
      // not copyable, not moveable
      PluginEntry(const PluginEntry &) = delete;
      PluginEntry(PluginEntry &&) = delete;
      PluginEntry &operator=(const PluginEntry &) = delete;
      PluginEntry &operator=(PluginEntry &&) = delete;

      // function that produces the clap_plugin_entry to be exported
      // template T needs to be derived from PluginEntry and default-constructible
      template <typename T,
                typename = typename std::enable_if<std::is_base_of<PluginEntry, T>::value &&
                                                   std::is_default_constructible<T>::value>::type>
      static const clap_plugin_entry clapPluginEntry() noexcept;

   protected:
      PluginEntry() = default;
      virtual ~PluginEntry() = default;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      // clap_plugin_entry
      virtual bool init(const char *plugin_path) noexcept { return true; }
      virtual void deinit() noexcept {}
      virtual const void *getFactory(const char *factoryId) const noexcept { return nullptr; };
      virtual bool enableDraftFactories() const noexcept { return false; }
      // needed for adding custom factories to a class derived from PluginEntry
      template <typename T,
                typename = typename std::enable_if<std::is_base_of<PluginEntry, T>::value &&
                                                   std::is_default_constructible<T>::value>::type>
      static const T &getInstance() noexcept;

      // clap_plugin_factory
      virtual bool implementsPluginFactory() const noexcept { return false; }
      virtual uint32_t pluginFactoryGetPluginCount() const noexcept { return 0; }
      virtual const clap_plugin_descriptor_t *
      pluginFactoryGetPluginDescriptor(uint32_t index) const noexcept {
         return nullptr;
      }
      virtual const clap_plugin_t *pluginFactoryCreatePlugin(const clap_host_t &host,
                                                             const char *plugin_id) const noexcept {
         return nullptr;
      }
      // This method is meant for implementing contract checking, it isn't part of CLAP.
      // The default implementation will be slow, so consider overriding it with a faster one.
      // Returns -1 if the plugin isn't found.
      virtual int32_t getPluginIndexForPluginId(const char *pluginId) const noexcept;
      virtual bool isValidPluginId(const char *pluginId) const noexcept;
      virtual const clap_plugin_descriptor *
      getPluginDescriptorForPluginId(const char *pluginId) const noexcept;

   private:
      // custom deleter so the virtual destructor does not have to be public
      struct Deleter {
            void operator()(PluginEntry<h, l> *pluginEntry) const noexcept;
      };
      using InstancePtr = std::unique_ptr<PluginEntry<h, l>, Deleter>;
      static InstancePtr _instance;

      /////////////
      // Utility //
      /////////////

      static void hostMisbehaving(const char *msg) noexcept;
      static void hostMisbehaving(const std::string &msg) noexcept { hostMisbehaving(msg.c_str()); }
      static bool ensureInitialized(const char *method) noexcept;
      template <typename T>
      static bool
      ensureFactory(const T *factory, const T &staticFactory, const char *method) noexcept;

      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      // clap_plugin_entry
      template <typename T>
      static bool clapInit(const char *plugin_path) noexcept;
      static void clapDeinit() noexcept;
      static const void *clapGetFactory(const char *factory_id) noexcept;
      static std::mutex _initLock;
      static int _initCounter;

      // clap_plugin_factory
      static uint32_t clapPluginFactoryGetPluginCount(const clap_plugin_factory *factory) noexcept;
      static const clap_plugin_descriptor *
      clapPluginFactoryGetPluginDescriptor(const clap_plugin_factory *factory,
                                           uint32_t index) noexcept;
      static const clap_plugin_t *clapPluginFactoryCreatePlugin(const clap_plugin_factory *factory,
                                                                const clap_host *host,
                                                                const char *plugin_id);

      // interfaces
      static const clap_plugin_factory _pluginEntryPluginFactory;
   };
}} // namespace clap::helpers
