#pragma once

#include "GL/glew.h"
#include "GL/gl.h"
#include "GLFW/glfw3.h"

#include <utility>

namespace gl {

// OpenGL Types

using b8 = GLboolean;
using b32 = GLbitfield;
using c8 = GLchar;
using cf32 = GLclampf;
using cf64 = GLclampd;
using e32 = GLenum;
using f32 = GLfloat;
using f64 = GLdouble;
using i32 = GLint;
using s32 = GLsizei;
using u32 = GLuint;


// OpenGL Constants



// OpenGL Functions

inline void attach_shader               (u32 program, u32 shader)           { glAttachShader(program, shader); }
inline void bind_buffer                 (e32 target, u32 buffer)            { glBindBuffer(target, buffer); }
inline void bind_vao                    (u32 vao)                           { glBindVertexArray(vao); }
inline void bind_vertex_array           (u32 vao)                           { glBindVertexArray(vao); }
inline void buffer_data                 (e32 target, s32 size, void const* data, e32 usage) { glBufferData(target, size, data, usage); }
inline void clear                       (b32 mask)                          { glClear(mask); }
inline void clear_color                 (cf32 r, cf32 g, cf32 b, cf32 a)    { glClearColor(r, g, b, a); }
inline void compile_shader              (u32 shader)                        { glCompileShader(shader); }
inline u32  create_program              ()                                  { return glCreateProgram(); }
inline u32  create_shader               (e32 type)                          { return glCreateShader(type); }
inline void delete_buffer               (u32& buffer)                       { glDeleteBuffers(1, &buffer); }
inline void delete_buffers              (s32 n, u32* buffers)               { glDeleteBuffers(n, buffers); }
inline void delete_program              (u32 program)                       { glDeleteProgram(program); }
inline void delete_shader               (u32 shader)                        { glDeleteShader(shader); }
inline void delete_vertex_array         (u32 vao)                           { glDeleteVertexArrays(1, &vao); }
inline void delete_vertex_arrays        (s32 n, u32* vaos)                  { glDeleteVertexArrays(n, vaos); }
inline void draw_arrays                 (e32 mode, i32 first, s32 count)    { glDrawArrays(mode, first, count); }
inline void draw_elements               (e32 mode, s32 count, e32 type, void const* indices) { glDrawElements(mode, count, type, indices); }
inline void enable                      (e32 cap)                           { glEnable(cap); }
inline void enable_vertex_attrib_array  (u32 index)                         { glEnableVertexAttribArray(index); }
inline u32  generate_buffer             ()                                  { u32 buffer; glGenBuffers(1, &buffer); return buffer; }
inline void generate_buffer             (u32& buffer)                       { glGenBuffers(1, &buffer); }
inline void generate_buffers            (u32 count, u32* buffers)           { glGenBuffers(count, buffers); }
inline void gen_buffers                 (u32 count, u32* buffers)           { glGenBuffers(count, buffers); }
inline u32  generate_vertex_array       ()                                  { u32 vao; glGenVertexArrays(1, &vao); return vao; }
inline void generate_vertex_array       (u32& vao)                          { glGenVertexArrays(1, &vao); }
inline void generate_vertex_arrays      (s32 n, u32* vaos)                  { glGenVertexArrays(n, vaos); }
inline void gen_vertex_arrays           (s32 n, u32* vaos)                  { glGenVertexArrays(n, vaos); }
inline void get_program_info_log        (u32 program, s32 bufsize, s32* length, char* infolog) { glGetProgramInfoLog(program, bufsize, length, infolog); }
inline void get_program_iv              (u32 program, e32 pname, i32* params) { glGetProgramiv(program, pname, params); }
inline void get_shader_info_log         (u32 shader, s32 max_length, s32* length, char* info_log) { glGetShaderInfoLog(shader, max_length, length, info_log); }
inline void get_shader_iv               (u32 shader, e32 pname, i32* params) { glGetShaderiv(shader, pname, params); }
inline i32  get_uniform_location        (u32 program, c8 const* name)       { return glGetUniformLocation(program, name); }
inline b8   is_program                  (u32 program)                       { return glIsProgram(program); }
inline b8   is_shader                   (u32 shader)                        { return glIsShader(shader); }
inline void link_program                (u32 program)                       { glLinkProgram(program); }
inline void patch_parameter             (e32 pname, i32 value)              { glPatchParameteri(pname, value); }
inline void polygon_mode                (e32 face, e32 mode)                { glPolygonMode(face, mode); }
inline void shader_source               (u32 shader, s32 count, char const* const* string, s32 const* length) { glShaderSource(shader, count, string, length); }
inline void uniform_mat4f               (i32 location, s32 count, b8 transpose, f32 const* value) { glUniformMatrix4fv(location, count, transpose, value); }
inline void use_program                 (u32 program)                       { glUseProgram(program); }
inline void validate_program            (u32 program)                       { glValidateProgram(program); }
inline void vertex_attrib               (i32 index, f32 const* value)       { glVertexAttrib4fv(index, value); }
inline void vertex_attrib_pointer       (i32 index, s32 size, e32 type, b8 normalized, s32 stride, void const* pointer) { glVertexAttribPointer(index, size, type, normalized, stride, pointer); }


// OpenGL Shader Keywords

#define attribute   attribute
#define centroid    centroid
#define flat        flat
#define in          in
#define inout       inout
#define interface   interface
#define layout      layout
#define mat2        mat2
#define mat3        mat3
#define mat4        mat4
#define out         out
#define patch       patch
#define precision   precision
#define sample      sample
#define uniform     uniform
#define varying     varying
#define vec2        vec2
#define vec3        vec3
#define vec4        vec4


} // namespace gl


namespace glfw {

// GLFW Types

using cursor_pos_callback_t = GLFWcursorposfun;
using key_callback_t = GLFWkeyfun;
using monitor_handle = GLFWmonitor*;
using mouse_button_callback_t = GLFWmousebuttonfun;
using vidmode_handle = GLFWvidmode const*;
using window_handle = GLFWwindow*;
using window_size_callback_t = GLFWwindowsizefun;

// GLFW Functions

inline auto   create_window              (int width, int height, char const* title, monitor_handle monitor, window_handle share) -> window_handle { return glfwCreateWindow(width, height, title, monitor, share); }
inline void   destroy_window             (window_handle window)                              { glfwDestroyWindow(window); }
inline void   focus_window               (window_handle window)                              { glfwFocusWindow(window); }
inline void   get_cursor_pos             (window_handle window, double* xpos, double* ypos)  { glfwGetCursorPos(window, xpos, ypos); }
inline auto   get_cursor_pos             (window_handle window)                              { double xpos, ypos; glfwGetCursorPos(window, &xpos, &ypos); return std::make_pair(xpos, ypos); }
inline void   get_framebuffer_size       (window_handle window, int* width, int* height)     { glfwGetFramebufferSize(window, width, height); }
inline auto   get_framebuffer_size       (window_handle window)                              { int width, height; glfwGetFramebufferSize(window, &width, &height); return std::make_pair(width, height); }
inline int    get_key                    (window_handle window, int key)                     { return glfwGetKey(window, key); }
inline double get_time                   ()                                                  { return glfwGetTime(); }
inline int    get_window_attrib          (window_handle window, int attrib)                  { return glfwGetWindowAttrib(window, attrib); }
inline auto   get_video_mode             (monitor_handle monitor)    -> vidmode_handle       { return glfwGetVideoMode(monitor); }
inline auto   get_window_monitor         (window_handle window)      -> monitor_handle       { return glfwGetWindowMonitor(window); }
inline float  get_window_opacity         (window_handle window)                              { return glfwGetWindowOpacity(window); }
inline void   get_window_pos             (window_handle window, int* xpos, int* ypos)        { glfwGetWindowPos(window, xpos, ypos); }
inline auto   get_window_pos             (window_handle window)                              { int xpos, ypos; glfwGetWindowPos(window, &xpos, &ypos); return std::make_pair(xpos, ypos); }
inline void   get_window_size            (window_handle window, int* width, int* height)     { glfwGetWindowSize(window, width, height); }
inline auto   get_window_size            (window_handle window)                              { int width, height; glfwGetWindowSize(window, &width, &height); return std::make_pair(width, height); }
inline void*  get_window_user_pointer    (window_handle window)                              { return glfwGetWindowUserPointer(window); }
inline auto   get_primary_monitor        ()                          -> monitor_handle       { return glfwGetPrimaryMonitor(); }
inline void   hide_window                (window_handle window)                              { glfwHideWindow(window); }
inline void   iconify_window             (window_handle window)                              { glfwIconifyWindow(window); }
inline int    init                       ()                                                  { return glfwInit(); }
inline void   make_context_current       (window_handle window)                              { glfwMakeContextCurrent(window); }
inline void   maximize_window            (window_handle window)                              { glfwMaximizeWindow(window); }
inline void   poll_events                ()                                                  { glfwPollEvents(); }
inline void   restore_window             (window_handle window)                              { glfwRestoreWindow(window); }
inline void   set_cursor_pos_callback    (window_handle window, cursor_pos_callback_t callback) { glfwSetCursorPosCallback(window, callback); }
inline void   set_input_mode             (window_handle window, int mode, int value)         { glfwSetInputMode(window, mode, value); }
inline void   set_key_callback           (window_handle window, key_callback_t callback)     { glfwSetKeyCallback(window, callback); }
inline void   set_mouse_button_callback  (window_handle window, mouse_button_callback_t callback) { glfwSetMouseButtonCallback(window, callback); }
inline void   set_window_aspect_ratio    (window_handle window, int numer, int denom)        { glfwSetWindowAspectRatio(window, numer, denom); }
inline void   set_window_monitor         (window_handle window, monitor_handle monitor, int xpos, int ypos, int width, int height, int refresh_rate) { glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, refresh_rate); }
inline void   set_window_opacity         (window_handle window, float opacity)               { glfwSetWindowOpacity(window, opacity); }
inline void   set_window_should_close    (window_handle window, int value)                   { glfwSetWindowShouldClose(window, value); }
inline void   set_window_size            (window_handle window, int width, int height)      { glfwSetWindowSize(window, width, height); }
inline void   set_window_size_callback   (window_handle window, window_size_callback_t callback) { glfwSetWindowSizeCallback(window, callback); }
inline void   set_window_size_limits     (window_handle window, int min_width, int min_height, int max_width, int max_height) { glfwSetWindowSizeLimits(window, min_width, min_height, max_width, max_height); }
inline void   set_window_title           (window_handle window, char const* title)           { glfwSetWindowTitle(window, title); }
inline void   set_window_user_pointer    (window_handle window, void* pointer)               { glfwSetWindowUserPointer(window, pointer); }
inline void   show_window                (window_handle window)                              { glfwShowWindow(window); }
inline void   swap_buffers               (window_handle window)                              { glfwSwapBuffers(window); }
inline void   terminate                  ()                                                  { glfwTerminate(); }
inline void   viewport                   (int x, int y, int width, int height)               { glViewport(x, y, width, height); }
inline void   window_hint                (int hint, int value)                               { glfwWindowHint(hint, value); }
inline int    window_should_close        (window_handle window)                              { return glfwWindowShouldClose(window); }

} // namespace glfw

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
