#ifndef Plotter_H
#define Plotter_H

#include "TColor.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"

class Plotter {
 public:
  Plotter() = default;
  ~Plotter() = default;

  static void SetStyle(bool graypalette = false, bool title = true);
  static void SetStyleHisto(TH1 *histo, int marker = 24, int color = kBlack);
  static void SetStyleGraph(TGraph *graph, int marker = 24, int color = kBlack);

 private:
  static constexpr float fLabelSize = 0.05;   // 0.05
  static constexpr float fTitleSize = 0.055;  // 0.055
};

#endif  // Plotter_H
