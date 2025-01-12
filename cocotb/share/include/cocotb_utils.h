/******************************************************************************
 * Copyright (c) 2013 Potential Ventures Ltd
 * Copyright (c) 2013 SolarFlare Communications Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of Potential Ventures Ltd,
 *       SolarFlare Communications Inc nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL POTENTIAL VENTURES LTD BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef COCOTB_UTILS_H_
#define COCOTB_UTILS_H_

#include <exports.h>
#ifdef COCOTBUTILS_EXPORTS
#define COCOTBUTILS_EXPORT COCOTB_EXPORT
#else
#define COCOTBUTILS_EXPORT COCOTB_IMPORT
#endif

#include <gpi_logging.h>

#define xstr(a) str(a)
#define str(a) #a

extern "C" COCOTBUTILS_EXPORT void* utils_dyn_open(const char* lib_name);
extern "C" COCOTBUTILS_EXPORT void* utils_dyn_sym(void* handle,
                                                  const char* sym_name);
extern "C" COCOTBUTILS_EXPORT int is_python_context;

// to_python and to_simulator are implemented as macros instead of functions so
// that the logs reference the user's lineno and filename

#define to_python()                                      \
    do {                                                 \
        if (is_python_context) {                         \
            LOG_ERROR("FATAL: We are calling up again"); \
            exit(1);                                     \
        }                                                \
        ++is_python_context;                             \
        LOG_TRACE("Returning to Python");                \
    } while (0)

#define to_simulator()                                              \
    do {                                                            \
        if (!is_python_context) {                                   \
            LOG_ERROR("FATAL: We have returned twice from Python"); \
            exit(1);                                                \
        }                                                           \
        --is_python_context;                                        \
        LOG_TRACE("Returning to simulator");                        \
    } while (0)

#define COCOTB_UNUSED(x) ((void)x)

template <typename F>
class Deferable {
  public:
    constexpr Deferable(F f) : f_(f){};
    ~Deferable() { f_(); }

  private:
    F f_;
};

template <typename F>
constexpr Deferable<F> make_deferable(F f) {
    return Deferable<F>(f);
}

#define DEFER1(a, b) a##b
#define DEFER0(a, b) DEFER1(a, b)
#define DEFER(statement) \
    auto DEFER0(_defer, __COUNTER__) = make_deferable([&]() { statement; });

#endif /* COCOTB_UTILS_H_ */
