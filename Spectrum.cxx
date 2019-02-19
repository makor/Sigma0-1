#include "Spectrum.h"
#include <cmath>
#include "Fitter.h"
#include "Plotter.h"
#include "TCanvas.h"
#include "TFile.h"

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
      fTriggerEffHist(nullptr),
      fAddendum(),
      fTriggerEfficiency(0.745),
      fTriggerEfficiencyErr(0.019),
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
      fTriggerEffHist(nullptr),
      fAddendum(add),
      fTriggerEfficiency(0.745),
      fTriggerEfficiencyErr(0.019),
      fBranchingRatio(0.639),
      fNEvents(-1.),
      fIntervalWidth(0.005) {}

void Spectrum::GetpTSpectra(bool isRec) {
  auto pTvec = globalpTbins;
  auto hist = (isRec) ? fRecInvMassPt : fMCInvMassPt;
  if (!hist) {
    TString output = (isRec) ? "Reconstructed" : "MC";
    std::cerr << "ERROR Spectrum: " << output << " histogram missing!";
    return;
  }
  TString name = hist->GetName();
  name += (isRec) ? "_data" : "_MC";
  name += "_Rebinned";
  auto histSpectrum = Spectrum::GetBinnedHistogram(name);
  TString title = "; #it{p}_{T} (GeV/#it{c}); N_{";
  title += (isRec) ? "data" : "MC";
  title += "}";
  histSpectrum->SetTitle(title);
  for (unsigned int i = 0; i < pTvec.size() - 1; ++i) {
    auto histPt = Spectrum::GetHistoProjectionY(hist, pTvec[i], pTvec[i + 1]);
    Plotter::SetStyleHisto(histPt);
    histPt->SetMarkerSize(0.6);
    TString namepT = histPt->GetName();
    namepT += (isRec) ? "_data" : "_MC";
    histPt->SetName(namepT);
    title = TString::Format(
        "%.2f < #it{p}_{T} < %.2f GeV/#it{c}; M_{#Lambda#gamma} "
        "(GeV/#it{c}^{2}); Entries",
        pTvec[i], pTvec[i + 1]);
    histPt->SetTitle(title);
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
    std::cerr << "ERROR Spectrum: Inv. mass spectrum missing!\n";
    return;
  }

  GetpTSpectra(true);
  GetpTSpectra(false);

  if (!fRecSpectrum || !fMCSpectrum || !fMCTruth) {
    std::cerr << "ERROR Spectrum: Spectrum missing!\n";
    return;
  }
  if (fNEvents < 0) {
    std::cerr
        << "ERROR Spectrum: Number of events for normalization missing!\n";
    return;
  }

  std::cout << "======================================================\n";
  std::cout << "Normalizing the spectra with " << fNEvents << " events \n";
  SetTriggerEfficiency();

  TString name = fMCTruth->GetName();
  //Correct for primary vertex reconstruction efficiency of 92.8 +/- 1.2 percent
  //https://arxiv.org/pdf/1411.4981.pdf
  name += "_PVRcorrected";
  fMCTruthCorrected = (TH1F*)fMCTruth->Clone(name);
  fMCTruthCorrected->Scale(0.928);
  fMCTruthCorrected->SetTitle(
      "; #it{p}_{T} (GeV/#it{c}); N_{MC truth} #times BR");
  Plotter::SetStyleHisto(fMCTruthCorrected);

  name = fMCSpectrum->GetName();
  name += "_Efficiency";
  fEfficiency = (TH1F*)fMCSpectrum->Clone(name);
  fEfficiency->Divide(fMCTruthCorrected);
  fEfficiency->SetTitle("; #it{p}_{T} (GeV/#it{c}); A #times #varepsilon");

  name = fMCSpectrum->GetName();
  name += "_Corrected";
  fCorrSpectrum = (TH1F*)fRecSpectrum->Clone(name);
  fCorrSpectrum->Divide(fEfficiency);

  /// Scale according to the bin width
  ScaleBinWidth(fCorrSpectrum);

  /// Event normalization
  fCorrSpectrum->Scale(1.f / fNEvents);

  /// Treat Sigma and AntiSigma the same
  fCorrSpectrum->Scale(0.5);

  /// Trigger efficiency
  fCorrSpectrum->Multiply(GetTriggerEfficiency());
  fCorrSpectrum->SetTitle(
      "; #it{p}_{T} (GeV/#it{c}); #frac{1}{N_{INEL}} #frac{d^{2} N}{dy "
      "d#it{p}_{T}}");
}

void Spectrum::ScaleBinWidth(TH1F* hist) {
  for (unsigned int i = 1; i <= hist->GetNbinsX(); ++i) {
    const float binWidth = hist->GetBinWidth(i);
    hist->SetBinContent(i, hist->GetBinContent(i) / binWidth);
    hist->SetBinError(i, hist->GetBinError(i) / binWidth);
  }
}

void Spectrum::SetTriggerEfficiency() {
  /// Enters as nEvt_corr = nEvt / eff
  /// and hence is multiplied to the spectrum
  std::cout << "Trigger efficiency: " << fTriggerEfficiency << " +- "
            << fTriggerEfficiencyErr << "\n";

  fTriggerEffHist = GetBinnedHistogram("triggerEfficiency");
  for (unsigned int i = 1; i <= fTriggerEffHist->GetNbinsX(); ++i) {
    fTriggerEffHist->SetBinContent(i, fTriggerEfficiency);
    fTriggerEffHist->SetBinError(i, fTriggerEfficiencyErr);
  }
  fTriggerEffHist->SetTitle("; #it{p}_{T} (GeV/#it{c}); #varepsilon_{Trigger}");
}

TH1F* Spectrum::RebinSpectrum(const TH1F* originalHist,
                              const bool statisticalUncertainties, TF1* fit) {
  if (!originalHist) {
    std::cerr << "ERROR Spectrum: Input histogram missing!\n";
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

TH1F* Spectrum::RebinHistogram(TH1F* originalHist) {
  const int nbins = globalpTbins.size();
  double bins[nbins];
  std::copy(globalpTbins.begin(), globalpTbins.end(), bins);
  TString name = originalHist->GetName();
  name += "_rebinned";
  return dynamic_cast<TH1F*>(originalHist->Rebin(nbins - 1, name, bins));
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

  auto c = new TCanvas();
  c->Divide(3, 3);
  outfile->mkdir("Data");
  outfile->cd("Data");
  int padCounter = 1;
  for (const auto& it : fInvMassRec) {
    (*it).Write((*it).GetName());
    c->cd(padCounter++);
    it->GetXaxis()->SetRangeUser(1.15, 1.23);
    it->Draw();
  }
  c->Print("Plot/rec.pdf");

  auto d = new TCanvas();
  d->Divide(3, 3);
  padCounter = 1;
  outfile->cd();
  outfile->mkdir("MC");
  outfile->cd("MC");
  for (const auto& it : fInvMassMC) {
    (*it).Write((*it).GetName());
    d->cd(padCounter++);
    it->GetXaxis()->SetRangeUser(1.15, 1.23);
    it->Draw();
  }
  d->Print("Plot/mc.pdf");

  outfile->cd();
  GetReconstructedSpectrum()->Write("RecSpectrum");
  GetMCSpectrum()->Write("MCSpectrum");
  GetMCTruthCorrected()->Write("MCTruth");
  GetEfficiency()->Write("Efficiency");
  GetTriggerEfficiency()->Write("TriggerEfficiency");
  GetCorrectedSpectrum()->Write("CorrectedSpectrum");

  outfile->Write();
  outfile->Close();

  auto e = new TCanvas();
  GetReconstructedSpectrum()->Draw();
  e->Print("Plot/recSpectrum.pdf");

  auto f = new TCanvas();
  GetMCSpectrum()->Draw();
  f->Print("Plot/mcSpectrum.pdf");

  auto g = new TCanvas();
  g->SetLogy();
  GetMCTruthCorrected()->Draw();
  g->Print("Plot/mcTruthCorrected.pdf");

  auto h = new TCanvas();
  h->SetLogy();
  GetEfficiency()->Draw();
  h->Print("Plot/efficiency.pdf");

  auto i = new TCanvas();
  i->SetLogy();
  GetCorrectedSpectrum()->GetXaxis()->SetRangeUser(0,10);
  GetCorrectedSpectrum()->GetYaxis()->SetRangeUser(0.000001,0.04);
  GetCorrectedSpectrum()->Draw();
  i->Print("Plot/corrSpectrum.pdf");
}
