#ifndef Spectrum_H
#define Spectrum_H

#include "TH1.h"

class Spectrum {
 public:
  Spectrum();
  ~Spectrum() = default;

  void SetRecSpectrum(TH1F* spec) { fRecSpectrum = spec; }
  void SetMCSpectrum(TH1F* spec) { fMCSpectrum = spec; }
  void SetMCTruth(TH1F* spec) { fMCTruth = spec; }
  void ComputeCorrectedSpectrum();

  TH1F* GetEfficiency() const { return fEfficiency; }
  TH1F* GetCorrectedSpectrum() const { return fCorrSpectrum; }

 private:
  TH1F* fRecSpectrum;
  TH1F* fMCSpectrum;
  TH1F* fMCTruth;
  TH1F* fCorrSpectrum;
  TH1F* fEfficiency;
};

#endif  // Spectrum_H
