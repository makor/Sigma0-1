#ifndef Fitter_H
#define Fitter_H

#include <cmath>
#include "TF1.h"
#include "TH1.h"
#include "TLatex.h"
#include "TPad.h"
#include "TStyle.h"

class Fitter {
 public:
  Fitter();
  ~Fitter();
  void FitLambda();
  void FitSigma();

  TF1 *GetTotalFit() const { return fTotalFit; }
  TF1 *GetSignalFit() const { return fSignal; }
  TF1 *GetBackgroundFit() const { return fBackground; }

  float GetSignalCount() const { return fSignalInt; }
  float GetSignalCountError() const { return fSignalIntErr; }
  float GetBackgroundCount() const { return fBkgCount; }
  float GetBackgroundCountError() const { return fBkgCountErr; }
  float GetMeanSignal() const { return fMeanSignal; }
  float GetMeanSignalErr() const { return fMeanSignalErr; }
  float GetSigmaSignal() const { return fSigmaSignal; }
  float GetSigmaSignalErr() const { return fSigmaSignalErr; }
  float GetSoB() const { return fSignalCount / fBkgCount; }
  float GetSoBError() const;
  float GetPurity() const { return fSignalCount / (fSignalCount + fBkgCount); }
  float GetPurityError() const;
  TH1F *GetSignalHisto();

  void SetSpectrum(TH1F *hist) { fSpectrum = hist; }
  void SetIntegralWidth(const float intWidth) { fIntegralWidth = intWidth; }

  void PrintStats(TString particle) const;

 private:
  TH1F *fSpectrum;
  TH1F *fSignalSpectrum;

  TF1 *fTotalFit;
  TF1 *fSignal;
  TF1 *fBackground;

  float fSignalAmp;
  float fSignalAmpErr;
  float fSignalWidth;
  float fSignalWidthErr;
  float fSignalAmp2;
  float fSignalAmpErr2;
  float fSignalWidth2;
  float fSignalWidthErr2;
  float fSignalInt;
  float fSignalIntErr;
  float fSignalInt2;
  float fSignalIntErr2;
  float fSignalCount;
  float fSignalCountErr;
  float fBkgCount;
  float fBkgCountErr;

  float fLowerBound;
  float fUpperBound;
  float fIntegralWidth;

  float fMeanSignal;
  float fMeanSignalErr;
  float fSigmaSignal;
  float fSigmaSignalErr;
};

inline float Fitter::GetSoBError() const {
  return std::sqrt(std::pow(fSignalCountErr, 2) / std::pow(fBkgCount, 2) +
                   std::pow(fBkgCountErr, 2) * std::pow(fSignalCount, 2) /
                       std::pow(fBkgCount, 4));
}

inline float Fitter::GetPurityError() const {
  return std::sqrt(std::pow(fSignalCountErr, 2) * std::pow(fBkgCount, 2) /
                       std::pow((fSignalCount * fBkgCount), 4) +
                   std::pow(fBkgCountErr, 2) * std::pow(fSignalCount, 2) /
                       std::pow((fSignalCount * fBkgCount), 4));
}

inline TH1F *Fitter::GetSignalHisto() {
  if (!fSignalSpectrum) {
    fSignalSpectrum = new TH1F(
        Form("%s_signalOnly", fSpectrum->GetName()), "", fSpectrum->GetNbinsX(),
        fSpectrum->GetXaxis()->GetXmin(), fSpectrum->GetXaxis()->GetXmax());
  } else {
    fSignalSpectrum->Reset();
  }
  for (int i = 0; i < fSpectrum->GetEntries(); ++i) {
    float weight = fSpectrum->GetBinContent(i) -
                   fBackground->Eval(fSpectrum->GetBinCenter(i));
    fSignalSpectrum->SetBinContent(i, weight);
    fSignalSpectrum->SetBinError(i, std::sqrt(weight));
  }
  return fSignalSpectrum;
}

inline void Fitter::PrintStats(TString particle) const {
  float count = fSignalCount;
  int pow = 0;
  while (count > 999.9f) {
    count /= 1000.f;
    pow += 3;
  }
  TLatex LambdaLabel;
  LambdaLabel.SetNDC(kTRUE);
  LambdaLabel.SetTextSize(gStyle->GetTextSize() * 0.8);
  LambdaLabel.DrawLatex(
      gPad->GetUxmax() - 0.8, gPad->GetUymax() - 0.33,
      Form("#splitline{#splitline{#splitline{%s: %.1f #times 10^{%i}}{m = "
           "%.1f (MeV/#it{c}^{2})}}{#sigma = %.1f "
           "(MeV/#it{c}^{2})}}{Purity = %.1f %%}",
           particle.Data(), count, pow, fMeanSignal * 1000.f,
           fSigmaSignal * 1000.f, GetPurity() * 100.f));
}

#endif  // Fitter_H
