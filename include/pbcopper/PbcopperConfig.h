#ifndef PBCOPPER_PBCOPPERCONFIG_H
#define PBCOPPER_PBCOPPERCONFIG_H

// Reduce the number of exposed symbols in order to speed up
// DSO load times
// https://gcc.gnu.org/wiki/Visibility

// clang-format off
#if defined _WIN32 || defined __CYGWIN__
#  ifdef PBCOPPER_BUILDING_LIBRARY
#    ifdef __GNUC__
#      define PBCOPPER_PUBLIC_API __attribute__((dllexport))
#    else
#      define PBCOPPER_PUBLIC_API __declspec(dllexport)  // Note: gcc seems to also supports this syntax
#    endif
#  else
#    ifdef __GNUC__
#      define PBCOPPER_PUBLIC_API __attribute__((dllimport))
#    else
#      define PBCOPPER_PUBLIC_API __declspec(dllimport)  // Note: gcc seems to also supports this syntax
#    endif
#  endif
#  define PBCOPPER_PRIVATE_API
#else
#  if __GNUC__ >= 4
#    define PBCOPPER_PUBLIC_API __attribute__((visibility("default")))
#    define PBCOPPER_PRIVATE_API __attribute__((visibility("hidden")))
#  else
#    define PBCOPPER_PUBLIC_API
#    define PBCOPPER_PRIVATE_API
#  endif
#endif
// clang-format on

/// Disable use of getrandom(), which requires Linux kernel 3.17+.
/// This define allows use of getentropy() in glibc 2.25+, otherwise
/// fallback to 'posix' provider
#ifndef BOOST_UUID_RANDOM_PROVIDER_DISABLE_GETRANDOM
#define BOOST_UUID_RANDOM_PROVIDER_DISABLE_GETRANDOM
#endif

/// Switch for warnings on deprecated APIs
#ifdef PACBIO_NODEPRECATED_API
#define PBCOPPER_DEPRECATED [[deprecated("see header for recommended alternative")]]
#else
#define PBCOPPER_DEPRECATED
#endif

#ifdef __CUDACC__

#ifndef PB_CUDA_HOST
#define PB_CUDA_HOST __host__
#endif

#ifndef PB_CUDA_DEVICE
#define PB_CUDA_DEVICE __device__
#endif

#ifndef PB_CUDA_CONSTANT
#define PB_CUDA_CONSTANT __constant__
#endif

#ifndef PB_CUDA_FORCEINLINE
#define PB_CUDA_FORCEINLINE __forceinline__
#endif

#else

#ifndef PB_CUDA_HOST
#define PB_CUDA_HOST
#endif

#ifndef PB_CUDA_DEVICE
#define PB_CUDA_DEVICE
#endif

#ifndef PB_CUDA_CONSTANT
#define PB_CUDA_CONSTANT
#endif

#ifndef PB_CUDA_FORCEINLINE
#define PB_CUDA_FORCEINLINE
#endif

#endif

constexpr unsigned int PB_CUDA_WARP_SIZE = 32U;

constexpr unsigned int PB_CUDA_FULL_MASK = 0xFFFF'FFFFU;

#endif  // PBCOPPER_PBCOPPERCONFIG_H
