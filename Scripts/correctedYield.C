#include "FileReader.h"
#include "Fitter.h"
#include "Plotter.h"
#include "Spectrum.h"
#include "TCanvas.h"
#include "global.h"

int main(int argc, char* argv[]) {
  Plotter::SetStyle();
  Spectrum spec;

  auto filenameData = TString(argv[1]);
  auto filenameMC = TString(argv[2]);
  auto appendix = TString(argv[3]);

  auto sigmaHistData =
      FileReader::GetHist2D(filenameData, appendix, "Sigma0", "fHistInvMassPt");
  auto antiSigmaHistData = FileReader::GetHist2D(
      filenameData, appendix, "AntiSigma0", "fHistInvMassPt");

  auto sigmaHistMC =
      FileReader::GetHist2D(filenameMC, appendix, "Sigma0", "fHistInvMassPt");
  auto antiSigmaHistMC = FileReader::GetHist2D(filenameMC, appendix,
                                               "AntiSigma0", "fHistInvMassPt");

  auto pTvec = globalpTbins;
  for (unsigned int i = 0; i < pTvec.size(); ++i) {
    // do the projection
    // auto hist = ProjY(i);
    //...
  }
}
