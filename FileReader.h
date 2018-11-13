#ifndef FileReader_H
#define FileReader_H

#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

class FileReader {
 public:
  FileReader() = default;
  ~FileReader() = default;

  /// Read a TH2F from the file
  /// \param filename Path to the file
  /// \param appendix Appendix of the content (kMB, kHighMultV0, ..._
  /// \param path TList inside the file
  /// \param histname name of the histogram
  /// \return TH2F from the file
  static TH2F *GetHist2D(TString filename, TString appendix,
                         std::vector<TString> path, TString histname);
  /// Read a TH1F from the file
  /// \param filename Path to the file
  /// \param appendix Appendix of the content (kMB, kHighMultV0, ..._
  /// \param path TList inside the file
  /// \param histname name of the histogram
  /// \return TH1F from the file
  static TH1F *GetHist1D(TString filename, TString appendix,
                         std::vector<TString> path, TString histname);

  /// Read a TProfile from the file
  /// \param filename Path to the file
  /// \param appendix Appendix of the content (kMB, kHighMultV0, ..._
  /// \param path TList inside the file
  /// \param histname name of the histogram
  /// \return TProfile from the file
  static TProfile *GetProfile(TString filename, TString appendix,
                              std::vector<TString> path, TString histnamee);

 private:
  /// Check whether the desired file/appendix/path/histogram exists
  /// \param filename Path to the file
  /// \param appendix Appendix of the content (kMB, kHighMultV0, ..._
  /// \param path TList inside the file
  /// \param histname name of the histogram
  /// \return Whether the desired file/appendix/path/histogram exists
  static bool CheckStringSanity(TString filename, TString appendix,
                                std::vector<TString> path, TString histname);
};

inline bool FileReader::CheckStringSanity(TString filename, TString appendix,
                                          std::vector<TString> path,
                                          TString histname) {
  bool isOK = true;
  bool exists = true;
  if (filename.IsNull() && filename.IsAscii()) {
    std::cerr << "ERROR FileReader: Specify filename\n";
    isOK = false;
    goto exitThroughTheGiftShop;
  }
  if (appendix.IsNull() && appendix.IsAscii()) {
    std::cerr << "ERROR FileReader: Specify appendix\n";
    isOK = false;
    goto exitThroughTheGiftShop;
  }
  for (const auto &it : path) {
    if (it.IsNull() && it.IsAscii()) {
      std::cerr << "ERROR FileReader: Specify path\n";
      isOK = false;
      goto exitThroughTheGiftShop;
    }
  }
  if (histname.IsNull() && histname.IsAscii()) {
    std::cerr << "ERROR FileReader: Specify histname\n";
    isOK = false;
    goto exitThroughTheGiftShop;
  }
  if (isOK) {
    auto file = TFile::Open(filename);
    if (!file) {
      std::cerr << "ERROR FileReader: File does not exist\n";
      exists = false;
      goto exitThroughTheGiftShop;
    }
    TString name = "Sigma0_Femto_" + appendix;
    auto dir = file->GetDirectory(name);
    if (!dir) {
      std::cerr << "ERROR FileReader: Directory does not exist\n";
      exists = false;
      goto exitThroughTheGiftShop;
    }
    name = "histo_" + appendix;
    auto histoList = (TList *)dir->Get(name);
    TList *results = histoList;
    for (const auto &it : path) {
      results = (TList *)results->FindObject(it);
    }
    if (!results) {
      std::cerr << "ERROR FileReader: List does not exist\n";
      exists = false;
      goto exitThroughTheGiftShop;
    }
    auto hist = results->FindObject(histname);
    if (!hist) {
      std::cerr << "ERROR FileReader: Histogram does not exist\n";
      exists = false;
      goto exitThroughTheGiftShop;
    }
  }
exitThroughTheGiftShop:
  return (exists && isOK);
}
#endif  // FileReader_H
