#include "Spectrum.h"
#include <iostream>

Spectrum::Spectrum()
    : fRecSpectrum(nullptr),
      fMCSpectrum(nullptr),
      fMCTruth(nullptr),
      fCorrSpectrum(nullptr),
      fEfficiency(nullptr) {}

void Spectrum::ComputeCorrectedSpectrum() {
  if (!fRecSpectrum || !fMCSpectrum || !fMCTruth) {
    std::cerr << "Spectrum missing!\n";
    return;
  }
  fEfficiency = (TH1F*)fMCSpectrum->Clone(Form("%s_Efficiency", fMCSpectrum->GetName()));
  fEfficiency->Divide(fMCTruth);

  fCorrSpectrum = (TH1F*)fRecSpectrum->Clone(Form("%s_Corrected", fMCSpectrum->GetName()));
  fCorrSpectrum->Divide(fEfficiency);
}
