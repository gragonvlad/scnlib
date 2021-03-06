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

#ifndef SCN_DETAIL_LOCALE_H
#define SCN_DETAIL_LOCALE_H

#include "result.h"
#include "string_view.h"
#include "util.h"

namespace scn {
#if SCN_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
#endif
#if SCN_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

    namespace detail {
        template <typename CharT>
        class truename_falsename_storage {
        public:
            using char_type = CharT;
            using string_type = std::basic_string<char_type>;
            using string_view_type = basic_string_view<char_type>;

            truename_falsename_storage(const void* loc);

            string_type get_true_str() const
            {
                return m_truename;
            }
            string_type get_false_str() const
            {
                return m_falsename;
            }

            string_view_type get_true_view() const
            {
                return string_view_type(m_truename.data(), m_truename.size());
            }
            string_view_type get_false_view() const
            {
                return string_view_type(m_falsename.data(), m_falsename.size());
            }

        private:
            string_type m_truename;
            string_type m_falsename;
        };

        // Hand write to avoid C locales and thus for noticeable performance
        // gains
        static bool is_space(char ch)
        {
            return ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' ||
                   ch == '\v' || ch == '\f';
        }
        static bool is_space(wchar_t ch)
        {
            return ch == L' ' || ch == L'\n' || ch == L'\t' || ch == L'\r' ||
                   ch == L'\v' || ch == L'\f';
        }

        static bool is_digit(char ch)
        {
            return ch >= '0' && ch <= '9';
        }
        static bool is_digit(wchar_t ch)
        {
            return ch >= L'0' && ch <= L'9';
        }

        template <typename CharT>
        struct default_widen;
        template <>
        struct default_widen<char> {
            static char widen(char ch)
            {
                return ch;
            }
        };
        template <>
        struct default_widen<wchar_t> {
            static wchar_t widen(char ch)
            {
                auto ret = std::btowc(static_cast<int>(ch));
                if (ret == WEOF) {
                    return static_cast<wchar_t>(-1);
                }
                return static_cast<wchar_t>(ret);
            }
        };

        template <typename CharT>
        struct default_narrow;
        template <>
        struct default_narrow<char> {
            static char narrow(char ch, char)
            {
                return ch;
            }
        };
        template <>
        struct default_narrow<wchar_t> {
            static char narrow(wchar_t ch, char def)
            {
                auto ret = std::wctob(static_cast<wint_t>(ch));
                if (ret == EOF) {
                    return def;
                }
                return static_cast<char>(ret);
            }
        };

        template <typename CharT>
        struct locale_defaults;
        template <>
        struct locale_defaults<char> {
            static string_view truename()
            {
                return {"true"};
            }
            static string_view falsename()
            {
                return {"false"};
            }
            static char decimal_point()
            {
                return '.';
            }
            static char thousands_separator()
            {
                return ',';
            }
        };
        template <>
        struct locale_defaults<wchar_t> {
            static wstring_view truename()
            {
                return {L"true"};
            }
            static wstring_view falsename()
            {
                return {L"false"};
            }
            static wchar_t decimal_point()
            {
                return L'.';
            }
            static wchar_t thousands_separator()
            {
                return L',';
            }
        };
    }  // namespace detail

    template <typename CharT>
    class basic_locale_ref : public detail::disable_copy {
    public:
        using char_type = CharT;
        using string_type = std::basic_string<char_type>;
        using string_view_type = basic_string_view<char_type>;
        using iterator = typename string_view_type::iterator;

        basic_locale_ref() = default;
        explicit basic_locale_ref(const void* loc);

        const void* get_ptr() const
        {
            return m_locale;
        }

        bool is_space(char_type ch) const
        {
            if (SCN_LIKELY(is_default())) {
                return detail::is_space(ch);
            }
            return _is_space(ch);
        }
        bool is_digit(char_type ch) const
        {
            if (SCN_LIKELY(is_default())) {
                return detail::is_digit(ch);
            }
            return _is_digit(ch);
        }

        char_type decimal_point() const
        {
            return m_decimal_point;
        }
        char_type thousands_separator() const
        {
            return m_thousands_separator;
        }

        string_view_type truename() const
        {
            return string_view_type(m_truename.data(), m_truename.size());
        }
        string_view_type falsename() const
        {
            return string_view_type(m_falsename.data(), m_falsename.size());
        }

        CharT widen(char ch) const
        {
            if (SCN_LIKELY(is_default())) {
                return detail::default_widen<CharT>::widen(ch);
            }
            return _widen(ch);
        }
        char narrow(CharT ch, char def) const
        {
            if (SCN_LIKELY(is_default())) {
                return detail::default_narrow<CharT>::narrow(ch, def);
            }
            return _narrow(ch, def);
        }

        template <typename T>
        result<size_t> read_num(T& val, const string_type& buf);

        bool is_default() const
        {
            return m_locale == nullptr;
        }

        static basic_locale_ref get_default()
        {
            return basic_locale_ref();
        }

    private:
        bool _is_space(char_type) const;
        bool _is_digit(char_type) const;
        CharT _widen(char ch) const;
        char _narrow(char_type ch, char def) const;

        using defaults = detail::locale_defaults<char_type>;

        const void* m_locale{nullptr};
        detail::unique_ptr<detail::truename_falsename_storage<char_type>>
            m_truefalse_storage{nullptr};
        string_view_type m_truename{defaults::truename()};
        string_view_type m_falsename{defaults::falsename()};
        char_type m_decimal_point{defaults::decimal_point()};
        char_type m_thousands_separator{defaults::thousands_separator()};
    };

#if SCN_CLANG
#pragma clang diagnostic pop
#endif
#if SCN_GCC
#pragma GCC diagnostic pop
#endif
}  // namespace scn

#if defined(SCN_HEADER_ONLY) && SCN_HEADER_ONLY && !defined(SCN_LOCALE_CPP)
#include "locale.cpp"
#endif

#endif  // SCN_DETAIL_LOCALE_H
