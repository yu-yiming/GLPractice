#pragma once

#include <chrono>
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <source_location>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#include "utility.hpp"

namespace stdf = std::filesystem;
namespace stdr = std::ranges;
namespace stdv = std::ranges::views;

namespace gltool {

struct colored_message;

struct formatted_message;

struct message;

struct text_color;

struct text_format;

#ifdef __GNUG__
// Help gcc demangle the stupid names returned by typeid.
inline std::string demangle(char const* name) {
    int status;
    std::unique_ptr<char, void(*)(void*)> res(abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
    return status == 0 ? res.get() : name;
}
#else
inline std::string demangle(char const* name) {
    return name;
}
#endif // __GNUG__ defined

template<typename T>
std::string typeid_name = demangle(typeid(T).name());

/**
 * @brief A wrapper around std::string that has a message tag that gives hints
 * about the style of the message when it is printed.
 */
struct message {
    struct hash {
        std::size_t operator ()(message const& msg) const {
            return std::hash<std::string>()(msg.content);
        }
    };

    std::string content = "";
    enum : unsigned {
        NONE = 0, DEBUG = 1, INFO = 2, WARNING = 4, ERROR = 8
    } type = NONE;
};

message operator ""_info(char const* str, std::size_t) {
    return { .content = str, .type = message::INFO };
}

message operator ""_warning(char const* str, std::size_t) {
    return { .content = str, .type = message::WARNING };
}

message operator ""_error(char const* str, std::size_t) {
    return { .content = str, .type = message::ERROR };
}


/**
 * @brief A Wrapper class to manipulate text colors using ANSI codes. See link 
 * [Wiki](https://en.wikipedia.org/wiki/ANSI_escape_code).
 * 
 * We can either use its instantiated objects as IO manipulators, or a string 
 * formatting "guard" that colorize an input string. 
 * @example As an IO manipulator:
 * @code
 *      int main() {
 *          using gltool::text_color;
 *          auto blue = text_color::of("blue");
 *          std::cout << blue << "Hello, World!" << "(this is also blue)" << std::endl;
 *      }
 * @endcode
 * @example As a string formatting guard:
 * @code
 *      int main() {
 *          using gltool::text_color;
 *          auto blue = text_color::of("blue");
 *          std::cout << blue("Hello, World!") << "(this is back to normal color)" << std::endl;
 *      }
 * @endcode
 */
struct text_color {
    struct hash {
        std::size_t operator ()(text_color const& color) const {
            return std::hash<unsigned>()(static_cast<unsigned>(color.type));
        }
    };

    /**
     * @brief The Type enum
     * Layout explanation:
     *      0...0                       0...0                                   0...0                        0            000           00           0            0
     * [red-channel]  [green-channe/background-regular-colorsl]   [blue-channel/text-regular-colors]  [default-flag]  [text-mode]  [color-mode]  [target]   [regular-flag]
     *     24-31                        16-23                                   8-15                         7            4-6          2-3           1            0
     */
    mutable enum : unsigned {
        MASK = ~0u, DEFAULT_COLOR = 0,
        // regular-color flag
        MASK_REGULAR_FLAG = 0b1, MASK_RGB = ~0b1u, REGULAR = 0b1,
        // target
        MASK_TARGET = 0b1, MASK_TEXT = ~0b10u, BACKGROUND = 0b0000010, BOTH = 0b0000011,
        // color modes
        MASK_COLOR_MODE = 0b1100, MASK_NORMAL_COLOR = ~0b1100u, BRIGHT = 0b0100, DIM = 0b1000,
        // text modes
        MASK_TEXT_MODE = 0b111u << 4, MASK_NORMAL_TEXT = ~0b1110000u, ITALIC = 0b0010000, UNDERLINE = 0b0100000, SLOW_BLINKING = 0b0110000,
        FAST_BLINKING = 0b1000000, REVERSE = 0b1010000, HIDDEN = 0b1100000, STRIKED = 0b1110000,
        // non-RGB scaled colors
        MASK_DEFAULT = ~0b10000000u, MASK_REGULAR = 0b11111111u << 8,
        BLACK = 1 << 8, RED = 1 << 9, GREEN = 1 << 10, YELLOW = 1 << 11, BLUE = 1 << 12, PURPLE = 1 << 13, CYAN = 1 << 14, WHITE = 1 << 15,
        MASK_REGULAR_FOREGROUND = 0b11111111u << 8,
        BLACK_FOREGROUND = BLACK, RED_FOREGROUND = RED, GREEN_FOREGROUND = GREEN, YELLOW_FOREGROUND = YELLOW, BLUE_FOREGROUND = BLUE,
        PURPLE_FOREGROUND = PURPLE, CYAN_FOREGROUND = CYAN, WHITE_FOREGROUND = WHITE,
        MASK_REGULAR_BACKGROUND = 0b11111111u << 16,
        BLACK_BACKGROUND = 1 << 16, RED_BACKGROUND = 1 << 17, GREEN_BACKGROUND = 1 << 18, YELLOW_BACKGROUND = 1 << 19,
        BLUE_BACKGROUND = 1 << 20, PURPLE_BACKGROUND = 1 << 21, CYAN_BACKGROUND = 1 << 22, WHITE_BACKGROUND = 1 << 23
    } type;

    using enum_type = decltype(type);

    text_color(unsigned t = REGULAR | WHITE) noexcept
        : type(static_cast<enum_type>(t)) {}

    text_color(text_color const& other) noexcept
        : type(other.type) {}

    text_color& operator =(text_color const& other) noexcept {
        type = other.type;
        return *this;
    }

    colored_message operator ()(std::string str) const;

    colored_message operator ()(message const& msg) const;

    colored_message operator ()(message&& msg) const noexcept;

    colored_message operator ()(colored_message const& msg) const;

    colored_message operator ()(colored_message&& msg) const noexcept;

    int color_mode() const noexcept {
        return (type & MASK_COLOR_MODE) >> 2;
    }

    static text_color from_rgb(unsigned r, unsigned g, unsigned b) noexcept {
        unsigned color = (r << 24) + (g << 16) + (b << 8);
        return text_color(color);
    }

    bool is(unsigned t) const noexcept {
        return (type & t) == t;
    }

    /**
     * @brief Construct a text_color object from a string. Only the ANSI default colors
     * are supported, i.e. "black", "red", "green", "yellow", "blue", "purple", "cyan", "white",
     * case insensitive.
     * TODO: Support case insensitivity. 
     * *(I need better support of the ranges library support for an elegant implementation)
     */
    static text_color of(std::string const& color) {
        static auto color_map = std::unordered_map<std::string, text_color>({
            { "black", BLACK }, { "red", RED }, { "green", GREEN }, { "yellow", YELLOW }, { "blue", BLUE }, { "purple", PURPLE }, { "cyan", CYAN }, { "white", WHITE }
        });
        return color_map[color];
    }

    text_color& reset(auto... flags) {
        type = enum_type(type & DEFAULT_COLOR);
        type = enum_type(type | (flags | ...));
        return *this;
    }

    text_color& set(auto... flags) {
        type = enum_type(type | (flags | ...));
        return *this;
    }

    int text_mode() const noexcept {
        int result = (type & MASK_TEXT_MODE) >> 4;
        return result == 0 ? 0 : result + 2;
    }

    /**
     * @brief Return the ANSI code for the current color, given that the current color mode is
     * regular. Otherwise the white color will be returned.
     * 
     * @return int The ANSI code, betwen 30 and 37 (foreground colors). Default is 37 (white).
     */
    int to_regular_color() const noexcept {
        static auto fgcolor_code_map = std::unordered_map<text_color, int, hash>({
            { BLACK, 30 }, { RED, 31 }, { GREEN, 32 }, { YELLOW, 33 }, 
            { BLUE, 34 }, { PURPLE, 35 }, { CYAN, 36 }, { WHITE, 37 }
        }, 8, hash());
        if (fgcolor_code_map.contains(type & MASK_REGULAR_FOREGROUND)) {
            return fgcolor_code_map[type & MASK_REGULAR_FOREGROUND];
        }
        return fgcolor_code_map[WHITE];
    }

    /**
     * @brief Return the ANSI code for the current background color, given that the current mode
     * is regular. Otherwise the white color will be returned.
     * 
     * @return int The ANSI code, betwen 40 and 47 (background colors). Default is 40 (black).
     */
    int to_regular_color_background() const noexcept {
        static auto bgcolor_code_map = std::unordered_map<text_color, int, hash>({
            { BLACK_BACKGROUND, 40 }, { RED_BACKGROUND, 41 }, { GREEN_BACKGROUND, 42 }, 
            { YELLOW_BACKGROUND, 43 }, { BLUE_BACKGROUND, 44 }, { PURPLE_BACKGROUND, 45 }, 
            { CYAN_BACKGROUND, 46 }, { WHITE_BACKGROUND, 47 }
        }, 8, hash());
        if (bgcolor_code_map.contains(type & MASK_REGULAR_BACKGROUND)) {
            return bgcolor_code_map[type & MASK_REGULAR_BACKGROUND];
        }
        return bgcolor_code_map[BLACK_BACKGROUND];
    }

    std::tuple<unsigned, unsigned, unsigned> to_rgb() const noexcept {
        return std::make_tuple((type >> 8) & MASK, (type >> 16) & MASK, (type >> 24) & MASK);
    }

    friend bool operator ==(text_color const& lhs, text_color const& rhs) noexcept {
        return lhs.type == rhs.type;
    }
};


namespace colors {

text_color g_default_color;

// Regular foreground and background colors.
text_color const k_black            (text_color::REGULAR | text_color::BLACK),
                 k_red              (text_color::REGULAR | text_color::RED),
                 k_green            (text_color::REGULAR | text_color::GREEN),
                 k_yellow           (text_color::REGULAR | text_color::YELLOW),
                 k_blue             (text_color::REGULAR | text_color::BLUE),
                 k_purple           (text_color::REGULAR | text_color::PURPLE),
                 k_cyan             (text_color::REGULAR | text_color::CYAN),
                 k_white            (text_color::REGULAR | text_color::WHITE),
                 k_black_background (text_color::REGULAR | text_color::BLACK_BACKGROUND),
                 k_red_background   (text_color::REGULAR | text_color::RED_BACKGROUND),
                 k_green_background (text_color::REGULAR | text_color::GREEN_BACKGROUND),
                 k_yellow_background(text_color::REGULAR | text_color::YELLOW_BACKGROUND),
                 k_blue_background  (text_color::REGULAR | text_color::BLUE_BACKGROUND),
                 k_purple_background(text_color::REGULAR | text_color::PURPLE_BACKGROUND),
                 k_cyan_background  (text_color::REGULAR | text_color::CYAN_BACKGROUND),
                 k_white_background (text_color::REGULAR | text_color::WHITE_BACKGROUND);

// Text modes and color modes.
text_color const k_bright       (text_color::REGULAR | text_color::BRIGHT),
                 k_dim          (text_color::REGULAR | text_color::DIM),
                 k_italic       (text_color::REGULAR | text_color::ITALIC),
                 k_underline    (text_color::REGULAR | text_color::UNDERLINE),
                 k_fast_blinking(text_color::REGULAR | text_color::FAST_BLINKING),
                 k_slow_blinking(text_color::REGULAR | text_color::SLOW_BLINKING),
                 k_reverse      (text_color::REGULAR | text_color::REVERSE),
                 k_hidden       (text_color::REGULAR | text_color::HIDDEN),
                 k_striked      (text_color::REGULAR | text_color::STRIKED);

std::unordered_map<std::ostream*, text_color> g_previous_color;

bool g_update_previous_color = true;

}

/**
 * @brief A wrapper class for a message object equipped with a color.
 */
struct colored_message {
    struct hash {
        std::size_t operator ()(colored_message const& cmsg) const noexcept;
    };

    /**
     * @brief Add color option to a message. Update the current colored message and return
     * its reference.
     */
    colored_message& with(text_color const& opts) & noexcept;

    /**
     * @brief Add color option to a message. Move the current colorered message to the 
     * return value. We need this overload to deal with colored_message objects as prvalues.
     * @example The colored message is returned by a function.
     * @code
     *      int main() {
     *          auto blue = text_color::of("blue");
     *          auto cmsg = blue("Hello").with(text_color::BOLD).with(text_color::UNDERLINE);
     *          std::cout << cmsg << std::endl;     // cmsg was moved twice, instead of copied.
     *      }
     */
    colored_message with(text_color const& opts) && noexcept;

    message msg;
    text_color color;
};

struct text_format {
    colored_message prefix = { .msg = {}, .color = colors::k_white };
    colored_message suffix = { .msg = {}, .color = colors::k_white };
    colored_message separator = { .msg = { " " }, .color = colors::k_white };
    mutable enum : unsigned {
        NONE = 0,
        DEBUG = 1,                  // Only print in debug mode.
        OVERRIDE = 0b10,            // Override previous formats.
        AUTO_WRAP = 0b100,          // Wrap the text if it exceeds some certain width
        TIME_STAMP = 0b1000,        // Add a time stamp to the message.
        SOURCE_LOCATION = 0b10000,  // Add the source location to the message.
    } type = DEBUG;
    unsigned args = 0;

    text_format(std::string p = "", std::string s = "", std::string sep = " ") noexcept
        : prefix({ .msg = { .content = std::move(p), .type = message::NONE }, .color = colors::k_white }),
          suffix({ .msg = { .content = std::move(s), .type = message::NONE }, .color = colors::k_white }),
          separator({ .msg = { .content = std::move(sep), .type = message::NONE }, .color = colors::k_white }) {}
    
    text_format separated_by(std::string s, text_color c = colors::k_white) && noexcept {
        separator.msg.content = std::move(s);
        separator.color = c;
        return std::move(*this);
    }

    text_format with_prefix(std::string p, text_color c = colors::k_white) && noexcept {
        prefix.msg.content = std::move(p);
        prefix.color = c;
        return std::move(*this);
    }

    text_format with_suffix(std::string s, text_color c = colors::k_white) && noexcept {
        suffix.msg.content = std::move(s);
        suffix.color = c;
        return std::move(*this);
    }
};

struct formatted_message {
    struct hash {
        std::size_t operator ()(formatted_message const& fmsg) const noexcept;
    };

    formatted_message& with(struct text_format const& opts) & noexcept;

    formatted_message with(struct text_format const& opts) && noexcept;

    message msg;
    text_format format;
};


namespace formats {

inline auto const k_default = text_format();

inline auto const k_newline = text_format().with_suffix("\n"),
                  k_double_newline = text_format().with_suffix("\n\n");


inline auto g_show_time = true;

inline auto g_show_source_location = true;

} // namespace formats


colored_message text_color::operator ()(std::string str) const {
    return { .msg = { .content = str, .type = message::NONE }, .color = *this };
}

colored_message text_color::operator ()(message const& msg) const {
    return { msg, *this };
}

colored_message text_color::operator ()(message&& cmsg) const noexcept {
    return { std::move(cmsg), *this };
}

colored_message text_color::operator ()(colored_message const& cmsg) const {
    auto result = cmsg;
    result.color.set(this->type);
    return result;
}

colored_message text_color::operator ()(colored_message&& cmsg) const noexcept {
    cmsg.color.type = text_color::enum_type(cmsg.color.type | this->type);
    return std::move(cmsg);
}

/**
 * @brief An IO manipulator that changes the color of the following text.
 */
std::ostream& operator <<(std::ostream& os, text_color color) {
    if (colors::g_update_previous_color) {
        colors::g_previous_color[&os] = color;
    }
    // If the color is RGB based:
    if (!color.is(text_color::REGULAR)) {
        auto const [r, g, b] = color.to_rgb();
        os << "\033[38m" << r << ';' << g << ';' << b << "\033[m";
        return os;
    }
    bool starting = true;
    auto const check_starting = [&os, &starting] {
        if (!starting) {
            os << ';';
        }
        else {
            starting = false;
        }
    };

    os << "\033[";
    if (color.text_mode()) {
        check_starting();
        os << color.text_mode();
    }
    if (color.color_mode()) {
        check_starting();
        os << color.color_mode();
    }
    if (color.to_regular_color()) {
        check_starting();
        os << color.to_regular_color();
    }
    if (color.to_regular_color_background()) {
        check_starting();
        os << color.to_regular_color_background();
    }
    return os << 'm';
}


/**
 * @brief Print the message with its message type.
 */
std::ostream& operator <<(std::ostream& os, message const& msg) {
    using namespace colors;

    std::ostream& operator <<(std::ostream&, colored_message const&);

    if (msg.type & message::DEBUG) {
        os << k_white("(Debug)").with(text_color::BRIGHT) << '\n';
    }

    switch (msg.type) {
    case message::INFO:    return os << k_white("[INFORMATION]") << k_blue(msg);
    case message::WARNING: return os << k_white("[WARNING]") << '\n' << k_yellow(msg);
    case message::ERROR:   return os << k_white("[ERROR]") << '\n' << k_red(msg);
    default:               return os << msg.content;
    }
}

std::size_t colored_message::hash::operator ()(colored_message const& cmsg) const noexcept {
    return message::hash()(cmsg.msg) ^ text_color::hash()(cmsg.color);
}

/**
 * @brief Print a colored message. The color of the text is changed temporarily, and
 * then restored to the previous color.
 */
std::ostream& operator <<(std::ostream& os, colored_message const& cmsg) {
    using namespace gltool::colors;
    if (!colors::g_previous_color.contains(&os)) {
        g_previous_color[&os] = g_default_color;
    }
    g_update_previous_color = false;
    os << cmsg.color << cmsg.msg << g_previous_color[&os];
    g_update_previous_color = true;
    return os;
}


colored_message& colored_message::with(text_color const& opts) & noexcept {
    color.type = static_cast<text_color::enum_type>(color.type | opts.type);
    return *this;
}

colored_message colored_message::with(text_color const& opts) && noexcept {
    color.type = static_cast<text_color::enum_type>(color.type | opts.type);
    return std::move(*this);
}


/**
 * @brief The logger class
 * Binds with a output stream (std::ostream&) and provides logging utilities.
 * If the logger owns the stream, it will be freed via RAII, and the global hashtable
 * (colors::k_previous_color) will also erase the stream pointer.
 */
class logger {
public:
    class logger_aux {
    public:
        logger_aux(logger& logger, 
                   std::source_location const location = std::source_location::current(),
                   std::chrono::system_clock::time_point const time = std::chrono::system_clock::now())
            : m_logger(logger), 
              m_location(location),
              m_time(time) {}

        template<typename T>
            requires (!std::is_scalar_v<T>)
        friend logger_aux&& operator <<(logger_aux&& logger, T* ptr) {
            std::move(logger) << typeid_name<T> << " Object at ";
            logger.m_logger.log(ptr);       // Avoid recursive call.
            return std::move(logger);
        }

        /**
         * @brief Stream operation for printable objects.
         */
        friend logger_aux&& operator <<(logger_aux&& logger, streamable auto&& obj) {
            logger.log_time();
            logger.log_location();
            logger.m_logged = true;
            logger.m_logger.log(std::forward<decltype(obj)>(obj), logger.m_indent);
            if (logger.m_indent) {
                logger.m_indent = false;
            }
            return std::move(logger);
        }

        friend logger_aux&& operator <<(logger_aux&& logger, std::ostream& (*manip)(std::ostream&)) {
            logger.m_logger.log(manip);
            return std::move(logger);
        }

        // TODO: Make this function more elegant.
        void log_location() {
            if (!m_logged && formats::g_show_source_location) {
                auto const path = stdf::path(m_location.file_name());
                m_logger.log("[ ", true);
                m_logger.log(colors::k_blue("FILE: "));
                m_logger.log(path.filename().string());
                m_logger.log("; ");
                m_logger.log(colors::k_blue("LINE: "));
                m_logger.log(m_location.line());
                m_logger.log(" in ");
                m_logger.log(colors::k_blue("FUNCTION: "));
                m_logger.log(m_location.function_name());
                m_logger.log(" ] \n");
            }
        }

        void log_time() {
            if (!m_logged && formats::g_show_time) {
                auto time = std::chrono::system_clock::to_time_t(m_time);
                auto gmt_time = std::gmtime(&time);
                m_logger.log("[ ", true);
                m_logger.log(colors::k_blue("TIME: "));
                m_logger.log(std::put_time(gmt_time, "%Y-%m-%d %H:%M:%S"));
                m_logger.log(" ] ");
            }
        }

    private:
        logger& m_logger;
        std::source_location m_location;
        std::chrono::system_clock::time_point m_time;
        bool m_logged = false;
        bool m_indent = true;
    };

    logger(std::nullptr_t)
        : m_out(std::cout),
          m_owning(false),
          m_active(false) {}

    logger(std::string const& path) try
        : logger(*new std::ofstream(path), true) {}
    catch (std::exception const& e) {
        std::cerr << colors::k_red("Error on contruction of logger object.") << e.what();
    }

    logger(std::ostream& out = std::cerr, bool owning = false)
        : m_out(out),
          m_owning(owning) {}

    virtual ~logger() {
        if (m_owning) {
            delete &m_out;
            if (colors::g_previous_color.contains(&m_out)) {
                colors::g_previous_color.erase(&m_out);
            }
        }
    }

    /**
     * @brief Get the auxiliary logger object, which carries the source location and time
     * of the caller.
     * 
     * @param location 
     * @param time 
     * @return logger_aux 
     */
    logger_aux operator ()(std::source_location location = std::source_location::current(),
                           std::chrono::system_clock::time_point time = std::chrono::system_clock::now()) {
        return logger_aux(*this, location, time);
    }

    /**
     * @brief Log error message and exit the program.
     * * This function throws an exception!
     */
    [[noreturn]]
    void exception(streamable auto&& msg) {
        m_out << colors::k_red << "Exception: " << msg << colors::k_white << std::endl;
        throw 42;
    }

    void indent() {
        ++m_indent;
    }

    void log(streamable auto&& msg, bool indent = false) {
        if (!m_active) {
            return;
        }
        if (indent) {
            int ct = m_indent;
            while (ct--) { 
                m_out << "  ";
            }
        }
        m_out << msg;
    }

    void log(std::ostream& (*manip)(std::ostream&)) {
        if (!m_active) {
            return;
        }
        manip(m_out);
    }

    template<typename... Args>
    void logf(char const* format, Args... args) {
        if (!m_active) {
            return;
        }
        char buf[1024];
        sprintf(buf, format, args...);
        m_out << buf;
    }

    void unindent() {
        --m_indent;
    }

private:
    std::ostream& m_out;
    unsigned m_indent = 0;
    bool m_owning;
    bool m_active = true;
};

#define INDENT(LOG) auto indent_guard = gltool::scoped_operation(LOG, &gltool::logger::indent, &gltool::logger::unindent)

} // namespace gltool

