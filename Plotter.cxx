#include "Plotter.h"
#include "TStyle.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Plotter::SetStyle(bool graypalette, bool title) {
  gStyle->Reset("Plain");
  gStyle->SetCanvasPreferGL(1);
  gStyle->SetOptTitle(title);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetOptStat(0);
  if (graypalette) {
    gStyle->SetPalette(8, 0);
  } else {
    gStyle->SetPalette(1);
  }
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameLineWidth(1);
  TColor color(kWhite, 1, 0, 0, " ", 0.);
  gStyle->SetStatColor(kWhite);
  gStyle->SetTitleColor(kWhite);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetHistLineWidth(1);
  gStyle->SetHistLineColor(kRed);
  gStyle->SetFuncWidth(2);
  gStyle->SetFuncColor(kGreen);
  gStyle->SetLineWidth(2);
  gStyle->SetLabelSize(0.045, "xyz");
  gStyle->SetLabelOffset(0.01, "y");
  gStyle->SetLabelOffset(0.01, "x");
  gStyle->SetLabelColor(kBlack, "xyz");
  gStyle->SetTitleSize(0.05, "xyz");
  gStyle->SetTitleOffset(1.25, "y");
  gStyle->SetTitleOffset(1.2, "x");
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetTextSizePixels(26);
  gStyle->SetTextFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(kWhite);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetPalette(55);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Plotter::SetStyleHisto(TH1 *histo, int marker, int color) {
  histo->GetXaxis()->SetLabelSize(fLabelSize);
  histo->GetXaxis()->SetTitleSize(fTitleSize);
  histo->GetXaxis()->SetLabelOffset(0.01);
  histo->GetXaxis()->SetTitleOffset(1.2);
  histo->GetXaxis()->SetLabelFont(42);
  histo->GetYaxis()->SetLabelSize(fLabelSize);
  histo->GetYaxis()->SetTitleSize(fTitleSize);
  histo->GetYaxis()->SetLabelOffset(0.01);
  histo->GetYaxis()->SetTitleOffset(1.25);
  histo->GetZaxis()->SetLabelSize(fLabelSize);
  histo->GetZaxis()->SetTitleSize(fTitleSize);
  histo->GetZaxis()->SetLabelOffset(0.01);
  histo->GetZaxis()->SetTitleOffset(1.25);
  histo->SetMarkerStyle(marker);
  histo->SetMarkerColor(color);
  histo->SetLineColor(color);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Plotter::SetStyleGraph(TGraph *graph, int marker, int color) {
  graph->GetXaxis()->SetLabelSize(fLabelSize);
  graph->GetXaxis()->SetTitleSize(fTitleSize);
  graph->GetXaxis()->SetTitleColor(kBlack);
  graph->GetXaxis()->SetLabelFont(42);
  graph->GetYaxis()->SetLabelSize(fLabelSize);
  graph->GetYaxis()->SetTitleSize(fTitleSize);
  graph->GetYaxis()->SetLabelFont(42);
  graph->SetMarkerStyle(marker);
  graph->SetMarkerColor(color);
  graph->SetLineColor(color);
  graph->GetXaxis()->SetTitleOffset(1.15);
  graph->GetYaxis()->SetTitleOffset(1.25);
}
