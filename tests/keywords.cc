#include <set>

#include <clap/helpers/keywords.hh>

#include <catch2/catch.hpp>

CATCH_TEST_CASE("trim", "basic") {
   CATCH_CHECK(clap::helpers::trim<std::string>("") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>("   ") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>(" \t\t  ") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>(" \t\t  \r") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>("\n \t\t  ") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>(" \t\t  ") == "");
   CATCH_CHECK(clap::helpers::trim<std::string>(" \t\t\r") == "");

   CATCH_CHECK(clap::helpers::trim<std::string>("a") == "a");
   CATCH_CHECK(clap::helpers::trim<std::string>("ab") == "ab");
   CATCH_CHECK(clap::helpers::trim<std::string>("a b") == "a b");
   CATCH_CHECK(clap::helpers::trim<std::string>("a b ") == "a b");
   CATCH_CHECK(clap::helpers::trim<std::string>(" a b") == "a b");
   CATCH_CHECK(clap::helpers::trim<std::string>(" a b\n") == "a b");
}

using vs = std::vector<std::string>;

static vs keywordsToSet(const std::string& keywords)
{
    vs s;
    clap::helpers::foreachKeyword<std::string>(keywords, [&s] (const std::string& k) {
        s.push_back(k);
    });
    return s;
}

CATCH_TEST_CASE("keywords", "basic") {
    CATCH_CHECK(keywordsToSet("") == vs{});
    CATCH_CHECK(keywordsToSet("a") == vs{"a"});
    CATCH_CHECK(keywordsToSet("ab") == vs{"ab"});
    CATCH_CHECK(keywordsToSet("a;b") == vs{"a", "b"});
    CATCH_CHECK(keywordsToSet("a;b;c") == vs{"a", "b", "c"});
    CATCH_CHECK(keywordsToSet(";;;a;;;;b;c;;;;") == vs{"a", "b", "c"});
    CATCH_CHECK(keywordsToSet(";;; a\r;;;;b  ;\t\tc;;;;") == vs{"a", "b", "c"});
    CATCH_CHECK(keywordsToSet("a;b s;c") == vs{"a", "b s", "c"});
    CATCH_CHECK(keywordsToSet("a;  b  ;  ; c") == vs{"a", "b", "c"});
}