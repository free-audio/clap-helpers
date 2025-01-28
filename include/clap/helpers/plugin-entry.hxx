#include "plugin-entry.hh"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_factory PluginEntry<h, l>::_pluginEntryPluginFactory = {
      clapPluginFactoryGetPluginCount,
      clapPluginFactoryGetPluginDescriptor,
      clapPluginFactoryCreatePlugin,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T, typename ValidImpl>
   const clap_plugin_entry PluginEntry<h, l>::clapPluginEntry() noexcept {
      return clap_plugin_entry{
         CLAP_VERSION,
         clapInit<T>,
         clapDeinit,
         clapGetFactory,
      };
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T, typename ValidImpl>
   const T &PluginEntry<h, l>::getInstance() noexcept {
      assert(_instance &&
             "make sure to call this only between plugin_entry.init and plugin_entry.deinit");
      return *static_cast<const T *>(_instance.get());
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   int32_t PluginEntry<h, l>::getPluginIndexForPluginId(const char *pluginId) const noexcept {
      const auto count = pluginFactoryGetPluginCount();
      for (uint32_t i = 0; i < count; ++i) {
         auto desc = pluginFactoryGetPluginDescriptor(i);
         if (!std::strcmp(desc->id, pluginId))
            return static_cast<int32_t>(i);
      }
      return -1;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginEntry<h, l>::isValidPluginId(const char *pluginId) const noexcept {
      return getPluginIndexForPluginId(pluginId) != -1;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_descriptor *
   PluginEntry<h, l>::getPluginDescriptorForPluginId(const char *pluginId) const noexcept {
      const auto count = pluginFactoryGetPluginCount();
      for (uint32_t i = 0; i < count; ++i) {
         auto desc = pluginFactoryGetPluginDescriptor(i);
         if (!std::strcmp(desc->id, pluginId))
            return desc;
      }
      return nullptr;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginEntry<h, l>::deletePluginEntry(PluginEntry<h, l> *pluginEntry) noexcept {
      delete pluginEntry;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   typename PluginEntry<h, l>::InstancePtr PluginEntry<h, l>::_instance{nullptr, deletePluginEntry};

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginEntry<h, l>::hostMisbehaving(const char *msg) noexcept {
      std::cerr << msg << std::endl;
      if (h == MisbehaviourHandler::Terminate)
         std::terminate();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginEntry<h, l>::ensureInitialized(const char *method) noexcept {
      if (l >= CheckingLevel::Minimal && !_instance) {
         std::ostringstream msg;
         msg << "Host called the method " << method
             << " before plugin_entry.init or after plugin_entry.deinit";
         hostMisbehaving(msg.str());
         return false;
      }
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T>
   bool PluginEntry<h, l>::ensureFactory(const T *factory,
                                         const T &staticFactory,
                                         const char *method) noexcept {
      if (!ensureInitialized(method))
         return false;
      if (l >= CheckingLevel::Minimal && factory != &staticFactory) {
         std::ostringstream msg;
         msg << "Host called the method " << method << " with an invalid factory pointer";
         hostMisbehaving(msg.str());
         return false;
      }
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T>
   bool PluginEntry<h, l>::clapInit(const char *plugin_path) noexcept {
      if (l >= CheckingLevel::Minimal) {
         const auto method = "plugin_entry.init";
         if (!plugin_path || !plugin_path[0]) {
            std::ostringstream msg;
            msg << "Host called the method " << method << " with null or empty plugin_path";
            hostMisbehaving(msg.str());
            return false;
         }
      }
      std::lock_guard<std::mutex> guard(_initLock);
      const auto count = ++_initCounter;
      if (count > 1)
         return true;
      assert(!_instance && "the _initCounter mechanism should protect us from initializing twice");
      auto impl = new T();
      if (!impl->init(plugin_path)) {
         _initCounter = 0;
         return false;
      }
      _instance = InstancePtr(static_cast<PluginEntry *>(impl), deletePluginEntry);
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginEntry<h, l>::clapDeinit() noexcept {
      std::lock_guard<std::mutex> guard(_initLock);
      const auto count = --_initCounter;
      if (count == 0) {
         assert(_instance &&
                "the _initCounter mechanism should protect us from deinitializing twice");
         _instance->deinit();
         _instance.reset();
      }
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   std::mutex PluginEntry<h, l>::_initLock{};

   template <MisbehaviourHandler h, CheckingLevel l>
   int PluginEntry<h, l>::_initCounter{};

   template <MisbehaviourHandler h, CheckingLevel l>
   const void *PluginEntry<h, l>::clapGetFactory(const char *factory_id) noexcept {
      if (!ensureInitialized("plugin_entry.get_factory"))
         return nullptr;
      auto &self = *_instance;
      if (!std::strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) && self.implementsPluginFactory())
         return &_pluginEntryPluginFactory;
      if (self.enableDraftFactories()) {
         // put draft factories here
      }
      return self.getFactory(factory_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t
   PluginEntry<h, l>::clapPluginFactoryGetPluginCount(const clap_plugin_factory *factory) noexcept {
      if (!ensureFactory(factory, _pluginEntryPluginFactory, "plugin_factory.get_plugin_count"))
         return 0;
      auto &self = *_instance;
      return self.pluginFactoryGetPluginCount();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_descriptor *
   PluginEntry<h, l>::clapPluginFactoryGetPluginDescriptor(const clap_plugin_factory *factory,
                                                           uint32_t index) noexcept {
      const auto method = "plugin_factory.get_plugin_descriptor";
      if (!ensureFactory(factory, _pluginEntryPluginFactory, method))
         return nullptr;
      auto &self = *_instance;
      if (l >= CheckingLevel::Minimal) {
         auto count = self.pluginFactoryGetPluginCount();
         if (index >= count) {
            std::ostringstream msg;
            msg << "Host called " << method << " with an index out of bounds: " << index
                << " >= " << count;
            hostMisbehaving(msg.str());
            return nullptr;
         }
      }
      auto desc = self.pluginFactoryGetPluginDescriptor(index);
      assert(desc && "pluginFactoryGetPluginDescriptor must not return nullptr");
      assert(desc->id && desc->id[0] && desc->name && desc->name[0] &&
             "for the clap_plugin_descriptor returned by pluginFactoryGetPluginDescriptor, all "
             "mandatory fields must be set properly");
      assert(desc->vendor && desc->url && desc->manual_url && desc->support_url && desc->version &&
             desc->description && desc->features &&
             "for the clap_plugin_descriptor returned by pluginFactoryGetPluginDescriptor, all "
             "non-mandatory fields must be set non-zero (at least blank)");
      return desc;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_t *PluginEntry<h, l>::clapPluginFactoryCreatePlugin(
      const struct clap_plugin_factory *factory, const clap_host *host, const char *plugin_id) {
      const auto method = "plugin_factory.create_plugin";
      if (!ensureFactory(factory, _pluginEntryPluginFactory, method))
         return nullptr;
      if (l >= CheckingLevel::Minimal) {
         if (!host) {
            std::ostringstream msg;
            msg << "Host called " << method << " with a host nullptr";
            hostMisbehaving(msg.str());
            return nullptr;
         }
         if (!plugin_id) {
            std::ostringstream msg;
            msg << "Host called " << method << " with a plugin_id nullptr";
            hostMisbehaving(msg.str());
            return nullptr;
         }
         if (!clap_version_is_compatible(host->clap_version)) {
            std::ostringstream msg;
            msg << "Host called " << method << " with a host of an incompatible clap version";
            hostMisbehaving(msg.str());
            return nullptr;
         }
         if (!host->name || !host->version || !host->get_extension || !host->request_restart ||
             !host->request_process || !host->request_callback) {
            std::ostringstream msg;
            msg << "Host called " << method << " with a host not completely implemented";
            hostMisbehaving(msg.str());
            return nullptr;
         }
      }
      auto &self = *_instance;
      if (l >= CheckingLevel::Maximal) {
         if (!self.isValidPluginId(plugin_id)) {
            std::ostringstream msg;
            msg << "Host called " << method << " with an invalid plugin_id: " << plugin_id;
            hostMisbehaving(msg.str());
         }
      }
      auto plugin = self.pluginFactoryCreatePlugin(*host, plugin_id);
      assert(plugin && "pluginFactoryCreatePlugin must not return nullptr");
      assert(plugin->desc && plugin->init && plugin->destroy && plugin->activate &&
             plugin->deactivate && plugin->start_processing && plugin->stop_processing &&
             plugin->reset && plugin->process && plugin->get_extension && plugin->on_main_thread &&
             "pluginFactoryCreatePlugin must return a completely implemented clap_plugin");
      assert(plugin->desc == self.getPluginDescriptorForPluginId(plugin_id) &&
             "for the clap_plugin returned by pluginFactoryCreatePlugin, plugin->desc must point "
             "to the same plugin_descriptor as plugin_factory.get_plugin_descriptor would return");
      return plugin;
   }

}} // namespace clap::helpers
