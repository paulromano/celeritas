//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file PDGNumber.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/OpaqueId.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//

//! Opaque type used for PDG particle definition codes (can be negative!)
using PDGNumber = OpaqueId<struct PDG, int>;

//---------------------------------------------------------------------------//
/*!
 * \namespace pdg
 *
 * Unique standard model particle identifiers by the Particle Data Group.
 *
 * This namespace acts an enumeration for PDG codes that are used by the
 * various processes in Celeritas. (Unlike an enumeration, though, PDG codes
 * can be arbitary and aren't limited to the ones defined below.) They should
 * be extended as needed when new particle types are used by processes.
 */
namespace pdg
{
//---------------------------------------------------------------------------//
//!@{
//! Particle Data Group Monte Carlo number codes.
#define CELER_DEFINE_PDGNUMBER(NAME, VALUE) \
    inline constexpr PDGNumber NAME() { return PDGNumber{VALUE}; }

// Sorted by (abs(val), val < 0)
CELER_DEFINE_PDGNUMBER(electron, 11)
CELER_DEFINE_PDGNUMBER(positron, -11)
CELER_DEFINE_PDGNUMBER(mu_minus, 13)
CELER_DEFINE_PDGNUMBER(mu_plus, -13)
CELER_DEFINE_PDGNUMBER(gamma, 22)
CELER_DEFINE_PDGNUMBER(pi_plus, 211)
CELER_DEFINE_PDGNUMBER(pi_minus, -211)
CELER_DEFINE_PDGNUMBER(kaon_plus, 321)
CELER_DEFINE_PDGNUMBER(kaon_minus, -321)
CELER_DEFINE_PDGNUMBER(proton, 2212)
CELER_DEFINE_PDGNUMBER(anti_proton, -2212)
CELER_DEFINE_PDGNUMBER(he3, 1000020030)
CELER_DEFINE_PDGNUMBER(anti_he3, -1000020030)
CELER_DEFINE_PDGNUMBER(alpha, 1000020040)
CELER_DEFINE_PDGNUMBER(anti_alpha, -1000020040)
CELER_DEFINE_PDGNUMBER(deuteron, 1000010020)
CELER_DEFINE_PDGNUMBER(anti_deuteron, -1000010020)
CELER_DEFINE_PDGNUMBER(triton, 1000010030)
CELER_DEFINE_PDGNUMBER(anti_triton, -1000010030)

#undef CELER_DEFINE_PDGNUMBER
//!@}
//---------------------------------------------------------------------------//
} // namespace pdg
} // namespace celeritas
