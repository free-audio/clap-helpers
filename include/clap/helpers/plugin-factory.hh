#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <clap/factory/plugin-factory.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

/// @page PluginFactory
///
/// Usage example
/// @code
/// bool clap_entry_init_impl(const char *path) {
///    if (!clap::helpers::PluginFactory::init(path))
///       return false;
///    PluginFactory::registerPlugin(PluginTypeA::descriptor(),
///                                  [](const clap_host &host) -> const clap_plugin * {
///                                     auto plugin = new PluginTypeA(host);
///                                     return plugin->clapPlugin();
///                                  });
///    // example: only list PluginTypeB if someProperty can be successfully parsed:
///    if (auto someProperty = parsePropertyFromFileRelativeToPath(path))
///       PluginFactory::registerPlugin(PluginTypeB::descriptor(),
///                                     [](const clap_host &host) -> const clap_plugin * {
///                                        auto plugin = new PluginTypeB(host, *someProperty);
///                                        return plugin->clapPlugin();
///                                     });
///    // ...
///    return true;
/// }
///
/// const void *clap_entry_get_factory_impl(const char *factory_id) {
///    if (strcmp(factoryId, CLAP_PLUGIN_FACTORY_ID) == 0)
///       return PluginFactory::clapPluginFactory();
///    // ...
/// }
///
/// void clap_entry_deinit_impl() {
///    PluginFactory::deinit();
///    // ...
/// }
/// @endcode

namespace clap { namespace helpers {

   /// @brief C++ glue and checks
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginFactory {
   public:
      // not copyable, not moveable
      PluginFactory(const PluginFactory &) = delete;
      PluginFactory(PluginFactory &&) = delete;
      PluginFactory &operator=(const PluginFactory &) = delete;
      PluginFactory &operator=(PluginFactory &&) = delete;

      // Call this inside clap_entry->init
      static bool init() noexcept;
      // Call this inside clap_entry->deinit
      static void deinit() noexcept;
      using PluginCreateCallback = std::function<const clap_plugin *(const clap_host &host)>;
      // Call this inside clap_entry->init
      // It has to be called after PluginFactory::init
      static void registerPlugin(const clap_plugin_descriptor &descriptor,
                                 PluginCreateCallback &&pluginCreateCallback) noexcept;

      static const clap_plugin_factory *clapPluginFactory() noexcept;

   private:
      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      // clap_plugin_factory
      static uint32_t clapGetPluginCount(const clap_plugin_factory *factory) noexcept;
      static const clap_plugin_descriptor *
      clapGetPluginDescriptor(const clap_plugin_factory *factory, uint32_t index) noexcept;
      static const clap_plugin *clapCreatePlugin(const clap_plugin_factory *factory,
                                                 const clap_host *host,
                                                 const char *pluginId) noexcept;

      /////////////
      // Utility //
      /////////////

      static void hostMisbehaving(const char *msg) noexcept;
      static void hostMisbehaving(const std::string &msg) noexcept { hostMisbehaving(msg.c_str()); }
      static bool ensurePluginFactory(const char *method,
                                      const struct clap_plugin_factory *factory) noexcept;

      //////////////
      // Instance //
      //////////////

      PluginFactory() = default;
      uint32_t getPluginCount() const noexcept;
      const clap_plugin_descriptor *getPluginDescriptor(uint32_t index) const noexcept;
      const clap_plugin *createPlugin(const clap_host *host, const char *pluginId) const noexcept;

      static std::unique_ptr<PluginFactory> _instance;
      static const clap_plugin_factory_t _pluginFactory;

      struct PluginEntry {
         const clap_plugin_descriptor &_descriptor;
         const PluginCreateCallback _createCallback;
      };
      std::vector<PluginEntry> _plugins;
   };
}} // namespace clap::helpers
