#ifndef SpectrumFitter_H
#define SpectrumFitter_H

/// \file SpectrumFitter.h
/// \brief Definition of Spectrum fitter class
/// \author A. Mathis

#include <iostream>
#include "TF1.h"
#include "TH1.h"

/// \class SpectrumFitter
class SpectrumFitter {
 public:
  enum FitFunction {
    Tsallis = 0,  ///< Use the Tsallis function
  };

  /// Constructor
  SpectrumFitter();

  /// Constructor
  /// \param fit Fit function to be used
  SpectrumFitter(FitFunction fit = Tsallis);

  /// Use Levy-Tsallis function for the fit
  /// The parametrization comes from TrueTsallis() in
  /// $ALICE_PHYSICS/PWGLF/SPECTRA/Utils/SpectraUtils.C
  void SetupTsallis();

  /// Set the efficiency corrected spectrum
  /// \param spec Efficiency corrected spectrum
  void SetSpectrum(TH1F* spec) { fSpectrum = spec; }

  /// Fit the Spectrum
  void FitSpectrum();

 private:
  TH1F* fSpectrum;    ///< Spectrum to be fitted
  TF1* fFitFunction;  ///< Function that is used to fit
  double fYield;      ///< pT integrated yield
  double fYieldErr;   ///< Error of the pT integrated Yield
  double fMassSigma;  ///< Mass of the Sigma0, needed for the fit
};
#endif  // SpectrumFitter_H
