#include "SpectrumFitter.h"
#include <cmath>
#include "TDatabasePDG.h"
#include "global.h"

SpectrumFitter::SpectrumFitter()
    : fSpectrum(nullptr),
      fFitFunction(nullptr),
      fYield(0.),
      fYieldErr(0.),
      fMassSigma(TDatabasePDG::Instance()->GetParticle(3212)->Mass()) {
  SetupTsallis();
}

SpectrumFitter::SpectrumFitter(FitFunction fit)
    : fSpectrum(nullptr),
      fFitFunction(nullptr),
      fYield(0.),
      fYieldErr(0.),
      fMassSigma(TDatabasePDG::Instance()->GetParticle(3212)->Mass()) {
  if (fit == Tsallis) {
    SetupTsallis();
  } else {
    std::cerr << "ERROR SpectrumFitter: Fit function not defined! \n";
  }
}

void SpectrumFitter::SetupTsallis() {
  fFitFunction = new TF1("TsallisFunc",
                         [&](double* x, double* p) {
                           Double_t pt = x[0];
                           Double_t mass = p[0];
                           Double_t mt = std::sqrt(pt * pt + mass * mass);
                           Double_t q = p[1];
                           Double_t T = p[2];
                           Double_t dNdy = p[3];

                           Double_t part1 = pt * mt;
                           Double_t part2 = part1 / T;
                           Double_t part3 = (2. - q) * (3. - 2. * q);
                           Double_t part4 = (2. - q) * mass * mass +
                                            2. * mass * T + 2. * T * T;
                           Double_t part5 = part3 / part4;

                           Double_t part6 = 1. + (q - 1.) * mass / T;
                           Double_t part7 = std::pow(part6, 1. / (q - 1.));

                           Double_t part8 = 1. + (q - 1.) * mt / T;
                           Double_t part9 = std::pow(part8, -q / (q - 1.));
                           return part2 * dNdy * part5 * part7 * part9;
                         },
                         0, 10, 4);
  fFitFunction->SetParameters(fMassSigma, 1.1, 0.1, 0.2);
  fFitFunction->SetParNames("mass", "q", "T", "dN/dy");
  fFitFunction->FixParameter(0, fMassSigma);
}

void SpectrumFitter::FitSpectrum() {
  if (!fSpectrum) {
    std::cerr << "ERROR SpectrumFitter: Spectrum missing \n";
    return;
  }
  const float lowerBoundary = globalpTbins[0];
  const float upperBoundary = globalpTbins[globalpTbins.size() - 1];
  std::cout << "======================================================\n";
  std::cout << "Fitting \n";
  fSpectrum->Fit(fFitFunction, "", "RQEM", 0, 10);
  fYield = fFitFunction->GetParameter(3);
  fYieldErr = fFitFunction->GetParError(3);
  std::cout << "Chi2/NDF: "
            << fFitFunction->GetChisquare() / fFitFunction->GetNDF() << "\n";
}
