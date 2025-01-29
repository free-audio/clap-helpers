#include "clap/helpers/host.hxx"
#include "clap/helpers/plugin-entry.hxx"
#include "clap/helpers/plugin.hxx"

#include <catch2/catch_all.hpp>

namespace {
   using test_entry = clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                                                 clap::helpers::CheckingLevel::Maximal>;

   struct test_entry_default_impl : public test_entry {};

   CATCH_TEST_CASE("Use PluginEntry default implementation") {
      auto clapPluginEntry = test_entry::clapPluginEntry<test_entry_default_impl>();
      CATCH_CHECK(clapPluginEntry.init("test/plugin/path"));
      CATCH_CHECK(!clapPluginEntry.get_factory(CLAP_PLUGIN_FACTORY_ID));
      clapPluginEntry.deinit();
   }

   struct test_entry_custom_impl : public test_entry {
      static std::string _initPluginPath;
      using plugin_glue = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                                clap::helpers::CheckingLevel::Minimal>;
      struct plugin : plugin_glue {
         static const clap_plugin_descriptor *getDescription() {
            static const char *features[] = {nullptr};
            static clap_plugin_descriptor desc = {CLAP_VERSION,
                                                  "org.free-audio.test-case.plugin",
                                                  "Test Case Plugin",
                                                  "",
                                                  "",
                                                  "",
                                                  "",
                                                  "",
                                                  "",
                                                  &features[0]};
            return &desc;
         }

         plugin(const clap_host &host) : plugin_glue(getDescription(), &host) {}
      };

      bool implementsPluginFactory() const noexcept override { return true; }
      uint32_t pluginFactoryGetPluginCount() const noexcept override { return 1; }
      const clap_plugin_descriptor_t *
      pluginFactoryGetPluginDescriptor(uint32_t) const noexcept override {
         return plugin::getDescription();
      }
      const clap_plugin *pluginFactoryCreatePlugin(const clap_host_t &host,
                                                   const char *) const noexcept override {
         return (new plugin(host))->clapPlugin();
      }

      bool init(const char *pluginPath) noexcept override {
         _initPluginPath = pluginPath;
         return true;
      }
      void deinit() noexcept override { _initPluginPath.clear(); }
   };
   std::string test_entry_custom_impl::_initPluginPath{};

   using test_host_glue = clap::helpers::Host<clap::helpers::MisbehaviourHandler::Ignore,
                                              clap::helpers::CheckingLevel::Minimal>;
   struct test_host : test_host_glue {
      test_host() : test_host_glue{"Test Case Host", nullptr, nullptr, "1.0.0"} {}
      bool threadCheckIsMainThread() const noexcept override { return true; };
      bool threadCheckIsAudioThread() const noexcept override { return false; };
      void requestRestart() noexcept override {};
      void requestProcess() noexcept override {};
      void requestCallback() noexcept override {};
   };

   CATCH_TEST_CASE("Use PluginEntry custom implementation") {
      CATCH_CHECK(test_entry_custom_impl::_initPluginPath.empty());
      const auto clapPluginEntry = test_entry::clapPluginEntry<test_entry_custom_impl>();

      // clap_plugin_entry.init
      const auto testPluginPath = "test/plugin/path";
      CATCH_CHECK(clapPluginEntry.init(testPluginPath));
      CATCH_CHECK(test_entry_custom_impl::_initPluginPath == testPluginPath);

      // clap_plugin_factory
      const auto factory = static_cast<const clap_plugin_factory *>(
         clapPluginEntry.get_factory(CLAP_PLUGIN_FACTORY_ID));
      CATCH_REQUIRE(factory);
      CATCH_CHECK(factory->get_plugin_count(factory) == 1);
      CATCH_REQUIRE(factory->get_plugin_descriptor(factory, 0) ==
                    test_entry_custom_impl::plugin::getDescription());
      test_host testHost;
      const auto plugin = factory->create_plugin(
         factory, testHost.clapHost(), test_entry_custom_impl::plugin::getDescription()->id);
      CATCH_REQUIRE(plugin);
      CATCH_CHECK(plugin->desc == test_entry_custom_impl::plugin::getDescription());
      plugin->destroy(plugin); // clean up after test

      // clap_plugin_entry.deinit
      clapPluginEntry.deinit();
      CATCH_CHECK(test_entry_custom_impl::_initPluginPath.empty());
   }
} // namespace
