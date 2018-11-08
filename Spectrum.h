#ifndef Spectrum_H
#define Spectrum_H

/// \file Spectrum.h
/// \brief Definition of Spectrum computation class
/// \author A. Mathis

#include "TF1.h"
#include "TH1.h"

/// \class Spectrum
/// This class is used to compute efficiency corrected spectra according to
/// reconstructed spectra, MC spectra and the MC truth
/// Furthermore, some nice helper functions are implemented.
class Spectrum {
 public:
  /// Constructor
  Spectrum();

  /// Destructor
  ~Spectrum() = default;

  /// Set the reconstructed pT spectrum
  /// \param spec Reconstructed pT spectrum
  void SetRecSpectrum(TH1F* spec) { fRecSpectrum = spec; }

  /// Set the pT spectrum from MC
  /// \param spec pT spectrum from MC
  void SetMCSpectrum(TH1F* spec) { fMCSpectrum = spec; }

  /// Set the pT spectrum of the MC truth
  /// \param spec pT spectrum of the MC truth
  void SetMCTruth(TH1F* spec) { fMCTruth = spec; }

  /// Compute the efficiency-corrected pT spectrum
  void ComputeCorrectedSpectrum();

  /// Get the efficiency vs. pT
  /// \return Efficiency (fMCSpectrum/fMCTruth)
  TH1F* GetEfficiency() const { return fEfficiency; }

  /// Get the efficiency-corrected pT spectrum
  /// \return Efficiency-corrected pT spectrum
  TH1F* GetCorrectedSpectrum() const { return fCorrSpectrum; }

  /// Function to rebin histograms taking into account the uncertainties and
  /// including cases with incompatible bins. Implementation by A. Knospe in
  /// AliPhysics/PWGLF/RESONANCES/macros/utils/RebinSpectrum.C
  /// \param originalHist Histogram to be rebinned
  /// \param statisticalUncertainties Flag whether the histogram contains
  /// statistical or systematic uncertatinties
  /// \param fit Fit function to be used for the interpolation in case of
  /// incompatible bins. If not set, an exponential function is used
  /// \return Rebinned originalHist
  TH1F* RebinHisto(const TH1F* originalHist,
                   const bool statisticalUncertainties = true,
                   TF1* fit = nullptr) const;

  /// Function to get a histogram with the proper binning according to
  /// globalpTbins
  /// \param name Name of the histogram
  /// \param title Title of the histogram (title, x-axis, yaxis)
  /// \return Empty histogram with the pT bins specified in globalpTbins
  TH1F* GetBinnedHistogram(
      TString name, TString title = "; #it{p}_{T} (GeV/#it{c}; Entries") const;

 private:
  TH1F* fRecSpectrum;   ///< Reconstructed pT spectrum
  TH1F* fMCSpectrum;    ///< pT spectrum from MC
  TH1F* fMCTruth;       ///< pT spectrum of the MC truth
  TH1F* fCorrSpectrum;  ///< Efficiency-corrected pT spectrum
  TH1F* fEfficiency;    ///< Efficiency (fMCSpectrum/fMCTruth)
};

#endif  // Spectrum_H
