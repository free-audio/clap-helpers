#include "clap/helpers/plugin-entry.hxx"

#include <catch2/catch_all.hpp>

template class clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                                          clap::helpers::CheckingLevel::Maximal>;
template class clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                                          clap::helpers::CheckingLevel::Minimal>;
template class clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                                          clap::helpers::CheckingLevel::None>;
template class clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Ignore,
                                          clap::helpers::CheckingLevel::Maximal>;

CATCH_TEST_CASE("PluginEntry - Link") {
   clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                              clap::helpers::CheckingLevel::Maximal> *e0 = nullptr;
   clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                              clap::helpers::CheckingLevel::Minimal> *e1 = nullptr;
   clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Terminate,
                              clap::helpers::CheckingLevel::None> *e2 = nullptr;
   clap::helpers::PluginEntry<clap::helpers::MisbehaviourHandler::Ignore,
                              clap::helpers::CheckingLevel::Maximal> *e3 = nullptr;
}
