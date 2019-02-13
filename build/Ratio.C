#include "TDirectory.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TList.h"

void Ratio(const char* filename =
              "SpectrumOutput_Sigma0.root",
          const char* filename2 = "SigMac-31jan19.root") {

      auto* file = TFile::Open(filename);
      auto* Spec = (TH1F*)file->Get("CorrectedSpectrum");
      auto* file2 = TFile::Open(filename2);
      auto* Spec2 = (TH1F*)file2->Get("h_spectrum");
      int const X = Spec->GetNbinsX();
      int const X1 = Spec2->GetNbinsX();
      int const Y = Spec->GetNbinsY();
      int const Y1 = Spec2->GetNbinsY();
      int const Z = Spec->GetNbinsZ();
      int const Z1 = Spec2->GetNbinsZ();

      std::cout <<"X bins Spec, Spec2" << X<<" "<<X1 <<"\n";
      std::cout <<"Y bins Spec, Spec2" << Y<<" "<<Y1 <<"\n";
      std::cout <<"Z bins Spec, Spec2" << Z<<" "<<Z1 <<"\n";

      for ( int i = 1; i < X+1 ; i++) {
          std::cout << i << "\n";
          float const Bin = Spec->GetBinContent(i);
          std::cout <<"X bins Spec"<<" " << Bin <<"\n";
          float const Bin2 = Spec2->GetBinContent(i+1);
          std::cout <<"X bins Spec2"<<" " << Bin2 <<"\n";
          float const Ratio = Bin/Bin2;
          std::cout <<"Ratio"<<" " <<Ratio <<"\n";
          Spec->SetBinContent(i,Ratio);
          std::cout <<"Spec new Bin"<<" " << Spec->GetBinContent(i) <<"\n";
      }

      TCanvas *Ratio = new TCanvas();
      //Mult1->SetTitle("; #it{p}_{T} (GeV/#it{c}); #Sigma^{0} / #Lambda ");
      // Ratio->SetLogy();
      //Spec->Divide(Spec2);
      Spec->Draw();
      Ratio->SetRightMargin(0.09);
      Ratio->SetLeftMargin(0.15);
      Ratio->SetBottomMargin(0.15);
      Ratio->Print("Plot/RecSpectrum-Ratio.pdf");
}
