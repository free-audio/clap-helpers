#include <clap/helpers/host-proxy.hxx>
#include <clap/helpers/param-queue.hh>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>
#include <clap/helpers/reducing-param-queue.hh>
#include <clap/helpers/reducing-param-queue.hxx>

#include <catch2/catch_all.hpp>

template class clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                     clap::helpers::CheckingLevel::Maximal>;
template class clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                     clap::helpers::CheckingLevel::Minimal>;
template class clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                     clap::helpers::CheckingLevel::None>;
template class clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                     clap::helpers::CheckingLevel::Maximal>;

template class clap::helpers::HostProxy<clap::helpers::MisbehaviourHandler::Terminate,
                                        clap::helpers::CheckingLevel::Maximal>;
template class clap::helpers::HostProxy<clap::helpers::MisbehaviourHandler::Terminate,
                                        clap::helpers::CheckingLevel::Minimal>;
template class clap::helpers::HostProxy<clap::helpers::MisbehaviourHandler::Terminate,
                                        clap::helpers::CheckingLevel::None>;
template class clap::helpers::HostProxy<clap::helpers::MisbehaviourHandler::Ignore,
                                        clap::helpers::CheckingLevel::Maximal>;

CATCH_TEST_CASE("Plugin - Link") {
   clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                         clap::helpers::CheckingLevel::Maximal> *p0 = nullptr;
   clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                         clap::helpers::CheckingLevel::Minimal> *p1 = nullptr;
   clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                         clap::helpers::CheckingLevel::None> *p2 = nullptr;
   clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                         clap::helpers::CheckingLevel::Maximal> *p3 = nullptr;
}

namespace {
   struct params_plugin : clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                                clap::helpers::CheckingLevel::Maximal> {
      params_plugin(const clap_plugin_descriptor *desc, const clap_host *host)
         : Plugin(desc, host) {}

      bool implementsParams() const noexcept override { return true; }
      uint32_t paramsCount() const noexcept override { return 1; }
      bool paramsInfo(uint32_t paramIndex, clap_param_info *info) const noexcept override {
         if (paramIndex != 0)
            return false;
         *info = clap_param_info{};
         info->min_value = 0;
         info->max_value = 1;
         return true;
      }
      bool paramsValue(clap_id paramId, double *value) noexcept override {
         if (!value)
            return false;
         *value = 0.5;
         return true;
      }
   };
} // namespace

CATCH_TEST_CASE("Plugin - params.value with a null out pointer") {
   static const clap_host host = {
      CLAP_VERSION,
      nullptr,
      "Test Case Host",
      "Free Audio",
      "http://cleveraudio.org",
      "1.0.0",
      [](const clap_host *, const char *) -> const void * { return nullptr; },
      [](const clap_host *) {},
      [](const clap_host *) {},
      [](const clap_host *) {}};
   static const char *features[] = {CLAP_PLUGIN_FEATURE_INSTRUMENT, nullptr};
   static const clap_plugin_descriptor desc = {CLAP_VERSION,
                                               "org.free-audio.test-case.params",
                                               "Test Case Params",
                                               "Free Audio",
                                               "http://cleveraudio.org",
                                               "",
                                               "",
                                               "1.0.0",
                                               "",
                                               &features[0]};

   params_plugin plugin(&desc, &host);
   auto clapPlugin = plugin.clapPlugin();
   CATCH_REQUIRE(clapPlugin->init(clapPlugin));

   auto params = static_cast<const clap_plugin_params *>(
      clapPlugin->get_extension(clapPlugin, CLAP_EXT_PARAMS));
   CATCH_REQUIRE(params != nullptr);
   CATCH_CHECK(!params->get_value(clapPlugin, 0, nullptr));
}
