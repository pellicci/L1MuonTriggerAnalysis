// ******************************
// Efficiency Plotter
// ( Provide plots for efficiency and compute efficiency using
// TEfficiency from ROOT itself )
// ******************************

class EfficiencyPlotter
{

public :

  EfficiencyPlotter(TFile* file, 
		    std::string name = "", float minPt = 0) : outFile_(file) , 
							      baseName_(name),
							      minPt_(minPt) { config(); };   
  ~EfficiencyPlotter();

  void fill(triggeredMuonsIt & muon, 
	    L1Analysis::L1AnalysisEventDataFormat * event,
	    L1Analysis::L1AnalysisRecoVertexDataFormat * vtx);
  
  void config();
  void plotAndSave();
  void save() { outFile_->Write(); return ; };
  void printHisto(TCanvas * canvas, std::string tag) {  
    canvas->SaveAs((std::string("plots/")+baseName_+"/"+baseName_+tag+".png").c_str());
    return;
  };
  float minMuPt() { return minPt_+10; };
  

protected :

  TFile* outFile_;
  std::string baseName_;
  float minPt_;

public :

  histoMap histos_;
  hTH1Map  hTH1F_;
  
};


EfficiencyPlotter::~EfficiencyPlotter()
{

    histos_.clear();

};

void EfficiencyPlotter::config() 
{ 

  if (baseName_!="")
    {
      outFile_->mkdir(baseName_.c_str());
      outFile_->cd(baseName_.c_str());
      system(string("mkdir -p plots/" + baseName_).c_str());
    }

    string name  = (baseName_ + "_hGmtPtVsQual");
    string title = name + ";tight muon p_{t} [GeV/c];efficiency";
    hTH1F_["hGmtPtVsQual"] = new TH2F(name.c_str(),name.c_str(),60,0.5,60.5,7,0.5,7.5);

    name  = baseName_ + "_hEffVsPt";
    histos_["hEffVsPt"] = new TEfficiency(name.c_str(),title.c_str(),60,0.5,60.5);

    name  = baseName_ + "_hEffVsEta";
    title = name + ";tight muon #eta;efficiency";
    histos_["hEffVsEta"] = new TEfficiency(name.c_str(),title.c_str(),56,-1.05,1.05);

    name  = (baseName_ + "_hGmtEtaVsQual");
    hTH1F_["hGmtEtaVsQual"] = new TH2F(name.c_str(),name.c_str(),56,-1.05,1.05,7,0.5,7.5);

    name  = (baseName_ + "_hEffVsPhi");
    title = name + ";tight muon #phi [rad];efficiency";
    histos_["hEffVsPhi"] = new TEfficiency(name.c_str(),title.c_str(),48,-TMath::Pi(),TMath::Pi());

    name  = (baseName_ + "_hGmtPhiVsQual");
    hTH1F_["hGmtPhiVsQual"] = new TH2F(name.c_str(),name.c_str(),48,-TMath::Pi(),TMath::Pi(),7,0.5,7.5);

    name  = (baseName_ + "_hEffPhiVsEta");
    title = name + ";tight muon #phi [rad];tight muon #eta;efficiency";
    histos_["hEffPhiVsEta"] = new TEfficiency(name.c_str(),title.c_str(),48,-TMath::Pi(),TMath::Pi(),56,-1.05,1.05);

    // name  = (baseName_ + "_hEffVsVtx");
    // title = name + ";N. reco vtx ;efficiency";
    // histos_["hEffVsVtx"] = new TEfficiency(name.c_str(),title.c_str(),25,0.,50.);

    // name  = (baseName_ + "_hGmtVtxVsQual");
    // hTH1F_["hGmtVtxVsQual"] = new TH2F(name.c_str(),name.c_str(),25,0.,50.,7,0.5,7.5);

}

void EfficiencyPlotter::fill(triggeredMuonsIt & muon, 
			     L1Analysis::L1AnalysisEventDataFormat * event,
			     L1Analysis::L1AnalysisRecoVertexDataFormat * vtx) 
{
  
  float pt     = muon->my_mu->pt.at(muon->my_imu);
  float eta    = muon->my_mu->eta.at(muon->my_imu);
  float phi    = muon->my_mu->phi.at(muon->my_imu);

  // int  nVtx  = vtx ? vtx->nVtx : 0;

  bool hasTrigger = muon->hasTriggerMatch() && 
    ((muon->my_gmt->Pt.at(muon->my_igmt)) +0.01 > minPt_);

  
  float gmtEta       = 999.;
  int   gmtQual      = 999;
  int   gmtChambId   = 999;
  
  if (muon->hasTriggerMatch()) 
    {
      gmtEta  = muon->my_gmt->Eta.at(muon->my_igmt);
      gmtQual = muon->my_gmt->Qual.at(muon->my_igmt);      
    }
  
  if (fabs(eta) < MAX_MU_ETA)
    {
      histos_["hEffVsPt"]->Fill(hasTrigger,pt);
      
      if (hasTrigger) 
	{
	  hTH1F_["hGmtPtVsQual"]->Fill(pt,gmtQual);
	}
    }  
  
  if (pt > minMuPt())
    {
      histos_["hEffVsEta"]->Fill(hasTrigger,eta);
 
      histos_["hEffPhiVsEta"]->Fill(hasTrigger,phi,eta);

      if (hasTrigger) 
	{
	  hTH1F_["hGmtBinEta"]->Fill(gmtEta);
	  hTH1F_["hGmtEtaVsQual"]->Fill(eta,gmtQual);
	}
      
    }

  if (pt > minMuPt() && fabs(eta) < MAX_MU_ETA)
    {
      histos_["hEffVsPhi"]->Fill(hasTrigger,phi);
      if (hasTrigger) 
	{
	  hTH1F_["hGmtPhiVsQual"]->Fill(phi,gmtQual);
	}

      // histos_["hEffVsVtx"]->Fill(hasTrigger,nVtx);
      // if (hasTrigger) 
      // 	{
      // 	  hTH1F_["hGmtVtxVsQual"]->Fill(nVtx,gmtQual);
      // 	}
    }  


  return;

}

void EfficiencyPlotter::plotAndSave() 
{ 

  setTDRStyle();
  gStyle->SetOptTitle(0);

  TCanvas *cEffvsPt = new TCanvas((baseName_+"cEffvsPt").c_str(),
				  (baseName_+"cEffvsPt").c_str(),500,500);
  cEffvsPt->cd();
  cEffvsPt->SetGrid();  

  histos_["hEffVsPt"]->Draw();

  TH1 const *hEffVsPtTotal = histos_["hEffVsPt"]->GetTotalHistogram(); 
  
  THStack * hEffvsPtStack = new THStack("hEffvsPtStack", "hEffvsPtStack");
  for(int ybin=1; ybin<=hTH1F_["hGmtPtVsQual"]->GetNbinsY(); ++ybin)
    {
      stringstream px ;
      px<<ybin<<"QualityPt";
      TH1D* projection = static_cast<TH2*>(hTH1F_["hGmtPtVsQual"])->ProjectionX(px.str().c_str(),ybin,ybin);

      projection->SetFillColor(colorMap[ybin-1]);
      projection->SetLineColor(colorMap[ybin-1]);
      projection->Divide(hEffVsPtTotal);
      hEffvsPtStack->Add(projection);
    }

  hEffvsPtStack->Draw("same");
  histos_["hEffVsPt"]->Draw("same");
  
  cEffvsPt->Update();
  histos_["hEffVsPt"]->GetPaintedGraph()->GetXaxis()->SetRangeUser(0.,60.);
  histos_["hEffVsPt"]->GetPaintedGraph()->GetYaxis()->SetRangeUser(0.,1.1);
  histos_["hEffVsPt"]->GetPaintedGraph()->GetXaxis()->SetTitleSize(0.04);
  histos_["hEffVsPt"]->GetPaintedGraph()->GetYaxis()->SetTitleSize(0.04);

  printHisto(cEffvsPt,"EffvsPt");

  TCanvas *cEffvsEta = new TCanvas((baseName_+"cEffvsEta").c_str(),
				   (baseName_+"cEffvsEta").c_str(),500,500);
  cEffvsEta->cd();
  cEffvsEta->SetGrid();  

  histos_["hEffVsEta"]->Draw();

  TH1 const *hEffVsEtaTotal = histos_["hEffVsEta"]->GetTotalHistogram(); 

  THStack * hEffvsEtaStack = new THStack("hEffvsEtaStack","hEffvsEtaStack");
  for(int ybin=1; ybin<=hTH1F_["hGmtEtaVsQual"]->GetNbinsY(); ++ybin)
    {
      stringstream px ;
      px<<ybin<<"QualityEta";
      TH1D* projection = static_cast<TH2*>(hTH1F_["hGmtEtaVsQual"])->ProjectionX(px.str().c_str(),ybin,ybin);
      
      projection->SetFillColor(colorMap[ybin-1]);
      projection->SetLineColor(colorMap[ybin-1]);
      projection->Divide(hEffVsEtaTotal);
      hEffvsEtaStack->Add(projection);
    }

  hEffvsEtaStack->Draw("same");
  histos_["hEffVsEta"]->Draw("same");

  cEffvsEta->Update();
  histos_["hEffVsEta"]->GetPaintedGraph()->GetYaxis()->SetRangeUser(0,1.1);
  histos_["hEffVsEta"]->GetPaintedGraph()->GetXaxis()->SetTitleSize(0.04);
  histos_["hEffVsEta"]->GetPaintedGraph()->GetYaxis()->SetTitleSize(0.04);
  
  printHisto(cEffvsEta,"EffvsEta");
  

  TCanvas *cEffvsEtaGmt = new TCanvas((baseName_+"cEffvsEtaGmt").c_str(),
				      (baseName_+"cEffvsEtaGmt").c_str(),500,500);
  cEffvsEtaGmt->cd();
  cEffvsEtaGmt->SetGrid();  

  TH1 *hGmtBinEta = hTH1F_["hGmtBinEta"]; 

  THStack * hEffvsGmtEtaStack = new THStack("hEffvsGmtEtaStack","hEffGmtvsEtaStack");
  for(int ybin=1; ybin<=hTH1F_["hGmtBinEtaVsQual"]->GetNbinsY(); ++ybin)
    {
      stringstream px ;
      px<<ybin<<"QualityGmtEta";
      TH1D* projection = static_cast<TH2*>(hTH1F_["hGmtBinEtaVsQual"]) ->ProjectionX(px.str().c_str(),ybin,ybin);
	  
      projection->SetFillColor(colorMap[ybin-1]);
      projection->SetLineColor(colorMap[ybin-1]);
      projection->Divide(hGmtBinEta);
      hEffvsGmtEtaStack->Add(projection);
    }
  
  hEffvsGmtEtaStack->Draw();
  hEffvsGmtEtaStack->GetXaxis()->SetTitle("Gmt #eta");
  hEffvsGmtEtaStack->GetYaxis()->SetTitle("quality contrib. to efficiency");
  hEffvsGmtEtaStack->GetXaxis()->SetTitleSize(0.04);
  hEffvsGmtEtaStack->GetYaxis()->SetTitleSize(0.04);    
  
  printHisto(cEffvsEtaGmt,"EffvsGmtEta");


  TCanvas *cEffvsPhi = new TCanvas((baseName_+"cEffvsPhi").c_str(),
				   (baseName_+"cEffvsPhi").c_str(),500,500);
  cEffvsPhi->cd();
  cEffvsPhi->SetGrid();  
  
  histos_["hEffVsPhi"]->Draw();

  TH1 const *hEffVsPhiTotal = histos_["hEffVsPhi"]->GetTotalHistogram(); 

  THStack * hEffvsPhiStack = new THStack("hEffvsPhiStack", "hEffvsPhiStack");
  for(int ybin=1; ybin<=hTH1F_["hGmtPhiVsQual"]->GetNbinsY(); ++ybin)
    {
      stringstream px ;
      px<<ybin<<"QualityPhi";
      TH1D* projection = static_cast<TH2*>(hTH1F_["hGmtPhiVsQual"]) ->ProjectionX(px.str().c_str(),ybin,ybin);
      
      projection->SetFillColor(colorMap[ybin-1]);
      projection->SetLineColor(colorMap[ybin-1]);
      projection->Divide(hEffVsPhiTotal);
      hEffvsPhiStack->Add(projection);
    }
  
  hEffvsPhiStack->Draw("same");
  histos_["hEffVsPhi"]->Draw("same");

  cEffvsPhi->Update();
  histos_["hEffVsPhi"]->GetPaintedGraph()->GetYaxis()->SetRangeUser(0.,1.1);
  histos_["hEffVsPhi"]->GetPaintedGraph()->GetXaxis()->SetTitleSize(0.04);
  histos_["hEffVsPhi"]->GetPaintedGraph()->GetYaxis()->SetTitleSize(0.04);
  
  printHisto(cEffvsPhi,"EffvsPhi");


  TCanvas *cEffPhivsEta = new TCanvas((baseName_+"cEffPhivsEta").c_str(),
				      (baseName_+"cEffPhivsEta").c_str(),500,500);
  cEffPhivsEta->cd();
  cEffPhivsEta->SetGrid();  

  histos_["hEffPhiVsEta"]->Draw("colz");
  //    std::cout << histos_["hEffPhiVsEta"]->GetPaintedGraph() << std::endl;
  //    histos_["hEffPhiVsEta"]->GetPaintedHistogram()->SetMaximum(1.);
  //    histos_["hEffPhiVsEta"]->GetPaintedHistogram()->GetXaxis()->SetTitleSize(0.04);
  //    histos_["hEffPhiVsEta"]->GetPaintedHistogram()->GetYaxis()->SetTitleSize(0.04);

  printHisto(cEffPhivsEta,"EffPhivsEta");


  TCanvas *cEffvsVtx = new TCanvas((baseName_+"cEffvsVtx").c_str(),
				   (baseName_+"cEffvsVtx").c_str(),500,500);
  cEffvsVtx->cd();
  cEffvsVtx->SetGrid();  
  
  histos_["hEffVsVtx"]->Draw();
  
  TH1 const *hEffVsVtxTotal = histos_["hEffVsVtx"]->GetTotalHistogram(); 
  
  THStack * hEffvsVtxStack = new THStack("hEffvsVtxStack", "hEffvsVtxStack");
  for(int ybin=1; ybin<=hTH1F_["hGmtVtxVsQual"]->GetNbinsY(); ++ybin)
    {
      stringstream px ;
      px<<ybin<<"QualityVtx";
      TH1D* projection = static_cast<TH2*>(hTH1F_["hGmtVtxVsQual"]) ->ProjectionX(px.str().c_str(),ybin,ybin);
      
      projection->SetFillColor(colorMap[ybin-1]);
      projection->SetLineColor(colorMap[ybin-1]);
      projection->Divide(hEffVsVtxTotal);
      hEffvsVtxStack->Add(projection);
    }
  
  hEffvsVtxStack->Draw("same");
  histos_["hEffVsVtx"]->Draw("same");
  
  cEffvsVtx->Update();
  histos_["hEffVsVtx"]->GetPaintedGraph()->GetXaxis()->SetRangeUser(5.,30.);
  histos_["hEffVsVtx"]->GetPaintedGraph()->GetYaxis()->SetRangeUser(0.,1.1);
  histos_["hEffVsVtx"]->GetPaintedGraph()->GetXaxis()->SetTitleSize(0.04);
  histos_["hEffVsVtx"]->GetPaintedGraph()->GetYaxis()->SetTitleSize(0.04);
  
  printHisto(cEffvsVtx,"EffvsVtx");
  
  save();

  return;

}

//////////////////////////////////////////////////////////
// Helper classfunction to plot togheter efficiency plots
//////////////////////////////////////////////////////////

void plotAndSaveAll(std::vector<EfficiencyPlotter*> & plotters, std::string plotName)
{
  
  TCanvas *canvas = new TCanvas(("c"+plotName).c_str(),
				("c"+plotName).c_str(),500,500);
  canvas->cd();
  canvas->SetGrid();  
  
  std::vector<EfficiencyPlotter*>::const_iterator plotterIt  = plotters.begin();
  std::vector<EfficiencyPlotter*>::const_iterator plotterEnd = plotters.end();
  
  int iPlot = 0;
  TF1 *fitFunc = 0;
  for(;plotterIt!=plotterEnd;++plotterIt,++iPlot)
    {
      TEfficiency * plot = (*plotterIt)->histos_["h"+plotName];
      plot->SetLineColor(iPlot+1);
      plot->Draw(iPlot == 0 ? "" : "same");
      
      canvas->Update();
      
      if (plotName.find("Pt") == string::npos) 
	plot->GetPaintedGraph()->GetYaxis()->SetRangeUser(0.8,1.1);
      
      if (iPlot == 0 && plotName.find("Lumi") != string::npos)
	{ 
	  plot->GetPaintedGraph()->Fit("pol1");
	  fitFunc = plot->GetPaintedGraph()->GetFunction("pol1");
	}
    }
  
  if (fitFunc) 
    {
      fitFunc->Draw("same");
      canvas->Update();
    }
  
  system(string("mkdir -p plots/All/").c_str());
  canvas->SaveAs(("plots/All/All"+plotName+".png").c_str());
  
  return;
  
}
