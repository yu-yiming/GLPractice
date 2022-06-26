#pragma once

#include <concepts>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace gltool {


template<typename T, typename U>
concept mutref_of = std::same_as<std::decay_t<T>, std::decay_t<U>> 
                  && !std::same_as<T, std::add_const_t<std::remove_reference_t<U>>>;

template<typename T>
concept streamable = requires (std::decay_t<T> t) {
    { std::declval<std::ostream&>() << t } -> std::same_as<std::ostream&>;
};

std::string read_file(char const* path) {
    auto file = std::ifstream(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + std::string(path));
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

template<typename T>
struct scoped_operation {
    template<typename... Args>
    scoped_operation(T& object, void (T::* do_something)(Args...), void (T::* undo_something)(Args...), Args&&... args)
        : m_undo_function([&object, undo_something, &args...] { (object.*undo_something)(args...); }) {

        (object.*do_something)(args...);
    }
    
    template<typename... Args>
    scoped_operation(T const& object, void (T::* do_something)(Args...) const, void (T::* undo_something)(Args...) const, Args&&... args)
        : m_undo_function([&object, undo_something, &args...] { (object.*undo_something)(args...); }) {

        (object.*do_something)(args...);
    }

    ~scoped_operation() {
        m_undo_function();
    }

    std::function<void ()> m_undo_function;
};

} // namespace gl::detail