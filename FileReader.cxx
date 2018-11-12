#include "FileReader.h"

TH2F* FileReader::GetHist2D(TString filename, TString appendix, TString path,
                            TString histname) {
  if (!CheckStringSanity(filename, appendix, path, histname)) {
    return nullptr;
  }
  auto file = TFile::Open(filename);
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  auto results = (TList*)histoList->FindObject(path);
  auto hist2D = (TH2F*)results->FindObject(histname);
  hist2D->Sumw2();
  return hist2D;
}

TH1F* FileReader::GetHist1D(TString filename, TString appendix, TString path,
                            TString histname) {
  if (!CheckStringSanity(filename, appendix, path, histname)) {
    return nullptr;
  }
  auto file = TFile::Open(filename);
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  auto results = (TList*)histoList->FindObject(path);
  auto hist1D = (TH1F*)results->FindObject(histname);
  hist1D->Sumw2();
  return hist1D;
}

TProfile* FileReader::GetProfile(TString filename, TString appendix,
                                 TString path, TString histname) {
  if (!CheckStringSanity(filename, appendix, path, histname)) {
    return nullptr;
  }
  auto file = TFile::Open(filename);
  TString name = "Sigma0_Femto_" + appendix;
  auto dir = file->GetDirectory(name);
  name = "histo_" + appendix;
  auto histoList = (TList*)dir->Get(name);
  auto results = (TList*)histoList->FindObject(path);
  auto profile = (TProfile*)results->FindObject(histname);
  return profile;
}
