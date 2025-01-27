#pragma once

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "macros.hh"
#include "plugin-factory.hh"

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_factory_t PluginFactory<h, l>::_pluginFactory = {
      clapGetPluginCount,
      clapGetPluginDescriptor,
      clapCreatePlugin,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginFactory<h, l>::init() noexcept {
      if (l >= CheckingLevel::Minimal) {
         if (_instance)
            CLAP_HELPERS_UNLIKELY {
               hostMisbehaving("tried to initialize plugin factory twice");
               return false;
            }
      }
      _instance = std::unique_ptr<PluginFactory>(new PluginFactory());
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginFactory<h, l>::deinit() noexcept {
      if (l >= CheckingLevel::Minimal && !_instance)
         CLAP_HELPERS_UNLIKELY {
            hostMisbehaving(
               "tried to deinitialize plugin factory twice or without prior initialization");
            return;
         }
      _instance.reset();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginFactory<h, l>::registerPlugin(const clap_plugin_descriptor &descriptor,
                                            PluginCreateCallback &&pluginCreateCallback) noexcept {
      assert(_instance);
      _instance->_plugins.push_back({descriptor, std::move(pluginCreateCallback)});
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_factory *PluginFactory<h, l>::clapPluginFactory() noexcept {
      if (l >= CheckingLevel::Minimal && !_instance)
         CLAP_HELPERS_UNLIKELY {
            hostMisbehaving(
               "called entry->get_factory(CLAP_PLUGIN_FACTORY_ID) before entry->init or "
               "after entry->deinit");
            return nullptr;
         }
      return &_pluginFactory;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginFactory<h, l>::hostMisbehaving(const char *msg) noexcept {
      std::cerr << msg << std::endl;
      if (h == MisbehaviourHandler::Terminate)
         std::terminate();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool
   PluginFactory<h, l>::ensurePluginFactory(const char *method,
                                            const struct clap_plugin_factory *factory) noexcept {
      if (l == CheckingLevel::None)
         return true;
      if (factory != &_pluginFactory)
         CLAP_HELPERS_UNLIKELY {
            std::ostringstream msg;
            msg << "called plugin_factory->" << method << " with incorrect factory pointer";
            hostMisbehaving(msg.str());
            return false;
         }
      if (!_instance)
         CLAP_HELPERS_UNLIKELY {
            std::ostringstream msg;
            msg << "called plugin_factory->" << method
                << " before entry->init or after entry->deinit";
            hostMisbehaving(msg.str());
            return false;
         }
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t
   PluginFactory<h, l>::clapGetPluginCount(const struct clap_plugin_factory *factory) noexcept {
      if (!ensurePluginFactory("get_plugin_count", factory))
         return 0;
      return _instance->getPluginCount();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_descriptor_t *
   PluginFactory<h, l>::clapGetPluginDescriptor(const struct clap_plugin_factory *factory,
                                                uint32_t index) noexcept {
      if (!ensurePluginFactory("get_plugin_descriptor", factory))
         return nullptr;
      return _instance->getPluginDescriptor(index);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_t *
   PluginFactory<h, l>::clapCreatePlugin(const struct clap_plugin_factory *factory,
                                         const clap_host_t *host,
                                         const char *plugin_id) noexcept {
      if (!ensurePluginFactory("create_plugin", factory))
         return nullptr;
      return _instance->createPlugin(host, plugin_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t PluginFactory<h, l>::PluginFactory::getPluginCount() const noexcept {
      return _plugins.size();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_descriptor_t *
   PluginFactory<h, l>::getPluginDescriptor(uint32_t index) const noexcept {
      if (l >= CheckingLevel::Minimal && index >= getPluginCount())
         CLAP_HELPERS_UNLIKELY {
            hostMisbehaving("called plugin_factory->get_plugin_descriptor with too large index");
            return nullptr;
         }
      return &_plugins[index]._descriptor;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_t *PluginFactory<h, l>::createPlugin(const clap_host_t *host,
                                                          const char *pluginId) const noexcept {

      if (l >= CheckingLevel::Minimal) {
         if (!host)
            CLAP_HELPERS_UNLIKELY {
               hostMisbehaving("passed a host nullptr to plugin_factory->create_plugin");
               return nullptr;
            }
         if (!clap_version_is_compatible(host->clap_version))
            CLAP_HELPERS_UNLIKELY {
               hostMisbehaving(
                  "called plugin_factory->create_plugin for a host with incompatible CLAP version");
               return nullptr;
            }
      }
      auto it = std::find_if(_plugins.begin(), _plugins.end(), [pluginId](const auto &pluginEntry) {
         return strcmp(pluginId, pluginEntry._descriptor.id) == 0;
      });
      if (l >= CheckingLevel::Minimal && it == _plugins.end())
         CLAP_HELPERS_UNLIKELY {
            hostMisbehaving("called plugin_factory->create_plugin with invalid plugin id");
            return nullptr;
         }
      auto plugin = it->_createCallback(*host);
      assert(
         plugin && plugin->desc /*&& plugin->plugin_data*/ && plugin->init && plugin->destroy &&
         plugin->activate && plugin->deactivate && plugin->start_processing &&
         plugin->stop_processing && plugin->reset && plugin->process && plugin->get_extension &&
         plugin->on_main_thread); // the host relies on the callback providing a proper CLAP plugin
      assert(&it->_descriptor ==
             plugin->desc); // use the same descriptor for registerPlugin as for the plugin itself
      return plugin;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   std::unique_ptr<PluginFactory<h, l>> PluginFactory<h, l>::_instance{};
}} // namespace clap::helpers
