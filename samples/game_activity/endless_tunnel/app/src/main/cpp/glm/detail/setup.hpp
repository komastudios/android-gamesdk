///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Mathematics (glm.g-truc.net)
///
/// Copyright (c) 2005 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file glm/core/setup.hpp
/// @date 2006-11-13 / 2013-03-30
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#ifndef GLM_SETUP_INCLUDED
#define GLM_SETUP_INCLUDED

#include <cassert>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Version

#define GLM_VERSION					95
#define GLM_VERSION_MAJOR			0
#define GLM_VERSION_MINOR			9
#define GLM_VERSION_PATCH			5
#define GLM_VERSION_REVISION		3

///////////////////////////////////////////////////////////////////////////////////////////////////
// Platform

#define GLM_PLATFORM_UNKNOWN		0x00000000
#define GLM_PLATFORM_WINDOWS		0x00010000
#define GLM_PLATFORM_LINUX			0x00020000
#define GLM_PLATFORM_APPLE			0x00040000
//#define GLM_PLATFORM_IOS			0x00080000
#define GLM_PLATFORM_ANDROID		0x00100000
#define GLM_PLATFORM_CHROME_NACL	0x00200000
#define GLM_PLATFORM_UNIX			0x00400000
#define GLM_PLATFORM_QNXNTO			0x00800000
#define GLM_PLATFORM_WINCE			0x01000000

#ifdef GLM_FORCE_PLATFORM_UNKNOWN
#	define GLM_PLATFORM GLM_PLATFORM_UNKNOWN
#elif defined(__QNXNTO__)
#	define GLM_PLATFORM GLM_PLATFORM_QNXNTO
#elif defined(__APPLE__)
#	define GLM_PLATFORM GLM_PLATFORM_APPLE
#elif defined(WINCE)
#	define GLM_PLATFORM GLM_PLATFORM_WINCE
#elif defined(_WIN32)
#	define GLM_PLATFORM GLM_PLATFORM_WINDOWS
#elif defined(__native_client__)
#	define GLM_PLATFORM GLM_PLATFORM_CHROME_NACL
#elif defined(__ANDROID__)
#	define GLM_PLATFORM GLM_PLATFORM_ANDROID
#elif defined(__linux)
#	define GLM_PLATFORM GLM_PLATFORM_LINUX
#elif defined(__unix)
#	define GLM_PLATFORM GLM_PLATFORM_UNIX
#else
#	define GLM_PLATFORM GLM_PLATFORM_UNKNOWN
#endif//

// Report platform detection
#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_PLATFORM_DISPLAYED))
#	define GLM_MESSAGE_PLATFORM_DISPLAYED
#	if(GLM_PLATFORM & GLM_PLATFORM_QNXNTO)
#		pragma message("GLM: QNX platform detected")
//#	elif(GLM_PLATFORM & GLM_PLATFORM_IOS)
//#		pragma message("GLM: iOS platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_APPLE)
#		pragma message("GLM: Apple platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_WINCE)
#		pragma message("GLM: WinCE platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_WINDOWS)
#		pragma message("GLM: Windows platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_CHROME_NACL)
#		pragma message("GLM: Native Client detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_ANDROID)
#		pragma message("GLM: Android platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_LINUX)
#		pragma message("GLM: Linux platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_UNIX)
#		pragma message("GLM: UNIX platform detected")
#	elif(GLM_PLATFORM & GLM_PLATFORM_UNKNOWN)
#		pragma message("GLM: platform unknown")
#	else
#		pragma message("GLM: platform not detected")
#	endif
#endif//GLM_MESSAGE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Compiler

// User defines: GLM_FORCE_COMPILER_UNKNOWN
// TODO ? __llvm__ 

#define GLM_COMPILER_UNKNOWN		0x00000000

// Intel
#define GLM_COMPILER_INTEL			0x00100000
#define GLM_COMPILER_INTEL9			0x00100010
#define GLM_COMPILER_INTEL10_0		0x00100020
#define GLM_COMPILER_INTEL10_1		0x00100030
#define GLM_COMPILER_INTEL11_0		0x00100040
#define GLM_COMPILER_INTEL11_1		0x00100050
#define GLM_COMPILER_INTEL12_0		0x00100060
#define GLM_COMPILER_INTEL12_1		0x00100070
#define GLM_COMPILER_INTEL13_0		0x00100080

// Visual C++ defines
#define GLM_COMPILER_VC				0x01000000
#define GLM_COMPILER_VC8			0x01000070
#define GLM_COMPILER_VC9			0x01000080
#define GLM_COMPILER_VC10			0x01000090
#define GLM_COMPILER_VC11			0x010000A0
#define GLM_COMPILER_VC12			0x010000B0

// GCC defines
#define GLM_COMPILER_GCC			0x02000000
#define GLM_COMPILER_GCC34			0x02000050
#define GLM_COMPILER_GCC35			0x02000060
#define GLM_COMPILER_GCC40			0x02000070
#define GLM_COMPILER_GCC41			0x02000080
#define GLM_COMPILER_GCC42			0x02000090
#define GLM_COMPILER_GCC43			0x020000A0
#define GLM_COMPILER_GCC44			0x020000B0
#define GLM_COMPILER_GCC45			0x020000C0
#define GLM_COMPILER_GCC46			0x020000D0
#define GLM_COMPILER_GCC47			0x020000E0
#define GLM_COMPILER_GCC48			0x020000F0
#define GLM_COMPILER_GCC49			0x02000100

// Borland C++
#define GLM_COMPILER_BC				0x04000000

// CodeWarrior
#define GLM_COMPILER_CODEWARRIOR	0x08000000

// CUDA
#define GLM_COMPILER_CUDA			0x10000000
#define GLM_COMPILER_CUDA30			0x10000010
#define GLM_COMPILER_CUDA31			0x10000020
#define GLM_COMPILER_CUDA32			0x10000030
#define GLM_COMPILER_CUDA40			0x10000040
#define GLM_COMPILER_CUDA41			0x10000050
#define GLM_COMPILER_CUDA42			0x10000060

// Clang
#define GLM_COMPILER_CLANG			0x20000000
#define GLM_COMPILER_CLANG26		0x20000010
#define GLM_COMPILER_CLANG27		0x20000020
#define GLM_COMPILER_CLANG28		0x20000030
#define GLM_COMPILER_CLANG29		0x20000040
#define GLM_COMPILER_CLANG30		0x20000050
#define GLM_COMPILER_CLANG31		0x20000060
#define GLM_COMPILER_CLANG32		0x20000070
#define GLM_COMPILER_CLANG33		0x20000080
#define GLM_COMPILER_CLANG40		0x20000090
#define GLM_COMPILER_CLANG41		0x200000A0
#define GLM_COMPILER_CLANG42		0x200000B0
#define GLM_COMPILER_CLANG43		0x200000C0
#define GLM_COMPILER_CLANG50		0x200000D0

// LLVM GCC
#define GLM_COMPILER_LLVM_GCC		0x40000000

// Build model
#define GLM_MODEL_32				0x00000010
#define GLM_MODEL_64				0x00000020

// Force generic C++ compiler
#ifdef GLM_FORCE_COMPILER_UNKNOWN
#	define GLM_COMPILER GLM_COMPILER_UNKNOWN

#elif defined(__INTEL_COMPILER)
#	if __INTEL_COMPILER == 900
#		define GLM_COMPILER GLM_COMPILER_INTEL9
#	elif __INTEL_COMPILER == 1000
#		define GLM_COMPILER GLM_COMPILER_INTEL10_0
#	elif __INTEL_COMPILER == 1010
#		define GLM_COMPILER GLM_COMPILER_INTEL10_1
#	elif __INTEL_COMPILER == 1100
#		define GLM_COMPILER GLM_COMPILER_INTEL11_0
#	elif __INTEL_COMPILER == 1110
#		define GLM_COMPILER GLM_COMPILER_INTEL11_1
#	elif __INTEL_COMPILER == 1200
#		define GLM_COMPILER GLM_COMPILER_INTEL12_0
#	elif __INTEL_COMPILER == 1210
#		define GLM_COMPILER GLM_COMPILER_INTEL12_1
#	elif __INTEL_COMPILER >= 1300
#		define GLM_COMPILER GLM_COMPILER_INTEL13_0
#	else
#		define GLM_COMPILER GLM_COMPILER_INTEL
#	endif

// CUDA
#elif defined(__CUDACC__)
#	if !defined(CUDA_VERSION) && !defined(GLM_FORCE_CUDA)
#		include <cuda.h>  // make sure version is defined since nvcc does not define it itself! 
#	endif
#	if CUDA_VERSION < 3000
#		error "GLM requires CUDA 3.0 or higher"
#	else
#		define GLM_COMPILER GLM_COMPILER_CUDA
#	endif

// Visual C++
#elif defined(_MSC_VER)
#	if _MSC_VER < 1400
#		error "GLM requires Visual C++ 2005 or higher"
#	elif _MSC_VER == 1400
#		define GLM_COMPILER GLM_COMPILER_VC8
#	elif _MSC_VER == 1500
#		define GLM_COMPILER GLM_COMPILER_VC9
#	elif _MSC_VER == 1600
#		define GLM_COMPILER GLM_COMPILER_VC10
#	elif _MSC_VER == 1700
#		define GLM_COMPILER GLM_COMPILER_VC11
#	elif _MSC_VER >= 1800
#		define GLM_COMPILER GLM_COMPILER_VC12
#	else//_MSC_VER
#		define GLM_COMPILER GLM_COMPILER_VC
#	endif//_MSC_VER

// Clang
#elif defined(__clang__)
#	if (__clang_major__ <= 1) || ((__clang_major__ == 2) && (__clang_minor__ < 6))
#		error "GLM requires Clang 2.6 or higher"
#	elif(__clang_major__ == 2) && (__clang_minor__ == 6)
#		define GLM_COMPILER GLM_COMPILER_CLANG26
#	elif(__clang_major__ == 2) && (__clang_minor__ == 7)
#		define GLM_COMPILER GLM_COMPILER_CLANG27
#	elif(__clang_major__ == 2) && (__clang_minor__ == 8)
#		define GLM_COMPILER GLM_COMPILER_CLANG28
#	elif(__clang_major__ == 2) && (__clang_minor__ == 9)
#		define GLM_COMPILER GLM_COMPILER_CLANG29
#	elif(__clang_major__ == 3) && (__clang_minor__ == 0)
#		define GLM_COMPILER GLM_COMPILER_CLANG30
#	elif(__clang_major__ == 3) && (__clang_minor__ == 1)
#		define GLM_COMPILER GLM_COMPILER_CLANG31
#	elif(__clang_major__ == 3) && (__clang_minor__ == 2)
#		define GLM_COMPILER GLM_COMPILER_CLANG32
#	elif(__clang_major__ == 3) && (__clang_minor__ == 3)
#		define GLM_COMPILER GLM_COMPILER_CLANG33
#	elif(__clang_major__ == 4) && (__clang_minor__ == 0)
#		define GLM_COMPILER GLM_COMPILER_CLANG40
#	elif(__clang_major__ == 4) && (__clang_minor__ == 1)
#		define GLM_COMPILER GLM_COMPILER_CLANG41
#	elif(__clang_major__ == 4) && (__clang_minor__ == 2)
#		define GLM_COMPILER GLM_COMPILER_CLANG42
#	elif(__clang_major__ == 4) && (__clang_minor__ >= 3)
#		define GLM_COMPILER GLM_COMPILER_CLANG43
#	elif(__clang_major__ > 4)
#		define GLM_COMPILER GLM_COMPILER_CLANG50
#	else
#		define GLM_COMPILER GLM_COMPILER_CLANG
#	endif

// G++ 
#elif(defined(__GNUC__) || defined(__MINGW32__))// || defined(__llvm__) || defined(__clang__)
#	if (__GNUC__ == 3) && (__GNUC_MINOR__ == 4)
#		define GLM_COMPILER GLM_COMPILER_GCC34
#	elif (__GNUC__ == 3) && (__GNUC_MINOR__ == 5)
#		define GLM_COMPILER GLM_COMPILER_GCC35
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 0)
#		define GLM_COMPILER (GLM_COMPILER_GCC40)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 1)
#		define GLM_COMPILER (GLM_COMPILER_GCC41)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 2)
#		define GLM_COMPILER (GLM_COMPILER_GCC42)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 3)
#		define GLM_COMPILER (GLM_COMPILER_GCC43)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 4)
#		define GLM_COMPILER (GLM_COMPILER_GCC44)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 5)
#		define GLM_COMPILER (GLM_COMPILER_GCC45)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 6)
#		define GLM_COMPILER (GLM_COMPILER_GCC46)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 7)
#		define GLM_COMPILER (GLM_COMPILER_GCC47)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ == 8)
#		define GLM_COMPILER (GLM_COMPILER_GCC48)
#	elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 9)
#		define GLM_COMPILER (GLM_COMPILER_GCC49)
#	elif (__GNUC__ > 4 )
#		define GLM_COMPILER (GLM_COMPILER_GCC49)
#	else
#		define GLM_COMPILER (GLM_COMPILER_GCC)
#	endif

// Borland C++
#elif defined(_BORLANDC_)
#	define GLM_COMPILER GLM_COMPILER_BC

// Codewarrior
#elif defined(__MWERKS__)
#	define GLM_COMPILER GLM_COMPILER_CODEWARRIOR

#else
#	define GLM_COMPILER GLM_COMPILER_UNKNOWN
#endif

#ifndef GLM_COMPILER
#error "GLM_COMPILER undefined, your compiler may not be supported by GLM. Add #define GLM_COMPILER 0 to ignore this message."
#endif//GLM_COMPILER

// Report compiler detection
#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_COMPILER_DISPLAYED))
#	define GLM_MESSAGE_COMPILER_DISPLAYED
#	if(GLM_COMPILER & GLM_COMPILER_CUDA)
#		pragma message("GLM: CUDA compiler detected")
#	elif(GLM_COMPILER & GLM_COMPILER_VC)
#		pragma message("GLM: Visual C++ compiler detected")
#	elif(GLM_COMPILER & GLM_COMPILER_CLANG)
#		pragma message("GLM: Clang compiler detected")
#	elif(GLM_COMPILER & GLM_COMPILER_LLVM_GCC)
#		pragma message("GLM: LLVM GCC compiler detected")
#	elif(GLM_COMPILER & GLM_COMPILER_INTEL)
#		pragma message("GLM: Intel Compiler detected")
#	elif(GLM_COMPILER & GLM_COMPILER_GCC)
#		if(GLM_COMPILER == GLM_COMPILER_GCC_LLVM)
#			pragma message("GLM: LLVM GCC compiler detected")
#		elif(GLM_COMPILER == GLM_COMPILER_GCC_CLANG)
#			pragma message("GLM: CLANG compiler detected")
#		else
#			pragma message("GLM: GCC compiler detected")
#		endif
#	elif(GLM_COMPILER & GLM_COMPILER_BC)
#		pragma message("GLM: Borland compiler detected but not supported")
#	elif(GLM_COMPILER & GLM_COMPILER_CODEWARRIOR)
#		pragma message("GLM: Codewarrior compiler detected but not supported")
#	else
#		pragma message("GLM: Compiler not detected")
#	endif
#endif//GLM_MESSAGE

/////////////////
// Build model //

#if(defined(__arch64__) || defined(__LP64__) || defined(_M_X64) || defined(__ppc64__) || defined(__x86_64__))
#		define GLM_MODEL	GLM_MODEL_64
#elif(defined(__i386__) || defined(__ppc__))
#	define GLM_MODEL	GLM_MODEL_32
#else
#	define GLM_MODEL	GLM_MODEL_32
#endif//

#if(!defined(GLM_MODEL) && GLM_COMPILER != 0)
#	error "GLM_MODEL undefined, your compiler may not be supported by GLM. Add #define GLM_MODEL 0 to ignore this message."
#endif//GLM_MODEL

#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_MODEL_DISPLAYED))
#	define GLM_MESSAGE_MODEL_DISPLAYED
#	if(GLM_MODEL == GLM_MODEL_64)
#		pragma message("GLM: 64 bits model")
#	elif(GLM_MODEL == GLM_MODEL_32)
#		pragma message("GLM: 32 bits model")
#	endif//GLM_MODEL
#endif//GLM_MESSAGE

/////////////////
// C++ Version //

// User defines: GLM_FORCE_CXX98

#define GLM_LANG_CXX_FLAG			(1 << 0)
#define GLM_LANG_CXX98_FLAG			(1 << 1)
#define GLM_LANG_CXX03_FLAG			(1 << 2)
#define GLM_LANG_CXX0X_FLAG			(1 << 3)
#define GLM_LANG_CXX11_FLAG			(1 << 4)
#define GLM_LANG_CXX1Y_FLAG			(1 << 5)
#define GLM_LANG_CXXMS_FLAG			(1 << 6)
#define GLM_LANG_CXXGNU_FLAG		(1 << 7)

#define GLM_LANG_CXX			GLM_LANG_CXX_FLAG
#define GLM_LANG_CXX98			(GLM_LANG_CXX | GLM_LANG_CXX98_FLAG)
#define GLM_LANG_CXX03			(GLM_LANG_CXX98 | GLM_LANG_CXX03_FLAG)
#define GLM_LANG_CXX0X			(GLM_LANG_CXX03 | GLM_LANG_CXX0X_FLAG)
#define GLM_LANG_CXX11			(GLM_LANG_CXX0X | GLM_LANG_CXX11_FLAG)
#define GLM_LANG_CXX1Y			(GLM_LANG_CXX11 | GLM_LANG_CXX1Y_FLAG)
#define GLM_LANG_CXXMS			GLM_LANG_CXXMS_FLAG
#define GLM_LANG_CXXGNU			GLM_LANG_CXXGNU_FLAG

#if(defined(GLM_FORCE_CXX1Y))
#	define GLM_LANG GLM_LANG_CXX1Y
#elif(defined(GLM_FORCE_CXX11))
#	define GLM_LANG GLM_LANG_CXX11
#elif(defined(GLM_FORCE_CXX03))
#	define GLM_LANG GLM_LANG_CXX03
#elif(defined(GLM_FORCE_CXX98))
#	define GLM_LANG GLM_LANG_CXX98
#else
#	if(__cplusplus >= 201103L)
#		define GLM_LANG GLM_LANG_CXX11
#	elif((GLM_COMPILER & GLM_COMPILER_CLANG) == GLM_COMPILER_CLANG)
#		if(GLM_PLATFORM == GLM_PLATFORM_APPLE)
#			define GLM_DETAIL_MAJOR 1
#		else
#			define GLM_DETAIL_MAJOR 0
#		endif
#		if(__clang_major__ < (2 + GLM_DETAIL_MAJOR))
#			define GLM_LANG GLM_LANG_CXX
#		elif(__has_feature(cxx_auto_type))
#			define GLM_LANG GLM_LANG_CXX0X
#		else
#			define GLM_LANG GLM_LANG_CXX98
#		endif
#	elif((GLM_COMPILER & GLM_COMPILER_GCC) == GLM_COMPILER_GCC)
#		if defined(__GXX_EXPERIMENTAL_CXX0X__)
#			define GLM_LANG GLM_LANG_CXX0X
#		else
#			define GLM_LANG GLM_LANG_CXX98
#		endif
#	elif(GLM_COMPILER & GLM_COMPILER_VC)
#		if(defined(_MSC_EXTENSIONS))
#			if(GLM_COMPILER >= GLM_COMPILER_VC10)
#				define GLM_LANG (GLM_LANG_CXX0X | GLM_LANG_CXXMS_FLAG)
#			else
#				define GLM_LANG (GLM_LANG_CXX98 | GLM_LANG_CXXMS_FLAG)
#			endif
#		else
#			if(GLM_COMPILER >= GLM_COMPILER_VC10)
#				define GLM_LANG GLM_LANG_CXX0X
#			else
#				define GLM_LANG GLM_LANG_CXX98
#			endif
#		endif
#	elif(GLM_COMPILER & GLM_COMPILER_INTEL)
#		if(defined(_MSC_EXTENSIONS))
#			if(GLM_COMPILER >= GLM_COMPILER_INTEL13_0)
#				define GLM_LANG (GLM_LANG_CXX0X | GLM_LANG_CXXMS_FLAG)
#			else
#				define GLM_LANG (GLM_LANG_CXX98 | GLM_LANG_CXXMS_FLAG)
#			endif
#		else
#			if(GLM_COMPILER >= GLM_COMPILER_INTEL13_0)
#				define GLM_LANG (GLM_LANG_CXX0X)
#			else
#				define GLM_LANG (GLM_LANG_CXX98)
#			endif
#		endif
#	elif(__cplusplus >= 199711L)
#		define GLM_LANG GLM_LANG_CXX98
#	else
#		define GLM_LANG GLM_LANG_CXX
#	endif
#endif

#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_LANG_DISPLAYED))
#	define GLM_MESSAGE_LANG_DISPLAYED
#	if(GLM_LANG & GLM_LANG_CXXGNU_FLAG)
#		pragma message("GLM: C++ with language extensions")
#	elif(GLM_LANG & GLM_LANG_CXXMS_FLAG)
#		pragma message("GLM: C++ with language extensions")
#	elif(GLM_LANG & GLM_LANG_CXX11_FLAG)
#		pragma message("GLM: C++11")
#	elif(GLM_LANG & GLM_LANG_CXX0X_FLAG)
#		pragma message("GLM: C++0x")
#	elif(GLM_LANG & GLM_LANG_CXX03_FLAG)
#		pragma message("GLM: C++03")
#	elif(GLM_LANG & GLM_LANG_CXX98_FLAG)
#		pragma message("GLM: C++98")
#	else
#		pragma message("GLM: C++ language undetected")
#	endif//GLM_MODEL
#	pragma message("GLM: #define GLM_FORCE_CXX98, GLM_FORCE_CXX03, GLM_LANG_CXX11 or GLM_FORCE_CXX1Y to force using a specific version of the C++ language")
#endif//GLM_MESSAGE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Has of C++ features

#ifndef __has_feature
#	define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#	define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif

// http://clang.llvm.org/cxx_status.html
// http://gcc.gnu.org/projects/cxx0x.html
// http://msdn.microsoft.com/en-us/library/vstudio/hh567368(v=vs.120).aspx

// N1720
#define GLM_HAS_STATIC_ASSERT ( \
	(GLM_LANG & GLM_LANG_CXX11_FLAG) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC10)) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_GCC) && (GLM_COMPILER >= GLM_COMPILER_GCC43)) || \
	__has_feature(cxx_static_assert))

// N1988
#define GLM_HAS_EXTENDED_INTEGER_TYPE ( \
	(GLM_LANG & GLM_LANG_CXX11_FLAG) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC11)) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_GCC) && (GLM_COMPILER >= GLM_COMPILER_GCC43)) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_CLANG) && (GLM_COMPILER >= GLM_COMPILER_CLANG29)))

// N2235
#define GLM_HAS_CONSTEXPR ( \
	(GLM_LANG & GLM_LANG_CXX11_FLAG) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_GCC) && (GLM_COMPILER >= GLM_COMPILER_GCC46)) || \
	__has_feature(cxx_constexpr))

// N2672
#define GLM_HAS_INITIALIZER_LISTS ( \
	(GLM_LANG & GLM_LANG_CXX11_FLAG) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && ((GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC12))) || \
	((GLM_LANG & GLM_LANG_CXX0X_FLAG) && (GLM_COMPILER & GLM_COMPILER_GCC) && (GLM_COMPILER >= GLM_COMPILER_GCC44)) || \
	__has_feature(cxx_generalized_initializers))

// OpenMP
#ifdef _OPENMP 
#	if(GLM_COMPILER & GLM_COMPILER_GCC)
#		if(GLM_COMPILER > GLM_COMPILER_GCC47)
#			define GLM_HAS_OPENMP 31
#		elif(GLM_COMPILER > GLM_COMPILER_GCC44)
#			define GLM_HAS_OPENMP 30
#		elif(GLM_COMPILER > GLM_COMPILER_GCC42)
#			define GLM_HAS_OPENMP 25
#		endif
#	endif//(GLM_COMPILER & GLM_COMPILER_GCC)

#	if(GLM_COMPILER & GLM_COMPILER_VC)
#		if(GLM_COMPILER > GLM_COMPILER_VC8)
#			define GLM_HAS_OPENMP 20
#		endif
#	endif//(GLM_COMPILER & GLM_COMPILER_GCC)
#endif

// Not standard
#define GLM_HAS_ANONYMOUS_UNION (GLM_LANG & GLM_LANG_CXXMS_FLAG)

/////////////////
// Platform 

// User defines: GLM_FORCE_PURE GLM_FORCE_SSE2 GLM_FORCE_AVX

#define GLM_ARCH_PURE		0x0000
#define GLM_ARCH_SSE2		0x0001
#define GLM_ARCH_SSE3		0x0002// | GLM_ARCH_SSE2
#define GLM_ARCH_AVX		0x0008// | GLM_ARCH_SSE3 | GLM_ARCH_SSE2
#define GLM_ARCH_AVX2		0x0010// | GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2

#if(defined(GLM_FORCE_PURE))
#	define GLM_ARCH GLM_ARCH_PURE
#elif(defined(GLM_FORCE_AVX2))
#	define GLM_ARCH (GLM_ARCH_AVX2 | GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#elif(defined(GLM_FORCE_AVX))
#	define GLM_ARCH (GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#elif(defined(GLM_FORCE_SSE3))
#	define GLM_ARCH (GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#elif(defined(GLM_FORCE_SSE2))
#	define GLM_ARCH (GLM_ARCH_SSE2)
#elif(GLM_COMPILER & GLM_COMPILER_VC)
#	if _M_IX86_FP == 2 && defined(__AVX__)
#		define GLM_ARCH (GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#	elif _M_IX86_FP == 2
#		define GLM_ARCH (GLM_ARCH_SSE2)
#	else
#		define GLM_ARCH (GLM_ARCH_PURE)
#	endif
#elif((GLM_PLATFORM & GLM_PLATFORM_APPLE) && (GLM_COMPILER & GLM_COMPILER_GCC))
#	define GLM_ARCH GLM_ARCH_PURE
#elif(((GLM_COMPILER & GLM_COMPILER_GCC) && (defined(__i386__) || defined(__x86_64__))) || (GLM_COMPILER & GLM_COMPILER_LLVM_GCC))
#	if defined(__AVX2__) 
#		define GLM_ARCH (GLM_ARCH_AVX2 | GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#	elif defined(__AVX__)
#		define GLM_ARCH (GLM_ARCH_AVX | GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#	elif defined(__SSE3__)
#		define GLM_ARCH (GLM_ARCH_SSE3 | GLM_ARCH_SSE2)
#	elif defined(__SSE2__)
#		define GLM_ARCH (GLM_ARCH_SSE2)
#	else
#		define GLM_ARCH (GLM_ARCH_PURE)
#	endif
#else
#	define GLM_ARCH GLM_ARCH_PURE
#endif

// With MinGW-W64, including intrinsic headers before intrin.h will produce some errors. The problem is
// that windows.h (and maybe other headers) will silently include intrin.h, which of course causes problems.
// To fix, we just explicitly include intrin.h here.
#if defined(__MINGW32__) && (GLM_ARCH != GLM_ARCH_PURE)
#	include <intrin.h>
#endif

//#if(GLM_ARCH != GLM_ARCH_PURE)
#if(GLM_ARCH & GLM_ARCH_AVX2)
#	include <immintrin.h>
#endif//GLM_ARCH
#if(GLM_ARCH & GLM_ARCH_AVX)
#	include <immintrin.h>
#endif//GLM_ARCH
#if(GLM_ARCH & GLM_ARCH_SSE4)
#	include <smmintrin.h>
#endif//GLM_ARCH
#if(GLM_ARCH & GLM_ARCH_SSE3)
#	include <pmmintrin.h>
#endif//GLM_ARCH
#if(GLM_ARCH & GLM_ARCH_SSE2)
#	include <emmintrin.h>
#	if(GLM_COMPILER == GLM_COMPILER_VC8) // VC8 is missing some intrinsics, workaround
		inline float _mm_cvtss_f32(__m128 A) { return A.m128_f32[0]; }
		inline __m128 _mm_castpd_ps(__m128d PD) { union { __m128 ps; __m128d pd; } c; c.pd = PD; return c.ps; }
		inline __m128d _mm_castps_pd(__m128 PS) { union { __m128 ps; __m128d pd; } c; c.ps = PS; return c.pd; }
		inline __m128i _mm_castps_si128(__m128 PS) { union { __m128 ps; __m128i pi; } c; c.ps = PS; return c.pi; }
		inline __m128 _mm_castsi128_ps(__m128i PI) { union { __m128 ps; __m128i pi; } c; c.pi = PI; return c.ps; }
#	endif
#endif//GLM_ARCH
//#endif//(GLM_ARCH != GLM_ARCH_PURE)

#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_ARCH_DISPLAYED))
#	define GLM_MESSAGE_ARCH_DISPLAYED
#	if(GLM_ARCH == GLM_ARCH_PURE)
#		pragma message("GLM: Platform independent code")
#	elif(GLM_ARCH & GLM_ARCH_SSE2)
#		pragma message("GLM: SSE2 instruction set")
#	elif(GLM_ARCH & GLM_ARCH_SSE3)
#		pragma message("GLM: SSE3 instruction set")
#	elif(GLM_ARCH & GLM_ARCH_SSE4)
#		pragma message("GLM: SSE4 instruction set")
#	elif(GLM_ARCH & GLM_ARCH_AVX)
#		pragma message("GLM: AVX instruction set")
#	elif(GLM_ARCH & GLM_ARCH_AVX2)
#		pragma message("GLM: AVX2 instruction set")
#	endif//GLM_ARCH
#	pragma message("GLM: #define GLM_FORCE_PURE to avoid using platform specific instruction sets")
#endif//GLM_MESSAGE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Radians

//#define GLM_FORCE_RADIANS

///////////////////////////////////////////////////////////////////////////////////////////////////
// Static assert

#if GLM_HAS_STATIC_ASSERT
#	define GLM_STATIC_ASSERT(x, message) static_assert(x, message)
#elif(defined(BOOST_STATIC_ASSERT))
#	define GLM_STATIC_ASSERT(x, message) BOOST_STATIC_ASSERT(x)
#elif(GLM_COMPILER & GLM_COMPILER_VC)
#	define GLM_STATIC_ASSERT(x, message) typedef char __CASSERT__##__LINE__[(x) ? 1 : -1]
#else
#	define GLM_STATIC_ASSERT(x, message)
#	define GLM_STATIC_ASSERT_NULL
#endif//GLM_LANG

///////////////////////////////////////////////////////////////////////////////////////////////////
// Qualifiers 

// User defines: GLM_FORCE_INLINE GLM_FORCE_CUDA

#if(defined(GLM_FORCE_CUDA) || (GLM_COMPILER & GLM_COMPILER_CUDA))
#	define GLM_CUDA_FUNC_DEF __device__ __host__ 
#	define GLM_CUDA_FUNC_DECL __device__ __host__ 
#else
#	define GLM_CUDA_FUNC_DEF
#	define GLM_CUDA_FUNC_DECL
#endif

#if GLM_COMPILER & GLM_COMPILER_GCC
#	define GLM_VAR_USED __attribute__ ((unused))
#else
#	define GLM_VAR_USED
#endif

#if(defined(GLM_FORCE_INLINE))
#	if((GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC8))
#		define GLM_INLINE __forceinline
#	elif((GLM_COMPILER & GLM_COMPILER_GCC) && (GLM_COMPILER >= GLM_COMPILER_GCC34))
#		define GLM_INLINE __attribute__((always_inline)) inline
#	elif(GLM_COMPILER & GLM_COMPILER_CLANG)
#		define GLM_INLINE __attribute__((always_inline))
#	else
#		define GLM_INLINE inline
#	endif//GLM_COMPILER
#else
#	define GLM_INLINE inline
#endif//defined(GLM_FORCE_INLINE)

#define GLM_FUNC_DECL GLM_CUDA_FUNC_DECL
#define GLM_FUNC_QUALIFIER GLM_CUDA_FUNC_DEF GLM_INLINE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Swizzle operators

// User defines: GLM_SWIZZLE

#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_SWIZZLE_DISPLAYED))
#	define GLM_MESSAGE_SWIZZLE_DISPLAYED
#	if defined(GLM_SWIZZLE)
#		pragma message("GLM: Swizzling operators enabled")
#	else
#		pragma message("GLM: Swizzling operators disabled, #define GLM_SWIZZLE to enable swizzle operators")
#	endif
#endif//GLM_MESSAGE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Length type

// User defines: GLM_FORCE_SIZE_T_LENGTH

namespace glm
{
#if defined(GLM_FORCE_SIZE_T_LENGTH)
	typedef std::size_t length_t;
#else
	typedef int length_t;
#endif
}//namespace glm

#if(defined(GLM_MESSAGES) && !defined(GLM_MESSAGE_FORCE_SIZE_T_LENGTH))
#	define GLM_MESSAGE_FORCE_SIZE_T_LENGTH
#	if defined(GLM_FORCE_SIZE_T_LENGTH)
#		pragma message("GLM: .length() returns glm::length_t, a typedef of std::size_t")
#	else
#		pragma message("GLM: .length() returns glm::length_t, a typedef of int following the GLSL specification")
#		pragma message("GLM: #define GLM_FORCE_SIZE_T_LENGTH for .length() to return a std::size_t")
#	endif
#endif//GLM_MESSAGE

///////////////////////////////////////////////////////////////////////////////////////////////////
// Qualifiers

#if((GLM_COMPILER & GLM_COMPILER_VC) && (GLM_COMPILER >= GLM_COMPILER_VC8))
#	define GLM_DEPRECATED __declspec(deprecated)
#	define GLM_ALIGN(x) __declspec(align(x))
#	define GLM_ALIGNED_STRUCT(x) __declspec(align(x)) struct
#	define GLM_RESTRICT __declspec(restrict)
#	define GLM_RESTRICT_VAR __restrict
#elif(GLM_COMPILER & GLM_COMPILER_INTEL)
#	define GLM_DEPRECATED
#	define GLM_ALIGN(x) __declspec(align(x))
#	define GLM_ALIGNED_STRUCT(x) __declspec(align(x)) struct
#	define GLM_RESTRICT
#	define GLM_RESTRICT_VAR __restrict
#elif(GLM_COMPILER & (GLM_COMPILER_GCC | GLM_COMPILER_CLANG))
#	define GLM_DEPRECATED __attribute__((__deprecated__))
#	define GLM_ALIGN(x) __attribute__((aligned(x)))
#	define GLM_ALIGNED_STRUCT(x) struct __attribute__((aligned(x)))
#	define GLM_RESTRICT __restrict__
#	define GLM_RESTRICT_VAR __restrict__
#else
#	define GLM_DEPRECATED
#	define GLM_ALIGN
#	define GLM_ALIGNED_STRUCT(x)
#	define GLM_RESTRICT
#	define GLM_RESTRICT_VAR
#endif//GLM_COMPILER

#if GLM_HAS_CONSTEXPR
#	define GLM_CONSTEXPR constexpr
#else
#	define GLM_CONSTEXPR
#endif

#endif//GLM_SETUP_INCLUDED
