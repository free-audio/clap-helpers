#pragma once

#if defined(__cplusplus) && __cplusplus >= 202002L
#   define CLAP_HELPERS_UNLIKELY [[unlikely]]
#else
#   define CLAP_HELPERS_UNLIKELY
#endif

#if defined(__cpp_exceptions)
#   define CLAP_HELPERS_THROW(X) throw X
#else
#   define CLAP_HELPERS_THROW(X) std::terminate()
#endif
