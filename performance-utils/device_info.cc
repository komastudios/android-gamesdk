#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>

#include "frameworks/opt/gamesdk/performance-utils/device_info.pb.h"

namespace {
typedef long long int int64;
typedef unsigned int uint32;
typedef unsigned char uint8;

// size of GL view and texture in future
constexpr int WIDTH = 8;
constexpr int HEIGHT = WIDTH;

int readNumCpus(){
  const int ERROR = -1;
  std::ifstream f("/sys/devices/system/cpu/kernel_max");
  if(f.fail()) return ERROR;
  int result = ERROR;
  f >> result;
  if(f.fail()) return ERROR;
  result++; // num cpu = max index + 1
  return result;
}

int64 readCpuFreqMax(int cpuIndex){
  const int64 ERROR = -1;
  std::string fileName =
    "/sys/devices/system/cpu/cpu" +
    std::to_string(cpuIndex) +
    "/cpufreq/cpuinfo_max_freq";
  std::ifstream f(fileName);
  if(f.fail()) return ERROR;
  int64 result = ERROR;
  f >> result;
  if(f.fail()) return ERROR;
  return result;
}

namespace string_util{
  bool startsWith(const std::string& text, const std::string& start){
    int n = start.length();
    if(text.length() < n) return false;
    for(int i = 0; i < n; i++){
      if(text[i] != start[i]) return false;
    }
    return true;
  }

  void splitSpaceDelimUnique(const std::string toSplit, std::set<std::string>& result){
    int n = toSplit.length();
    int from = 0;
    for(int i = 0; i < n; i++){
      if(toSplit[i] == ' '){
        int len = i - from;
        if(len > 0){
          result.insert(toSplit.substr(from, len));
        }
        from = i + 1;
      }
    }
    int len = n - from;
    if(len > 0){
      result.insert(toSplit.substr(from, len));
    }
  }
} // namespace string_util

std::vector<std::string> readHardware(){
  std::vector<std::string> result;
  std::ifstream f("/proc/cpuinfo");
  if(f.fail()) return result;
  const std::string FIELD_KEY = "Hardware\t: ";
  std::string line;
  while(std::getline(f, line)){
    if(::string_util::startsWith(line, FIELD_KEY)){
      std::string val = line.substr(FIELD_KEY.length(), std::string::npos);
      result.push_back(val);
    }
  }
  return result;
}

std::set<std::string> readFeatures(){
  std::set<std::string> result;
  std::ifstream f("/proc/cpuinfo");
  if(f.fail()) return result;
  const std::string FIELD_KEY = "Features\t: ";
  std::string line;
  while(std::getline(f, line)){
    if(::string_util::startsWith(line, FIELD_KEY)){
      std::string features = line.substr(FIELD_KEY.length(), std::string::npos);
      ::string_util::splitSpaceDelimUnique(features, result);
    }
  }
  return result;
}

// TODO: fail more gracefully
void assertEGl(const char* title){
  EGLint error = eglGetError();
  if(error != EGL_SUCCESS){
    std::cerr << "*EGL Error: " << title << ": "
      << std::hex << (int)error << std::dec;
    exit(1);
  }
}

void flushGlErrors(const std::string& at = ""){
  while(auto e = glGetError()){
    if(e == GL_NO_ERROR) break;
    std::cerr << "*GL error: " << std::hex << (int)e << std::dec << at << std::endl;
  }
}

void setupEGl() {
  EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assertEGl("eglGetDisplay");

  eglInitialize(display, nullptr, nullptr); // do not care about egl version
  assertEGl("eglInitialize");

  EGLint configAttribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_NONE
  };
  EGLConfig config;
  EGLint numConfigs = -1;
  eglChooseConfig(display, configAttribs, &config, 1, &numConfigs);
  assertEGl("eglChooseConfig");

  EGLint contextAttribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
  assertEGl("eglCreateContext");

  EGLint pbufferAttribs[] = {
      EGL_WIDTH,  WIDTH,
      EGL_HEIGHT, HEIGHT,
      EGL_NONE
  };
  EGLSurface surface = eglCreatePbufferSurface(display, config, pbufferAttribs);
  assertEGl("eglCreatePbufferSurface");

  eglMakeCurrent(display, surface, surface, context);
  assertEGl("eglMakeCurrent");
}

namespace gl_util{
  GLfloat getFloat(GLenum e){
    GLfloat result = -1;
    glGetFloatv(e, &result);
    return result;
  }
  GLint getInt(GLenum e){
    GLint result = -1;
    glGetIntegerv(e, &result);
    return result;
  }
  GLboolean getBool(GLenum e){
    GLboolean result = false;
    glGetBooleanv(e, &result);
    return result;
  }

  typedef const GLubyte* MyTypeGlStr;
  typedef MyTypeGlStr (*MyTypeGlGetstringi)(GLenum, GLint);
  MyTypeGlGetstringi glGetStringi;

  typedef void (*MyTypeGlGetInteger64v)(GLenum, GLint64*);
  MyTypeGlGetInteger64v glGetInteger64v;

  GLint64 getInt64(GLenum e){
    GLint64 result = -1;
    glGetInteger64v(e, &result);
    return result;
  }

  typedef void (*MyTypeGlGetIntegeri_v)(GLenum, GLuint, GLint*);
  MyTypeGlGetIntegeri_v glGetIntegeri_v;

  GLint getIntIndexed(GLenum e, GLuint index){
    GLint result = -1;
    glGetIntegeri_v(e, index, &result);
    return result;
  }
} // namespace gl_util

void addGlConstsV2_0(device_info::gl& gl){
  gl.set_gl_aliased_line_width_range(        ::gl_util::getFloat(GL_ALIASED_LINE_WIDTH_RANGE));
  gl.set_gl_aliased_point_size_range(        ::gl_util::getFloat(GL_ALIASED_POINT_SIZE_RANGE));
  gl.set_gl_max_combined_texture_image_units(::gl_util::getInt(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_cube_map_texture_size(       ::gl_util::getInt(GL_MAX_CUBE_MAP_TEXTURE_SIZE));
  gl.set_gl_max_fragment_uniform_vectors(    ::gl_util::getInt(GL_MAX_FRAGMENT_UNIFORM_VECTORS));
  gl.set_gl_max_renderbuffer_size(           ::gl_util::getInt(GL_MAX_RENDERBUFFER_SIZE));
  gl.set_gl_max_texture_image_units(         ::gl_util::getInt(GL_MAX_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_texture_size(                ::gl_util::getInt(GL_MAX_TEXTURE_SIZE));
  gl.set_gl_max_varying_vectors(             ::gl_util::getInt(GL_MAX_VARYING_VECTORS));
  gl.set_gl_max_vertex_attribs(              ::gl_util::getInt(GL_MAX_VERTEX_ATTRIBS));
  gl.set_gl_max_vertex_texture_image_units(  ::gl_util::getInt(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_vertex_uniform_vectors(      ::gl_util::getInt(GL_MAX_VERTEX_UNIFORM_VECTORS));
  gl.set_gl_max_viewport_dims(               ::gl_util::getInt(GL_MAX_VIEWPORT_DIMS));
  gl.set_gl_shader_compiler(                 ::gl_util::getBool(GL_SHADER_COMPILER));
  gl.set_gl_subpixel_bits(                   ::gl_util::getInt(GL_SUBPIXEL_BITS));

  GLint numCompressedFormats = ::gl_util::getInt(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
  gl.set_gl_num_compressed_texture_formats(numCompressedFormats);
  std::vector<GLint> compressedFormats(numCompressedFormats);
  glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, &compressedFormats[0]);
  for(auto x : compressedFormats){
    gl.add_gl_compressed_texture_formats(x);
  }

  GLint numShaderBinFormats = ::gl_util::getInt(GL_NUM_SHADER_BINARY_FORMATS);
  gl.set_gl_num_shader_binary_formats(numShaderBinFormats);
  std::vector<GLint> shaderBinFormats(numShaderBinFormats);
  glGetIntegerv(GL_SHADER_BINARY_FORMATS, &shaderBinFormats[0]);
  for(auto x: shaderBinFormats){
    gl.add_gl_shader_binary_formats(x);
  }

  // shader precision formats
  GLint spfr = -1; // range
  GLint spfp = -1; // precision
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_LOW_FLOAT, &spfr, &spfp);
  gl.set_spf_vertex_float_low_range(spfr);
  gl.set_spf_vertex_float_low_prec(spfp);
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_MEDIUM_FLOAT, &spfr, &spfp);
  gl.set_spf_vertex_float_med_range(spfr);
  gl.set_spf_vertex_float_med_prec(spfp);
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_HIGH_FLOAT, &spfr, &spfp);
  gl.set_spf_vertex_float_hig_range(spfr);
  gl.set_spf_vertex_float_hig_prec(spfp);
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_LOW_INT, &spfr, &spfp);
  gl.set_spf_vertex_int_low_range(spfr);
  gl.set_spf_vertex_int_low_prec(spfp);
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_MEDIUM_INT, &spfr, &spfp);
  gl.set_spf_vertex_int_med_range(spfr);
  gl.set_spf_vertex_int_med_prec(spfp);
  glGetShaderPrecisionFormat(GL_VERTEX_SHADER, GL_HIGH_INT, &spfr, &spfp);
  gl.set_spf_vertex_int_hig_range(spfr);
  gl.set_spf_vertex_int_hig_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_LOW_FLOAT, &spfr, &spfp);
  gl.set_spf_fragment_float_low_range(spfr);
  gl.set_spf_fragment_float_low_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT, &spfr, &spfp);
  gl.set_spf_fragment_float_med_range(spfr);
  gl.set_spf_fragment_float_med_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, &spfr, &spfp);
  gl.set_spf_fragment_float_hig_range(spfr);
  gl.set_spf_fragment_float_hig_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_LOW_INT, &spfr, &spfp);
  gl.set_spf_fragment_int_low_range(spfr);
  gl.set_spf_fragment_int_low_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_INT, &spfr, &spfp);
  gl.set_spf_fragment_int_med_range(spfr);
  gl.set_spf_fragment_int_med_prec(spfp);
  glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_INT, &spfr, &spfp);
  gl.set_spf_fragment_int_hig_range(spfr);
  gl.set_spf_fragment_int_hig_prec(spfp);
}
void addGlConstsV3_0(device_info::gl& gl){
  const GLint GL_MAX_3D_TEXTURE_SIZE                           = 0x8073;
  const GLint GL_MAX_ARRAY_TEXTURE_LAYERS                      = 0x88FF;
  const GLint GL_MAX_COLOR_ATTACHMENTS                         = 0x8CDF;
  const GLint GL_MAX_COMBINED_UNIFORM_BLOCKS                   = 0x8A2E;
  const GLint GL_MAX_DRAW_BUFFERS                              = 0x8824;
  const GLint GL_MAX_ELEMENTS_INDICES                          = 0x80E9;
  const GLint GL_MAX_ELEMENTS_VERTICES                         = 0x80E8;
  const GLint GL_MAX_FRAGMENT_INPUT_COMPONENTS                 = 0x9125;
  const GLint GL_MAX_FRAGMENT_UNIFORM_BLOCKS                   = 0x8A2D;
  const GLint GL_MAX_FRAGMENT_UNIFORM_COMPONENTS               = 0x8B49;
  const GLint GL_MAX_PROGRAM_TEXEL_OFFSET                      = 0x8905;
  const GLint GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS = 0x8C8A;
  const GLint GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS       = 0x8C8B;
  const GLint GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS    = 0x8C80;
  const GLint GL_MAX_UNIFORM_BUFFER_BINDINGS                   = 0x8A2F;
  const GLint GL_MAX_VARYING_COMPONENTS                        = 0x8B4B;
  const GLint GL_MAX_VERTEX_OUTPUT_COMPONENTS                  = 0x9122;
  const GLint GL_MAX_VERTEX_UNIFORM_BLOCKS                     = 0x8A2B;
  const GLint GL_MAX_VERTEX_UNIFORM_COMPONENTS                 = 0x8B4A;
  const GLint GL_MIN_PROGRAM_TEXEL_OFFSET                      = 0x8904;
  const GLint GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT               = 0x8A34;
  const GLint GL_MAX_SAMPLES                                   = 0x8D57;
  gl.set_gl_max_3d_texture_size(                          ::gl_util::getInt(GL_MAX_3D_TEXTURE_SIZE));
  gl.set_gl_max_array_texture_layers(                     ::gl_util::getInt(GL_MAX_ARRAY_TEXTURE_LAYERS));
  gl.set_gl_max_color_attachments(                        ::gl_util::getInt(GL_MAX_COLOR_ATTACHMENTS));
  gl.set_gl_max_combined_uniform_blocks(                  ::gl_util::getInt(GL_MAX_COMBINED_UNIFORM_BLOCKS));
  gl.set_gl_max_draw_buffers(                             ::gl_util::getInt(GL_MAX_DRAW_BUFFERS));
  gl.set_gl_max_elements_indices(                         ::gl_util::getInt(GL_MAX_ELEMENTS_INDICES));
  gl.set_gl_max_elements_vertices(                        ::gl_util::getInt(GL_MAX_ELEMENTS_VERTICES));
  gl.set_gl_max_fragment_input_components(                ::gl_util::getInt(GL_MAX_FRAGMENT_INPUT_COMPONENTS));
  gl.set_gl_max_fragment_uniform_blocks(                  ::gl_util::getInt(GL_MAX_FRAGMENT_UNIFORM_BLOCKS));
  gl.set_gl_max_fragment_uniform_components(              ::gl_util::getInt(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS));
  gl.set_gl_max_program_texel_offset(                     ::gl_util::getInt(GL_MAX_PROGRAM_TEXEL_OFFSET));
  gl.set_gl_max_transform_feedback_interleaved_components(::gl_util::getInt(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS));
  gl.set_gl_max_transform_feedback_separate_attribs(      ::gl_util::getInt(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS));
  gl.set_gl_max_transform_feedback_separate_components(   ::gl_util::getInt(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS));
  gl.set_gl_max_uniform_buffer_bindings(                  ::gl_util::getInt(GL_MAX_UNIFORM_BUFFER_BINDINGS));
  gl.set_gl_max_varying_components(                       ::gl_util::getInt(GL_MAX_VARYING_COMPONENTS));
  gl.set_gl_max_vertex_output_components(                 ::gl_util::getInt(GL_MAX_VERTEX_OUTPUT_COMPONENTS));
  gl.set_gl_max_vertex_uniform_blocks(                    ::gl_util::getInt(GL_MAX_VERTEX_UNIFORM_BLOCKS));
  gl.set_gl_max_vertex_uniform_components(                ::gl_util::getInt(GL_MAX_VERTEX_UNIFORM_COMPONENTS));
  gl.set_gl_min_program_texel_offset(                     ::gl_util::getInt(GL_MIN_PROGRAM_TEXEL_OFFSET));
  gl.set_gl_uniform_buffer_offset_alignment(              ::gl_util::getInt(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT));
  gl.set_gl_max_samples(                                  ::gl_util::getInt(GL_MAX_SAMPLES));

  const GLint GL_MAX_TEXTURE_LOD_BIAS = 0x84FD;
  gl.set_gl_max_texture_lod_bias(::gl_util::getFloat(GL_MAX_TEXTURE_LOD_BIAS));

  const GLint GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS = 0x8A33;
  const GLint GL_MAX_ELEMENT_INDEX                        = 0x8D6B;
  const GLint GL_MAX_SERVER_WAIT_TIMEOUT                  = 0x9111;
  const GLint GL_MAX_UNIFORM_BLOCK_SIZE                   = 0x8A30;
  ::gl_util::glGetInteger64v = reinterpret_cast<::gl_util::MyTypeGlGetInteger64v>(eglGetProcAddress("glGetInteger64v"));
  gl.set_gl_max_combined_fragment_uniform_components(::gl_util::getInt64(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS));
  gl.set_gl_max_element_index(                       ::gl_util::getInt64(GL_MAX_ELEMENT_INDEX));
  gl.set_gl_max_server_wait_timeout(                 ::gl_util::getInt64(GL_MAX_SERVER_WAIT_TIMEOUT));
  gl.set_gl_max_uniform_block_size(                  ::gl_util::getInt64(GL_MAX_UNIFORM_BLOCK_SIZE));
}
void addGlConstsV3_1(device_info::gl& gl){
  const GLint GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS      = 0x92DC;
  const GLint GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE          = 0x92D8;
  const GLint GL_MAX_COLOR_TEXTURE_SAMPLES               = 0x910E;
  const GLint GL_MAX_COMBINED_ATOMIC_COUNTERS            = 0x92D7;
  const GLint GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS     = 0x92D1;
  const GLint GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS = 0x8266;
  const GLint GL_MAX_COMBINED_IMAGE_UNIFORMS             = 0x90CF;
  const GLint GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES    = 0x8F39;
  const GLint GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS      = 0x90DC;
  const GLint GL_MAX_COMPUTE_ATOMIC_COUNTERS             = 0x8265;
  const GLint GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS      = 0x8264;
  const GLint GL_MAX_COMPUTE_IMAGE_UNIFORMS              = 0x91BD;
  const GLint GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS       = 0x90DB;
  const GLint GL_MAX_COMPUTE_SHARED_MEMORY_SIZE          = 0x8262;
  const GLint GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS         = 0x91BC;
  const GLint GL_MAX_COMPUTE_UNIFORM_BLOCKS              = 0x91BB;
  const GLint GL_MAX_COMPUTE_UNIFORM_COMPONENTS          = 0x8263;
  const GLint GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS      = 0x90EB;
  const GLint GL_MAX_DEPTH_TEXTURE_SAMPLES               = 0x910F;
  const GLint GL_MAX_FRAGMENT_ATOMIC_COUNTERS            = 0x92D6;
  const GLint GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS     = 0x92D0;
  const GLint GL_MAX_FRAGMENT_IMAGE_UNIFORMS             = 0x90CE;
  const GLint GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS      = 0x90DA;
  const GLint GL_MAX_FRAMEBUFFER_HEIGHT                  = 0x9316;
  const GLint GL_MAX_FRAMEBUFFER_SAMPLES                 = 0x9318;
  const GLint GL_MAX_FRAMEBUFFER_WIDTH                   = 0x9315;
  const GLint GL_MAX_IMAGE_UNITS                         = 0x8F38;
  const GLint GL_MAX_INTEGER_SAMPLES                     = 0x9110;
  const GLint GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET       = 0x8E5F;
  const GLint GL_MAX_SAMPLE_MASK_WORDS                   = 0x8E59;
  const GLint GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS      = 0x90DD;
  const GLint GL_MAX_UNIFORM_LOCATIONS                   = 0x826E;
  const GLint GL_MAX_VERTEX_ATOMIC_COUNTERS              = 0x92D2;
  const GLint GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS       = 0x92CC;
  const GLint GL_MAX_VERTEX_ATTRIB_BINDINGS              = 0x82DA;
  const GLint GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET       = 0x82D9;
  const GLint GL_MAX_VERTEX_ATTRIB_STRIDE                = 0x82E5;
  const GLint GL_MAX_VERTEX_IMAGE_UNIFORMS               = 0x90CA;
  const GLint GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS        = 0x90D6;
  const GLint GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET       = 0x8E5E;
  const GLint GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT  = 0x90DF;
  gl.set_gl_max_atomic_counter_buffer_bindings     (::gl_util::getInt(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS));
  gl.set_gl_max_atomic_counter_buffer_size         (::gl_util::getInt(GL_MAX_ATOMIC_COUNTER_BUFFER_SIZE));
  gl.set_gl_max_color_texture_samples              (::gl_util::getInt(GL_MAX_COLOR_TEXTURE_SAMPLES));
  gl.set_gl_max_combined_atomic_counters           (::gl_util::getInt(GL_MAX_COMBINED_ATOMIC_COUNTERS));
  gl.set_gl_max_combined_atomic_counter_buffers    (::gl_util::getInt(GL_MAX_COMBINED_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_combined_compute_uniform_components(::gl_util::getInt(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS));
  gl.set_gl_max_combined_image_uniforms            (::gl_util::getInt(GL_MAX_COMBINED_IMAGE_UNIFORMS));
  gl.set_gl_max_combined_shader_output_resources   (::gl_util::getInt(GL_MAX_COMBINED_SHADER_OUTPUT_RESOURCES));
  gl.set_gl_max_combined_shader_storage_blocks     (::gl_util::getInt(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_compute_atomic_counters            (::gl_util::getInt(GL_MAX_COMPUTE_ATOMIC_COUNTERS));
  gl.set_gl_max_compute_atomic_counter_buffers     (::gl_util::getInt(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_compute_image_uniforms             (::gl_util::getInt(GL_MAX_COMPUTE_IMAGE_UNIFORMS));
  gl.set_gl_max_compute_shader_storage_blocks      (::gl_util::getInt(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_compute_shared_memory_size         (::gl_util::getInt(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE));
  gl.set_gl_max_compute_texture_image_units        (::gl_util::getInt(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_compute_uniform_blocks             (::gl_util::getInt(GL_MAX_COMPUTE_UNIFORM_BLOCKS));
  gl.set_gl_max_compute_uniform_components         (::gl_util::getInt(GL_MAX_COMPUTE_UNIFORM_COMPONENTS));
  gl.set_gl_max_compute_work_group_invocations     (::gl_util::getInt(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS));
  gl.set_gl_max_depth_texture_samples              (::gl_util::getInt(GL_MAX_DEPTH_TEXTURE_SAMPLES));
  gl.set_gl_max_fragment_atomic_counters           (::gl_util::getInt(GL_MAX_FRAGMENT_ATOMIC_COUNTERS));
  gl.set_gl_max_fragment_atomic_counter_buffers    (::gl_util::getInt(GL_MAX_FRAGMENT_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_fragment_image_uniforms            (::gl_util::getInt(GL_MAX_FRAGMENT_IMAGE_UNIFORMS));
  gl.set_gl_max_fragment_shader_storage_blocks     (::gl_util::getInt(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_framebuffer_height                 (::gl_util::getInt(GL_MAX_FRAMEBUFFER_HEIGHT));
  gl.set_gl_max_framebuffer_samples                (::gl_util::getInt(GL_MAX_FRAMEBUFFER_SAMPLES));
  gl.set_gl_max_framebuffer_width                  (::gl_util::getInt(GL_MAX_FRAMEBUFFER_WIDTH));
  gl.set_gl_max_image_units                        (::gl_util::getInt(GL_MAX_IMAGE_UNITS));
  gl.set_gl_max_integer_samples                    (::gl_util::getInt(GL_MAX_INTEGER_SAMPLES));
  gl.set_gl_max_program_texture_gather_offset      (::gl_util::getInt(GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET));
  gl.set_gl_max_sample_mask_words                  (::gl_util::getInt(GL_MAX_SAMPLE_MASK_WORDS));
  gl.set_gl_max_shader_storage_buffer_bindings     (::gl_util::getInt(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS));
  gl.set_gl_max_uniform_locations                  (::gl_util::getInt(GL_MAX_UNIFORM_LOCATIONS));
  gl.set_gl_max_vertex_atomic_counters             (::gl_util::getInt(GL_MAX_VERTEX_ATOMIC_COUNTERS));
  gl.set_gl_max_vertex_atomic_counter_buffers      (::gl_util::getInt(GL_MAX_VERTEX_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_vertex_attrib_bindings             (::gl_util::getInt(GL_MAX_VERTEX_ATTRIB_BINDINGS));
  gl.set_gl_max_vertex_attrib_relative_offset      (::gl_util::getInt(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET));
  gl.set_gl_max_vertex_attrib_stride               (::gl_util::getInt(GL_MAX_VERTEX_ATTRIB_STRIDE));
  gl.set_gl_max_vertex_image_uniforms              (::gl_util::getInt(GL_MAX_VERTEX_IMAGE_UNIFORMS));
  gl.set_gl_max_vertex_shader_storage_blocks       (::gl_util::getInt(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS));
  gl.set_gl_min_program_texture_gather_offset      (::gl_util::getInt(GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET));
  gl.set_gl_shader_storage_buffer_offset_alignment (::gl_util::getInt(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT));

  const GLint GL_MAX_SHADER_STORAGE_BLOCK_SIZE = 0x90DE;
  gl.set_gl_max_shader_storage_block_size(::gl_util::getInt64(GL_MAX_SHADER_STORAGE_BLOCK_SIZE));

  const GLint GL_MAX_COMPUTE_WORK_GROUP_COUNT = 0x91BE;
  const GLint GL_MAX_COMPUTE_WORK_GROUP_SIZE  = 0x91BF;
  ::gl_util::glGetIntegeri_v = reinterpret_cast<::gl_util::MyTypeGlGetIntegeri_v>(eglGetProcAddress("glGetIntegeri_v"));
  gl.set_gl_max_compute_work_group_count_0(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0));
  gl.set_gl_max_compute_work_group_count_1(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1));
  gl.set_gl_max_compute_work_group_count_2(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2));
  gl.set_gl_max_compute_work_group_size_0(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0));
  gl.set_gl_max_compute_work_group_size_1(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1));
  gl.set_gl_max_compute_work_group_size_2(::gl_util::getIntIndexed(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2));
}
void addGlConstsV3_2(device_info::gl& gl){
  const GLint GL_CONTEXT_FLAGS                                   = 0x821E;
  const GLint GL_FRAGMENT_INTERPOLATION_OFFSET_BITS              = 0x8E5D;
  const GLint GL_LAYER_PROVOKING_VERTEX                          = 0x825E;
  const GLint GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS        = 0x8A32;
  const GLint GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS    = 0x8E1E;
  const GLint GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS = 0x8E1F;
  const GLint GL_MAX_DEBUG_GROUP_STACK_DEPTH                     = 0x826C;
  const GLint GL_MAX_DEBUG_LOGGED_MESSAGES                       = 0x9144;
  const GLint GL_MAX_DEBUG_MESSAGE_LENGTH                        = 0x9143;
  const GLint GL_MAX_FRAMEBUFFER_LAYERS                          = 0x9317;
  const GLint GL_MAX_GEOMETRY_ATOMIC_COUNTERS                    = 0x92D5;
  const GLint GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS             = 0x92CF;
  const GLint GL_MAX_GEOMETRY_IMAGE_UNIFORMS                     = 0x90CD;
  const GLint GL_MAX_GEOMETRY_INPUT_COMPONENTS                   = 0x9123;
  const GLint GL_MAX_GEOMETRY_OUTPUT_COMPONENTS                  = 0x9124;
  const GLint GL_MAX_GEOMETRY_OUTPUT_VERTICES                    = 0x8DE0;
  const GLint GL_MAX_GEOMETRY_SHADER_INVOCATIONS                 = 0x8E5A;
  const GLint GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS              = 0x90D7;
  const GLint GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS                = 0x8C29;
  const GLint GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS            = 0x8DE1;
  const GLint GL_MAX_GEOMETRY_UNIFORM_BLOCKS                     = 0x8A2C;
  const GLint GL_MAX_GEOMETRY_UNIFORM_COMPONENTS                 = 0x8DDF;
  const GLint GL_MAX_LABEL_LENGTH                                = 0x82E8;
  const GLint GL_MAX_PATCH_VERTICES                              = 0x8E7D;
  const GLint GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS                = 0x92D3;
  const GLint GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS         = 0x92CD;
  const GLint GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS                 = 0x90CB;
  const GLint GL_MAX_TESS_CONTROL_INPUT_COMPONENTS               = 0x886C;
  const GLint GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS              = 0x8E83;
  const GLint GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS          = 0x90D8;
  const GLint GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS            = 0x8E81;
  const GLint GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS        = 0x8E85;
  const GLint GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS                 = 0x8E89;
  const GLint GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS             = 0x8E7F;
  const GLint GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS             = 0x92D4;
  const GLint GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS      = 0x92CE;
  const GLint GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS              = 0x90CC;
  const GLint GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS            = 0x886D;
  const GLint GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS           = 0x8E86;
  const GLint GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS       = 0x90D9;
  const GLint GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS         = 0x8E82;
  const GLint GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS              = 0x8E8A;
  const GLint GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS          = 0x8E80;
  const GLint GL_MAX_TESS_GEN_LEVEL                              = 0x8E7E;
  const GLint GL_MAX_TESS_PATCH_COMPONENTS                       = 0x8E84;
  const GLint GL_MAX_TEXTURE_BUFFER_SIZE                         = 0x8C2B;
  const GLint GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT                 = 0x919F;
  const GLint GL_REset_NOTIFICATION_STRATEGY                     = 0x8256;
  gl.set_gl_context_flags                                  (::gl_util::getInt(GL_CONTEXT_FLAGS));
  gl.set_gl_fragment_interpolation_offset_bits             (::gl_util::getInt(GL_FRAGMENT_INTERPOLATION_OFFSET_BITS));
  gl.set_gl_layer_provoking_vertex                         (::gl_util::getInt(GL_LAYER_PROVOKING_VERTEX));
  gl.set_gl_max_combined_geometry_uniform_components       (::gl_util::getInt(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS));
  gl.set_gl_max_combined_tess_control_uniform_components   (::gl_util::getInt(GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS));
  gl.set_gl_max_combined_tess_evaluation_uniform_components(::gl_util::getInt(GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS));
  gl.set_gl_max_debug_group_stack_depth                    (::gl_util::getInt(GL_MAX_DEBUG_GROUP_STACK_DEPTH));
  gl.set_gl_max_debug_logged_messages                      (::gl_util::getInt(GL_MAX_DEBUG_LOGGED_MESSAGES));
  gl.set_gl_max_debug_message_length                       (::gl_util::getInt(GL_MAX_DEBUG_MESSAGE_LENGTH));
  gl.set_gl_max_framebuffer_layers                         (::gl_util::getInt(GL_MAX_FRAMEBUFFER_LAYERS));
  gl.set_gl_max_geometry_atomic_counters                   (::gl_util::getInt(GL_MAX_GEOMETRY_ATOMIC_COUNTERS));
  gl.set_gl_max_geometry_atomic_counter_buffers            (::gl_util::getInt(GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_geometry_image_uniforms                    (::gl_util::getInt(GL_MAX_GEOMETRY_IMAGE_UNIFORMS));
  gl.set_gl_max_geometry_input_components                  (::gl_util::getInt(GL_MAX_GEOMETRY_INPUT_COMPONENTS));
  gl.set_gl_max_geometry_output_components                 (::gl_util::getInt(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS));
  gl.set_gl_max_geometry_output_vertices                   (::gl_util::getInt(GL_MAX_GEOMETRY_OUTPUT_VERTICES));
  gl.set_gl_max_geometry_shader_invocations                (::gl_util::getInt(GL_MAX_GEOMETRY_SHADER_INVOCATIONS));
  gl.set_gl_max_geometry_shader_storage_blocks             (::gl_util::getInt(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_geometry_texture_image_units               (::gl_util::getInt(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_geometry_total_output_components           (::gl_util::getInt(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS));
  gl.set_gl_max_geometry_uniform_blocks                    (::gl_util::getInt(GL_MAX_GEOMETRY_UNIFORM_BLOCKS));
  gl.set_gl_max_geometry_uniform_components                (::gl_util::getInt(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS));
  gl.set_gl_max_label_length                               (::gl_util::getInt(GL_MAX_LABEL_LENGTH));
  gl.set_gl_max_patch_vertices                             (::gl_util::getInt(GL_MAX_PATCH_VERTICES));
  gl.set_gl_max_tess_control_atomic_counters               (::gl_util::getInt(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS));
  gl.set_gl_max_tess_control_atomic_counter_buffers        (::gl_util::getInt(GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_tess_control_image_uniforms                (::gl_util::getInt(GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS));
  gl.set_gl_max_tess_control_input_components              (::gl_util::getInt(GL_MAX_TESS_CONTROL_INPUT_COMPONENTS));
  gl.set_gl_max_tess_control_output_components             (::gl_util::getInt(GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS));
  gl.set_gl_max_tess_control_shader_storage_blocks         (::gl_util::getInt(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_tess_control_texture_image_units           (::gl_util::getInt(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_tess_control_total_output_components       (::gl_util::getInt(GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS));
  gl.set_gl_max_tess_control_uniform_blocks                (::gl_util::getInt(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS));
  gl.set_gl_max_tess_control_uniform_components            (::gl_util::getInt(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS));
  gl.set_gl_max_tess_evaluation_atomic_counters            (::gl_util::getInt(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS));
  gl.set_gl_max_tess_evaluation_atomic_counter_buffers     (::gl_util::getInt(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS));
  gl.set_gl_max_tess_evaluation_image_uniforms             (::gl_util::getInt(GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS));
  gl.set_gl_max_tess_evaluation_input_components           (::gl_util::getInt(GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS));
  gl.set_gl_max_tess_evaluation_output_components          (::gl_util::getInt(GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS));
  gl.set_gl_max_tess_evaluation_shader_storage_blocks      (::gl_util::getInt(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS));
  gl.set_gl_max_tess_evaluation_texture_image_units        (::gl_util::getInt(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS));
  gl.set_gl_max_tess_evaluation_uniform_blocks             (::gl_util::getInt(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS));
  gl.set_gl_max_tess_evaluation_uniform_components         (::gl_util::getInt(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS));
  gl.set_gl_max_tess_gen_level                             (::gl_util::getInt(GL_MAX_TESS_GEN_LEVEL));
  gl.set_gl_max_tess_patch_components                      (::gl_util::getInt(GL_MAX_TESS_PATCH_COMPONENTS));
  gl.set_gl_max_texture_buffer_size                        (::gl_util::getInt(GL_MAX_TEXTURE_BUFFER_SIZE));
  gl.set_gl_texture_buffer_offset_alignment                (::gl_util::getInt(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT));
  gl.set_gl_reset_notification_strategy                    (::gl_util::getInt(GL_REset_NOTIFICATION_STRATEGY));

  const GLint GL_MAX_FRAGMENT_INTERPOLATION_OFFSET               = 0x8E5C;
  const GLint GL_MIN_FRAGMENT_INTERPOLATION_OFFSET               = 0x8E5B;
  const GLint GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY              = 0x9382;
  const GLint GL_MULTISAMPLE_LINE_WIDTH_RANGE                    = 0x9381;
  gl.set_gl_max_fragment_interpolation_offset( ::gl_util::getFloat(GL_MAX_FRAGMENT_INTERPOLATION_OFFSET));
  gl.set_gl_min_fragment_interpolation_offset( ::gl_util::getFloat(GL_MIN_FRAGMENT_INTERPOLATION_OFFSET));
  gl.set_gl_multisample_line_width_granularity(::gl_util::getFloat(GL_MULTISAMPLE_LINE_WIDTH_GRANULARITY));
  gl.set_gl_multisample_line_width_range(      ::gl_util::getFloat(GL_MULTISAMPLE_LINE_WIDTH_RANGE));

  const GLint GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED         = 0x8221;
  gl.set_gl_primitive_restart_for_patches_supported(::gl_util::getBool(GL_PRIMITIVE_RESTART_FOR_PATCHES_SUPPORTED));
}
} // namespace

int main(){//int argc, const char * argv[]) {
  device_info::root proto;

  for(const std::string& s : readHardware()){
    proto.add_hardware(s);
  }
  for(const std::string& s : readFeatures()){
    proto.add_cpu_extension(s);
  }

  int numCpu = readNumCpus();
  for(int cpuIndex = 0; cpuIndex < numCpu; cpuIndex++){
    device_info::cpu_core* newCore = proto.add_cpu_core();
    int64 freqMax = readCpuFreqMax(cpuIndex);
    if(freqMax > 0){
      newCore->set_freq_max(freqMax);
    }
  }

  setupEGl();

  device_info::gl& gl = *proto.mutable_gl();
  gl.set_renderer(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
  gl.set_vendor(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
  gl.set_version(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
  gl.set_shading_language_version(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

  GLint glVerMajor = -1;
  GLint glVerMinor = -1;
  const GLint GL_MAJOR_VERSION = 0x821B;
  glGetIntegerv(GL_MAJOR_VERSION, &glVerMajor);
  if(glGetError() != GL_NO_ERROR){ // if GL_MAJOR_VERSION is not recognized, assume version 2.0
    glVerMajor = 2;
    glVerMinor = 0;
  }
  else{
    const GLint GL_MINOR_VERSION = 0x821C;
    glGetIntegerv(GL_MINOR_VERSION, &glVerMinor);
  }
  gl.set_version_major(glVerMajor);
  gl.set_version_minor(glVerMinor);

  // gl extensions
  if(glVerMajor >= 3){
    int numExts = -1;
    const GLint GL_NUM_EXTENSIONS = 0x821D;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExts);
    ::gl_util::glGetStringi = reinterpret_cast<::gl_util::MyTypeGlGetstringi>(eglGetProcAddress("glGetStringi"));
    for(int i = 0; i < numExts; i++){
      std::string s = reinterpret_cast<const char*>(::gl_util::glGetStringi(GL_EXTENSIONS, i));
      gl.add_extension(s);
    }
  }
  else{
    std::string exts = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    std::set<std::string> split;
    ::string_util::splitSpaceDelimUnique(exts, split);
    for(const std::string& s : split){
      gl.add_extension(s);
    }
  }

  if(glVerMajor > 2 || (glVerMajor == 2 && glVerMinor >= 0)){ // >= gles 2.0
    addGlConstsV2_0(gl);
  }
  if(glVerMajor > 3 || (glVerMajor == 3 && glVerMinor >= 0)){ // >= gles 3.0
    addGlConstsV3_0(gl);
  }
  if(glVerMajor > 3 || (glVerMajor == 3 && glVerMinor >= 1)){ // >= gles 3.1
    addGlConstsV3_1(gl);
  }
  if(glVerMajor > 3 || (glVerMajor == 3 && glVerMinor >= 2)){ // >= gles 3.2
    addGlConstsV3_2(gl);
  }

  std::cout << "*Proto debug begin:" << std::endl;
  proto.PrintDebugString();
  std::cout << "*Proto debug end." << std::endl;

  flushGlErrors();

  std::cout << "fin." << std::endl;
  return 0;
}
