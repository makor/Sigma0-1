#include "Fitter.h"
#include "TDatabasePDG.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

Fitter::Fitter()
    : fSpectrum(nullptr),
      fSignalSpectrum(nullptr),
      fTotalFit(nullptr),
      fDoubleGauss(nullptr),
      fSignal(nullptr),
      fBackground(nullptr),
      fSignalCount(0),
      fSignalCountErr(0),
      fBkgCount(1e-30),
      fBkgCountErr(0),
      fLowerBound(0.f),
      fUpperBound(0.f),
      fIntegralWidth(0.005),
      fMeanSignal(0),
      fMeanSignalErr(0),
      fSigmaSignal(0),
      fSigmaSignalErr(0) {}

Fitter::~Fitter() {
  delete fSignal;
  delete fBackground;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Fit Background with second order polynomial, excluding Mlambda +/- 10 MeV
// Fit the Lambda peak with a double Gaussian
void Fitter::FitLambda() {
  if (!fSpectrum) {
    std::cerr << "No histogram to fit - run SetSpectrum() first \n";
    return;
  }

  const float LambdaMass = TDatabasePDG::Instance()->GetParticle(3122)->Mass();
  fLowerBound = LambdaMass - fIntegralWidth;
  fUpperBound = LambdaMass + fIntegralWidth;

  TF1 *backgroundNoPeak =
      new TF1("backgroundNoPeak",
              [&](double *x, double *p) {
                if (x[0] > fLowerBound && x[0] < fUpperBound) {
                  TF1::RejectPoint();
                  return (double)0;
                }
                return p[0] + p[1] * x[0] + p[2] * x[0] * x[0];
              },
              LambdaMass - 0.025, LambdaMass + 0.025, 3);
  TFitResultPtr backgroundR = fSpectrum->Fit(
      "backgroundNoPeak", "SRQ0EM", "", LambdaMass - 0.025, LambdaMass + 0.025);

  // parse then to proper TF1
  fBackground = new TF1("fBackground", "pol2", 0, 1.5);
  fBackground->SetParameter(0, backgroundNoPeak->GetParameter(0));
  fBackground->SetParameter(1, backgroundNoPeak->GetParameter(1));
  fBackground->SetParameter(2, backgroundNoPeak->GetParameter(2));

  // remove background from signal
  auto signalOnly = GetSignalHisto();

  //  // fit signal only
  TF1 *signalSingleGauss = new TF1("signalSingleGauss", "gaus",
                                   LambdaMass - 0.01, LambdaMass + 0.01);
  signalSingleGauss->FixParameter(1, LambdaMass);
  signalOnly->Fit("signalSingleGauss", "Q0REM", "", LambdaMass - 0.01,
                  LambdaMass + 0.01);

  fSignal = new TF1("fSignal", "gaus(0) + gaus(3)", 1.1, 1.3);
  fSignal->SetParameter(0, 0.75 * fSpectrum->GetMaximum());
  fSignal->SetParameter(1, signalSingleGauss->GetParameter(1));
  fSignal->SetParLimits(1, LambdaMass - 0.005, LambdaMass + 0.005);
  fSignal->SetParameter(2, 2.f * signalSingleGauss->GetParameter(2));
  fSignal->SetParameter(3, 0.2 * fSpectrum->GetMaximum());
  fSignal->SetParameter(4, signalSingleGauss->GetParameter(1));
  fSignal->SetParLimits(4, LambdaMass - 0.005, LambdaMass + 0.005);
  fSignal->SetParameter(5, 0.5 * signalSingleGauss->GetParameter(2));
  TFitResultPtr r = signalOnly->Fit("fSignal", "SRQ0EM", "", 1.095, 1.15);

  fTotalFit = new TF1("fTotalFit", "fBackground + fSignal", 1.1, 1.13);
  fTotalFit->SetNpx(1000);
  fTotalFit->SetParameter(3, 0.75 * fSpectrum->GetMaximum());
  fTotalFit->SetParameter(4, fSignal->GetParameter(1));
  fTotalFit->SetParLimits(4, LambdaMass - 0.0025, LambdaMass + 0.0025);
  fTotalFit->SetParameter(5, fSignal->GetParameter(2));
  fTotalFit->SetParameter(6, 0.2 * fSpectrum->GetMaximum());
  fTotalFit->SetParameter(7, fSignal->GetParameter(4));
  fTotalFit->SetParLimits(7, LambdaMass - 0.0025, LambdaMass + 0.0025);
  fTotalFit->SetParameter(8, fSignal->GetParameter(5));
  fTotalFit->SetLineColor(kOrange + 2);
  fSpectrum->Fit("fTotalFit", "SRQEM", "", 1.095, 1.15);

  delete fBackground;
  fBackground = new TF1("fBackground", "pol2", 1.05, 1.25);
  fBackground->SetParameter(0, fTotalFit->GetParameter(0));
  fBackground->SetParameter(1, fTotalFit->GetParameter(1));
  fBackground->SetParameter(2, fTotalFit->GetParameter(2));
  fBackground->SetLineStyle(3);
  fBackground->SetLineColor(kOrange + 2);

  delete fSignal;
  fSignal = new TF1("fSignal", "gaus(0) + gaus(3)", 1.05, 1.25);
  fSignal->SetParameter(0, fTotalFit->GetParameter(3));
  fSignal->SetParameter(1, fTotalFit->GetParameter(4));
  fSignal->SetParameter(2, fTotalFit->GetParameter(5));
  fSignal->SetParameter(3, fTotalFit->GetParameter(6));
  fSignal->SetParameter(4, fTotalFit->GetParameter(7));
  fSignal->SetParameter(5, fTotalFit->GetParameter(8));
  fSignal->SetLineStyle(3);
  fSignal->SetLineColor(kOrange + 2);

  // Extract signal as integral
  //First Gaus
  fSignalAmp = fSignal->GetParameter(0);
  fSignalAmpErr = fSignal->GetParError(0);
  fSignalWidth = fSignal->GetParameter(2)*1000;
  fSignalWidthErr = fSignal->GetParError(2)*1000;

  fSignalInt = std::sqrt(2*TMath::Pi())*fSignalAmp*fSignalWidth;
  fSignalIntErr = fSignalInt*std::sqrt(std::pow(fSignalWidthErr/fSignalWidth,2)+std::pow(fSignalAmpErr/fSignalAmp,2));
  //Second Gaus
  fSignalAmp2 = fSignal->GetParameter(3);
  fSignalAmpErr2 = fSignal->GetParError(3);
  fSignalWidth2 = fSignal->GetParameter(5)*1000;
  fSignalWidthErr2 = fSignal->GetParError(5)*1000;

  fSignalInt2 = std::sqrt(2*TMath::Pi())*fSignalAmp2*fSignalWidth2;
  fSignalIntErr2 = fSignalInt2*std::sqrt(std::pow(fSignalWidthErr2/fSignalWidth2,2)+std::pow(fSignalAmpErr2/fSignalAmp2,2));

  //Combined
  fSignalInt += fSignalInt2;
  fSignalIntErr += fSignalIntErr2;

  fSignalCount = fSignal->Integral(fLowerBound, fUpperBound) /
                 double(fSpectrum->GetBinWidth(1));
  fSignalCountErr =
      fSignal->IntegralError(fLowerBound, fUpperBound, r->GetParams(),
                             r->GetCovarianceMatrix().GetMatrixArray()) /
      double(fSpectrum->GetBinWidth(1));

  fBkgCount = fBackground->Integral(fLowerBound, fUpperBound) /
              float(fSpectrum->GetBinWidth(1));
  fBkgCountErr = fBackground->IntegralError(
                     fLowerBound, fUpperBound, backgroundR->GetParams(),
                     backgroundR->GetCovarianceMatrix().GetMatrixArray()) /
                 float(fSpectrum->GetBinWidth(1));

  const float amp1Err = fTotalFit->GetParError(3);
  const float amp2Err = fTotalFit->GetParError(6);
  const float mean1Err = fTotalFit->GetParError(4);
  const float mean2Err = fTotalFit->GetParError(7);
  const float width1Err = fTotalFit->GetParError(5);
  const float width2Err = fTotalFit->GetParError(8);

  const float amp1 = fTotalFit->GetParameter(3);
  const float amp2 = fTotalFit->GetParameter(6);
  const float mean1 = fTotalFit->GetParameter(4);
  const float mean2 = fTotalFit->GetParameter(7);
  const float width1 = fTotalFit->GetParameter(5);
  const float width2 = fTotalFit->GetParameter(8);

  fMeanSignal = (amp1 * mean1 + amp2 * mean2) / (amp1 + amp2);
  fSigmaSignal = (amp1 * width1 + amp2 * width2) / (amp1 + amp2);
  fMeanSignalErr =
      (amp1Err * amp1Err * std::pow(((amp2 * (mean1 - mean2)) /
                                     ((amp1 + amp2) * (amp1 + amp2))),
                                    2) +
       mean1Err * mean1Err * std::pow(amp1 / (amp1 + amp2), 2) +
       amp2Err * amp2Err *
           std::pow((amp1 * (-mean1 + mean2)) / ((amp1 + amp2) * (amp1 + amp2)),
                    2) +
       mean2Err * mean2Err * std::pow(amp2 / (amp1 + amp2), 2));
  fSigmaSignalErr =
      (amp1Err * amp1Err * std::pow(((amp2 * (width1 - width2)) /
                                     ((amp1 + amp2) * (amp1 + amp2))),
                                    2) +
       width1Err * width1Err * std::pow(amp1 / (amp1 + amp2), 2) +
       amp2Err * amp2Err * std::pow((amp1 * (-width1 + width2)) /
                                        ((amp1 + amp2) * (amp1 + amp2)),
                                    2) +
       width2Err * width2Err * std::pow(amp2 / (amp1 + amp2), 2));

  delete signalSingleGauss;
}

void Fitter::FitSigma() {
  if (!fSpectrum) {
    std::cerr << "No histogram to fit - run SetSpectrum() first \n";
    return;
  }

  const float SigmaMass = TDatabasePDG::Instance()->GetParticle(3212)->Mass();

  fLowerBound = SigmaMass - fIntegralWidth;
  fUpperBound = SigmaMass + fIntegralWidth;

  // Fit Background with forth order polynomial, excluding Msigma +/- 6 MeV
  TF1 *background_noPeak = new TF1(
      "background_noPeak",
      [&](double *x, double *p) {
        if (x[0] > SigmaMass - 0.006 && x[0] < SigmaMass + 0.006) {
          TF1::RejectPoint();
          return (double)0;
        }
        return p[0] + p[1] * x[0] + p[2] * x[0] * x[0] +
               p[3] * x[0] * x[0] * x[0] + p[4] * x[0] * x[0] * x[0] * x[0];
      },
      1.15, 1.25, 5);
  background_noPeak->FixParameter(4, 0);
  TFitResultPtr backgroundR =
      fSpectrum->Fit("background_noPeak", "SRQ0EM", "", 1.165, 1.22);

  // parse then to proper TF1
  TF1 *background2 = new TF1("background2", "pol4", 1.1, 1.3);
  background2->SetParameter(0, background_noPeak->GetParameter(0));
  background2->SetParameter(1, background_noPeak->GetParameter(1));
  background2->SetParameter(2, background_noPeak->GetParameter(2));
  background2->SetParameter(3, background_noPeak->GetParameter(3));
  background2->FixParameter(4, background_noPeak->GetParameter(4));

  // Parse to combined function pol4 + gaus
  TF1 *sigma_singleGauss =
      new TF1("sigma_singleGauss", "background2 + gaus(5)", 1.1, 1.23);
  sigma_singleGauss->SetNpx(1000);
  sigma_singleGauss->FixParameter(0, background2->GetParameter(0));
  sigma_singleGauss->FixParameter(1, background2->GetParameter(1));
  sigma_singleGauss->FixParameter(2, background2->GetParameter(2));
  sigma_singleGauss->FixParameter(3, background2->GetParameter(3));
  sigma_singleGauss->FixParameter(4, background2->GetParameter(4));
  sigma_singleGauss->SetParameter(
      5, fSpectrum->GetBinContent(fSpectrum->FindBin(SigmaMass)) -
             background2->Eval(SigmaMass));
  sigma_singleGauss->SetParameter(6, SigmaMass);
  sigma_singleGauss->SetParameter(7, 0.001);
  fSpectrum->Fit("sigma_singleGauss", "S0RQEM", "", 1.155, 1.22);
  // Parse to combined function pol4 + gaus with constrained sigma mass
  fTotalFit = new TF1("fTotalFit", "sigma_singleGauss", 1.1, 1.3);
  fTotalFit->SetNpx(1000);
  fTotalFit->SetParLimits(6, SigmaMass - 0.005, SigmaMass + 0.005);
  fTotalFit->SetParameter(7, 0.001);
  fTotalFit->FixParameter(4, 0);
//  fTotalFit->FixParameter(5,40);
  fTotalFit->SetLineColor(kOrange + 2);
  TFitResultPtr fullFit = fSpectrum->Fit("fTotalFit", "SRQEM", "", 1.165, 1.22);

  // Get refitted Background function
  fBackground = new TF1("fBackground_refit", "pol4(0)", 1.1, 1.3);
  fBackground->SetNpx(1000);
  fBackground->SetParameter(0, fTotalFit->GetParameter(0));
  fBackground->SetParameter(1, fTotalFit->GetParameter(1));
  fBackground->SetParameter(2, fTotalFit->GetParameter(2));
  fBackground->SetParameter(3, fTotalFit->GetParameter(3));
  fBackground->FixParameter(4, 0);
  fBackground->SetLineStyle(3);
  fBackground->SetLineColor(kOrange + 2);
  fBackground->Draw("same");

  fSignal = new TF1("fSignal", "gaus(0)", 1.05, 1.25);
  fSignal->SetParameter(0, fTotalFit->GetParameter(5));
  fSignal->SetParameter(1, fTotalFit->GetParameter(6));
  fSignal->SetParameter(2, fTotalFit->GetParameter(7));
  fSignal->SetLineStyle(3);
  fSignal->SetLineColor(kOrange + 2);

  fSignalAmp = fTotalFit->GetParameter(5);
  fSignalAmpErr = fTotalFit->GetParError(5);
  fSignalWidth = fTotalFit->GetParameter(7)*1000;
  fSignalWidthErr = fTotalFit->GetParError(7)*1000;
  fSignalInt = std::sqrt(2*TMath::Pi())*fSignalAmp*fSignalWidth;
  fSignalIntErr = fSignalInt*std::sqrt(std::pow(fSignalWidthErr/fSignalWidth,2)+std::pow(fSignalAmpErr/fSignalAmp,2));

  fSignalCount = fSignal->Integral(fLowerBound, fUpperBound) /
                 double(fSpectrum->GetBinWidth(1));
  fSignalCountErr =
      fSignal->IntegralError(fLowerBound, fUpperBound, fullFit->GetParams(),
                             fullFit->GetCovarianceMatrix().GetMatrixArray()) /
      float(fSpectrum->GetBinWidth(1));

  fBkgCount = fBackground->Integral(fLowerBound, fUpperBound) /
              double(fSpectrum->GetBinWidth(1));
  fBkgCountErr = fBackground->IntegralError(
                     fLowerBound, fUpperBound, backgroundR->GetParams(),
                     backgroundR->GetCovarianceMatrix().GetMatrixArray()) /
                 float(fSpectrum->GetBinWidth(1));

  fMeanSignal = fTotalFit->GetParameter(6);
  fSigmaSignal = fTotalFit->GetParameter(7);
  fMeanSignalErr = fTotalFit->GetParError(6);
  fSigmaSignalErr = fTotalFit->GetParError(7);


 // TF1 *sigma_doubleGauss = new TF1("sigma_doubleGauss", "sigma_singleGauss + gaus(8)", 1.1, 1.23);
 // sigma_doubleGauss->SetNpx(1000);
 // sigma_doubleGauss->FixParameter(0, sigma_singleGauss->GetParameter(0));
 // sigma_doubleGauss->FixParameter(1, sigma_singleGauss->GetParameter(1));
 // sigma_doubleGauss->FixParameter(2, sigma_singleGauss->GetParameter(2));
 // sigma_doubleGauss->FixParameter(3, sigma_singleGauss->GetParameter(3));
 // sigma_doubleGauss->FixParameter(4, sigma_singleGauss->GetParameter(4));
 // sigma_doubleGauss->FixParameter(5, sigma_singleGauss->GetParameter(5));
 // sigma_doubleGauss->FixParameter(6, sigma_singleGauss->GetParameter(6));
 // sigma_doubleGauss->FixParameter(7, sigma_singleGauss->GetParameter(7));
 // sigma_doubleGauss->SetParameter(8, sigma_singleGauss->GetParameter(5)+20);
 // sigma_doubleGauss->SetParLimits(9, sigma_singleGauss->GetParameter(6)-0.001,sigma_singleGauss->GetParameter(6)+0.001 );
 // sigma_doubleGauss->FixParameter(10, 0.001);
 // sigma_doubleGauss->SetLineColor(kGreen + 2);
 // fSpectrum->Fit("sigma_doubleGauss", "S0RQEM", "", 1.155, 1.22);
//  fDoubleGauss = new TF1("fDoubleGauss", "sigma_doubleGauss", 1.1, 1.3);
//  fDoubleGauss->SetNpx(1000);
 // fDoubleGauss->FixParameter(4, 0);
 // sigma_doubleGauss->SetParLimits(8, 0,50 );
 // fDoubleGauss->SetLineColor(kGreen + 2);
 // fDoubleGauss->Draw("same");
//  TFitResultPtr fullFit2 = fSpectrum->Fit("fDoubleGauss", "SRQEM", "", 1.165, 1.22);


  delete sigma_singleGauss;
  delete background2;
}
