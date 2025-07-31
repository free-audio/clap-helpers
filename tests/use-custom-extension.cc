#include <clap/helpers/host.hxx>
#include <clap/helpers/plugin-proxy.hxx>
#include <clap/helpers/plugin.hxx>

#include <catch2/catch_all.hpp>

namespace {
   //////////////////////
   // custom extension //
   //////////////////////

   static CLAP_CONSTEXPR const char CLAP_HELPERS_EXT_TEST_PING_PONG[] =
      "clap-helpers.test.ping-pong";

   extern "C" {
   typedef struct clap_plugin_test_ping_pong {
      // Request the plugin to send back a pong
      // [main-thread]
      void(CLAP_ABI *ping)(const clap_plugin_t *plugin);
      // Respond to a ping received from the plugin
      // [main-thread]
      void(CLAP_ABI *pong)(const clap_plugin_t *plugin);
   } clap_plugin_ping_ping_t;

   typedef struct clap_host_test_ping_pong {
      // Request the host to send back a pong
      // [main-thread]
      void(CLAP_ABI *ping)(const clap_host_t *host);
      // Respond to a ping received from the host
      // [main-thread]
      void(CLAP_ABI *pong)(const clap_host_t *host);
   } clap_host_test_ping_pong_t;
   }

   /////////////////////////
   // template parameters //
   /////////////////////////

   static constexpr auto test_mh = clap::helpers::MisbehaviourHandler::Terminate;
   static constexpr auto test_cl = clap::helpers::CheckingLevel::Maximal;

   //////////////////////////////
   // test host implementation //
   //////////////////////////////

   using test_plugin_proxy_base = clap::helpers::PluginProxy<test_mh, test_cl>;
   using test_host_base = clap::helpers::Host<test_mh, test_cl>;

   struct test_plugin_proxy : test_plugin_proxy_base {
      const clap_plugin_test_ping_pong *_pluginPingPong{};

      test_plugin_proxy(const clap_plugin &plugin, const test_host_base &host)
         : test_plugin_proxy_base(plugin, host) {}

      bool init() noexcept {
         if (!test_plugin_proxy_base::init())
            return false;
         getExtension(_pluginPingPong, CLAP_HELPERS_EXT_TEST_PING_PONG);
         return true;
      }

      bool canUsePingPong() const {
         return _pluginPingPong && _pluginPingPong->ping && _pluginPingPong->pong;
      }
      void ping() const {
         ensureMainThread("test_ping_pong.ping");
         _pluginPingPong->ping(&_plugin);
      }
      void pong() const {
         ensureMainThread("test_ping_pong.ping");
         _pluginPingPong->pong(&_plugin);
      }
   };

   struct test_host : test_host_base {
      std::unique_ptr<test_plugin_proxy> _pluginProxy;
      bool _pluginCalledPong{};

      test_host()
         : test_host_base("Test Case Host", "Free Audio", "http://cleveraudio.org", "1.0.0") {}
      ~test_host() { _pluginProxy->destroy(); }

      bool threadCheckIsMainThread() const noexcept override { return true; };
      bool threadCheckIsAudioThread() const noexcept override { return false; };
      void requestRestart() noexcept override {};
      void requestProcess() noexcept override {};
      void requestCallback() noexcept override {};

      const void *getExtension(const char *extensionId) const noexcept override {
         if (!strcmp(extensionId, CLAP_HELPERS_EXT_TEST_PING_PONG)) {
            static const clap_host_test_ping_pong hostPingPong{
               [](const clap_host *host) {
                  auto &self = *static_cast<test_host *>(host->host_data);
                  self.ensureMainThread("clap_host_test_ping_pong.ping");
                  if (self._pluginProxy->canUsePingPong())
                     self._pluginProxy->pong();
               },
               [](const clap_host *host) {
                  auto &self = *static_cast<test_host *>(host->host_data);
                  self.ensureMainThread("clap_host_test_ping_pong.pong");
                  self._pluginCalledPong = true;
               },
            };
            return &hostPingPong;
         }
         return nullptr;
      }
   };

   ////////////////////////////////
   // test plugin implementation //
   ////////////////////////////////

   using test_host_proxy = clap::helpers::HostProxy<test_mh, test_cl>;
   using test_plugin_base = clap::helpers::Plugin<test_mh, test_cl>;

   struct test_host_custom_extension_proxy {
      test_host_proxy &_hostProxy;
      const clap_host_test_ping_pong *_hostPingPong{};

      test_host_custom_extension_proxy(test_host_proxy &hostProxy) : _hostProxy(hostProxy) {}
      void init() { _hostProxy.getExtension(_hostPingPong, CLAP_HELPERS_EXT_TEST_PING_PONG); }

      bool canUsePingPong() const {
         return _hostPingPong && _hostPingPong->ping && _hostPingPong->pong;
      }
      void ping() const noexcept {
         _hostProxy.ensureMainThread("test_ping_pong.ping");
         _hostPingPong->ping(_hostProxy.clapHost());
      }
      void pong() const noexcept {
         _hostProxy.ensureMainThread("test_ping_pong.ping");
         _hostPingPong->pong(_hostProxy.clapHost());
      }
   };

   struct test_plugin : test_plugin_base {
      test_host_proxy _hostProxy;
      test_host_custom_extension_proxy _hostExtensionProxy;
      bool _hostCalledPong{};

      static clap_plugin_descriptor &dummyDesc() {
         static clap_plugin_descriptor d{};
         return d;
      }
      test_plugin(const clap_host *host)
         : _hostProxy(host), test_plugin_base(&dummyDesc(), host), _hostExtensionProxy(_hostProxy) {
      }

      const void *extension(const char *id) noexcept {
         if (!strcmp(id, CLAP_HELPERS_EXT_TEST_PING_PONG)) {
            static const clap_plugin_test_ping_pong pluginPingPong{
               [](const clap_plugin *plugin) {
                  auto &self = *static_cast<test_plugin *>(&from(plugin));
                  self.ensureMainThread("clap_plugin_test_ping_pong.ping");
                  self._hostExtensionProxy.pong();
               },
               [](const clap_plugin *plugin) {
                  auto &self = *static_cast<test_plugin *>(&from(plugin));
                  self.ensureMainThread("clap_plugin_test_ping_pong.pong");
                  self._hostCalledPong = true;
               },
            };
            return &pluginPingPong;
         }
         return nullptr;
      }

      bool init() noexcept override {
         _hostExtensionProxy.init();
         return true;
      }
   };
} // namespace

CATCH_TEST_CASE("Use a custom extension") {
   CATCH_WHEN("host and plugin are initialized") {
      test_host host{};
      auto plugin = new test_plugin(host.clapHost());
      host._pluginProxy = std::make_unique<test_plugin_proxy>(*plugin->clapPlugin(), host);
      CATCH_REQUIRE(host._pluginProxy->init());

      CATCH_THEN("no pong responds are registered yet") {
         CATCH_REQUIRE_FALSE(plugin->_hostCalledPong);
         CATCH_REQUIRE_FALSE(host._pluginCalledPong);
      }

      CATCH_AND_WHEN("host calls ping on plugin") {
         host._pluginProxy->ping();
         CATCH_THEN("plugin responds with pong") { CATCH_CHECK(host._pluginCalledPong); }
      }

      CATCH_AND_WHEN("plugin calls ping on host") {
         CATCH_REQUIRE(plugin->_hostExtensionProxy.canUsePingPong());
         plugin->_hostExtensionProxy.ping();
         CATCH_THEN("host responds with pong") { CATCH_CHECK(plugin->_hostCalledPong); }
      }
   }
}
