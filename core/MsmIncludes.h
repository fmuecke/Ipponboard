#pragma once

// Centralize Boost.MSM include setup to silence deprecation messages while keeping
// legacy state machine code intact. The temporary defines are undone again so the
// rest of the translation unit is unaffected.

#ifndef BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#  define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#endif
#ifndef BOOST_MPL_LIMIT_VECTOR_SIZE
#  define BOOST_MPL_LIMIT_VECTOR_SIZE 50
#endif
#ifndef BOOST_MPL_LIMIT_MAP_SIZE
#  define BOOST_MPL_LIMIT_MAP_SIZE 50
#endif

#ifdef BOOST_BIND_GLOBAL_PLACEHOLDERS
#  define IPPONBOARD_RESTORE_BIND_GLOBAL_PLACEHOLDERS
#else
#  define BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif

#ifdef BOOST_ALLOW_DEPRECATED_HEADERS
#  define IPPONBOARD_RESTORE_ALLOW_DEPRECATED
#else
#  define BOOST_ALLOW_DEPRECATED_HEADERS
#endif

#include <boost/core/no_exceptions_support.hpp>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#ifdef IPPONBOARD_RESTORE_ALLOW_DEPRECATED
#  undef IPPONBOARD_RESTORE_ALLOW_DEPRECATED
#else
#  undef BOOST_ALLOW_DEPRECATED_HEADERS
#endif

#ifdef IPPONBOARD_RESTORE_BIND_GLOBAL_PLACEHOLDERS
#  undef IPPONBOARD_RESTORE_BIND_GLOBAL_PLACEHOLDERS
#else
#  undef BOOST_BIND_GLOBAL_PLACEHOLDERS
#endif
