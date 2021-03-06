// Copyright 2017-2019 Elias Kosunen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is a part of scnlib:
//     https://github.com/eliaskosunen/scnlib

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <scn/scn.h>

TEST_CASE("general")
{
    std::string data{"42 3.14 foobar true"};
    std::string copy = data;

    int i{0};
    double d{};
    std::string s(6, '\0');
    auto span = scn::make_span(&s[0], &s[0] + s.size());
    bool b{};
    auto stream = scn::make_stream(data.begin(), data.end());
    auto ret = scn::scan(stream, "{} {} {} {a}", i, d, span, b);

    CHECK(data == copy);
    CHECK(i == 42);
    CHECK(d == doctest::Approx(3.14));
    CHECK(s == "foobar");
    CHECK(b);
    CHECK(ret);

    if (!ret) {
        std::cout << __LINE__ << ": " << static_cast<int>(ret.get_code()) << '\n';
    }

#if 0
    int j{};
    ret = scn::input("{}", j);
    
    std::cout << j << '\n';
    CHECK(ret);
#endif
}

