#include "FileReader.h"
#include "Fitter.h"
#include "Plotter.h"
#include "TCanvas.h"

int main(int argc, char* argv[]) {
  Fitter sigmaFitter;

  Plotter::SetStyle();

  auto filename = TString(argv[1]);
  auto appendix = TString(argv[2]);
  auto sigmaHist =
      FileReader::GetHist2D(filename, appendix, "Sigma0", "fHistInvMassPt");
  auto sigmaSpectrum = (TH1F*)sigmaHist->ProjectionY("sigmaSpectrum");


  auto antisigmaHist = FileReader::GetHist2D(filename, appendix,
                                              "AntiSigma0", "fHistInvMassPt");
  auto antisigmaSpectrum =
      (TH1F*)antisigmaHist->ProjectionY("antisigmaSpectrum");

  antisigmaSpectrum->GetXaxis()->SetTitle("M_{#bar{p}#pi^{+}} (GeV/#it{c}^{2})");
  sigmaSpectrum->Add(antisigmaSpectrum);

  Plotter::SetStyleHisto(sigmaSpectrum);
  sigmaSpectrum->GetXaxis()->SetTitle("M_{#Lambda#gamma} (GeV/#it{c}^{2})");
  sigmaFitter.SetSpectrum(sigmaSpectrum);
  sigmaFitter.SetIntegralWidth(0.004);

  auto c = new TCanvas();
  sigmaSpectrum->GetXaxis()->SetRangeUser(1.155, 1.225);
  sigmaFitter.FitSigma();
  sigmaSpectrum->Draw("PE");
  sigmaFitter.PrintStats("#Sigma");
  sigmaFitter.GetBackgroundFit()->Draw("same");
  c->Print("Plot/Sigma.pdf");

  return 1;
}
