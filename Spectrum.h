#ifndef Spectrum_H
#define Spectrum_H

/// \file Spectrum.h
/// \brief Definition of Spectrum computation class
/// \author A. Mathis

#include <iostream>
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "global.h"

/// \class Spectrum
/// This class is used to compute efficiency corrected spectra according to
/// reconstructed spectra, MC spectra and the MC truth
/// Furthermore, some nice helper functions are implemented.
class Spectrum {
 public:
  /// Constructor
  Spectrum();

  /// Constructor
  /// \param add Addendum to the filename
  Spectrum(TString add);

  /// Destructor
  ~Spectrum() = default;

  /// Set the reconstructed inv. mass vs. pT spectrum
  /// \param spec Reconstructed inv. mass vs. pT spectrum
  void SetRecInvMassPt(TH2F* spec) { fRecInvMassPt = spec; }

  /// Set the MC inv. mass vs. pT spectrum
  /// \param spec MC inv. mass vs. pT spectrum
  void SetMCInvMassPt(TH2F* spec) { fMCInvMassPt = spec; }

  /// Set the reconstructed pT spectrum
  /// \param spec Reconstructed pT spectrum
  void SetRecSpectrum(TH1F* spec) { fRecSpectrum = spec; }

  /// Set the pT spectrum from MC
  /// \param spec pT spectrum from MC
  void SetMCSpectrum(TH1F* spec) { fMCSpectrum = spec; }

  /// Set the pT spectrum of the MC truth
  /// \param spec pT spectrum of the MC truth
  void SetMCTruth(TH1F* spec);

  /// Set the trigger efficiency
  /// \param eff Trigger efficiency
  /// \param err Error on the trigger efficiency
  void SetTriggerEfficiency(double eff, double err) {
    fTriggerEfficiency = eff;
    fTriggerEfficiencyErr = err;
  }

  /// Set the branching ratio used for the efficiency calculation
  /// \param br Branching ratio of the decay
  void SetBranchingRatio(double br) { fBranchingRatio = br; }

  /// Set the number of events used for normalization
  /// \param evts Number of events
  void SetNumberOfEvents(double evts) { fNEvents = evts; }

  /// Set the width of the interval around the mean value of the peak
  /// \param width Width of the interval around the mean value of the peak
  void SetIntervalWidth(double width) { fIntervalWidth = width; }

  /// Obtain slices from the inv. mass vs. pT spectra
  /// \param isRec Flag whether we process data or MC
  void GetpTSpectra(bool isRec = true);

  /// Compute the efficiency-corrected pT spectrum
  void ComputeCorrectedSpectrum();

  /// Scale according to the bin width
  /// \param hist Histogram to be scaled
  void ScaleBinWidth(TH1F* hist);

  /// Pass the trigger efficiency into a TH1F object such that the uncertainties
  /// are properly propagated
  void SetTriggerEfficiency();

  /// Get the reconstructed spectrum
  /// \return Reconstructed spectrum
  TH1F* GetReconstructedSpectrum() const;

  /// Get the MC spectrum
  /// \return MC spectrum
  TH1F* GetMCSpectrum() const;

  /// Get the raw MC truth
  /// \return Raw MC truth fMCTruth
  TH1F* GetMCTruth() const;

  /// Get the MC truth corrected for the branching ratio
  /// \return MC truth corrected for the branching ratio fMCTruthCorrected
  TH1F* GetMCTruthCorrected() const;

  /// Get the efficiency vs. pT
  /// \return Efficiency (fMCSpectrum/fMCTruthCorrected)
  TH1F* GetEfficiency() const;

  /// Get trigger efficiency
  /// \return Trigger efficiency with associated uncertainty
  TH1F* GetTriggerEfficiency() const;

  /// Get the efficiency-corrected pT spectrum
  /// \return Efficiency-corrected pT spectrum fCorrSpectrum
  TH1F* GetCorrectedSpectrum() const;

  /// Function to rebin histograms taking into account the uncertainties and
  /// including cases with incompatible bins. Implementation by A. Knospe in
  /// AliPhysics/PWGLF/RESONANCES/macros/utils/RebinSpectrum.C
  /// \param originalHist Histogram to be rebinned
  /// \param statisticalUncertainties Flag whether the histogram contains
  /// statistical or systematic uncertatinties
  /// \ param spectrum Flag whether we are dealing with a spectrum. In that
  /// case, we need to normalize to the bin widthh. On the other hand, when just
  /// rebinning a histogram, this is not needed
  /// \param fit Fit function to be used for the interpolation in case of
  /// incompatible bins. If not set, an exponential function is used
  /// \return Rebinned originalHist
  static TH1F* RebinSpectrum(const TH1F* originalHist,
                             const bool statisticalUncertainties = true,
                             TF1* fit = nullptr);

  /// Rebin a histogram according to the variable bins, using TH1::Rebin
  /// \param originalHist Histogram to be rebinned
  /// \return Rebinned originalHist
  static TH1F* RebinHistogram(TH1F* originalHist);

  /// Function to get a histogram with the proper binning according to
  /// globalpTbins
  /// \param name Name of the histogram
  /// \param title Title of the histogram (title, x-axis, yaxis)
  /// \return Empty histogram with the pT bins specified in globalpTbins
  static TH1F* GetBinnedHistogram(
      TString name, TString title = "; #it{p}_{T} (GeV/#it{c}; Entries");

  /// Projection y implementation to make sure the bins are properly chosen
  /// \param histo Histogram from which the projection is done
  /// \param xLow Lower boundary
  /// \param xUp Upper boundary
  /// \return TH1F One-dimensional histogram
  static TH1F* GetHistoProjectionY(const TH2F* histo, const double xLow,
                                   const double xUp);

  /// Dump all the histograms to a root file
  void WriteToFile() const;

 private:
  TH2F* fRecInvMassPt;             ///< Reconstructed inv. mass vs. pT spectrum
  TH2F* fMCInvMassPt;              ///< MC inv. mass vs. pT spectrum
  std::vector<TH1F*> fInvMassRec;  ///< Fitted reconstructed inv. mass spectra
  std::vector<TH1F*> fInvMassMC;   ///< Fitted reconstructed inv. mass spectra
  TH1F* fRecSpectrum;              ///< Reconstructed pT spectrum
  TH1F* fMCSpectrum;               ///< pT spectrum from MC
  TH1F* fMCTruth;                  ///< pT spectrum of the MC truth
  TH1F* fMCTruthCorrected;    ///< fMCTruth corrected for the branching ratio
  TH1F* fCorrSpectrum;        ///< Efficiency-corrected pT spectrum
  TH1F* fEfficiency;          ///< Efficiency (fMCSpectrum/fMCTruth)
  TH1F* fTriggerEffHist;      ///< Histogram for trigger efficiency correction
  TString fAddendum;          ///< Addendum to the output file name
  double fTriggerEfficiency;  ///< Trigger efficiency
  double fTriggerEfficiencyErr;  ///< Error of the trigger efficiency
  double fBranchingRatio;  ///< Branching ratio for the efficiency correction
  double fNEvents;         ///< Number of events used for normalization
  double fIntervalWidth;   ///< Width of the interval around the mean value of
                           /// the peak
};

inline void Spectrum::SetMCTruth(TH1F* spec) {
  if (spec->GetNbinsX() != globalpTbins.size() - 1) {
    std::cout << "Rebinning the MC truth\n";
    auto rebinSpec = RebinHistogram(spec);
    fMCTruth = rebinSpec;
  } else {
    fMCTruth = spec;
  }
}

inline TH1F* Spectrum::GetReconstructedSpectrum() const {
  if (!fRecSpectrum) {
    std::cerr << "ERROR Spectrum: No reconstructed spectrum yet! Run "
                 "GetpTSpectra() first \n";
    return nullptr;
  } else {
    return fRecSpectrum;
  }
}

inline TH1F* Spectrum::GetMCSpectrum() const {
  if (!fMCSpectrum) {
    std::cerr << "ERROR Spectrum: No MC spectrum yet! Run "
                 "GetpTSpectra() first \n";
    return nullptr;
  } else {
    return fMCSpectrum;
  }
}

inline TH1F* Spectrum::GetMCTruth() const {
  if (!fMCTruth) {
    std::cerr << "ERROR Spectrum: No raw MC truth histogram yet! Run "
                 "ComputeCorrectedSpectrum() first \n";
    return nullptr;
  } else {
    return fMCTruth;
  }
}

inline TH1F* Spectrum::GetMCTruthCorrected() const {
  if (!fMCTruthCorrected) {
    std::cerr << "ERROR Spectrum: No corrected MC truth histogram yet! Run "
                 "ComputeCorrectedSpectrum() first \n";
    return nullptr;
  } else {
    return fMCTruthCorrected;
  }
}

inline TH1F* Spectrum::GetEfficiency() const {
  if (!fEfficiency) {
    std::cerr << "ERROR Spectrum: No efficiency histogram yet! Run "
                 "ComputeCorrectedSpectrum() first \n";
    return nullptr;
  } else {
    return fEfficiency;
  }
}

inline TH1F* Spectrum::GetTriggerEfficiency() const {
  if (!fTriggerEffHist) {
    std::cerr << "ERROR Spectrum: No trigger efficiency histogram yet! Run "
                 "SetTriggerEfficiency() first \n";
    return nullptr;
  } else {
    return fTriggerEffHist;
  }
}

inline TH1F* Spectrum::GetCorrectedSpectrum() const {
  if (!fCorrSpectrum) {
    std::cerr << "ERROR Spectrum: No corrected spectrum yet! Run "
                 "ComputeCorrectedSpectrum() first \n";
    return nullptr;
  } else {
    return fCorrSpectrum;
  }
}
#endif  // Spectrum_H
