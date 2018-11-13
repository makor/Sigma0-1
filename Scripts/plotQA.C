#include "FileReader.h"
#include "Fitter.h"
#include "Plotter.h"
#include "TCanvas.h"

int main(int argc, char* argv[]) {
  Plotter::SetStyle();
  auto filename = TString(argv[1]);
  auto appendix = TString(argv[2]);
  auto cutStats =
      FileReader::GetHist1D(filename, appendix, {{"EventCuts"}}, "fCutStats");
  Plotter::SetStyleHisto(cutStats);

  auto photonInvMass = FileReader::GetHist1D(
      filename, appendix, {{"ConvCuts_00200008400000002280920000"}},
      "InvMass_after 00200008400000002280920000");
  Plotter::SetStyleHisto(photonInvMass);
  photonInvMass->SetTitle(";M_{e^{+}e^{-}} (GeV/#it{c}); Entries;");

  auto c = new TCanvas();
  cutStats->Draw();
  c->Print("Plot/CutStats.pdf");

  auto d = new TCanvas();
  d->SetLogy();
  photonInvMass->Draw();
  d->Print("Plot/PhotonInvMass.pdf");
}
