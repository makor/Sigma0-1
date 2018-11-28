#include "FileReader.h"
#include "Plotter.h"
#include "Spectrum.h"
#include "SpectrumFitter.h"

int main(int argc, char* argv[]) {
  Plotter::SetStyle();
  Spectrum specSigma("Sigma0");
  Spectrum specAntiSigma("AntiSigma0");

  auto filenameData = TString(argv[1]);
  auto filenameMC = TString(argv[2]);
  auto appendixData = TString(argv[3]);
  auto appendixMC = TString(argv[3]);;

  // Histogram for event normalization
  auto histEventCuts = FileReader::GetHist1D(filenameData, appendixData,
                                             {{"EventCuts"}}, "fHistCutQA");
  const float nEvents = histEventCuts->GetBinContent(5);

  // Histogram for Sigma0 Integral width
  auto histSigmaCuts = FileReader::GetHist1D(filenameData, appendixData,
                                             {{"Sigma0"}}, "fHistCutBooking");
  const float intervalWidth = histSigmaCuts->GetBinContent(1);

  // Sigma0 inv. mass spectrum from data
  auto sigmaHistData = FileReader::GetHist2D(filenameData, appendixData,
                                             {{"Sigma0"}}, "fHistInvMassPt");
  auto antiSigmaHistData = FileReader::GetHist2D(
      filenameData, appendixData, {{"AntiSigma0"}}, "fHistInvMassPt");

  // Sigma0 inv. mass spectrum from MC
  auto sigmaHistMC = FileReader::GetHist2D(filenameMC, appendixMC, {{"Sigma0"}},
                                           "fHistInvMassPt");
  auto antiSigmaHistMC = FileReader::GetHist2D(
      filenameMC, appendixMC, {{"AntiSigma0"}}, "fHistInvMassPt");

  // MC Truth
  auto sigmaMCTruth = FileReader::GetHist1D(
      filenameMC, appendixMC, {{"Sigma0", "MC"}}, "fHistMCTruthPt");
  auto antiSigmaMCTruth = FileReader::GetHist1D(
      filenameMC, appendixMC, {{"AntiSigma0", "MC"}}, "fHistMCTruthPt");

  specSigma.SetIntervalWidth(intervalWidth);
  specSigma.SetNumberOfEvents(nEvents);
  specSigma.SetRecInvMassPt(sigmaHistData);
  specSigma.SetMCInvMassPt(sigmaHistMC);
  specSigma.SetMCTruth(sigmaMCTruth);
  specSigma.ComputeCorrectedSpectrum();

  SpectrumFitter fitter(SpectrumFitter::Tsallis);
  fitter.SetSpectrum(specSigma.GetCorrectedSpectrum());
  fitter.FitSpectrum();

  specSigma.WriteToFile();
}
