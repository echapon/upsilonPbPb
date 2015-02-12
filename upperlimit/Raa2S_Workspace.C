#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "TCanvas.h"
#include "RooWorkspace.h"
#include "RooAbsReal.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooArgList.h"
#include "RooDataSet.h"
#include "RooPlot.h"
#include "RooStats/BayesianCalculator.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/SimpleInterval.h"
#include "TAxis.h"
#include <iostream>
#include <TString.h>
#include <TH1F.h>
#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TNtuple.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TROOT.h>
#include <fstream>
#include <TGraph.h>
#include "TMath.h"
#include "TF1.h"

#include "test_combine.C"

using namespace RooFit;
using namespace RooStats;

void Raa2S_Workspace(const char* name_pbpb="fitresult.root", const char* name_pp="fitresult_pp.root", const char* name_out="fitresult_combo.root"){

   // TFile File(filename);

   RooWorkspace * ws = test_combine(name_pbpb, name_pp);
   // File.GetObject("wcombo", ws);
   RooAbsData * data = ws->data("data");

   // RooDataSet * US_data = (RooDataSet*) data->reduce( "QQsign == QQsign::PlusMinus");
   // US_data->SetName("US_data");
   // ws->import(* US_data);
   // RooDataSet * hi_data = (RooDataSet*) US_data->reduce("dataCat == dataCat::hi");
   // hi_data->SetName("hi_data");
   // ws->import(* hi_data);
   // hi_data->Print();

   RooRealVar* raa2 = new RooRealVar("raa2","R_{AA}(#Upsilon (2S))",0.5,-1.,1);
   RooRealVar* leftEdge = new RooRealVar("leftEdge","leftEdge",0);
   RooRealVar* rightEdge = new RooRealVar("rightEdge","rightEdge",1);
   RooGenericPdf step("step", "step", "(@0 >= @1) && (@0 < @2)", RooArgList(*raa2, *leftEdge, *rightEdge));
   ws->import(step);
   ws->factory( "Uniform::flat(raa2)" );

   //pp Luminosities, Taa and efficiency ratios Systematics

   ws->factory( "Taa_hi[5.662e-9]" );
   ws->factory( "Taa_kappa[1.062]" ); // was 1.057
   ws->factory( "expr::alpha_Taa('pow(Taa_kappa,beta_Taa)',Taa_kappa,beta_Taa[0,-5,5])" );
   ws->factory( "prod::Taa_nom(Taa_hi,alpha_Taa)" );
   ws->factory( "Gaussian::constr_Taa(beta_Taa,glob_Taa[0,-5,5],1)" );

   ws->factory( "lumipp_hi[5.4]" );
   ws->factory( "lumipp_kappa[1.037]" ); // was 1.06
   ws->factory( "expr::alpha_lumipp('pow(lumipp_kappa,beta_lumipp)',lumipp_kappa,beta_lumipp[0,-5,5])" );
   ws->factory( "prod::lumipp_nom(lumipp_hi,alpha_lumipp)" );
   ws->factory( "Gaussian::constr_lumipp(beta_lumipp,glob_lumipp[0,-5,5],1)" );

   // ws->factory( "effRat1[1]" );
   // ws->factory( "effRat2[1]" );
   double effRat2value=0.953; // default value from MB
   // what is the value of the efficiency ratio? try to guess from the input file name
   string sfname(name_pbpb);
   if (sfname.find("dimuPt0005000_dimuY000120")!=string::npos) effRat2value=0.919;
   else if (sfname.find("dimuPt000500_dimuY000240")!=string::npos) effRat2value=0.941;
   else if (sfname.find("dimuPt5001200_dimuY000240")!=string::npos) effRat2value=0.973;
   else if (sfname.find("dimuPt0005000_dimuY120240")!=string::npos) effRat2value=1.;
   else if (sfname.find("dimuPt12002000_dimuY000240")!=string::npos) effRat2value=0.994;

   ws->factory( Form("effRat2_hi[%f]",effRat2value) );
   ws->factory( "effRat_kappa[1.054]" );
   ws->factory( "expr::alpha_effRat('pow(effRat_kappa,beta_effRat)',effRat_kappa,beta_effRat[0,-5,5])" );
   // ws->factory( "prod::effRat1_nom(effRat1_hi,alpha_effRat)" );
   ws->factory( "Gaussian::constr_effRat(beta_effRat,glob_effRat[0,-5,5],1)" );
   ws->factory( "prod::effRat2_nom(effRat2_hi,alpha_effRat)" );
   // ws->factory( "prod::effRat3_nom(effRat3_hi,alpha_effRat)" );
   //  
   ws->factory("Nmb_hi[1.161e9]");
   ws->factory("prod::denominator(Taa_nom,Nmb_hi)");
   ws->factory( "expr::lumiOverTaaNmbmodified('lumipp_nom/denominator',lumipp_nom,denominator)");
   RooFormulaVar *lumiOverTaaNmbmodified = ws->function("lumiOverTaaNmbmodified");
   //  
   //  RooRealVar *raa1 = ws->var("raa1");
   //  RooRealVar* nsig1_pp = ws->var("nsig1_pp");
   //  RooRealVar* effRat1 = ws->function("effRat1_nom");
   //  RooRealVar *raa2 = ws->var("raa2");
   //  RooRealVar* nsig2_pp = ws->var("nsig2_pp");
   //  RooRealVar* effRat2 = ws->function("effRat2_nom");
   RooRealVar* nsig2_pp = ws->var("N_{#Upsilon(2S)}_pp");
   cout << nsig2_pp << endl;
   RooRealVar* effRat2 = ws->function("effRat2_nom");
   //  
   //  RooFormulaVar nsig1_hi_modified("nsig1_hi_modified", "@0*@1*@3/@2", RooArgList(*raa1, *nsig1_pp, *lumiOverTaaNmbmodified, *effRat1));
   //  ws->import(nsig1_hi_modified);
   //  RooFormulaVar nsig2_hi_modified("nsig2_hi_modified", "@0*@1*@3/@2", RooArgList(*raa2, *nsig2_pp, *lumiOverTaaNmbmodified, *effRat2));
   //  ws->import(nsig2_hi_modified);
   RooFormulaVar nsig2_hi_modified("nsig2_hi_modified", "@0*@1*@3/@2", RooArgList(*raa2, *nsig2_pp, *lumiOverTaaNmbmodified, *effRat2));
   ws->import(nsig2_hi_modified);

   //  // background yield with systematics
   ws->factory( "nbkg_hi_kappa[1.10]" );
   ws->factory( "expr::alpha_nbkg_hi('pow(nbkg_hi_kappa,beta_nbkg_hi)',nbkg_hi_kappa,beta_nbkg_hi[0,-5,5])" );
   ws->factory( "SUM::nbkg_hi_nom(alpha_nbkg_hi*bkgPdf_hi)" );
   ws->factory( "Gaussian::constr_nbkg_hi(beta_nbkg_hi,glob_nbkg_hi[0,-5,5],1)" );
   RooAbsPdf* sig1S_hi = ws->pdf("cbcb_hi");
   RooAbsPdf* sig2S_hi = ws->pdf("sig2S_hi");
   RooAbsPdf* sig3S_hi = ws->pdf("sig3S_hi");
   RooAbsPdf* LSBackground_hi = ws->pdf("nbkg_hi_nom");
   RooRealVar* nsig1_hi = ws->var("N_{#Upsilon(1S)}_hi");
   RooFormulaVar* nsig2_hi = ws->function("nsig2_hi_modified");
   RooRealVar* nsig3_hi = ws->var("N_{#Upsilon(3S)}_hi");
   cout << nsig1_hi << " " << nsig3_hi << " " << nsig2_pp << endl;
   RooRealVar* norm_nbkg_hi = ws->var("n_{Bkgd}_hi");

   RooArgList pdfs_hi( *sig1S_hi,*sig2S_hi,*sig3S_hi, *LSBackground_hi);
   RooArgList norms_hi(*nsig1_hi,*nsig2_hi,*nsig3_hi, *norm_nbkg_hi);

   ////////////////////////////////////////////////////////////////////////////////

   ws->factory( "nbkg_pp_kappa[1.03]" );
   ws->factory( "expr::alpha_nbkg_pp('pow(nbkg_pp_kappa,beta_nbkg_pp)',nbkg_pp_kappa,beta_nbkg_pp[0,-5,5])" );
   ws->factory( "SUM::nbkg_pp_nom(alpha_nbkg_pp*bkgPdf_pp)" );
   ws->factory( "Gaussian::constr_nbkg_pp(beta_nbkg_pp,glob_nbkg_pp[0,-5,5],1)" );
   RooAbsPdf* sig1S_pp = ws->pdf("cbcb_pp");
   RooAbsPdf* sig2S_pp = ws->pdf("sig2S_pp");
   RooAbsPdf* sig3S_pp = ws->pdf("sig3S_pp");
   RooAbsPdf* LSBackground_pp = ws->pdf("nbkg_pp_nom");
   RooRealVar* nsig1_pp = ws->var("N_{#Upsilon(1S)}_pp");
   // RooRealVar* nsig2_pp = ws->var("N_{#Upsilon(2S)}_pp");
   RooRealVar* nsig3_pp = ws->var("N_{#Upsilon(3S)}_pp");
   RooRealVar* norm_nbkg_pp = ws->var("n_{Bkgd}_pp");

   RooArgList pdfs_pp( *sig1S_pp,*sig2S_pp,*sig3S_pp, *LSBackground_pp);
   RooArgList norms_pp( *nsig1_pp,*nsig2_pp,*nsig3_pp,*norm_nbkg_pp);

   RooAddPdf model_num("model_num", "model_num", pdfs_hi,norms_hi); 
   ws->import(model_num);
   ws->factory("PROD::model_hi(model_num, constr_nbkg_hi,constr_lumipp,constr_Taa,constr_effRat)");

   RooAddPdf model_den("model_den", "model_den", pdfs_pp,norms_pp); 
   ws->import(model_den);
   ws->factory("PROD::model_pp(model_den, constr_nbkg_pp)");

   ws->factory("SIMUL::joint(dataCat,hi=model_hi,pp=model_pp)");



   /////////////////////////////////////////////////////////////////////
   RooRealVar * pObs = ws->var("invariantMass"); // get the pointer to the observable
   RooArgSet obs("observables");
   obs.add(*pObs);
   obs.add( *ws->cat("dataCat"));    
   //  /////////////////////////////////////////////////////////////////////
   ws->var("glob_lumipp")->setConstant(true);
   ws->var("glob_Taa")->setConstant(true);
   ws->var("glob_effRat")->setConstant(true);
   ws->var("glob_nbkg_pp")->setConstant(true);
   ws->var("glob_nbkg_hi")->setConstant(true);
   RooArgSet globalObs("global_obs");
   globalObs.add( *ws->var("glob_lumipp") );
   globalObs.add( *ws->var("glob_Taa") );
   globalObs.add( *ws->var("glob_effRat") );
   globalObs.add( *ws->var("glob_nbkg_hi") );
   globalObs.add( *ws->var("glob_nbkg_pp") );

   // ws->Print();

   RooArgSet poi("poi");
   poi.add( *ws->var("raa2") );



   // create set of nuisance parameters
   RooArgSet nuis("nuis");
   nuis.add( *ws->var("beta_lumipp") );
   nuis.add( *ws->var("beta_nbkg_hi") );
   nuis.add( *ws->var("beta_nbkg_pp") );
   nuis.add( *ws->var("beta_Taa") );
   nuis.add( *ws->var("beta_effRat") );

   ws->var("#alpha_{CB}_hi")->setConstant(true);
   ws->var("#alpha_{CB}_pp")->setConstant(true);
   ws->var("#sigma_{CB1}_hi")->setConstant(true);
   ws->var("#sigma_{CB1}_pp")->setConstant(true);
   ws->var("#sigma_{CB2}/#sigma_{CB1}_hi")->setConstant(true);
   ws->var("#sigma_{CB2}/#sigma_{CB1}_pp")->setConstant(true);
   ws->var("Centrality")->setConstant(true);
   ws->var("N_{#Upsilon(1S)}_hi")->setConstant(true);
   ws->var("N_{#Upsilon(1S)}_pp")->setConstant(true);
   ws->var("N_{#Upsilon(3S)}_hi")->setConstant(true);
   ws->var("N_{#Upsilon(2S)}_pp")->setConstant(true);
   ws->var("N_{#Upsilon(3S)}_pp")->setConstant(true);
   ws->var("Nmb_hi")->setConstant(true);
   // ws->var("QQsign")->setConstant(true);
   ws->var("Taa_hi")->setConstant(true);
   ws->var("Taa_kappa")->setConstant(true);
   // ws->var("beta_Taa")->setConstant(true);
   // ws->var("beta_effRat")->setConstant(true);
   // ws->var("beta_lumipp")->setConstant(true);
   // ws->var("beta_nbkg_hi")->setConstant(true);
   // ws->var("beta_nbkg_pp")->setConstant(true);
   // ws->var("dataCat")->setConstant(true);
   ws->var("decay_hi")->setConstant(true);
   ws->var("decay_pp")->setConstant(true);
   ws->var("effRat2_hi")->setConstant(true);
   ws->var("effRat_kappa")->setConstant(true);
   // ws->var("glob_Taa")->setConstant(true);
   // ws->var("glob_effRat")->setConstant(true);
   // ws->var("glob_lumipp")->setConstant(true);
   // ws->var("glob_nbkg_hi")->setConstant(true);
   // ws->var("glob_nbkg_pp")->setConstant(true);
   // ws->var("invariantMass")->setConstant(true);
   ws->var("leftEdge")->setConstant(true);
   ws->var("lumipp_hi")->setConstant(true);
   ws->var("lumipp_kappa")->setConstant(true);
   ws->var("mass1S_hi")->setConstant(true);
   ws->var("mass1S_pp")->setConstant(true);
   ws->var("muMinusPt")->setConstant(true);
   ws->var("muPlusPt")->setConstant(true);
   ws->var("n_{Bkgd}_hi")->setConstant(true);
   ws->var("n_{Bkgd}_pp")->setConstant(true);
   ws->var("nbkg_hi_kappa")->setConstant(true);
   ws->var("nbkg_pp_kappa")->setConstant(true);
   ws->var("npow")->setConstant(true);
   // ws->var("raa3")->setConstant(true);
   ws->var("rightEdge")->setConstant(true);
   ws->var("sigmaFraction_hi")->setConstant(true);
   ws->var("sigmaFraction_pp")->setConstant(true);
   ws->var("turnOn_hi")->setConstant(true);
   ws->var("turnOn_pp")->setConstant(true);
   ws->var("upsPt")->setConstant(true);
   ws->var("upsRapidity")->setConstant(true);
   ws->var("vProb")->setConstant(true);
   ws->var("width_hi")->setConstant(true);
   ws->var("width_pp")->setConstant(true);
   // ws->var("x2raw")->setConstant(true);
   //  RooArgSet fixed_again("fixed_again");
   //  fixed_again.add( *ws->var("leftEdge") );
   //  fixed_again.add( *ws->var("rightEdge") );
   //  fixed_again.add( *ws->var("Taa_hi") );
   //  fixed_again.add( *ws->var("Nmb_hi") );
   //  fixed_again.add( *ws->var("lumipp_hi") );
   //  fixed_again.add( *ws->var("effRat1_hi") );
   //  fixed_again.add( *ws->var("effRat2_hi") );
   //  fixed_again.add( *ws->var("effRat3_hi") );
   //  fixed_again.add( *ws->var("nsig3_pp") );
   //  fixed_again.add( *ws->var("nsig1_pp") );
   //  fixed_again.add( *ws->var("nbkg_hi") );
   //  fixed_again.add( *ws->var("alpha") );
   //  fixed_again.add( *ws->var("nbkg_kappa") );
   //  fixed_again.add( *ws->var("Taa_kappa") );
   //  fixed_again.add( *ws->var("lumipp_kappa") );
   // fixed_again.add( *ws->var("mean_hi") );
   // fixed_again.add( *ws->var("mean_pp") );
   // fixed_again.add( *ws->var("width_hi") );
   // fixed_again.add( *ws->var("turnOn_hi") );
   // fixed_again.add( *ws->var("bkg_a1_pp") );
   // fixed_again.add( *ws->var("bkg_a2_pp") );
   // fixed_again.add( *ws->var("decay_hi") );
   // fixed_again.add( *ws->var("raa1") );
   // fixed_again.add( *ws->var("raa2") );
   //  fixed_again.add( *ws->var("nsig2_pp") );
   // fixed_again.add( *ws->var("sigma1") );
   //  fixed_again.add( *ws->var("nbkg_pp") );
   // fixed_again.add( *ws->var("npow") );
   // fixed_again.add( *ws->var("muPlusPt") );
   // fixed_again.add( *ws->var("muMinusPt") );
   // fixed_again.add( *ws->var("mscale_hi") );
   // fixed_again.add( *ws->var("mscale_pp") );
   //  
   // ws->Print();

   // create signal+background Model Config
   RooStats::ModelConfig sbHypo("SbHypo");
   sbHypo.SetWorkspace( *ws );
   sbHypo.SetPdf( *ws->pdf("joint") );
   sbHypo.SetObservables( obs );
   sbHypo.SetGlobalObservables( globalObs );
   sbHypo.SetParametersOfInterest( poi );
   sbHypo.SetNuisanceParameters( nuis );
   sbHypo.SetPriorPdf( *ws->pdf("step") ); // this is optional

   // ws->Print();
   /////////////////////////////////////////////////////////////////////
   RooAbsReal * pNll = sbHypo.GetPdf()->createNLL( *data,NumCPU(2) );
   RooMinuit(*pNll).migrad(); // minimize likelihood wrt all parameters before making plots
   RooPlot *framepoi = ((RooRealVar *)poi.first())->frame(Bins(10),Range(0.,0.2),Title("LL and profileLL in raa2"));
   pNll->plotOn(framepoi,ShiftToZero());
   
   RooAbsReal * pProfile = pNll->createProfile( globalObs ); // do not profile global observables
   pProfile->getVal(); // this will do fit and set POI and nuisance parameters to fitted values
   pProfile->plotOn(framepoi,LineColor(kRed));
   framepoi->SetMinimum(0);
   framepoi->SetMaximum(3);
   TCanvas *cpoi = new TCanvas();
   cpoi->cd(); framepoi->Draw();
   cpoi->SaveAs("cpoi.pdf");

   ((RooRealVar *)poi.first())->setMin(0.);
   RooArgSet * pPoiAndNuisance = new RooArgSet("poiAndNuisance");
   // pPoiAndNuisance->add(*sbHypo.GetNuisanceParameters());
   // pPoiAndNuisance->add(*sbHypo.GetParametersOfInterest());
   pPoiAndNuisance->add( nuis );
   pPoiAndNuisance->add( poi );
   sbHypo.SetSnapshot(*pPoiAndNuisance);

   RooPlot* xframeSB = pObs->frame(Title("SBhypo"));
   data->plotOn(xframeSB,Cut("dataCat==dataCat::hi"));
   RooAbsPdf *pdfSB = sbHypo.GetPdf();
   RooCategory *dataCat = ws->cat("dataCat");
   pdfSB->plotOn(xframeSB,Slice(*dataCat,"hi"),ProjWData(*dataCat,*data));
   TCanvas *c1 = new TCanvas();
   c1->cd(); xframeSB->Draw();
   c1->SaveAs("c1.pdf");

   delete pProfile;
   delete pNll;
   delete pPoiAndNuisance;
   ws->import( sbHypo );
   /////////////////////////////////////////////////////////////////////
   RooStats::ModelConfig bHypo = sbHypo;
   bHypo.SetName("BHypo");
   bHypo.SetWorkspace(*ws);
   pNll = bHypo.GetPdf()->createNLL( *data,NumCPU(2) );
   RooArgSet poiAndGlobalObs("poiAndGlobalObs");
   poiAndGlobalObs.add( poi );
   poiAndGlobalObs.add( globalObs );
   pProfile = pNll->createProfile( poiAndGlobalObs ); // do not profile POI and global observables
   ((RooRealVar *)poi.first())->setVal( 0 );  // set raa2=0 here
   pProfile->getVal(); // this will do fit and set nuisance parameters to profiled values
   pPoiAndNuisance = new RooArgSet( "poiAndNuisance" );
   pPoiAndNuisance->add( nuis );
   pPoiAndNuisance->add( poi );
   bHypo.SetSnapshot(*pPoiAndNuisance);

   RooPlot* xframeB = pObs->frame(Title("Bhypo"));
   data->plotOn(xframeB,Cut("dataCat==dataCat::hi"));
   RooAbsPdf *pdfB = bHypo.GetPdf();
   pdfB->plotOn(xframeB,Slice(*dataCat,"hi"),ProjWData(*dataCat,*data));
   TCanvas *c2 = new TCanvas();
   c2->cd(); xframeB->Draw();
   c2->SaveAs("c2.pdf");

   delete pProfile;
   delete pNll;
   delete pPoiAndNuisance;

   // import model config into workspace
   bHypo.SetWorkspace(*ws);
   ws->import( bHypo );
   /////////////////////////////////////////////////////////////////////
   ws->Print();
   bHypo.Print();
   sbHypo.Print();

   // save workspace to file
   ws -> SaveAs(name_out);

   return;
}




