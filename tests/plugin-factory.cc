#include <clap/helpers/plugin-factory.hxx>

#include <catch2/catch_all.hpp>

template class clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::Maximal>;
template class clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::Minimal>;
template class clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::None>;
template class clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Ignore,
                                            clap::helpers::CheckingLevel::Maximal>;

namespace {

   CATCH_TEST_CASE("PluginFactory - Link") {
      clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                   clap::helpers::CheckingLevel::Maximal> *pF0 = nullptr;
      clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                   clap::helpers::CheckingLevel::Minimal> *pF1 = nullptr;
      clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Terminate,
                                   clap::helpers::CheckingLevel::None> *pF2 = nullptr;
      clap::helpers::PluginFactory<clap::helpers::MisbehaviourHandler::Ignore,
                                   clap::helpers::CheckingLevel::Maximal> *pF3 = nullptr;
   }

} // namespace
