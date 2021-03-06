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

#include <doctest.h>
#include <scn/scn.h>

TEST_CASE("boolean")
{
    std::string data{"true false 0 1 2"};
    auto stream = scn::make_stream(data);
    bool b{false};

    {
        auto ret = scn::scan(stream, "{a}", b);
        CHECK(b);
        CHECK(ret);
        b = true;
    }
    {
        auto ret = scn::scan(stream, "{a}", b);
        CHECK(!b);
        CHECK(ret);
        b = true;
    }
    {
        auto ret = scn::scan(stream, "{}", b);
        CHECK(!b);
        CHECK(ret);
        b = false;
    }
    {
        auto ret = scn::scan(stream, "{}", b);
        CHECK(b);
        CHECK(ret);
        b = false;
    }
    {
        auto ret = scn::scan(stream, "{}", b);
        CHECK(!b);
        CHECK(!ret);
        if (!ret) {
            CHECK(ret == scn::error::invalid_scanned_value);
        }
        //b = false;
    }
}
