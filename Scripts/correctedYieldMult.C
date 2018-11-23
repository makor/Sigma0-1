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
  auto appendix = TString(argv[3]);
  auto MultAppendix = TString(argv[4]);

  auto Histname = TString("fHistPtMult_") + MultAppendix;
  auto HistnameTruth = TString("fHistMCTruthPtMult") + MultAppendix;

  // Histogram for event normalization TODO Should be changed to the
  // multiplicity one
  auto histEventCuts = FileReader::GetHist1D(filenameData, appendix,
                                             {{"EventCuts"}}, "fHistCutQA");
  const float nEvents = histEventCuts->GetBinContent(5);

  // Histogram for Sigma0 Integral width
  auto histSigmaCuts = FileReader::GetHist1D(filenameData, appendix,
                                             {{"Sigma0"}}, "fHistCutBooking");
  const float intervalWidth = histSigmaCuts->GetBinContent(1);

  // Sigma0 inv. mass spectrum from data
  auto sigmaHistData =
      FileReader::GetHist2D(filenameData, appendix, {{"Sigma0"}}, Histname);
  //  auto sigmaHistData1 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_1");
  //  auto sigmaHistData2 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_2");
  //  auto sigmaHistData3 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_3");
  auto sigmaHistData4 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_4");
  auto sigmaHistData5 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_5");
  auto sigmaHistData6 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_6");
  auto sigmaHistData7 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_7");
  auto sigmaHistData8 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_8");
  auto sigmaHistData9 = FileReader::GetHist2D(filenameData, appendix,
                                              {{"Sigma0"}}, "fHistPtMult_9");
  auto sigmaHistData10 = FileReader::GetHist2D(filenameData, appendix,
                                               {{"Sigma0"}}, "fHistPtMult_10");
  //  auto sigmaHistData11 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_11");
  //  auto sigmaHistData12 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_12");
  //  auto sigmaHistData13 = FileReader::GetHist2D(filenameData, appendix,
  //                                              {{"Sigma0"}},
  //                                              "fHistPtMult_13");

  //  sigmaHistData->Add(sigmaHistData1);
  //  sigmaHistData->Add(sigmaHistData2);
  //  sigmaHistData->Add(sigmaHistData3);
  sigmaHistData->Add(sigmaHistData4);
  sigmaHistData->Add(sigmaHistData5);
  sigmaHistData->Add(sigmaHistData6);
  sigmaHistData->Add(sigmaHistData7);
  sigmaHistData->Add(sigmaHistData8);
  sigmaHistData->Add(sigmaHistData9);
  sigmaHistData->Add(sigmaHistData10);
  //  sigmaHistData->Add(sigmaHistData11);
  //  sigmaHistData->Add(sigmaHistData12);
  //  sigmaHistData->Add(sigmaHistData13);

  auto antiSigmaHistData =
      FileReader::GetHist2D(filenameData, appendix, {{"AntiSigma0"}}, Histname);
  //  auto antiSigmaHistData1 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_1");
  //  auto antiSigmaHistData2 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_2");
  //  auto antiSigmaHistData3 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_3");
  auto antiSigmaHistData4 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_4");
  auto antiSigmaHistData5 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_5");
  auto antiSigmaHistData6 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_6");
  auto antiSigmaHistData7 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_7");
  auto antiSigmaHistData8 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_8");
  auto antiSigmaHistData9 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_9");
  auto antiSigmaHistData10 = FileReader::GetHist2D(
      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_10");
  //  auto antiSigmaHistData11 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_11");
  //  auto antiSigmaHistData12 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_12");
  //  auto antiSigmaHistData13 = FileReader::GetHist2D(
  //      filenameData, appendix, {{"Sigma0"}}, "fHistPtMult_13");

  //  antiSigmaHistData->Add(antiSigmaHistData1);
  //  antiSigmaHistData->Add(antiSigmaHistData2);
  //  antiSigmaHistData->Add(antiSigmaHistData3);
  antiSigmaHistData->Add(antiSigmaHistData4);
  antiSigmaHistData->Add(antiSigmaHistData5);
  antiSigmaHistData->Add(antiSigmaHistData6);
  antiSigmaHistData->Add(antiSigmaHistData7);
  antiSigmaHistData->Add(antiSigmaHistData8);
  antiSigmaHistData->Add(antiSigmaHistData9);
  antiSigmaHistData->Add(antiSigmaHistData10);
  //  antiSigmaHistData->Add(antiSigmaHistData11);
  //  antiSigmaHistData->Add(antiSigmaHistData12);
  //  antiSigmaHistData->Add(antiSigmaHistData13);

  // Sigma0 inv. mass spectrum from MC
  auto sigmaHistMC =
      FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}}, Histname);
  //  auto sigmaHistMC1 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_1");
  //  auto sigmaHistMC2 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_2");
  //  auto sigmaHistMC3 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_3");
  auto sigmaHistMC4 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_4");
  auto sigmaHistMC5 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_5");
  auto sigmaHistMC6 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_6");
  auto sigmaHistMC7 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_7");
  auto sigmaHistMC8 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_8");
  auto sigmaHistMC9 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                            "fHistPtMult_9");
  auto sigmaHistMC10 = FileReader::GetHist2D(filenameMC, appendix, {{"Sigma0"}},
                                             "fHistPtMult_10");
  //  auto sigmaHistMC11 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_11");
  //  auto sigmaHistMC12 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_12");
  //  auto sigmaHistMC13 = FileReader::GetHist2D(filenameMC, appendix,
  //  {{"Sigma0"}},
  //                                            "fHistPtMult_13");

  //  sigmaHistMC->Add(sigmaHistMC1);
  //  sigmaHistMC->Add(sigmaHistMC2);
  //  sigmaHistMC->Add(sigmaHistMC3);
  sigmaHistMC->Add(sigmaHistMC4);
  sigmaHistMC->Add(sigmaHistMC5);
  sigmaHistMC->Add(sigmaHistMC6);
  sigmaHistMC->Add(sigmaHistMC7);
  sigmaHistMC->Add(sigmaHistMC8);
  sigmaHistMC->Add(sigmaHistMC9);
  sigmaHistMC->Add(sigmaHistMC10);
  //  sigmaHistMC->Add(sigmaHistMC11);
  //  sigmaHistMC->Add(sigmaHistMC12);
  //  sigmaHistMC->Add(sigmaHistMC13);

  auto antiSigmaHistMC =
      FileReader::GetHist2D(filenameMC, appendix, {{"AntiSigma0"}}, Histname);
  //  auto antiSigmaHistMC1 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_1");
  //  auto antiSigmaHistMC2 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_2");
  //  auto antiSigmaHistMC3 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_3");
  auto antiSigmaHistMC4 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_4");
  auto antiSigmaHistMC5 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_5");
  auto antiSigmaHistMC6 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_6");
  auto antiSigmaHistMC7 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_7");
  auto antiSigmaHistMC8 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_8");
  auto antiSigmaHistMC9 = FileReader::GetHist2D(filenameMC, appendix,
                                                {{"Sigma0"}}, "fHistPtMult_9");
  auto antiSigmaHistMC10 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0"}}, "fHistPtMult_10");
  //  auto antiSigmaHistMC11 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_11");
  //  auto antiSigmaHistMC12 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_12");
  //  auto antiSigmaHistMC13 = FileReader::GetHist2D(filenameMC, appendix,
  //                                                {{"Sigma0"}},
  //                                                "fHistPtMult_13");

  //  antiSigmaHistMC->Add(antiSigmaHistMC1);
  //  antiSigmaHistMC->Add(antiSigmaHistMC2);
  //  antiSigmaHistMC->Add(antiSigmaHistMC3);
  antiSigmaHistMC->Add(antiSigmaHistMC4);
  antiSigmaHistMC->Add(antiSigmaHistMC5);
  antiSigmaHistMC->Add(antiSigmaHistMC6);
  antiSigmaHistMC->Add(antiSigmaHistMC7);
  antiSigmaHistMC->Add(antiSigmaHistMC8);
  antiSigmaHistMC->Add(antiSigmaHistMC9);
  antiSigmaHistMC->Add(antiSigmaHistMC10);
  //  antiSigmaHistMC->Add(antiSigmaHistMC11);
  //  antiSigmaHistMC->Add(antiSigmaHistMC12);
  //  antiSigmaHistMC->Add(antiSigmaHistMC13);

  // MC Truth
  auto sigmaMCTruth = FileReader::GetHist1D(filenameMC, appendix,
                                            {{"Sigma0", "MC"}}, HistnameTruth);
  //  auto sigmaMCTruth1 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult1");
  //  auto sigmaMCTruth2 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult2");
  //  auto sigmaMCTruth3 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult3");
  auto sigmaMCTruth4 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult4");
  auto sigmaMCTruth5 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult5");
  auto sigmaMCTruth6 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult6");
  auto sigmaMCTruth7 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult7");
  auto sigmaMCTruth8 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult8");
  auto sigmaMCTruth9 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult9");
  auto sigmaMCTruth10 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult10");
  //  auto sigmaMCTruth11 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult11");
  //  auto sigmaMCTruth12 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult12");
  //  auto sigmaMCTruth13 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult13");

  //  sigmaMCTruth->Add(sigmaMCTruth1);
  //  sigmaMCTruth->Add(sigmaMCTruth2);
  //  sigmaMCTruth->Add(sigmaMCTruth3);
  sigmaMCTruth->Add(sigmaMCTruth4);
  sigmaMCTruth->Add(sigmaMCTruth5);
  sigmaMCTruth->Add(sigmaMCTruth6);
  sigmaMCTruth->Add(sigmaMCTruth7);
  sigmaMCTruth->Add(sigmaMCTruth8);
  sigmaMCTruth->Add(sigmaMCTruth9);
  sigmaMCTruth->Add(sigmaMCTruth10);
  //  sigmaMCTruth->Add(sigmaMCTruth11);
  //  sigmaMCTruth->Add(sigmaMCTruth12);
  //  sigmaMCTruth->Add(sigmaMCTruth13);

  auto antiSigmaMCTruth = FileReader::GetHist1D(
      filenameMC, appendix, {{"AntiSigma0", "MC"}}, HistnameTruth);
  //  auto antiSigmaMCTruth1 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult1");
  //  auto antiSigmaMCTruth2 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult2");
  //  auto antiSigmaMCTruth3 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult3");
  auto antiSigmaMCTruth4 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult4");
  auto antiSigmaMCTruth5 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult5");
  auto antiSigmaMCTruth6 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult6");
  auto antiSigmaMCTruth7 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult7");
  auto antiSigmaMCTruth8 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult8");
  auto antiSigmaMCTruth9 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult9");
  auto antiSigmaMCTruth10 = FileReader::GetHist2D(
      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult10");
  //  auto antiSigmaMCTruth11 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult11");
  //  auto antiSigmaMCTruth12 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult12");
  //  auto antiSigmaMCTruth13 = FileReader::GetHist2D(
  //      filenameMC, appendix, {{"Sigma0", "MC"}}, "fHistMCTruthPtMult13");

  //  antiSigmaMCTruth->Add(antiSigmaMCTruth1);
  //  antiSigmaMCTruth->Add(antiSigmaMCTruth2);
  //  antiSigmaMCTruth->Add(antiSigmaMCTruth3);
  antiSigmaMCTruth->Add(antiSigmaMCTruth4);
  antiSigmaMCTruth->Add(antiSigmaMCTruth5);
  antiSigmaMCTruth->Add(antiSigmaMCTruth6);
  antiSigmaMCTruth->Add(antiSigmaMCTruth7);
  antiSigmaMCTruth->Add(antiSigmaMCTruth8);
  antiSigmaMCTruth->Add(antiSigmaMCTruth9);
  antiSigmaMCTruth->Add(antiSigmaMCTruth10);
  //  antiSigmaMCTruth->Add(antiSigmaMCTruth11);
  //  antiSigmaMCTruth->Add(antiSigmaMCTruth12);
  //  antiSigmaMCTruth->Add(antiSigmaMCTruth13);

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
