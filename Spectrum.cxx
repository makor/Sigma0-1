#include "Spectrum.h"
#include <cmath>
#include "Fitter.h"
#include "TFile.h"
#include "global.h"

Spectrum::Spectrum()
    : fRecInvMassPt(nullptr),
      fMCInvMassPt(nullptr),
      fInvMassRec(),
      fInvMassMC(),
      fRecSpectrum(nullptr),
      fMCSpectrum(nullptr),
      fMCTruth(nullptr),
      fMCTruthCorrected(nullptr),
      fCorrSpectrum(nullptr),
      fEfficiency(nullptr),
      fAddendum(),
      fBranchingRatio(0.639),
      fNEvents(-1.),
      fIntervalWidth(0.005) {}

Spectrum::Spectrum(TString add)
    : fRecInvMassPt(nullptr),
      fMCInvMassPt(nullptr),
      fInvMassRec(),
      fInvMassMC(),
      fRecSpectrum(nullptr),
      fMCSpectrum(nullptr),
      fMCTruth(nullptr),
      fMCTruthCorrected(nullptr),
      fCorrSpectrum(nullptr),
      fEfficiency(nullptr),
      fAddendum(add),
      fBranchingRatio(0.639),
      fNEvents(-1.),
      fIntervalWidth(0.005) {}

void Spectrum::GetpTSpectra(bool isRec) {
  auto pTvec = globalpTbins;
  auto hist = (isRec) ? fRecInvMassPt : fMCInvMassPt;
  if (!hist) {
    TString output = (isRec) ? "Reconstructed" : "MC";
    std::cerr << "ERROR: " << output << " histogram missing!";
    return;
  }
  TString name = hist->GetName();
  name += (isRec) ? "_data" : "_MC";
  name += "_Rebinned";
  hist->Sumw2();
  auto histSpectrum = Spectrum::GetBinnedHistogram(name);
  for (unsigned int i = 0; i < pTvec.size() - 1; ++i) {
    auto histPt = Spectrum::GetHistoProjectionY(hist, pTvec[i], pTvec[i + 1]);
    TString namepT = histPt->GetName();
    namepT += (isRec) ? "_data" : "_MC";
    histPt->SetName(namepT);
    Fitter fit;
    fit.SetSpectrum(histPt);
    fit.SetIntegralWidth(fIntervalWidth);
    fit.FitSigma();
    const int currentBin = histSpectrum->GetXaxis()->FindBin(
        pTvec[i] + (pTvec[i + 1] - pTvec[i]) / 2.f);
    histSpectrum->SetBinContent(currentBin, fit.GetSignalCount());
    histSpectrum->SetBinError(currentBin, fit.GetSignalCountError());
    if (isRec) {
      fInvMassRec.emplace_back(histPt);
    } else {
      fInvMassMC.emplace_back(histPt);
    }
  }
  delete hist;

  if (isRec) {
    SetRecSpectrum(histSpectrum);
  } else {
    SetMCSpectrum(histSpectrum);
  }
}

void Spectrum::ComputeCorrectedSpectrum() {
  if (!fRecInvMassPt || !fMCInvMassPt) {
    std::cerr << "ERROR: Inv. mass spectrum missing!\n";
    return;
  }

  GetpTSpectra(true);
  GetpTSpectra(false);

  if (!fRecSpectrum || !fMCSpectrum || !fMCTruth) {
    std::cerr << "ERROR: Spectrum missing!\n";
    return;
  }
  if (fNEvents < 0) {
    std::cerr << "ERROR: Number of events for normalization missing!\n";
    return;
  }

  std::cout << "Normalizing the spectra with " << fNEvents << " events \n";
  fRecSpectrum->Sumw2();
  fMCSpectrum->Sumw2();
  fMCTruth->Sumw2();

  TString name = fMCTruth->GetName();
  name += "_BRcorrected";
  fMCTruthCorrected = (TH1F*)fMCTruth->Clone(name);
  fMCTruthCorrected->Scale(fBranchingRatio);

  name = fMCSpectrum->GetName();
  name += "_Efficiency";
  fEfficiency = (TH1F*)fMCSpectrum->Clone(name);
  fEfficiency->Divide(fMCTruthCorrected);

  name = fMCSpectrum->GetName();
  name += "_Corrected";
  fCorrSpectrum = (TH1F*)fRecSpectrum->Clone(name);
  fCorrSpectrum->Divide(fEfficiency);
  fCorrSpectrum->Scale(1.f / fNEvents);
}

TH1F* Spectrum::RebinHisto(const TH1F* originalHist,
                           const bool statisticalUncertainties, TF1* fit) {
  if (!originalHist) {
    std::cerr << "ERROR: Input histogram missing!\n";
    return nullptr;
  }

  // the fit is used to compute the weights when a bin has to be split. In
  // case it does not exist we just use an exponential
  bool doFit = true;
  if (!fit) {
    fit = new TF1("fit", "[0]*exp([1]*x)", 0., 10.);
    fit->SetParameters(1., -1.);
    doFit = false;
  }

  const int nbins = globalpTbins.size();
  double bins[nbins];
  std::copy(globalpTbins.begin(), globalpTbins.end(), bins);

  TString name = originalHist->GetName();
  name += "_rebinned";
  TH1F* rebinnedHist =
      new TH1F(name, originalHist->GetTitle(), nbins - 1, bins);

  // TODO: the histogram for the fit should include the statistical and
  // systematic uncertainties
  name = originalHist->GetName();
  name += "_fit";
  auto fitHist = (TH1F*)originalHist->Clone(name);

  for (unsigned int l = 1; l <= rebinnedHist->GetNbinsX(); ++l) {
    const double lowERebin = rebinnedHist->GetXaxis()->GetBinLowEdge(l);
    const double upERebin = rebinnedHist->GetXaxis()->GetBinLowEdge(l + 1);
    const double widthRebin = rebinnedHist->GetXaxis()->GetBinWidth(l);
    const double e = 1.e-5 * widthRebin;

    double newBinContent = 0.;
    double newBinError = 0.;

    for (unsigned int k = 1; k <= originalHist->GetNbinsX(); ++k) {
      const double lowEOrig = originalHist->GetXaxis()->GetBinLowEdge(k);
      const double upEOrig = originalHist->GetXaxis()->GetBinLowEdge(k + 1);
      const double widthOrig = originalHist->GetXaxis()->GetBinWidth(k);

      double splitBin = 0.f;
      if (upEOrig <= lowERebin + e || upERebin <= lowEOrig + e) {
        continue;
        // bin k originalHist hi completely outside bin l of rebinnedHist
      } else if (lowERebin <= lowEOrig + e && upEOrig <= upERebin + e) {
        // bin k of originalHist completely contained within upEOrign l of
        // rebinnedHist
        splitBin = 1.;
      } else if (lowEOrig <= lowERebin + e || upERebin <= upEOrig + e) {
        // bin k of originalHist is split by the edge(s) of bin l of
        // rebinnedHist
        std::cerr << "Info in RebinSpectrum(): splitting originalHist("
                  << lowEOrig << "," << upEOrig << ") rebinnedHist("
                  << lowERebin << "," << upERebin << ")\n";

        if (doFit) {
          if (k == 1 || fitHist->GetBinContent(k - 1) < 1.e-30) {
            // k is the first non-empty bin of fitHist, fit bin k and the
            // two following bin
            fit->SetRange(fitHist->GetBinLowEdge(k),
                          fitHist->GetBinLowEdge(k + 3));
          } else if (k == fitHist->GetNbinsX() ||
                     fitHist->GetBinContent(k + 1) < 1.e-30) {
            // k is the last non-empty bin of fitHist, fit bin k and the
            // two preceeding bins
            fit->SetRange(fitHist->GetBinLowEdge(k - 2),
                          fitHist->GetBinLowEdge(k + 1));
          } else {
            // k is neither the first nor the last non-empty bin of fitHist
            fit->SetRange(fitHist->GetBinLowEdge(k - 1),
                          fitHist->GetBinLowEdge(k + 2));
          }

          fitHist->Fit(fit, "NR");
        }

        if (lowEOrig <= lowERebin + e && upEOrig <= upERebin + e) {
          // bin k of originalHist is split by the low edge of bin l of
          // rebinnedHist
          splitBin = fit->Integral(lowERebin, upEOrig) /
                     fit->Integral(lowEOrig, upEOrig);
        } else if (lowERebin <= lowEOrig + e && upERebin <= upEOrig + e) {
          // bin k of originalHist is split by the high edge of bin l of
          // rebinnedHist
          splitBin = fit->Integral(lowEOrig, upERebin) /
                     fit->Integral(lowEOrig, upEOrig);
        } else if (lowEOrig <= lowERebin + e && upERebin <= upEOrig + e) {
          // bin k of originalHist completely contains bin l of rebinnedHist
          splitBin = fit->Integral(lowERebin, upERebin) /
                     fit->Integral(lowEOrig, upEOrig);
        } else {
          std::cerr << "Error in RebinSpectrum(): undefined case: originalHist("
                    << lowEOrig << "," << upEOrig << ") rebinnedHist("
                    << lowERebin << "," << upERebin << ")\n";
          continue;
        }
      }

      newBinContent += splitBin * originalHist->GetBinContent(k) * widthOrig;
      // add the content of bin k of originalHist to the total
      if (statisticalUncertainties) {
        newBinError +=
            std::pow(splitBin * originalHist->GetBinError(k) * widthOrig, 2.f);
        // add (in quadrature) the uncertainties of bin k of originalHist to the
        // total
      } else {
        newBinError += splitBin * originalHist->GetBinError(k) * widthOrig;
        // add (linearly) the uncertainties of upEOrign k of originalHist to the
        // total (for systematic uncertainties)
      }
    }

    newBinContent /= widthRebin;
    if (statisticalUncertainties) newBinError = std::sqrt(newBinError);
    newBinError /= widthRebin;

    // fill the output histograms
    rebinnedHist->SetBinContent(l, newBinContent);
    rebinnedHist->SetBinError(l, newBinError);
  }
  return rebinnedHist;
}

TH1F* Spectrum::GetBinnedHistogram(TString name, TString title) {
  const int nbins = globalpTbins.size();
  double bins[nbins];
  std::copy(globalpTbins.begin(), globalpTbins.end(), bins);
  TH1F* newBinnedHist = new TH1F(name, title, nbins - 1, bins);
  return newBinnedHist;
}

TH1F* Spectrum::GetHistoProjectionY(const TH2F* histo, const double xLow,
                                    const double xUp) {
  TString name = TString::Format("%s_%.1f_%.1f", histo->GetName(), xLow, xUp);
  int binLow = 0;
  int binUp = 0;
  const double epsilon = 1.e-5 * histo->GetXaxis()->GetBinWidth(0);
  for (unsigned int i = 0; i < histo->GetNbinsX(); ++i) {
    const double edgeLow = histo->GetXaxis()->GetBinLowEdge(i);
    const double edgeUp = histo->GetXaxis()->GetBinUpEdge(i);
    if (std::abs(edgeLow - xLow) < epsilon) {
      binLow = i;
    }
    if (std::abs(edgeUp - xUp) < epsilon) {
      binUp = i;
      break;
    }
  }
  auto histOut = (TH1F*)histo->ProjectionY(name, binLow, binUp, "e");
  return histOut;
}

void Spectrum::WriteToFile() const {
  TString name = "SpectrumOutput";
  if (!fAddendum.IsNull()) {
    name += "_";
    name += fAddendum;
  }
  name += ".root";
  auto outfile = new TFile(name, "RECREATE");

  outfile->mkdir("Data");
  outfile->cd("Data");
  for (const auto& it : fInvMassRec) {
    (*it).Write((*it).GetName());
  }
  outfile->cd();
  outfile->mkdir("MC");
  outfile->cd("MC");
  for (const auto& it : fInvMassMC) {
    (*it).Write((*it).GetName());
  }

  outfile->cd();
  GetReconstructedSpectrum()->Write("RecSpectrum");
  GetMCSpectrum()->Write("MCSpectrum");
  GetMCTruthCorrected()->Write("MCTruth");
  GetEfficiency()->Write("Efficiency");
  GetCorrectedSpectrum()->Write("CorrectedSpectrum");

  outfile->Write();
  outfile->Close();
}
