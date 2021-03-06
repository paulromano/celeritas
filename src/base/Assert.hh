//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
/*! \file Assert.hh
 *  \brief Macros, exceptions, and helpers for assertions and error handling.
 */
//---------------------------------------------------------------------------//
#pragma once

#include "celeritas_config.h"
#include "Macros.hh"
#ifndef __CUDA_ARCH__
#    include <sstream>
#    include <stdexcept>
#    include <string>
#endif

//---------------------------------------------------------------------------//
// MACROS
//---------------------------------------------------------------------------//
/*!
 * \def REQUIRE
 *
 * Precondition debug assertion macro. It is to "require" that the input values
 * or initial state satisfy a precondition.
 */
/*!
 * \def CHECK
 *
 * Internal debug assertion macro. This replaces standard \c assert usage.
 */
/*!
 * \def ENSURE
 *
 * Postcondition debug assertion macro. Use to "ensure" that return values or
 * side effects are as expected when leaving a function.
 */
/*!
 * \def INSIST
 *
 * Always-on runtime assertion macro. This can check user input and input data
 * consistency, and will raise RuntimeError on failure with a descriptive error
 * message. This should not be used on device.
 */
/*!
 * \def CHECK_UNREACHABLE
 *
 * Assert if the code point is reached. When debug assertions are turned off,
 * this changes to a compiler hint that improves optimization.
 */

//! \cond
#define CELER_CUDA_ASSERT_(COND) \
    do                           \
    {                            \
        assert(COND);            \
    } while (0)
#define CELER_DEBUG_ASSERT_(COND, WHICH)                                        \
    do                                                                          \
    {                                                                           \
        if (CELER_UNLIKELY(!(COND)))                                            \
            ::celeritas::throw_debug_error(                                     \
                ::celeritas::DebugErrorType::WHICH, #COND, __FILE__, __LINE__); \
    } while (0)
#define CELER_RUNTIME_ASSERT_(COND, MSG)                              \
    do                                                                \
    {                                                                 \
        if (CELER_UNLIKELY(!(COND)))                                  \
        {                                                             \
            std::ostringstream celer_runtime_msg_;                    \
            celer_runtime_msg_ << MSG;                                \
            ::celeritas::throw_runtime_error(                         \
                celer_runtime_msg_.str(), #COND, __FILE__, __LINE__); \
        }                                                             \
    } while (0)
#define CELER_NOASSERT_(COND)   \
    do                          \
    {                           \
        if (false && (COND)) {} \
    } while (0)
//! \endcond

#if CELERITAS_DEBUG && defined(__CUDA_ARCH__)
#    define REQUIRE(COND) CELER_CUDA_ASSERT_(COND)
#    define CHECK(COND) CELER_CUDA_ASSERT_(COND)
#    define ENSURE(COND) CELER_CUDA_ASSERT_(COND)
#    define CHECK_UNREACHABLE CELER_CUDA_ASSERT_(false)
#elif CELERITAS_DEBUG && !defined(__CUDA_ARCH__)
#    define REQUIRE(COND) CELER_DEBUG_ASSERT_(COND, precondition)
#    define CHECK(COND) CELER_DEBUG_ASSERT_(COND, internal)
#    define ENSURE(COND) CELER_DEBUG_ASSERT_(COND, postcondition)
#    define CHECK_UNREACHABLE CELER_DEBUG_ASSERT_(false, unreachable)
#else
#    define REQUIRE(COND) CELER_NOASSERT_(COND)
#    define CHECK(COND) CELER_NOASSERT_(COND)
#    define ENSURE(COND) CELER_NOASSERT_(COND)
#    define CHECK_UNREACHABLE CELER_UNREACHABLE
#endif

#ifndef __CUDA_ARCH__
#    define INSIST(COND, MSG) CELER_RUNTIME_ASSERT_(COND, MSG)
#else
#    define INSIST(COND, MSG)                                                  \
        ::celeritas::throw_debug_error(::celeritas::DebugErrorType::assertion, \
                                       "Insist cannot be called from device "  \
                                       "code",                                 \
                                       __FILE__,                               \
                                       __LINE__)
#endif

/*!
 * \def CELER_CUDA_CALL
 *
 * Execute the wrapped statement and throw a RuntimeError if it fails.
 *
 * If it fails, we call \c cudaGetLastError to clear the error code.
 *
 * \code
 *     CELER_CUDA_CALL(cudaMalloc(&ptr_gpu, 100 * sizeof(float)));
 *     CELER_CUDA_CALL(cudaDeviceSynchronize());
 * \endcode
 */
#define CELER_CUDA_CALL(STATEMENT)                       \
    do                                                   \
    {                                                    \
        cudaError_t cuda_result_ = (STATEMENT);          \
        if (CELER_UNLIKELY(cuda_result_ != cudaSuccess)) \
        {                                                \
            cudaGetLastError();                          \
            ::celeritas::throw_cuda_call_error(          \
                cudaGetErrorString(cuda_result_),        \
                #STATEMENT,                              \
                __FILE__,                                \
                __LINE__);                               \
        }                                                \
    } while (0)

/*!
 * \def CELER_CUDA_CHECK_ERROR
 *
 * After a kernel launch or other call, check that no CUDA errors have
 * occurred. This is also useful for checking success after external CUDA
 * libraries have been called.
 */
#define CELER_CUDA_CHECK_ERROR() CELER_CUDA_CALL(cudaPeekAtLastError())

namespace celeritas
{
//---------------------------------------------------------------------------//
// FUNCTIONS
//---------------------------------------------------------------------------//
enum class DebugErrorType
{
    precondition,  //!< Precondition contract violation
    internal,      //!< Internal assertion check failure
    unreachable,   //!< Internal assertion: unreachable code path
    postcondition, //!< Postcondition contract violation
};

//---------------------------------------------------------------------------//
// FUNCTIONS
//---------------------------------------------------------------------------//
// Construct and throw a DebugError.
[[noreturn]] void throw_debug_error(DebugErrorType which,
                                    const char*    condition,
                                    const char*    file,
                                    int            line);

// Construct and throw a RuntimeError for failed CUDA calls.
[[noreturn]] void throw_cuda_call_error(const char* error_string,
                                        const char* code,
                                        const char* file,
                                        int         line);

#ifndef __CUDA_ARCH__
// Construct and throw a RuntimeError.
[[noreturn]] void throw_runtime_error(std::string msg,
                                      const char* condition,
                                      const char* file,
                                      int         line);
#endif

#ifndef __CUDA_ARCH__
//---------------------------------------------------------------------------//
// TYPES
//---------------------------------------------------------------------------//
/*!
 * Error thrown by Celeritas assertions.
 */
class DebugError : public std::logic_error
{
  public:
    // Delegating constructors
    explicit DebugError(const char* msg);
    explicit DebugError(const std::string& msg);
};

//---------------------------------------------------------------------------//
/*!
 * Error thrown by working code from unexpected runtime conditions.
 */
class RuntimeError : public std::runtime_error
{
  public:
    // Delegating constructor
    explicit RuntimeError(const char* msg);
    explicit RuntimeError(const std::string& msg);
};

#endif //__CUDA_ARCH__

//---------------------------------------------------------------------------//
} // namespace celeritas
