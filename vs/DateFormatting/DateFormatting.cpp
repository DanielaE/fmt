#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
#include <stdafx.h>

#include <algorithm>
#include <array>
#include <date/tz.h>
#include <fmt/format.h>
#include <string>
#include <vector>

using namespace date;
using namespace std::chrono;
using namespace std::chrono_literals;

template <typename FormatContext,
          typename Char = typename FormatContext::char_type>
struct back_insert_buf final : std::basic_streambuf<Char> {
    typedef typename FormatContext::iterator iterator;

    FMT_CONSTEXPR back_insert_buf(FormatContext & ctx) FMT_NOEXCEPT
    : it_(ctx.out()) {}
    FMT_CONSTEXPR operator iterator() const FMT_NOEXCEPT { return it_; }

private:
    iterator it_;

	typedef typename std::basic_streambuf<Char>::int_type int_type;

	int_type overflow(int_type c) FMT_OVERRIDE final {
        it_ = static_cast<Char>(c);
        return c;
    }

    std::streamsize xsputn(const Char * s,
                           std::streamsize count) FMT_OVERRIDE final {
        for (const auto e = s + count; s != e; ++s)
            it_ = *s;
        return count;
    }
};

#if 0
static constexpr char valid_timepoint_flags[] =
    "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%";

template <typename ParseContext>
auto validate_format(ParseContext & ctx) -> typename ParseContext::iterator {
    auto it = ctx.begin();
    auto it = fmt::internal::null_terminating_iterator<
        typename ParseContext::char_type>(ctx);
    constexpr auto valid_begin = std::begin(valid_timepoint_flags);
    constexpr auto valid_end   = std::end(valid_timepoint_flags);
    bool isFlag                = false;
    for (; it != ctx.end(); ++it) {
        const auto ch = *it;
        if (!ch | (ch == '}'))
            break;
        if (isFlag) {
            if (std::none_of(valid_begin, valid_end,
                             [&](char c) { return c == ch; }))
                ctx.on_error("invalid format specification");
            isFlag = false;
        } else {
            isFlag = ch == '%';
        }
    }
    using fmt::internal::pointer_from;
    return pointer_from(it);
}
#else

#    define PARSEOPTION 2

#    if PARSEOPTION == 0

// requires ctx.end() which is marked as deprecated
template <typename ParseContext>
auto validate_format(ParseContext & ctx) -> typename ParseContext::iterator {
    for (auto it = ctx.begin(); it != ctx.end(); ++it) {
        const auto ch = *it;
        if (!ch | (ch == '}'))
            return it;
    }
    ctx.on_error("invalid format specification");
    return {};
}
#    elif PARSEOPTION == 1
template <typename ParseContext>
auto validate_format(ParseContext & ctx) -> typename ParseContext::iterator {
    using fmt::internal::pointer_from;
    for (auto it = fmt::internal::null_terminating_iterator<
             typename ParseContext::char_type>(ctx);
         *it; ++it) {
        if (*it == '}')
            return pointer_from(it);
    }
    ctx.on_error("invalid format specification");
    return {};
}
#    elif PARSEOPTION == 2
template <typename ParseContext>
auto validate_format(ParseContext & ctx) -> typename ParseContext::iterator {
    unsigned pos = 0;
    for (auto ch : ctx) {
        if (!ch | (ch == '}'))
            return ctx.begin() + pos;
        ++pos;
    }
    ctx.on_error("invalid format specification");
    return {};
}
#endif

#endif

namespace fmt {
namespace chrono {

template <typename Char>
struct base_formatter {
	template <typename ParseContext>
	auto parse(ParseContext & ctx) -> typename ParseContext::iterator {
		const auto begin = ctx.begin();
		const auto end = std::find(ctx.begin(), ctx.end(), '}');
		if (end == begin) {
			date_format.push_back('%');
			date_format.push_back('x');
		} else {
			date_format.reserve(end - begin + 1);
			date_format.append(begin, end);
		}
		date_format.push_back(0);
		return end;
	}

protected:
	template <typename T, typename FormatContext>
	auto base_format(const T & t,
		FormatContext & ctx) -> typename FormatContext::iterator {
		back_insert_buf<FormatContext> buffer(ctx);
		std::basic_ostream<Char> output(&buffer);
		date::to_stream(output, date_format.data(), t);
		if (output.fail())
			ctx.on_error("invalid format specification");
		return buffer;
	}

private:
	basic_memory_buffer<Char> date_format;
};

} // namespace chrono

template <typename Rep, typename Period, typename Char>
struct formatter<duration<Rep, Period>, Char> : formatter<Rep, Char> {
	using base = formatter<Rep, Char>;
	using str_formatter = formatter<basic_string_view<Char>, Char>;
	using period_type = typename Period::type;

	template <typename FormatContext>
	auto format(const duration<Rep, Period> & d, FormatContext & ctx) {
		auto it = base::format(d.count(), ctx);
		it = ' '; // a single space according to SI formatting rules
		const auto u = detail::get_units<Char>(period_type{});
		return str_formatter{}.format({ u.data(), u.size() }, ctx);
	}
};

template <typename Clock, typename Duration, typename Char>
struct formatter<time_point<Clock, Duration>, Char> : chrono::base_formatter<Char> {
	template <typename FormatContext>
	auto format(const time_point<Clock, Duration> & tp,
		FormatContext & ctx) {
		return this->base_format(tp, ctx);
	}
};

template <typename Duration, typename TimeZonePtr, typename Char>
struct formatter<date::zoned_time<Duration, TimeZonePtr>, Char> : chrono::base_formatter<Char> {
	template <typename FormatContext>
	auto format(const date::zoned_time<Duration, TimeZonePtr> & zt,
		FormatContext & ctx) {
		return this->base_format(zt, ctx);
	}
};

} // namespace fmt

int main() {
	const auto myBirth = local_days{ 11_d / mar / 1963 } +5min;
	const auto mbZoned = make_zoned(current_zone(), myBirth);
	const auto minutes_am = round<minutes>(myBirth) - floor<days>(myBirth);

    std::vector<char> buf;
    fmt::format_to(std::back_inserter(buf), "{:%x %X}", myBirth);

    std::wstring wbuf;
    fmt::format_to_n(std::back_inserter(wbuf), 10, L"{:%A %d.%m.%Y %H:%M}", myBirth);

    wbuf = fmt::format(L"My birth date is {:%A %d.%m.%Y %H:%M}", myBirth);
	wbuf = fmt::format(L"{:%A %d.%m.%Y %H:%M %Z}, {} after midnight", mbZoned, minutes_am);

    wbuf.clear();
}
