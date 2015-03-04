#include "functions.C"
#include "RooGlobalFunc.h"
#include "RooWorkspace.h"

RooWorkspace* test_combine(const char* name_pbpb="fitresult.root", const char* name_pp="fitresult_pp.root")
{
   // const char *poiname="N_{#Upsilon(3S)}";
   TFile *f = new TFile(name_pbpb) ;
   TFile *f_pp = new TFile(name_pp) ;

   // Retrieve workspace from file
   RooWorkspace* ws = (RooWorkspace*) f->Get("ws");
   RooWorkspace* ws_pp = (RooWorkspace*) f_pp->Get("ws");

   // RooRealVar *theVar; 
   RooDataSet *data; RooAbsPdf *pdf;
   RooRealVar *theVar_pp; RooDataSet *data_pp; RooAbsPdf *pdf_pp;

   // theVar = ws->var(poiname);
   // pdf = ws->pdf("pdf");
   data =(RooDataSet *) ws->data("data");
   pdf_pp = ws_pp->pdf("pdf");
   data_pp =(RooDataSet *) ws_pp->data("data");

	RooCategory dataCat("dataCat", "dataCat");
	dataCat.defineType("hi");
	dataCat.defineType("pp");

	RooRealVar muppt("muPlusPt" ,"#mu+ pt",2,20,"GeV/c"); 
	RooRealVar mumpt("muMinusPt","#mu- pt",2,20,"GeV/c"); 
	RooRealVar upsPt("upsPt","p_{T}(#Upsilon)",0.,"GeV/c");
	RooRealVar vProb("vProb","vProb",0.05,1);
	//   RooRealVar upsEta("upsEta","#eta(#Upsilon)",0.,"");
	RooRealVar upsRapidity("upsRapidity", "upsRapidity", 0.);
	RooCategory QQsign("QQsign", "QQsign");
	QQsign.defineType("PlusMinus", 0);
	QQsign.defineType("PlusPlus", 1);
	QQsign.defineType("MinusMinus", 2);
	RooRealVar Centrality("Centrality", "Centrality", 0.);
	RooRealVar * mass = ws->var("invariantMass");
	if (!mass) {
		mass = new RooRealVar("invariantMass", "#mu#mu mass", mmin, mmax, 
				"GeV/c^{2}");
	}

	RooArgSet cols(*mass, muppt, mumpt, upsPt, vProb, upsRapidity, QQsign, Centrality);

	RooDataSet data_combo("data", "data", cols, RooFit::Index(dataCat),
/*Only for track rotation*/
		RooFit::Import("hi", *data), RooFit::Import("pp", *data_pp));

   RooWorkspace *wcombo = new RooWorkspace("wcombo","workspace for PbPb + pp");
   wcombo->import(data_combo);
	wcombo->import(*pdf_pp, RooFit::RenameAllNodes("pp"),
			RooFit::RenameAllVariablesExcept("pp", 
				"npow,invariantMass,"
				//"prior,"
				//"mean,"
				//"turnOn,"
				"f23,f3o2,"
				"x23,x3o2,"
				"alpha,"
				"sigma1"
				), 
			RooFit::RecycleConflictNodes());

   // // create the combined variable
   // RooRealVar* n3shi = wcombo->var("N_{#Upsilon(3S)}_hi");
   // RooRealVar* n3spp = wcombo->var("N_{#Upsilon(3S)}_pp");
   // RooFormulaVar x3raw("x3raw","x3raw","@0/@1",RooArgList(*n3shi,*n3spp));
   // cout << x3raw.getVal() << endl;
   // wcombo->import(x3raw);
   // wcombo->Print();

   RooAddPdf *sig1S = ws->pdf("cbcb");
   RooAddPdf *sig2S = ws->pdf("sig2S");
   RooAddPdf *sig3S = ws->pdf("sig3S");
   RooAddPdf *pdf_combinedbkgd = ws->pdf("bkgPdf");
   RooRealVar *nsig1f = ws->var("N_{#Upsilon(1S)}");
   RooRealVar *nsig2f = ws->var("N_{#Upsilon(2S)}");
   RooRealVar *nsig3f = ws->var("N_{#Upsilon(3S)}");
   RooRealVar *nbkgd = ws->var("n_{Bkgd}");
   // RooRealVar *x3raw = new RooRealVar("x3raw","x3raw",7e-4,-10,10);
   // RooRealVar *nsig3f_pp = ws_pp->var("N_{#Upsilon(3S)}"); nsig3f_pp->SetName("N_{#Upsilon(3S)}_pp");
   // RooFormulaVar *nsig3f_new = new RooFormulaVar("N_{#Upsilon(3S)}","@0*@1",RooArgList(*nsig3f_pp,*x3raw));

   RooAbsPdf *pdf_new = new RooAddPdf ("pdf","new total p.d.f.",
         RooArgList(*sig1S,*sig2S,*sig3S,*pdf_combinedbkgd),
         RooArgList(*nsig1f,*nsig2f,*nsig3f,*nbkgd));
	wcombo->import(*pdf_new, RooFit::RenameAllNodes("hi"),
			RooFit::RenameAllVariablesExcept("hi", 
				"npow,invariantMass,"
				//"prior,"
				//"mean,"
				//"turnOn,"
            // "f23,f3o2,"
				"x23,x3o2,"
				"alpha,"
				"sigma1,"
            "x3raw,N_{#Upsilon(3S)}_pp"
				), 
			RooFit::RecycleConflictNodes());
   wcombo->Print();
   RooSimultaneous* simPdf = buildSimPdf(*wcombo,dataCat);
   wcombo->Print();

   // not sure this is really needed since we will fit again in the later workspace creation
   // RooFitResult* fit_2nd;// fit results
   // fit_2nd = simPdf->fitTo(data_combo,
   //       // RooFit::Constrained(),
   //       RooFit::Save(kTRUE),
   //       RooFit::Extended(kTRUE),
   //       RooFit::Minos(kTRUE),
   //       RooFit::NumCPU(25));


   // fix all other variables in model:
   // everything except observables, POI, and nuisance parameters
   // must be constant
   wcombo->var("#alpha_{CB}_hi")->setConstant(true);
   wcombo->var("#alpha_{CB}_pp")->setConstant(true);
   wcombo->var("#sigma_{CB1}_hi")->setConstant(true);
   wcombo->var("#sigma_{CB1}_pp")->setConstant(true);
   wcombo->var("#sigma_{CB2}/#sigma_{CB1}_hi")->setConstant(true);
   wcombo->var("#sigma_{CB2}/#sigma_{CB1}_pp")->setConstant(true);
   wcombo->var("N_{#Upsilon(1S)}_hi")->setConstant(true);
   wcombo->var("N_{#Upsilon(1S)}_pp")->setConstant(true);
   wcombo->var("N_{#Upsilon(2S)}_hi")->setConstant(true);
   wcombo->var("N_{#Upsilon(2S)}_pp")->setConstant(true);
   wcombo->var("N_{#Upsilon(3S)}_pp")->setConstant(true);
   wcombo->var("decay_hi")->setConstant(true);
   wcombo->var("decay_pp")->setConstant(true);
   wcombo->var("mass1S_hi")->setConstant(true);
   wcombo->var("mass1S_pp")->setConstant(true);
   wcombo->var("n_{Bkgd}_hi")->setConstant(true);
   wcombo->var("n_{Bkgd}_pp")->setConstant(true);
   wcombo->var("npow")->setConstant(true);
   wcombo->var("sigmaFraction_hi")->setConstant(true);
   wcombo->var("sigmaFraction_pp")->setConstant(true);
   wcombo->var("turnOn_hi")->setConstant(true);
   wcombo->var("turnOn_pp")->setConstant(true);
   wcombo->var("width_hi")->setConstant(true);
   wcombo->var("width_pp")->setConstant(true);

   // wcombo->writeToFile("fitresult_combo.root");
   return wcombo;
}
