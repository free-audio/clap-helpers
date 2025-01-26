#include <clap/helpers/plugin-factory.hxx>
#include <clap/helpers/plugin.hxx>

#include <catch2/catch_all.hpp>

namespace {

   using test_plugin_factory =
      clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                   clap::helpers::CheckingLevel::Maximal>;

   using test_plugin_glue = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                                  clap::helpers::CheckingLevel::Minimal>;
   template <int TestPluginTypeIndex>
   class test_plugin : public test_plugin_glue
   {
   public:
      static const clap_plugin_descriptor *getDescription()
      {
         const static auto id =
            std::string{"org.free-audio.test-case.plugin."} + std::to_string(TestPluginTypeIndex);
         static auto desc = []() {
            clap_plugin_descriptor desc{};
            desc.id = id.c_str();
            return desc;
         }();
         return &desc;
      }

      test_plugin(const clap_host *host) : test_plugin_glue(getDescription(), host) {}
   };

   struct test_factory_setup
   {
      test_factory_setup() : _pluginFactoryInitialized{test_plugin_factory::init()} {}
      ~test_factory_setup() { test_plugin_factory::deinit(); }
      bool _pluginFactoryInitialized;
   };

   CATCH_TEST_CASE("Use PluginFactory") 
   {
      CATCH_WHEN("PluginFactory is initialized")
      {
         test_factory_setup setup{};
         CATCH_REQUIRE(setup._pluginFactoryInitialized);
         auto clapPluginFactory = test_plugin_factory::clapPluginFactory();
         CATCH_REQUIRE(clapPluginFactory);
         CATCH_REQUIRE(clapPluginFactory->get_plugin_count);
         CATCH_REQUIRE(clapPluginFactory->get_plugin_descriptor);
         CATCH_REQUIRE(clapPluginFactory->create_plugin);

         CATCH_THEN("plugin count is 0")
         {
            CATCH_CHECK(clapPluginFactory->get_plugin_count(clapPluginFactory) == 0);
         }

         CATCH_AND_WHEN("the first plugin is registered")
         {
            auto dummyHost = clap_host_t{};
            dummyHost.clap_version = CLAP_VERSION;

            using test_plugin_0 = test_plugin<0>;
            auto createPlugin0 = [&dummyHost](const clap_host &) { return (new test_plugin_0(&dummyHost))->clapPlugin(); };
            test_plugin_factory::registerPlugin(*test_plugin_0::getDescription(), createPlugin0);

            CATCH_THEN("the plugin can be accessed and instanciated")
            {
               CATCH_CHECK(clapPluginFactory->get_plugin_descriptor(clapPluginFactory, 0) ==
                           test_plugin_0::getDescription());

               auto plugin = clapPluginFactory->create_plugin(
                                 clapPluginFactory,
                                 &dummyHost,
                                 test_plugin_0::getDescription()->id);
               CATCH_CHECK(plugin->desc == test_plugin_0::getDescription());
         CATCH_REQUIRE(setup._pluginFactoryInitialized);
               plugin->destroy(plugin); plugin = nullptr; // free after test

               CATCH_AND_WHEN("a second plugin is registered")
               {
                  using test_plugin_1 = test_plugin<1>;
                  auto createPlugin1 = [&dummyHost](const clap_host &) { return (new test_plugin_1(&dummyHost))->clapPlugin(); };
                  test_plugin_factory::registerPlugin(*test_plugin_1::getDescription(), createPlugin1);

                  CATCH_THEN("both plugins can be accessed and instanciated")
                  {
                     CATCH_CHECK(clapPluginFactory->get_plugin_count(clapPluginFactory) == 2);

                     CATCH_CHECK(clapPluginFactory->get_plugin_descriptor(clapPluginFactory, 0) ==
                                 test_plugin_0::getDescription());
                     auto plugin0 = clapPluginFactory->create_plugin(
                                       clapPluginFactory,
                                       &dummyHost,
                                       test_plugin_0::getDescription()->id);
                     CATCH_CHECK(plugin0->desc == test_plugin_0::getDescription());

                     CATCH_CHECK(clapPluginFactory->get_plugin_descriptor(clapPluginFactory, 1) ==
                                 test_plugin_1::getDescription());
                     auto plugin1 = clapPluginFactory->create_plugin(
                                       clapPluginFactory,
                                       &dummyHost,
                                       test_plugin_1::getDescription()->id);
                     CATCH_CHECK(plugin1->desc == test_plugin_1::getDescription());

                     plugin0->destroy(plugin0); plugin0 = nullptr; // free after test
                     plugin1->destroy(plugin1); plugin1 = nullptr; // free after test
                  }
               }
            }
         }
      }
   }
} // namespace
