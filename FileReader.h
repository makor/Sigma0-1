#ifndef FileReader_H
#define FileReader_H

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

class FileReader {
 public:
  FileReader();
  ~FileReader();

  static TH2F* GetHist2D(TString filename, TString appendix, TString path,
                         TString histname);
  static TH1F* GetHist1D(TString filename, TString appendix, TString path,
                         TString histname);
  static TProfile* GetProfile(TString filename, TString appendix, TString path,
                              TString histname);

 private:
};

#endif  // FileReader_H
