//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file BetheHeitlerInteractor.hh
//---------------------------------------------------------------------------//
#pragma once

#include "base/Macros.hh"
#include "base/Types.hh"
#include "physics/base/Interaction.hh"
#include "physics/base/ParticleTrackView.hh"
#include "physics/base/Secondary.hh"
#include "physics/base/SecondaryAllocatorView.hh"
#include "physics/base/Units.hh"
#include "physics/material/ElementView.hh"
#include "BetheHeitlerInteractorPointers.hh"

namespace celeritas
{
//---------------------------------------------------------------------------//
/*!
 * Bethe-Heitler model for gamma -> e+e- (electron-pair production).
 *
 * Give an incident gamma, it adds a two pair-produced secondary electrons to
 * the secondary stack. No cutoffs are performed on the incident gamma energy.
 *
 * \note This performs the same sampling routine as in Geant4's
 * G4BetheHeitlerModel, as documented in section 6.5 of the Geant4 Physics
 * Reference (release 10.6), applicable to incident gammas with energy
 * \$f E_gamma \leq 100 GeV \$f. For \$f E_gamma > 80 \$f GeV, it is suggested
 * to use `G4PairProductionRelModel`.
 */
class BetheHeitlerInteractor
{
  public:
    //! Construct sampler from shared and state data
    inline CELER_FUNCTION
    BetheHeitlerInteractor(const BetheHeitlerInteractorPointers& shared,
                           const ParticleTrackView&              particle,
                           const Real3&                          inc_direction,
                           SecondaryAllocatorView&               allocate,
                           const ElementView&                    element);

    // Sample an interaction with the given RNG
    template<class Engine>
    inline CELER_FUNCTION Interaction operator()(Engine& rng);

    // >>> COMMON PROPERTIES

    // Minimum incident gamma energy for this model
    // (used for the parameterization in the
    // cross-section calculation).
    static CELER_CONSTEXPR_FUNCTION units::MevEnergy min_incident_energy()
    {
        return units::MevEnergy{1.5}; // 1.5 MeV
    }

    // Maximum incident gamma energy for this mode (used for the
    // parameterization in the cross-section calculation). Above this energy,
    // the cross section is constant.
    static CELER_CONSTEXPR_FUNCTION units::MevEnergy max_incident_energy()
    {
        return units::MevEnergy{100000.0}; // 100 GeV
    }

  private:
    // Calculates the screening variable, \deta, which is a function of
    // \epsilon. This is a measure of the "impact parameter" of the incident
    // photon.
    inline CELER_FUNCTION real_type impact_parameter(real_type eps) const;

    // Screening function, Phi_1, for the corrected Bethe-Heitler
    // cross-section calculation.
    inline CELER_FUNCTION real_type
    screening_phi1(real_type impact_parameter) const;

    // Screening function, Phi_2, for the corrected Bethe-Heitler
    // cross-section calculation.
    inline CELER_FUNCTION real_type
    screening_phi2(real_type impact_parameter) const;

    // Auxiliary screening function, Phi_1, for the "composition+rejection"
    // technique for sampling.
    inline CELER_FUNCTION real_type screening_phi1_aux(real_type delta) const;

    // Auxiliary screening function, Phi_2, for the "composition+rejection"
    // technique for sampling.
    inline CELER_FUNCTION real_type screening_phi2_aux(real_type delta) const;

    // Gamma energy divided by electron mass * csquared
    const BetheHeitlerInteractorPointers& shared_;

    // Sample outgoing particles directions.
    // Based on the G4ModifiedTsai sampler, a simplified sampler that does not
    // require exact momentum conservation (due to neglecting nucleus recoil).
    template<class Engine>
    inline CELER_FUNCTION real_type sample_cos_theta(real_type kinetic_energy,
                                                     Engine&   rng);

    // Incident gamma energy
    const units::MevEnergy inc_energy_;

    // Incident direction
    const Real3& inc_direction_;

    // Allocate space for a secondary particle
    SecondaryAllocatorView& allocate_;

    // Element properties for calculating screening functions and variables
    const ElementView& element_;

    // Cached minimum epsilon, m_e*c^2/E_gamma; kinematical limit for Y -> e+e-
    real_type epsilon0_;
};

//---------------------------------------------------------------------------//
} // namespace celeritas

#include "BetheHeitlerInteractor.i.hh"
