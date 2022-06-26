/**
 * @file application.hpp
 * @author Yiming Yu (yu.yiming.self@gmail.com)
 * @brief A header-only library of wrapped utilities for GLFW, based on C++20. 
 * Provides a OOP-like flavor for OpenGL programming.
 * @version 0.2 (under development)
 * @date 2022-06-22
 * 
 * @copyright Copyright (c) 2022
 * Licensed under the MIT License. See LICENSE file in the project root for full license information.
 * 
 */

#pragma once

#include "gl/opengl.hpp"

#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "log.hpp"
#include "utility.hpp"


#pragma region Customized Macro Fallbacks

/**
 * @brief Constants setup, library users can override them by defining corresponding macros before including this header.
 */
#ifndef M_GLFW_CONTEXT_MAJOR_VERSION
#define M_GLFW_CONTEXT_MAJOR_VERSION 3
#endif
#ifndef M_GLFW_CONTEXT_MINOR_VERSION
#define M_GLFW_CONTEXT_MINOR_VERSION 3
#endif
#ifndef M_GLFW_OPENGL_PROFILE
#define M_GLFW_OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE
#endif
#ifndef M_DEFAULT_BORDERED
#define M_DEFAULT_BORDERED true
#endif
#ifndef M_DEFAULT_CENTER_CURSOR
#define M_DEFAULT_CENTER_CURSOR false
#endif
#ifndef M_DEFAULT_DISABLE_CURSOR
#define M_DEFAULT_DISABLE_CURSOR false
#endif
#ifndef M_DEFAULT_FOCUSED
#define M_DEFAULT_FOCUSED true
#endif
#ifndef M_DEFAULT_FULLSCREEN
#define M_DEFAULT_FULLSCREEN false
#endif
#ifndef M_DEFAULT_HIDE_CURSOR
#define M_DEFAULT_HIDE_CURSOR false
#endif
#ifndef M_DEFAULT_MAXIMIZED
#define M_DEFAULT_MAXIMIZED false
#endif
#ifndef M_DEFAULT_RESIZABLE
#define M_DEFAULT_RESIZABLE true
#endif
#ifndef M_DEFAULT_TOPMOST
#define M_DEFAULT_TOPMOST false
#endif
#ifndef M_DEFAULT_TRANSPARENT
#define M_DEFAULT_TRANSPARENT false
#endif
#ifndef M_DEFAULT_VISIBLE
#define M_DEFAULT_VISIBLE true
#endif
#ifndef M_DEFAULT_WINDOW_HEIGHT
#define M_DEFAULT_WINDOW_HEIGHT 600
#endif
#ifndef M_DEFAULT_WINDOW_TITLE
#define M_DEFAULT_WINDOW_TITLE "OpenGL Application"
#endif
#ifndef M_DEFAULT_WINDOW_WIDTH
#define M_DEFAULT_WINDOW_WIDTH 800
#endif
#ifndef M_UNIFORM_MODEL_NAME
#define M_UNIFORM_MODEL_NAME "model"
#endif
#ifndef M_UNIFORM_PROJECTION_NAME
#define M_UNIFORM_PROJECTION_NAME "projection"
#endif
#ifndef M_UNIFORM_VIEW_NAME
#define M_UNIFORM_VIEW_NAME "view"
#endif
#ifndef M_FRONT_KEY
#define M_FRONT_KEY GLFW_KEY_W
#endif
#ifndef M_BACK_KEY
#define M_BACK_KEY GLFW_KEY_S
#endif
#ifndef M_LEFT_KEY
#define M_LEFT_KEY GLFW_KEY_A
#endif
#ifndef M_RIGHT_KEY
#define M_RIGHT_KEY GLFW_KEY_D
#endif

#pragma endregion

namespace gl {

#pragma region Forward Declarations

class application;

class buffer;

class camera;

class mesh;

class resource;

class resource_manager;

class shader;

class texture;

class vertex_array;

class window;

#pragma endregion

#pragma region Constants

/**
 * @brief namespace containing all constants that could be controlled by user-defined macros.
 */
namespace constants {

constexpr auto k_major_version           = M_GLFW_CONTEXT_MAJOR_VERSION;
constexpr auto k_minor_version           = M_GLFW_CONTEXT_MINOR_VERSION;
constexpr auto k_opengl_profile          = M_GLFW_OPENGL_PROFILE;

constexpr auto k_default_window_width    = M_DEFAULT_WINDOW_WIDTH;
constexpr auto k_default_window_height   = M_DEFAULT_WINDOW_HEIGHT;

constexpr auto k_default_bordered        = M_DEFAULT_BORDERED;
constexpr auto k_default_center_cursor   = M_DEFAULT_CENTER_CURSOR;
constexpr auto k_default_disable_cursor  = M_DEFAULT_DISABLE_CURSOR;
constexpr auto k_default_focused         = M_DEFAULT_FOCUSED;
constexpr auto k_default_fullscreen      = M_DEFAULT_FULLSCREEN;
constexpr auto k_default_hide_cursor     = M_DEFAULT_HIDE_CURSOR;
constexpr auto k_default_maximized       = M_DEFAULT_MAXIMIZED;
constexpr auto k_default_resizable       = M_DEFAULT_RESIZABLE;
constexpr auto k_default_topmost         = M_DEFAULT_TOPMOST;
constexpr auto k_default_transparent     = M_DEFAULT_TRANSPARENT;
constexpr auto k_default_visible         = M_DEFAULT_VISIBLE;
constexpr auto k_default_window_title    = M_DEFAULT_WINDOW_TITLE;

constexpr auto k_uniform_model_name      = M_UNIFORM_MODEL_NAME;
constexpr auto k_uniform_projection_name = M_UNIFORM_PROJECTION_NAME;
constexpr auto k_uniform_view_name       = M_UNIFORM_VIEW_NAME;

constexpr auto k_front_key               = M_FRONT_KEY;
constexpr auto k_back_key                = M_BACK_KEY;
constexpr auto k_left_key                = M_LEFT_KEY;
constexpr auto k_right_key               = M_RIGHT_KEY;

} // namespace constants

#pragma endregion Constants

#pragma region Global States

/**
 * @brief The main logger, bound to std::cout.
 */
inline auto LOG = gltool::logger(std::cout);

/**
 * @brief namespace containing all global variables that are mutable, i.e. states of the application.
 */
namespace states {

inline auto g_glfw_initialized = false;

inline auto g_glew_initialized = false;

inline auto g_application_created = false;

inline auto g_buffer_ct = 0;

inline auto g_camera_ct = 0;

inline auto g_mesh_ct = 0;

inline auto g_shader_ct = 0;

inline auto g_texture_ct = 0;

inline auto g_vertex_array_ct = 0;

inline auto g_window_ct = 0;

inline std::once_flag g_glfw_init_flag;

inline std::once_flag g_glew_init_flag;

inline std::once_flag g_resource_init_flag;

std::unique_ptr<resource_manager> g_resource_manager;

void resource_initialize() {
    std::call_once(states::g_resource_init_flag, [] {
        states::g_resource_manager = std::make_unique<resource_manager>();
    });
}

/**
 * @brief Initialize the GLFW library.
 * This is basically a wrapper around glfwInit().
 */
inline void glfw_initialize() {
    INDENT(LOG);
    LOG() << "Check if GLFW is initialized..." << std::endl;
    std::call_once(g_glfw_init_flag, [] {
        if (!glfw::init()) {
            LOG.exception("Failed to initialize GLFW");
        }
    });
    LOG() << "GLFW initialized." << std::endl;
}

/**
 * @brief Give hints to the next call of glfwCreateWindow(). See glfwWindowHint() for more details.
 * This function is a wrapper around glfwWindowHint(), so that we can pass all hints in one call.
 * 
 * @param hints A vector of hints, e.g. can be passed like { hint_1, value_1, hint_2, value_2, ... }.
 */
inline void glfw_hints(std::vector<gl::i32> const& hints) {
    INDENT(LOG);
    if (hints.size() % 2 != 0) {
        LOG.exception("glfwhints: hints must be a list of key-value pairs");
    }
    if (hints.size() == 0) {
        return;
    }
    for (gl::i32 i = 0; i < hints.size(); i += 2) {
        LOG() << "Giving GLFW hints on windows";
        glfw::window_hint(hints[i], hints[i + 1]);
    }
}

/**
 * @brief Another flavor of glfw_hints(). Pass the hints as parameter packs.
 * 
 * @param hint The first hint.
 * @param value The value of the first hint.
 * @param args Parameter packs of hints and values, or vectors of hints and values.
 */
void glfw_hints(gl::i32 hint, gl::i32 value, auto&&... args) {
    glfw::window_hint(hint, value);
    if constexpr (sizeof...(args) > 0) {
        glfw_hints(std::forward<decltype(args)>(args)...);
    }
}

void glew_initialize() {
    std::call_once(g_glew_init_flag, [] {
        // Warning: glewInit() must be called AFTER glfwMakeContextCurrent() is called,
        // which is done in the constructor of the window class.
        if (glewInit() != GLEW_OK) {
            glfw::terminate();
            LOG.exception("Failed to initialize GLFW");
        }
    });
}

/**
 * @brief Get the generator of names.
 * 
 * @param prefix 
 * @param counter 
 * @return A generator of names that takes a record of old names and returns a new name.
 */
inline auto next_name(std::string const& prefix, gl::i32& counter) {
    return [prefix, &counter] (auto&& record, std::string const& hint = "") -> std::string {
        INDENT(LOG);
        LOG() << "Finding next available resource name with hint " << hint << std::endl;
        if (record.find(hint) == record.cend()) {
            LOG() << "Provided hint name is available" << std::endl;
            return hint;
        }
        auto ss = std::stringstream();
        while (true) {
            ss << hint << prefix << counter++;
            auto name = ss.str();
            if (!record.contains(name)) {
                LOG() << "Next available name found: " << name << '\n';
                return name;
            }
        }
        [[unlikely]]
        return "";
    };
}

inline auto const next_vertex_array_name = next_name("generated-vao-", g_vertex_array_ct);

inline auto const next_buffer_name = next_name("generated-bo-", g_buffer_ct);

inline auto const next_camera_name = next_name("generated-camera-", g_camera_ct);

inline auto const next_mesh_name = next_name("generated-mesh-", g_mesh_ct);

inline auto const next_shader_name = next_name("generated-shader-", g_shader_ct);

inline auto const next_texture_name = next_name("generated-texture-", g_texture_ct);

inline auto const next_window_name = next_name("Generated Window ", g_window_ct);

} // namespace states

#pragma endregion // Global States

#pragma region Auxiliary Structs

/**
 * @brief Auxiliary classes to the functions.
 */
namespace aux {

struct size {
    gl::i32 width = 0;
    gl::i32 height = 0;
};

struct fsize {
    gl::f32 width = 0;
    gl::f32 height = 0;
};

struct pos {
    gl::i32 x = 0;
    gl::i32 y = 0;
};

struct fpos {
    gl::f32 x = 0.f;
    gl::f32 y = 0.f;
};

struct color {
    gl::f32 r = 0.f;
    gl::f32 g = 0.f;
    gl::f32 b = 0.f;
    gl::f32 a = 0.f;
};

/**
 * @brief GLFW window specifications. Aggregation of windows properties and GFLW 
 * window hints (see https://www.glfw.org/docs/3.3/window_guide.html#window_hints).
 * Used as the parameter of the gl::window constructor.
 */
struct window_specification {
    enum trait : gl::e32 {
        BORDERED        = 1,    // The window has a border (with title bar and close buttons).
        CENTER_CURSOR   = 2,    // The cursor is centered in the window.
        DISABLE_CURSOR  = 4,    // The cursor is disabled (unlimited cursor movement).
        FOCUSED         = 8,    // The window is focused (accepts input).
        FULLSCREEN      = 16,   // The window is displayed in fullscreen mode.
        HIDE_CURSOR     = 32,   // The cursor is hidden and the mouse is locked.
        MAXIMIZED       = 64,   // The window is maximized.
        RESIZABLE       = 128,  // The window is resizable.
        TOPMOST         = 256,  // The window is always on top of other windows.
        TRANSPARENT     = 512,  // The window is transparent.
        VISIBLE         = 1024, // The window is visible.
    };
    static constexpr auto k_default_trait = constants::k_default_bordered << 0       |
                                            constants::k_default_center_cursor << 1  |
                                            constants::k_default_disable_cursor << 2 |
                                            constants::k_default_focused << 3        |
                                            constants::k_default_fullscreen << 4     |
                                            constants::k_default_hide_cursor << 5    |
                                            constants::k_default_maximized << 6      |
                                            constants::k_default_resizable << 7      |
                                            constants::k_default_topmost << 8        |
                                            constants::k_default_transparent << 9    |
                                            constants::k_default_visible << 10;

    char const* title               = constants::k_default_window_title;
    gl::i32 width                   = constants::k_default_window_width;
    gl::i32 height                  = constants::k_default_window_height;
    gl::e32 traits                  = k_default_trait;
    gl::i32 major_version           = constants::k_major_version;
    gl::i32 minor_version           = constants::k_minor_version;
    gl::i32 opengl_profile          = constants::k_opengl_profile;
    glfw::window_handle shared_with = nullptr;
    std::vector<gl::i32> hints;
};

} // namespace aux

#pragma endregion // Auxiliary Structs

#pragma region Shader Class

/**
 * @brief The shader class that holds the compiled shader program and its uniforms.
 */
class shader {
public:
    friend class resource_manager;
    friend class mesh;

    shader() = default;

    shader(char const* vertex_shader_path, char const* fragment_shader_path)
        : m_vertex_shader_path(vertex_shader_path),
          m_fragment_shader_path(fragment_shader_path) {

        INDENT(LOG);
        LOG() << "Constructing shader object " << this << " with given paths"
              << "(Vertex shader: " << vertex_shader_path << "; Fragment shader: " << fragment_shader_path << ")" << std::endl;
        this->reload();     // Load the shader program.

        // Retrieve the uniform locations.
        m_uniform_model = gl::get_uniform_location(m_program, constants::k_uniform_model_name);
        m_uniform_view = gl::get_uniform_location(m_program, constants::k_uniform_view_name);
        m_uniform_projection = gl::get_uniform_location(m_program, constants::k_uniform_projection_name);
    }

    shader(gl::u32 program, gl::u32 uniform_model, gl::u32 uniform_view, gl::u32 uniform_projection, bool owning = true)
        : m_program           (program == 0            ? gl::create_program() : program),
          m_uniform_model     (uniform_model == 0      ? gl::get_uniform_location(m_program, constants::k_uniform_model_name) : uniform_model),
          m_uniform_view      (uniform_view == 0       ? gl::get_uniform_location(m_program, constants::k_uniform_view_name) : uniform_view),
          m_uniform_projection(uniform_projection == 0 ? gl::get_uniform_location(m_program, constants::k_uniform_projection_name) : uniform_projection),
          m_owning            (owning) {

        // If the passed program is not 0, then the caller owns the program object.
        if (program != 0 && !owning) {
            m_owning = false;
        }
    }

    shader(shader const& other) = delete;

    shader(shader&& other) noexcept
        : m_vertex_shader_path(other.m_vertex_shader_path),
          m_fragment_shader_path(other.m_fragment_shader_path),
          m_program(other.m_program),
          m_uniform_model(other.m_uniform_model),
          m_uniform_view(other.m_uniform_view),
          m_uniform_projection(other.m_uniform_projection),
          m_owning(other.m_owning) {

        INDENT(LOG);
        LOG() << "Moving shader object from " << &other << " to " << this << std::endl;
        other.m_owning = false;
    }

    shader& operator =(shader const& other) = delete;

    shader& operator =(shader&& other) noexcept {
        INDENT(LOG);
        LOG() << "Moving shader object from " << &other << " to " << this << std::endl;
        if (m_program == other.m_program) {
            return *this;
        }
        if (m_program != 0) {
            LOG() << "Deleting current shader program" << std::endl;
            gl::delete_program(m_program);
        }
        m_program = other.m_program;
        m_uniform_model = other.m_uniform_model;
        m_uniform_view = other.m_uniform_view;
        m_uniform_projection = other.m_uniform_projection;
        other.m_owning = false;
        return *this;
    }

    ~shader() {
        INDENT(LOG);
        LOG() << "Destructing shader object " << this << std::endl;
        if (m_owning) {
            LOG() << "Deleting current shader program" << std::endl;
            gl::delete_program(m_program);
        }
    }

    /**
     * @brief Compile shader source code and attach it to the shader program.
     * 
     * @param source_raw Shader source code.
     * @param type Either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     */
    void add_shader(char const* source_raw, GLenum type) {
        INDENT(LOG);
        LOG() << "Adding shader to " << this << std::endl;

        auto const shader = gl::create_shader(type);
        gl::shader_source(shader, 1, &source_raw, nullptr);
        gl::compile_shader(shader);

        check_status(shader);

        gl::attach_shader(m_program, shader);
        gl::delete_shader(shader);

        LOG() << "Shader added" << std::endl;
    }

    /**
     * @brief Compile and link the shader program.
     * 
     * @param source_1 @param source_2 Shader source code.
     * @param type_1 @param type_2 Either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
     */
    void bind(char const* source_1, GLenum type_1, char const* source_2 = nullptr, GLenum type_2 = GL_NONE) {
        INDENT(LOG);
        LOG() << "Binding multiple shaders to " << this << std::endl;

        if (m_owning) {
            LOG() << "Deleting current shader program" << std::endl;
            gl::delete_program(m_program);
        }
        m_program = gl::create_program();
        if (source_1 != nullptr) {
            this->add_shader(source_1, type_1);
        }
        if (source_2 != nullptr) {
            this->add_shader(source_2, type_2);
        }
        LOG() << "Linking shader program" << std::endl;
        gl::link_program(m_program);
        check_status(m_program);
    }

    void clear() {
        INDENT(LOG);
        LOG() << "Clearing shader object: " << this << std::endl;
        if (m_program != 0 && m_owning) {
            LOG() << "Deleting current shader program" << std::endl;
            gl::delete_program(m_program);
            m_owning = false;
        }
        m_program = 0;
        m_uniform_model = 0;
        m_uniform_projection = 0;
        m_uniform_view = 0;
    }

    static shader from_sources(char const* vertex_shader_source, char const* fragment_shader_source) {
        auto result = shader();
        result.bind(vertex_shader_source, GL_VERTEX_SHADER, fragment_shader_source, GL_FRAGMENT_SHADER);
        return result;
    }

    gl::u32 get_uniform_model() const noexcept {
        return m_uniform_model;
    }

    gl::u32 get_uniform_projection() const noexcept {
        return m_uniform_projection;
    }

    gl::u32 get_uniform_view() const noexcept {
        return m_uniform_view;
    }

    bool initialized() const noexcept {
        return m_program != 0;
    }

    bool is_wrapper_of(gl::u32 program) const noexcept {
        return m_program == program;
    }

    /**
     * @brief Reload the shader program from the source files.
     * This may be useful when the shader source code has been changed.
     */
    void reload() {
        INDENT(LOG);
        LOG() << "Loading shader sources from "
              << "vertex shader (path: " << m_vertex_shader_path << "), "
              << "fragment shader (path: " << m_fragment_shader_path << ")" << std::endl;

        if (m_owning) {
            LOG() << "Deleting current shader program" << std::endl;
            gl::delete_program(m_program);
        }
        try {
            auto vertex_source = gltool::read_file(m_vertex_shader_path);
            auto fragment_source = gltool::read_file(m_fragment_shader_path);
            this->bind(vertex_source.c_str(), GL_VERTEX_SHADER, fragment_source.c_str(), GL_FRAGMENT_SHADER);
        }
        // In case the gltool::read_file fails (e.g. file not found)
        catch (std::exception& e) {
            LOG() << "Error loading shader sources: ";
            LOG.exception(e.what());
        }

        m_owning = true;
        LOG() << "Shader loaded" << std::endl;
    }

    /**
     * @brief Set the uniform object in the shader program.
     * 
     * @param location_name Either "model", "projection", or "view".
     * @param value Pointer to a matrix object.
     */
    void set_uniform(std::string const& location_name, gl::f32 const* value) {
        if (location_name == constants::k_uniform_model_name) {
            gl::uniform_mat4f(m_uniform_model, 1, GL_FALSE, value);
        } 
        else if (location_name == constants::k_uniform_projection_name) {
            gl::uniform_mat4f(m_uniform_projection, 1, GL_FALSE, value);
        } 
        else if (location_name == constants::k_uniform_view_name) {
            gl::uniform_mat4f(m_uniform_view, 1, GL_FALSE, value);
        } 
        else {
            LOG.exception("Unknown uniform location name: " + location_name);
        }
    }

    /**
     * @brief Unbind the shader program.
     */
    void unbind() const {
        gl::use_program(0);
    }

    /**
     * @brief Bind the shader program.
     */
    void bind() const {
        if (this->initialized()) {
            gl::use_program(m_program);
        }
    }

    friend bool operator ==(shader const& lhs, shader const& rhs) noexcept {
        return lhs.m_program == rhs.m_program;
    }

private:
    /**
     * @brief Check the status of a OpenGL object, either a shader or a shader program.
     * Throw an exception if input object is neither of type GL_SHADER nor GL_PROGRAM.
     * @param object 
     */
    static void check_status(gl::u32 object) {
        INDENT(LOG);
        LOG() << "Checking status of the OpneGL object " << object << std::endl;

        gl::i32 status;
        gl::c8 log[512];
        if (gl::is_shader(object)) {
            gl::get_shader_iv(object, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE) {
                gl::get_shader_info_log(object, 512, nullptr, log);
                std::cerr << "Shader compilation failed" << std::endl;
                LOG.exception(log);
            }
        }
        else if (gl::is_program(object)) {
            gl::get_program_iv(object, GL_LINK_STATUS, &status);
            if (status == GL_FALSE) {
                gl::get_program_info_log(object, 512, nullptr, log);
                std::cerr << "Shader program linking failed" << std::endl;
                LOG.exception(log);
            }
            gl::validate_program(object);
            gl::get_program_iv(object, GL_VALIDATE_STATUS, &status);
            if (status == GL_FALSE) {
                gl::get_program_info_log(object, 512, nullptr, log);
                std::cerr << "Shader program validation failed" << std::endl;
                LOG.exception(log);
            }
        }
        else {
            LOG.exception("Object is not a shader or program");
        }

        LOG() << "Status checked" << std::endl;
    }

    char const* m_vertex_shader_path    = nullptr;
    char const* m_fragment_shader_path  = nullptr;
    gl::u32     m_program               = 0;
    gl::u32     m_uniform_model         = 0;
    gl::u32     m_uniform_view          = 0;
    gl::u32     m_uniform_projection    = 0;
    bool        m_owning                = true;
};

#pragma endregion // Shader Class

#pragma region Texture Class

// TODO
class texture {
public:
    friend class resource_manager;
    friend class mesh;
    
private:
    gl::u32 m_texture;
    bool m_initialized = false;
};

#pragma endregion // Texture Class

#pragma region Buffer Class

/**
 * @brief Buffer object wrapper,
 * Holding the handle to a buffer object (VBO or EBO), owning or non-owning.
 */
class buffer {
public:
    friend class resource_manager;
    friend class mesh;

    /**
     * @brief Construct a new buffer object of given type. If the parameter is omitted,
     * this will construct a vertex buffer object (Default construction).
     * 
     * @param type Either GL_ARRAY_BUFFER (VBO) or GL_ELEMENT_ARRAY_BUFFER (EBO).
     */
    buffer(gl::e32 type = GL_ARRAY_BUFFER)
        : m_object(gl::generate_buffer()),
          m_type(type) {

        INDENT(LOG);
        m_object = gl::generate_buffer();
        LOG() << "Generated buffer object: " << m_object << " owned by " << this << std::endl;
    }

    /**
     * @brief Construct a wrapper of an existing buffer object.
     * 
     * @param object The buffer object handle.
     * @param type Either GL_ARRAY_BUFFER (VBO) or GL_ELEMENT_ARRAY_BUFFER (EBO).
     * @param owning true if you want to manage the buffer object using RAII. Defaulted
     * to false so that the caller free the object itself.
     */
    buffer(gl::u32 object, gl::e32 type, bool owning = true)
        : m_object(object == 0 ? gl::generate_buffer() : object), 
          m_type(type), 
          m_owning(owning || object == 0) {

        INDENT(LOG);
        LOG() << "Wrapping existing buffer object: " << m_object << " with " << this
              << "(owning status: " << m_owning << ")" << std::endl;
    }

    buffer(buffer const&) = delete;

    buffer(buffer&& other) noexcept
        : m_object(other.m_object),
          m_type(other.m_type),
          m_owning(other.m_owning) {
        
        INDENT(LOG);
        LOG() << "Moving buffer object from " << &other << " to " << this << std::endl;
        other.m_owning = false;
    }

    ~buffer() {
        INDENT(LOG);
        LOG() << "Destructing buffer object: " << m_object << " owned by " << this << std::endl;
        this->clear();
    }

    buffer& operator =(buffer const&) = delete;

    buffer& operator =(buffer&& other) noexcept {
        INDENT(LOG);
        LOG() << "Moving buffer object from " << &other << " to " << this << std::endl;

        if (m_object == other.m_object) {
            return *this;
        }
        if (m_owning) {
            LOG() << "Deleting current buffer object: " << m_object << " ownend by " << this << std::endl;
            gl::delete_buffer(m_object);
        }
        m_object = other.m_object;
        m_type = other.m_type;
        m_owning = other.m_owning;
        other.m_owning = false;
        return *this;
    }

    /**
     * @brief Bind the buffer object.
     */
    void bind() const {
        INDENT(LOG);
        LOG() << "Binding current buffer object: " << m_object << " owned by " << this << std::endl;
        gl::bind_buffer(m_type, m_object);
    }

    /**
     * @brief Bind the buffer object to the given target.
     * 
     * @param data The buffer data.
     */
    template<typename T>
    void bind(std::span<T const> data) const {
        INDENT(LOG);
        LOG() << "Binding current buffer object: " << m_object << " owned by " << this << std::endl;
        gl::bind_buffer(m_type, m_object);
        LOG() << "Binding data (size: " << sizeof(T) * data.size() << " bytes) to buffer object: " << m_object << " owned by " << this << std::endl;
        gl::buffer_data(m_type, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
    }

    void clear() {
        INDENT(LOG);
        LOG() << "Clearing buffer object: " << m_object << " owned by " << this << std::endl;
        if (m_owning) {
            gl::delete_buffer(m_object);
            LOG() << "Buffer object deleted" << std::endl;
            m_owning = false;
        }
    }

    /**
     * @brief Test if the current object holds the given OpenGL buffer object.
     */
    bool is_wrapper_of(gl::u32 object) const {
        return m_object == object;
    }

    /**
     * @brief Unbind the buffer object.
     */
    void unbind() const {
        gl::bind_buffer(m_type, 0);
    }

    friend bool operator ==(buffer const& lhs, buffer const& rhs) noexcept {
        return lhs.m_object == rhs.m_object;
    }

private:
    gl::u32 m_object = 0;
    bool m_type = GL_ARRAY_BUFFER;
    bool m_owning = true;
};

#pragma endregion // Buffer Class

#pragma region Vertex Array Class

/**
 * @brief Vertex array object wrapper.
 * Holding the handle of a VAO, owning or non-owning.
 */
class vertex_array {
public:
    friend class resource_manager;
    friend class mesh;

    vertex_array()
        : m_object(gl::generate_vertex_array()) {}

    /**
     * @brief Construct a wrapper of an existing vertex array object.
     * 
     * @param object The VAO handle. If null (0) is passed, a new VAO will be generated.
     * @param owning true if you want to manage the VAO via RAII, defaulted to false so
     * that the caller free the resource itself. If @param object is null (0), this
     * parameter is ignored.
     */
    vertex_array(gl::u32 object, bool owning = true)
        : m_object(object == 0 ? gl::generate_vertex_array() : object), 
          m_owning(owning || object == 0) {}

    vertex_array(vertex_array const&) = delete;

    /**
     * @brief Move constructor. Transfer ownership of the VAO, but won't clear the moved 
     * vertex_array object @param other. If the @param other object doesn't own the VAO,
     * this is very much the same as a copy constructor.
     * 
     * @param other 
     */
    vertex_array(vertex_array&& other) noexcept
        : m_object(other.m_object),
          m_owning(other.m_owning) {

        other.m_owning = false;
    }

    ~vertex_array() {
        this->clear();
    }

    vertex_array& operator =(vertex_array const&) = delete;

    /**
     * @brief Move assignment operator. Free the current VAO and claim ownership of the 
     * @param other VAO. If the @param other object doesn't own the VAO, this is very much
     * the same as a copy assignment operator.
     * 
     * @param other 
     * @return vertex_array& 
     */
    vertex_array& operator =(vertex_array&& other) noexcept {
        INDENT(LOG);
        LOG() << "Moving vertex array object from " << &other << " to " << this << std::endl;
        if (m_object == other.m_object) {
            return *this;
        }
        if (m_owning) {
            LOG() << "Deleting current vertex array object: " << m_object << " owned by " << this << std::endl;
            gl::delete_vertex_array(m_object);
        }
        m_object = other.m_object;
        m_owning = other.m_owning;
        other.m_owning = false;
        LOG() << "Vertex array object moved" << std::endl;
        return *this;
    }

    /**
     * @brief Bind some buffers to the vertex array object.
     * !warning: The buffers are bound to null (0) after this call.
     * 
     * @param set_buffer_function The function to bind the buffers, customized
     * by the caller.
     */
    void bind(auto&& set_buffer_function) {
        gl::bind_vao(m_object);

        set_buffer_function();

        gl::bind_vao(0);
        gl::bind_buffer(GL_ARRAY_BUFFER, 0);

        // The EBO must be unbound after the VAO is unbound.
        gl::bind_buffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void clear() {
        INDENT(LOG);
        LOG() << "Clearing vertex array object: " << m_object << " owned by " << this << std::endl;
        if (m_owning) {
            gl::delete_vertex_array(m_object);
            m_owning = false;
        }
        LOG() << "Vertex array object deleted" << std::endl;
    }

    bool is_wrapper_of(gl::u32 object) const {
        return m_object == object;
    }

    friend bool operator ==(vertex_array const& lhs, vertex_array const& rhs) noexcept {
        return lhs.m_object == rhs.m_object;
    }

private:
    gl::u32 m_object;
    bool m_owning = true;
};

#pragma endregion // Vertex Array Class

#pragma region Mesh Class

/**
 * @brief A collection of vertices and indices that is basically a wrapper of a VAO,
 * a VBO and an EBO. The mesh object could be the owner of these objects or not. In 
 * the latter case, the caller is responsible for freeing the objects (usually, you
 * should use a resource object to construct meshes, in which case the VAO/VBO/EBO
 * are also managed by it).
 */
class mesh {
public:
    enum : gl::e32 { 
        NON_OWNING = 0,
        OWN_VAO = 0x1,
        OWN_VBO = 0x2,
        OWN_EBO = 0x4,
        OWN_ALL = OWN_VAO | OWN_VBO | OWN_EBO
    };

    friend class resource_manager;

    mesh()
        : m_array(0),
          m_vertices(0, GL_ARRAY_BUFFER),
          m_indices(0, GL_ELEMENT_ARRAY_BUFFER) {}

    /**
     * @brief Construct a new mesh object from exisiting VAO, VBO, and EBO. The mesh
     * object may own the objects if the passed object are null (equal to 0) or the
     * @param owning is set with certain flags. The RAII operations are resolved by 
     * the member variables.
     * 
     * @param vao The VAO handle.
     * @param vbo The VBO handle.
     * @param ebo The EBO handle.
     * @param owning
     */
    mesh(gl::u32 vao, gl::u32 vbo, gl::u32 ebo, gl::s32 index_ct, gl::e32 owning = OWN_ALL)
        : m_array   (vao == 0 ? gl::generate_vertex_array() : vao, vao == 0 || owning & OWN_VAO), 
          m_vertices(vbo == 0 ? gl::generate_buffer()       : vbo, GL_ARRAY_BUFFER, vbo == 0 || owning & OWN_VAO), 
          m_indices (ebo == 0 ? gl::generate_buffer()       : ebo, GL_ELEMENT_ARRAY_BUFFER, ebo == 0 || owning & OWN_EBO),
          m_index_ct(index_ct) {}

    /**
     * @brief Construct a new mesh object from exisiting VAO, VBO, and EBO wrappers. The
     * member variables of this mesh object are moved from the passed objects, so they may
     * take the ownership of the objects. Still, the caller can explicitly specify the 
     * expected ownership by the @param owning parameter. The RAII mechanism plays a role
     * when the passed objects have the ownership *and* the @param owning parameter is set
     * at the corresponding bit.
     * 
     * @param array
     * @param vertices 
     * @param indices 
     * @param owning 
     */
    mesh(vertex_array& array, buffer& vertices, buffer& indices, gl::s32 index_ct, gl::e32 owning = OWN_ALL)
        : mesh(array.m_object, vertices.m_object, indices.m_object, index_ct, owning) {

        if (owning & OWN_VAO) {
            array.m_owning = false;
        }
        if (owning & OWN_VBO) {
            vertices.m_owning = false;
        }
        if (owning & OWN_EBO) {
            indices.m_owning = false;
        }
    }

    /**
     * @brief Copy constructor is deleted.
     */
    mesh(mesh const& other) = delete;

    /**
     * @brief Move constructor. Transfer ownership of the VAO, VBO, and EBO of the
     * @param other mesh object. Note that this basically copies another mesh, but the newly
     * created mesh object takes the ownership of the resources if they are owned by the 
     * @param other mesh object.
     * 
     * @param other 
     */
    mesh(mesh&& other) noexcept
        : m_array(std::move(other.m_array)),
          m_vertices(std::move(other.m_vertices)),
          m_indices(std::move(other.m_indices)),
          m_index_ct(other.m_index_ct) {}

    /**
     * @brief Construct a new mesh object from a real vertex array and an index
     * array. Both arrays are flatten lists of triplets. For the vertices, the 
     * triplets are points in 3D space, while for the indices, the triplets are
     * indices in the first array that form triangles.
     * 
     * @param vertices A float array of vertex locations.
     * @param indices A unsigned int array of indices.
     * 
     * @example The following is an example of a mesh with 4 vertices and 4 triangles:
     * @code 
     *      gl::f32 vertices[] = {
     *         -1.0f, -1.0f, 0.0f,  // 0 - bottom left on the x-y plane
     *          1.0f, -1.0f, 0.0f,  // 1 - bottom right on the x-y plane
     *          0.0f,  1.0f, 0.0f,  // 2 - top center on the x-y plane
     *          0.0f,  0.0f, 1.0f   // 3 - up above the origin
     *      };
     *      gl::u32 indices[] = {
     *          0, 1, 2,
     *          0, 2, 3,
     *          0, 3, 1,
     *          1, 3, 2
     *      };
     *      auto tetrahedron = gl::mesh(vertices, indices);
     */
    mesh(std::span<gl::f32 const> vertices, std::span<gl::u32 const> indices)
        : mesh() {

        m_index_ct = indices.size();
        m_array.bind([this, &vertices, &indices]() {
            m_vertices.bind(vertices);
            m_indices.bind(indices);
            
            gl::vertex_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            gl::enable_vertex_attrib_array(0);
        });
    }

    /**
     * @brief Copy assignment operator is deleted.
     */
    mesh& operator =(mesh const&) = delete;

    /**
     * @brief Move assignment operator. 
     * 
     * @param other 
     * @return mesh& 
     */
    mesh& operator =(mesh&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        m_array = std::move(other.m_array);
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_index_ct = other.m_index_ct;
        return *this;
    }

    void clear() {
        m_array.clear();
        m_vertices.clear();
        m_indices.clear();
    }

    void render() {
        m_array.bind([this] {
            m_indices.bind();
            gl::draw_elements(GL_TRIANGLES, m_index_ct, GL_UNSIGNED_INT, nullptr);
        });
    }

    friend bool operator ==(mesh const& lhs, mesh const& rhs) {
        return lhs.m_array == rhs.m_array &&
               lhs.m_vertices == rhs.m_vertices &&
               lhs.m_indices == rhs.m_indices;
    }

private:
    vertex_array m_array;               /* Vertex array object (VAO) */
    buffer       m_vertices;            /* Vertex buffer object (VBO) */
    buffer       m_indices;             /* Element buffer object (EBO) */
    gl::s32      m_index_ct = 0;        /* Number of indices */
};

#pragma endregion // Mesh Class

#pragma region Camera Class

class camera {
    struct impl {
        glm::vec3 position = glm::vec3(1.f);
        glm::vec3 front    = glm::vec3(1.f);
        glm::vec3 right    = glm::vec3(1.f);
        glm::vec3 up       = glm::vec3(1.f);
        glm::vec3 world_up = glm::vec3(1.f);

        gl::f32 yaw = 0.f;
        gl::f32 pitch = 0.f;

        gl::f32 move_speed = 0.f;
        gl::f32 turn_speed = 0.f;
    };
public:
    camera(glm::vec3 initial_position, glm::vec3 initial_world_up, gl::f32 initial_yaw, gl::f32 initial_pitch, 
           gl::f32 initial_move_speed, gl::f32 initial_turn_speed)
        : m_pimpl(std::make_unique<impl>(impl {
            .position = initial_position,
            .world_up = initial_world_up,
            .yaw = initial_yaw,
            .pitch = initial_pitch,
            .move_speed = initial_move_speed,
            .turn_speed = initial_turn_speed
        })) {

        this->update();
    }

    camera(camera const& other) = delete;

    camera(camera&& other) noexcept = default;

    camera& operator =(camera const& other) = delete;

    camera& operator =(camera&& other) noexcept = default;

    glm::mat4 get_view_matrix() const {
        return glm::lookAt(m_pimpl->position, m_pimpl->position + m_pimpl->front, m_pimpl->up);
    }

    /**
     * @brief Calculate the front, right, and up vectors of the camera based
     * on the yaw, pitch, and world up vectors.
     */
    void update() {
        m_pimpl->front.x = cos(glm::radians(m_pimpl->yaw)) * cos(glm::radians(m_pimpl->pitch));
        m_pimpl->front.y = sin(glm::radians(m_pimpl->pitch));
        m_pimpl->front.z = sin(glm::radians(m_pimpl->yaw)) * cos(glm::radians(m_pimpl->pitch));
        m_pimpl->front = glm::normalize(m_pimpl->front);
        m_pimpl->right = glm::normalize(glm::cross(m_pimpl->front, m_pimpl->world_up));
        m_pimpl->up    = glm::normalize(glm::cross(m_pimpl->right, m_pimpl->front));
    }

// Slots:

    /**
     * @brief The camera's position is updated when certain signals (key press)
     * are received.
     * 
     * @param keys The whole key state provided by the window.
     * @param delta_time The elapsed time since last update.
     * TODO: Consider a better way to handle key press events.
     */
    void on_key_pressed(bool keys[], gl::f32 delta_time) {
        gl::f32 velocity = m_pimpl->move_speed * delta_time;
        if (keys[constants::k_front_key]) {
            m_pimpl->position += m_pimpl->front * velocity;
        }
        if (keys[constants::k_back_key]) {
            m_pimpl->position -= m_pimpl->front * velocity;
        }
        if (keys[constants::k_left_key]) {
            m_pimpl->position -= m_pimpl->right * velocity;
        }
        if (keys[constants::k_right_key]) {
            m_pimpl->position += m_pimpl->right * velocity;
        }
    }

    /**
     * @brief The camera's direction is updated when certain signals (mouse move)
     * are received.
     * 
     * @param x_delta The change in x-coordinate since last update.
     * @param y_delta The change in y-coordinate since last update.
     */
    void on_mouse_moved(gl::f32 x_delta, gl::f32 y_delta) {
        x_delta *= m_pimpl->turn_speed;
        y_delta *= m_pimpl->turn_speed;

        m_pimpl->yaw   += x_delta;
        m_pimpl->pitch += y_delta;

        // Make sure the camera doesn't flip over.
        if (m_pimpl->pitch > 89.f) {
            m_pimpl->pitch = 89.f;
        } 
        else if (m_pimpl->pitch < -89.f) {
            m_pimpl->pitch = -89.f;
        }

        this->update();
    }

private:
    std::unique_ptr<impl> m_pimpl;
};

#pragma endregion // Camera Class

#pragma region Window Class

/**
 * @brief The window class that holds the window and the OpenGL context.
 * There are wrappers for lots of GLFW functions. For example we can adjust the size
 * or the title of the window using set_size() and set_title(), which are more OOP-like.
 * 
 * This class is not, however, designed to be used as a polymorphic base class.
 * The polymorphic functions are implemented with function pointers, which can be
 * set with set_xxx_callback() member functions. This makes the class more flexible
 * and free from virtual function calls.
 * 
 * It's still welcomed to use this class as a base class.
 */
class window {
public:
    /**
     * @brief The render callback function takes two arguments:
     * @param window The window object to be rendered.
     * @param delta_time The elapsed time since last update.
     */
    using render_callback_t = std::function<void (window&, double)>;
    using logic_callback_t = std::function<void (window&, double)>;

    friend class application;

    static constexpr gl::i32 k_default_width = constants::k_default_window_width;
    static constexpr gl::i32 k_default_height = constants::k_default_window_height;

    /**
     * @brief Default cursor position callback.
     * Calculate the change in cursor position since last call.
     */
    static glfw::cursor_pos_callback_t k_default_cursor_pos_callback;

    /**
     * @brief Default keyboard event callback.
     * Handles ESCAPE key press by setting the window to be closed.
     * Also, update the keys states (m_keys) based on the key press.
     */
    static glfw::key_callback_t k_default_key_callback;

    static glfw::mouse_button_callback_t k_default_mouse_button_callback;

    static glfw::window_size_callback_t k_default_window_size_callback;

    static inline render_callback_t k_default_render_callback = +[](window&, double) {};

    static inline logic_callback_t k_default_logic_callback = +[](window&, double) {};

    /**
     * @brief Construct a new window object using the window specification auxiliary object.
     * 
     * @param spec The auxiliary object containg the parent application, title, sizes, fullscreen 
     * settings, and GLFW window hints. 
     */
    window(aux::window_specification&& spec) {
        using enum aux::window_specification::trait;

        states::glfw_initialize();
        states::glfw_hints(GLFW_CONTEXT_VERSION_MAJOR, spec.major_version,
                           GLFW_CONTEXT_VERSION_MINOR, spec.minor_version,
                           GLFW_OPENGL_PROFILE, spec.opengl_profile,
                           GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE,
                           spec.hints);

        auto const window_traits = spec.traits;

        if (window_traits & BORDERED == 0) {
            glfw::window_hint(GLFW_DECORATED, GL_FALSE);
        }
        if (window_traits & CENTER_CURSOR) {
            glfw::window_hint(GLFW_CENTER_CURSOR, GL_TRUE);
        }
        if (window_traits & FOCUSED == 0) {
            glfw::window_hint(GLFW_FOCUSED, GL_FALSE);
        }
        if (window_traits & MAXIMIZED) {
            glfw::window_hint(GLFW_MAXIMIZED, GL_TRUE);
        }
        if (window_traits & RESIZABLE == 0) {
            glfw::window_hint(GLFW_RESIZABLE, GL_FALSE);
        }
        if (window_traits & TOPMOST) {
            glfw::window_hint(GLFW_FLOATING, GL_TRUE);
        }
        if (window_traits & TRANSPARENT) {
            glfw::window_hint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
        }

        spec.width  = spec.width  <= 0 ? constants::k_default_window_width  : spec.width;
        spec.height = spec.height <= 0 ? constants::k_default_window_height : spec.height;

        m_monitor = glfw::get_primary_monitor();
        if (m_monitor == nullptr) {
            glfw::terminate();
            LOG.exception("Failed to get primary monitor");
        }

        m_window = glfw::create_window(spec.width, spec.height, spec.title, spec.traits & FULLSCREEN ? m_monitor : nullptr, spec.shared_with);
        if (m_window == nullptr) {
            glfw::terminate();
            LOG.exception("Could not create window");
        }

        // Input settings
        if (window_traits & HIDE_CURSOR) {
            glfw::set_input_mode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else if (window_traits & DISABLE_CURSOR) {
            glfw::set_input_mode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        glfw::make_context_current(m_window);

        // Bind the current window object with the underlying GLFW window handle.
        glfw::set_window_user_pointer(m_window, this);

        // When the window is resized, call the following callback.
        glfw::set_window_size_callback(m_window, k_default_window_size_callback);

        // Set customizable callbacks.
        glfw::set_key_callback(m_window, window::k_default_key_callback);
        glfw::set_mouse_button_callback(m_window, window::k_default_mouse_button_callback);

        glfw::get_window_pos(m_window, &m_position.x, &m_position.y);
        glfw::get_window_size(m_window, &m_size.width, &m_size.height);

        m_update_viewport = true;
    }

    /**
     * @brief Construct a new window object with given title, size, and hints.
     * 
     * @param t The title of the window.
     * @param w The width of the window. If a non-positive value is given (or the input is omitted), the default value will be used.
     * @param h The height of the window. If a non-positive value is given (or the input is omitted), the default value will be used.
     * @param hints The hints for the window creation. Default is version 3.3, core profile, and forward compatibility.
     */
    window(char const* t, gl::i32 w = k_default_width, gl::i32 h = k_default_height, auto... hints)
        : window({ .title = t, .width = w, .height = h, .hints = { hints... } }) {}

    /**
     * @brief Windows are non-copyable.         
     */
    window(window const&) = delete;

    window(window&& other) noexcept
        : m_window(other.m_window),
          m_monitor(other.m_monitor),
          m_size(other.m_size),
          m_viewport_size(other.m_viewport_size),
          m_cursor_last_pos(other.m_cursor_last_pos),
          m_cursor_delta(other.m_cursor_delta),
          m_last_time(other.m_last_time),
          m_render_callback(other.m_render_callback),
          m_logic_callback(other.m_logic_callback),
          m_keys(std::move(other.m_keys)),
          m_owning(other.m_owning),
          m_update_viewport(other.m_update_viewport),
          m_running(other.m_running),
          m_cursor_initialized(other.m_cursor_initialized) {

        other.m_window = nullptr;
        other.m_monitor = nullptr;
        other.m_owning = false;
    }

    ~window() {
        if (m_owning) {
            glfw::destroy_window(m_window);
        }
    }

    window& operator =(window const&) = delete;

    window& operator =(window&& other) noexcept {
        m_window = other.m_window;
        m_monitor = other.m_monitor;
        m_size = other.m_size;
        m_viewport_size = other.m_viewport_size;
        m_cursor_last_pos = other.m_cursor_last_pos;
        m_cursor_delta = other.m_cursor_delta;
        m_last_time = other.m_last_time;
        m_render_callback = other.m_render_callback;
        m_logic_callback = other.m_logic_callback;
        m_keys = std::move(other.m_keys);
        m_owning = other.m_owning;
        m_update_viewport = other.m_update_viewport;
        m_running = other.m_running;
        m_cursor_initialized = other.m_cursor_initialized;

        other.m_window = nullptr;
        other.m_monitor = nullptr;
        other.m_owning = false;

        return *this;
    }

    template<typename R, typename... Args>
    R call(R (window::* f)(Args...), Args&&... args) {
        return R();
    }

    void close() {
        glfw::set_window_should_close(m_window, GL_TRUE);
    }

    aux::pos get_cursor() const {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        return { static_cast<gl::i32>(x), static_cast<gl::i32>(y) };
    }

    float get_opacity() const noexcept {
        return glfwGetWindowOpacity(m_window);
    }

    aux::pos get_position() const noexcept {
        gl::i32 x, y;
        glfw::get_window_pos(m_window, &x, &y);
        return { x, y };
    }

    aux::size get_size() const {
        int w, h;
        glfw::get_framebuffer_size(m_window, &w, &h);
        return { w, h };
    }

    aux::fpos get_cursor_delta() noexcept {
        auto result = m_cursor_delta;
        m_cursor_delta = { 0, 0 };
        return result;
    }

    bool is_focused() const noexcept {
        return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) == GL_TRUE;
    }

    bool is_fullscreen() const noexcept {
        return glfwGetWindowMonitor(m_window) != nullptr;
    }

    bool is_maximized() const noexcept {
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) == GL_TRUE;
    }

    bool is_minimized() const noexcept {
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) == GL_TRUE;
    }

    bool is_running() const noexcept {
        return !glfw::window_should_close(m_window);
    }

    bool is_transparent() const noexcept {
        return glfwGetWindowAttrib(m_window, GLFW_TRANSPARENT_FRAMEBUFFER) == GL_TRUE;
    }

    bool is_visible() const noexcept {
        return glfwGetWindowAttrib(m_window, GLFW_VISIBLE) == GL_TRUE;
    }

    void set_aspect_ratio(gl::i32 width, gl::i32 height) {
        width = width <= 0 ? GLFW_DONT_CARE : width;
        height = height <= 0 ? GLFW_DONT_CARE : height;
        glfwSetWindowAspectRatio(m_window, width, height);
    }

    void set_cursor_pos_callback(glfw::cursor_pos_callback_t callback) {
        glfw::set_cursor_pos_callback(m_window, callback);
    }

    void set_focused(bool flag = true) noexcept {
        if (flag) {
            glfwFocusWindow(m_window);
        }
    }

    /**
     * @brief Set the window to fullscreen, or back to windowed mode.
     * 
     * @param flag Whether to set the window to fullscreen.
     */
    void set_fullscreen(bool flag = true) {
        if (this->is_fullscreen() == flag) {
            return;
        }

        if (flag) {
            // Save the current window position and size
            glfwGetWindowPos(m_window, &m_position.x, &m_position.y);
            glfwGetWindowSize(m_window, &m_size.width, &m_size.height);

            // Get resolution of the monitor
            GLFWvidmode const* mode = glfwGetVideoMode(m_monitor);
            // Set to fullscreen
            glfwSetWindowMonitor(m_window, m_monitor, 0, 0, mode->width, mode->height, 0);
        }
        else {
            // Restore the window position and size
            glfwSetWindowMonitor(m_window, nullptr, m_position.x, m_position.y, m_size.width, m_size.height, 0);
        }
        m_update_viewport = false;
    }

    void set_key_callback(GLFWkeyfun callback) {
        glfw::set_key_callback(m_window, callback);
    }

    void set_logic_callback(logic_callback_t callback) {
        m_logic_callback = callback;
    }

    void set_maximized(bool flag = true) {
        if (this->is_maximized() == flag) {
            return;
        }

        if (flag) {
            glfwMaximizeWindow(m_window);
        }
        else {
            glfwRestoreWindow(m_window);
        }
        m_update_viewport = false;
    }

    void set_minimized(bool flag = true) {
        if (this->is_minimized() == flag) {
            return;
        }

        if (flag) {
            glfwIconifyWindow(m_window);
        }
        else {
            glfwRestoreWindow(m_window);
        }
        m_update_viewport = false;
    }

    void set_opacity(float opacity) {
        glfwSetWindowOpacity(m_window, opacity);
    }

    void set_render_callback(render_callback_t callback) {
        m_render_callback = callback;
    }

    void set_size_limits(gl::i32 min_width, gl::i32 min_height, gl::i32 max_width, gl::i32 max_height) {
        min_width  = min_width  <= 0 ? GLFW_DONT_CARE : min_width;
        min_height = min_height <= 0 ? GLFW_DONT_CARE : min_height;
        max_width  = max_width  <= 0 ? GLFW_DONT_CARE : max_width;
        max_height = max_height <= 0 ? GLFW_DONT_CARE : max_height;
        glfwSetWindowSizeLimits(m_window, min_width, min_height, max_width, max_height);
    }

    void set_size_limits(aux::size const& min, aux::size const& max) {
        this->set_size_limits(min.width, min.height, max.width, max.height);
    }

    void set_title(char const* title) {
        glfw::set_window_title(m_window, title);
    }

    void set_visible(bool flag = true) {
        if (this->is_visible() == flag) {
            return;
        }

        if (flag) {
            glfw::show_window(m_window);
        }
        else {
            glfw::hide_window(m_window);
        }
        m_update_viewport = false;
    }

    void set_window_size_callback(glfw::window_size_callback_t callback) {
        glfw::set_window_size_callback(m_window, callback);
    }

    void toggle_fullscreen() {
        this->set_fullscreen(!this->is_fullscreen());
    }

    void toggle_maximized() {
        this->set_maximized(!this->is_maximized());
    }

    void toggle_visible() {
        this->set_visible(!this->is_visible());
    }

    /**
     * @brief Update the window. This function is called by the application object once 
     * at a time in the main loop. (i.e. the application::run() function).
     */
    void update() {
        glfw::make_context_current(m_window);

        // Update viewport if the window size has changed.
        if (m_update_viewport) {
            glfw::get_framebuffer_size(m_window, &m_viewport_size.width, &m_viewport_size.height);
            glfw::viewport(0, 0, m_viewport_size.width, m_viewport_size.height);
            m_update_viewport = false;
        }

        auto const now = glfw::get_time();
        auto const delta_time = now - m_last_time;
        m_last_time = now;

        // Clear the screen with default background color (black).
        gl::clear_color(0.0f, 0.0f, 0.0f, 1.0f);
        gl::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        this->m_logic_callback(*this, delta_time);
        this->m_render_callback(*this, delta_time);

        glfw::swap_buffers(m_window);
        glfw::poll_events();

        m_running &= glfw::get_key(m_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE;
        m_running &= glfw::window_should_close(m_window) == GL_FALSE;
    }

private:
    glfw::window_handle     m_window                = nullptr;                      /* window handle */
    glfw::monitor_handle    m_monitor               = nullptr;                      /* monitor handle */

    aux::pos                m_position;                                             /* (realtime) window position */
    aux::size               m_size;                                                 /* (realtime) window size */
    aux::size               m_viewport_size;                                        /* viewport size */
    aux::fpos               m_cursor_last_pos;                                      /* cursor position in the previous frame */
    aux::fpos               m_cursor_delta;                                         /* (realtime) cursor delta */
    gl::f32                 m_last_time             = 0.f;                          /* last time */

    render_callback_t       m_render_callback       = k_default_render_callback;    /* render callback */
    logic_callback_t        m_logic_callback        = k_default_logic_callback;     /* logic callback */
    std::unique_ptr<bool[]> m_keys                  = std::make_unique<bool[]>(512);/* key states */

    bool                    m_owning               = true;                          /* owning window */
    mutable bool            m_update_viewport      = false;                         /* flag indicating whether to update viewport */
    mutable bool            m_running              = true;                          /* flag indicating whether the window is running */

    mutable bool            m_cursor_initialized   = false;                         /* flag indicating whether the cursor position is initialized */
};

glfw::cursor_pos_callback_t window::k_default_cursor_pos_callback = +[](glfw::window_handle win, gl::f64 x, gl::f64 y) {
    auto* window_ptr = static_cast<window*>(glfw::get_window_user_pointer(win));
    
    if (!window_ptr->m_cursor_initialized) {
        window_ptr->m_cursor_last_pos.x = x;
        window_ptr->m_cursor_last_pos.y = y;
        window_ptr->m_cursor_initialized = true;
    }

    window_ptr->m_cursor_delta.x = x - window_ptr->m_cursor_last_pos.x;
    window_ptr->m_cursor_delta.y = window_ptr->m_cursor_last_pos.y - y;
    window_ptr->m_cursor_last_pos.x = x;
    window_ptr->m_cursor_last_pos.y = y;
};

glfw::key_callback_t window::k_default_key_callback = +[](glfw::window_handle win, gl::i32 key, gl::i32 scancode, gl::i32 action, gl::i32 mods) {
    auto* window_ptr = static_cast<window*>(glfw::get_window_user_pointer(win));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfw::set_window_should_close(win, GL_TRUE);
    }
    if (action == GLFW_PRESS) {
        window_ptr->m_keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        window_ptr->m_keys[key] = false;
    }
};

glfw::mouse_button_callback_t window::k_default_mouse_button_callback = +[](glfw::window_handle win, gl::i32 button, gl::i32 action, gl::i32 mods) {};

glfw::window_size_callback_t window::k_default_window_size_callback = +[](glfw::window_handle win, gl::i32 width, gl::i32 height) {
    auto* window_ptr = static_cast<window*>(glfw::get_window_user_pointer(win));

    if (window_ptr != nullptr) {
        window_ptr->m_size.width = width;
        window_ptr->m_size.height = height;
        window_ptr->m_update_viewport = true;
    }
};

/**
 * @brief Create a unique pointer to a window object.
 */
template<typename... Args>
std::unique_ptr<window> make_window(Args&&... args) {
    return std::make_unique<window>(std::forward<Args>(args)...);
}


#pragma endregion // Window Class

#pragma region Resource/ResourceManager Class

/**
 * @brief Resource manager for shaders, textures, VAO's and VBO's owned by an application.
 * Basically a wrapper around some std::unordered_map<std::string, resource_type>'s.
 */
class resource {
public:
    friend class resource_manager;

    resource() = default;

    resource(resource const&) = delete;

    resource(resource&& other) = default;

    resource& operator=(resource const&) = delete;

    resource& operator=(resource&& other) = default;

private:
    // 392 bytes of data while empty... Don't create a lot of resource objects.
    std::unordered_map<std::string, vertex_array> m_arrays;
    std::unordered_map<std::string, buffer> m_buffers;
    std::unordered_map<std::string, camera> m_cameras;
    std::unordered_map<std::string, mesh> m_meshes;
    std::unordered_map<std::string, shader> m_shaders;
    std::unordered_map<std::string, texture> m_textures;
    std::unordered_map<std::string, window> m_windows;
};


/**
 * @brief The resource manager class, holds all resources owned by the application
 * and provides the interface for creating, loading and unloading resources like
 * shaders, textures, windows, etc. Only one resource manager instance is allowed 
 * for each program.
 */
class resource_manager {
public:
    /**
     * @brief Proxy class for managing certain type of resources.
     * 
     * @tparam Resrc The type of resource to manage.
     */
    template<typename Resrc>
    class proxy {
    public:
        proxy(std::unordered_map<std::string, Resrc>& record)
            : m_record(record) {
            
            if constexpr (std::same_as<Resrc, buffer>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_buffer_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, camera>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_camera_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, mesh>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_mesh_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, shader>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_shader_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, texture>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_texture_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, vertex_array>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_vertex_array_name(m_record, name);
                };
            }
            else if constexpr (std::same_as<Resrc, window>) {
                m_next_name = [this](std::string const& name) {
                    return states::next_window_name(m_record, name);
                };
            }
            else {
                LOG.exception("Unsupported resource type");
            }
        }

        Resrc& operator [](gl::u32 idx) {
            if (idx >= m_record.size()) {
                LOG.exception("Index out of range");
            }
            auto&& [name, resrc] = *std::advance(m_record.begin(), idx);
            m_recently_used = name;
            return resrc;
        }

        Resrc& operator [](std::string name) {
            if (!m_record.contains(name)) {
                LOG.exception("No such resource: " + name);
            }
            auto&& result = m_record.at(name);
            m_recently_used = std::move(name);
            return result;
        }

        auto begin() {
            return m_record.begin();
        }

        auto begin() const {
            return m_record.begin();
        }

        bool contains(std::string const& name) {
            return m_record.contains(name);
        }

        template<typename... Args>
        void emplace(std::string& name, Args&&... args) {
            auto resrc = Resrc(std::forward<Args>(args)...);
            this->record(resrc, name);
            m_recently_used = name;
        }

        template<typename... Args>
        std::string emplace(std::string&& name, Args&&... args) {
            this->emplace(name, std::forward<Args>(args)...);     /* Call the lvalue ref version */
            return name;
        }

        auto end() {
            return m_record.end();
        }

        auto end() const {
            return m_record.end();
        }

        std::string find(gl::u32 resrc) const {
            for (auto& [name, obj] : m_record) {
                if (obj.is_wrapper_of(resrc)) {
                    m_recently_used = &obj;
                    return name;
                }
            }
            return "";
        }

        std::string find(Resrc& object) const {
            for (auto& [name, obj] : m_record) {
                if (obj == object) {
                    m_recently_used = &obj;
                    return name;
                }
            }
            return "";
        }

        /**
         * @brief Move ownership of a standalone resource to the resource manager.
         * The original resource will not be moved (but usually not deleted).
         * 
         * @param object 
         * @param name The given hint for the name of the resource. If the adopted
         * name is different from the given one, it will be updated for the caller.
         */
        void record(Resrc& object, std::string& name) {
            name = m_next_name(name);
            m_record.insert({ name, std::move(object) });
            // m_record.insert(std::make_pair(name, std::move(object)));
            // m_record.emplace(name, std::move(object));
            // m_record[name] = std::move(object);
            if constexpr (requires { object.m_owning; }) {
                object.m_owning = 0;
            }
            m_recently_used = name;
        }

        std::string record(Resrc& object, std::string&& name = "") {
            this->record(object, name);     /* Call the lvalue ref version */
            return name;
        }

        /**
         * @brief Access the most recently used resource. If it's not available
         * the first resource in the record will be returned.
         */
        Resrc& recent() {
            if (m_recently_used == "") {
                if (m_record.empty()) {
                    LOG.exception("No resources");
                }
                auto& [name, resrc] = *m_record.begin();
                m_recently_used = name;
                return resrc;
            }
            return m_record.at(m_recently_used);
        }

        void remove(std::string const& name) {
            m_record.erase(name);
            if (m_recently_used == name) {
                m_recently_used = "";
            }
        }

        bool rename(std::string const& old_name, std::string const& new_name) {
            if (!m_record.contains(old_name)) {
                return false;
            }
            m_record[new_name] = m_record[old_name];
            m_record.erase(old_name);

            if (m_recently_used == old_name) {
                m_recently_used = new_name;
            }
            return true;
        }

        /**
         * @brief Remove a resource from the resource manager and transfer
         * ownership to the caller.
         * 
         * @param name 
         * @return Resrc 
         */
        Resrc retrieve(std::string const& name) {
            if (!m_record.contains(name)) {
                LOG.exception("No such resource: " + name);
            }
            auto resrc = std::move(m_record[name]);
            m_record.erase(name);

            if (m_recently_used == name) {
                m_recently_used = "";
            }
            return resrc;           /* Move and return the resource object */
        }

    private:
        std::unordered_map<std::string, Resrc>& m_record;
        std::function<std::string (std::string const&)> m_next_name;
        mutable std::string m_recently_used;
    };

    resource_manager()
        : m_resource(std::make_unique<resource>()),
          vertex_arrays(m_resource->m_arrays),
          buffers(m_resource->m_buffers),
          cameras(m_resource->m_cameras),
          meshes(m_resource->m_meshes),
          shaders(m_resource->m_shaders),
          textures(m_resource->m_textures),
          windows(m_resource->m_windows) {}

private:
    std::unique_ptr<resource> m_resource;

// I have to put these back here because the constructor initializer list is executed
// the same order as the members in the class.
public:
    // 280 bytes of data...
    proxy<vertex_array> vertex_arrays;
    proxy<buffer> buffers;
    proxy<camera> cameras;
    proxy<mesh> meshes;
    proxy<shader> shaders;
    proxy<texture> textures;
    proxy<window> windows;
};


#pragma endregion // Resource/Resource Manager Class

#pragma region Application Class

/**
 * @brief The application class that manages windows, shaders, vertex buffers, vertex arrays,
 * and other resources. Runs a main loop that calls render callbacks for each window. In a
 * program, only one application object should be created, since it's related to all global
 * states. Also, it's unexpected to either copy or move an application object, so these 
 * operations are deleted.
 * 
 * It's guaranteed that there is at least one window in the application, i.e. the main window.
 * 
 * This is a polymorphic class, so you should inherit from it and override the startup() and 
 * shutdown() virtual functions.
 * 
 * @example The following is a typical example to create a trivial application object:
 * @code
 *      int main() {
 *          auto app = gl::application("My Application");
 *          app.run();      // Run the application, loop until the window is closed.
 *          return 0;
 *      }
 */
class application {
public:
    static inline auto g_application_created = false;

    application(char const* title = constants::k_default_window_title, 
                gl::i32 width = constants::k_default_window_width, 
                gl::i32 height = constants::k_default_window_height) {
        if (g_application_created) {
            LOG.exception("Only one application object should be created");
        }

        g_application_created = true;

        states::glfw_initialize();

        states::resource_initialize();

        // Initialize the main window
        states::g_resource_manager->windows.emplace(title, title, width, height);

        // Set this flag for historical reasons
        glewExperimental = GL_TRUE;

        states::glew_initialize();
    }    

    application(application const&) = delete;

    application(application&&) = delete;

    virtual ~application() {
        glfw::terminate();
    }

    application& operator =(application const&) = delete;

    application& operator =(application&&) = delete;

    template<typename... Args>
    void create_window(Args&&... args) {
        states::g_resource_manager->windows.emplace(std::forward<Args>(args)...);
    }

    // TODO: What is the current window?
    window& get_current_window() {
        return states::g_resource_manager->windows.recent();
    }

    resource_manager& get_resource_manager() {
        return *states::g_resource_manager;
    }

    [[deprecated("Consider another way to access window keys")]]
    bool* get_window_keys(window& win) {
        return win.m_keys.get();
    }

    bool is_running() const noexcept {
        return m_running;
    }

    /**
     * @brief Create a new window and return its handle.
     * 
     * @param title The title of the window.
     * @param width The width of the window, defaulted to M_DEFAULT_WINDOW_WIDTH.
     * @param height The height of the window, defaulted to M_DEFAULT_WINDOW_HEIGHT.
     * @param hints Hints for the window.
     * @return glfw::window_handle A handle of the new window; the caller doesn't own this pointer.
     */
    [[nodiscard]]
    static glfw::window_handle new_window(char const* title = constants::k_default_window_title, 
                                          gl::i32 width = constants::k_default_window_width, 
                                          gl::i32 height = constants::k_default_window_height, 
                                          auto... hints) {
        glfw::window_handle result;
        states::glfw_initialize();
        states::resource_initialize();
        states::g_resource_manager->windows.emplace(title, &result, title, width, height, hints...);
        states::glew_initialize();
        return result;
    }

    /**
     * @brief Run the application in a main loop, in which the application calls the update() method for each window.
     * Call the startup() before the first loop and the shutdown() after the last loop.
     */
    void run() {
        this->startup();
        auto& windows = states::g_resource_manager->windows;
        auto dead_windows = std::queue<std::string>();

        // Main loop
        do {
            m_running = false;

            // Render each window and keep record of whether the windows should be closed.
            for (auto& [name, win] : windows) {
                win.update();
                if (!win.is_running()) {
                    dead_windows.push(name);
                }
                else {
                    m_running = true;
                }
            }

            // Close all windows that should be closed.
            while (!dead_windows.empty()) {
                windows.remove(dead_windows.front());
                dead_windows.pop();
            }

        } while (m_running);

        this->shutdown();
    }

    virtual void shutdown() {}

    virtual void startup() {}

protected:

private:
    mutable bool m_running = true;
};

#pragma endregion // Application Class

} // namespace gl

#define GLSL(src) "#version 450 core\n"  #src