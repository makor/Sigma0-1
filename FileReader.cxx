#include "FileReader.h"

TH2F* FileReader::GetHist2D(TString filename, TString appendix,
                            std::vector<TString> path, TString histname) {
  auto file = TFile::Open(filename);
  if (!file) {
    std::cerr << "ERROR FileReader: File does not exist\n";
    return nullptr;
  }
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  if (!dir) {
    std::cerr << "ERROR FileReader: Directory does not exist\n";
    file->Close();
    return nullptr;
  }
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  TList* results = histoList;
  for (const auto& it : path) {
    results = (TList*)results->FindObject(it);
  }
  if (!results) {
    std::cerr << "ERROR FileReader: List does not exist\n";
    file->Close();
    return nullptr;
  }
  auto hist2D = (TH2F*)results->FindObject(histname);
  if (!hist2D) {
    std::cerr << "ERROR FileReader: Histogram does not exist\n";
    file->Close();
    return nullptr;
  }
  hist2D->Sumw2();
  file->Close();
  return hist2D;
}

TH1F* FileReader::GetHist1D(TString filename, TString appendix,
                            std::vector<TString> path, TString histname) {
  auto file = TFile::Open(filename);
  if (!file) {
    std::cerr << "ERROR FileReader: File does not exist\n";
    return nullptr;
  }
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  if (!dir) {
    std::cerr << "ERROR FileReader: Directory does not exist\n";
    file->Close();
    return nullptr;
  }
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  TList* results = histoList;
  for (const auto& it : path) {
    results = (TList*)results->FindObject(it);
  }
  if (!results) {
    std::cerr << "ERROR FileReader: List does not exist\n";
    file->Close();
    return nullptr;
  }
  auto hist1D = (TH1F*)results->FindObject(histname);
  if (!hist1D) {
    std::cerr << "ERROR FileReader: Histogram does not exist\n";
    file->Close();
    return nullptr;
  }
  hist1D->Sumw2();
  file->Close();
  return hist1D;
}

TProfile* FileReader::GetProfile(TString filename, TString appendix,
                                 std::vector<TString> path, TString histname) {
  auto file = TFile::Open(filename);
  if (!file) {
    std::cerr << "ERROR FileReader: File does not exist\n";
    file->Close();
    return nullptr;
  }
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  if (!dir) {
    std::cerr << "ERROR FileReader: Directory does not exist\n";
    file->Close();
    return nullptr;
  }
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  TList* results = histoList;
  for (const auto& it : path) {
    results = (TList*)results->FindObject(it);
  }
  if (!results) {
    std::cerr << "ERROR FileReader: List does not exist\n";
    file->Close();
    return nullptr;
  }
  auto profile = (TProfile*)results->FindObject(histname);
  if (!profile) {
    std::cerr << "ERROR FileReader: Histogram does not exist\n";
    file->Close();
    return nullptr;
  }
  file->Close();
  return profile;
}
