#pragma once

#include <cstddef>
#include <functional>
#include <string>

namespace clap { namespace helpers {

   // Prefer string_view with C++17

   template <typename string_type = typename std::string>
   string_type trim(const string_type& str)
   {
      auto start = str.find_first_not_of(" \t\r\n");
      if (start == string_type::npos)
         return {};
      auto end = str.find_last_not_of(" \t\r\n");
      return str.substr(start, end - start + 1);
   }

   template <typename string_type = typename std::string>
   void foreachKeyword(const string_type &keywords,
                       const std::function<void(const string_type &keyword)> &onKeyword) {
      size_t pos = 0;
      string_type in = keywords;

      while (!in.empty()) {
         pos = in.find(';');
         string_type keyword = trim(in.substr(0, pos));
         if (!keyword.empty())
            onKeyword(keyword);
         if (pos == string_type::npos)
            return;
         in = in.substr(pos + 1);
      }
   }
}} // namespace clap::helpers