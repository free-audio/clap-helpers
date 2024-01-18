#pragma once

#if defined(__cplusplus) && __cplusplus >= 201703L
#   define CLAP_IF_CONSTEXPR if constexpr
#else
#   define CLAP_IF_CONSTEXPR if
#endif

#if defined(__cplusplus) && __cplusplus >= 202002L
#   define CLAP_HELPERS_UNLIKELY [[unlikely]]
#else
#   define CLAP_HELPERS_UNLIKELY
#endif
