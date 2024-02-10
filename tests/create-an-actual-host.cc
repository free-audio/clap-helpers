/*
 * Actually use host.hh / host.hxx to create a host. Assert that it is constructable
 */

#include "clap/helpers/host.hh"
#include "clap/helpers/host.hxx"
#include "clap/helpers/plugin-proxy.hh"
#include "clap/helpers/plugin-proxy.hxx"

#include <type_traits>

#include <catch2/catch_all.hpp>

struct test_host : clap::helpers::Host<clap::helpers::MisbehaviourHandler::Terminate,
                                       clap::helpers::CheckingLevel::Maximal>
{
   test_host() : clap::helpers::Host<clap::helpers::MisbehaviourHandler::Terminate,
                                     clap::helpers::CheckingLevel::Maximal>(
                                       "Test Case Host",
                                       "Free Audio",
                                       "http://cleveraudio.org",
                                       "1.0.0") {}

   bool threadCheckIsMainThread() const noexcept override { return true; };
   bool threadCheckIsAudioThread() const noexcept override { return false; };

   void requestRestart() noexcept override {};
   void requestProcess() noexcept override {};
   void requestCallback() noexcept override {};
};

CATCH_TEST_CASE("Create an Actual Host")
{
   CATCH_SECTION("Test Host is Creatable")
   {
      CATCH_REQUIRE(std::is_constructible<test_host>::value);
   }
}

using test_plugin_proxy = clap::helpers::PluginProxy<clap::helpers::MisbehaviourHandler::Terminate,
                                                     clap::helpers::CheckingLevel::Maximal>;

CATCH_TEST_CASE("Create an Actual Plugin Proxy")
{
   CATCH_SECTION("Test Plugin Proxy is Creatable")
   {
      CATCH_REQUIRE(std::is_constructible<test_plugin_proxy, const clap_plugin&, const test_host&>::value);
   }
}
