// All libraries you could wish for
#include <iostream>                 // class for inputoutput streams
#include <stdio.h>                  // printf, scanf, puts, NULL
#include <stdlib.h>                 // srand, rand, atof
#include <time.h>                   // time
#include <fstream>                  // class for inputoutput file streams
#include <dirent.h>
#include <string.h>           // class to manipulate strings
#include <sstream>                    // class for parsing strings
#include <vector>


#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TPaveStats.h>
#include <TPad.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TString.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TLorentzVector.h>


#include <unistd.h>
#define GetCurrentDir getcwd

#include <TTree.h>
#include <TClonesArray.h>
#include "Math/SMatrix.h"

using namespace std;

//Definitions:
//Succesfull data: Data that fits some criteria
//Succesfull event: Event in which all data is succesfull   

int main(int argc, char* argv[])
{ 

  //Choose your selection criteria, a negative number means that there is no criteria and all events are chossen
  int lepton_num =1;
  int jet_num=3;
  //Lower energy bound 
  double lepton_energy = 40.0;
  double jet_energy = 30.0;
  double met_energy = 60.0;
  //Upper eta bound
  double lepton_eta = 2.5;
  double jet_eta =2.4; 

  //This will be used to use only succesful events
  int criteria1=0;
  int criteria2=0;
  int criteria3=0;

  //For now we are asuming you are only interested in electron, muons, or their anti particles

  //Essentials

  //Upload the file with the data, make sure the adress of the file matches the one in your computer
  TFile* file = TFile::Open("/Users/Fer/Documents/traajo/samples/NeroNtuples_9.root"); // TFile::Open() instead of a constructor since it works over xrootd etc. =D
  
  //Upload the tree with the event data
  TTree *tree=(TTree*)file->Get("nero/events");

  //Variables for the for loop

  //Get how many events we have to loop through
  int nentries = tree->GetEntries();
  //int nentries=300;
  //Create a variable to store the mass values
  Double_t mass;

/////////////////////////////////////////////////////

  //Lepton criteria

  //Create a variable to store all the lepton event data
  TClonesArray *leptondata = new TClonesArray::TClonesArray ( "TLorentzVector", nentries);

  //Create the vector to store all the particle identifiers
  std::vector<Int_t> * lepPdgId= 0;

  //Specify where all the lepton event data will be stored
  tree->SetBranchAddress("lepP4", &leptondata);

  //Specify where all the lepton identifiers will be stored
  tree->SetBranchAddress("lepPdgId", &lepPdgId);

  
/////////////////////////////////////////////////////

  //MET criteria

  //Create a variable to store all the "met" data
  TClonesArray *metdata = new TClonesArray::TClonesArray ( "TLorentzVector", nentries);

  //Specify where all the "met" data will be stored
  tree->SetBranchAddress("metP4", &metdata);

/////////////////////////////////////////////////////

  //Eta criteria

  //Create the variable for Eta
  Double_t eta;

/////////////////////////////////////////////////////

  //Jet criteria

  //Create a variable to store all the jet event data
  TClonesArray *jetdata = new TClonesArray::TClonesArray ( "TLorentzVector", nentries);

  //Specify where all the jet event data will be stored
  tree->SetBranchAddress("jetP4", &jetdata);


/////////////////////////////////////////////////////

  //Histograms for succesful events

  //Lepton histogram
  TH1F *hist_lepton_mass_2 = new TH1F("hist_lepton_mass_2", "Lepton transverse momentum", 50, 0, 150);

  //W bosson histogram
  TH1F *hist_w_mass_2 = new TH1F("hist_w_mass_2", "W mass", 50, 0, 150);

  //MET histogram
  TH1F *hist_met_mass_2 = new TH1F("hist_met_mass_2", "Missing transverse energy (MET)", 50, 0, 150);

  //Jet histogram
  TH1F *hist_jet_mass_2 = new TH1F("hist_jet_mass_2", "Jet transverse energy", 50, 0, 150);

  //Histogram to plot the distribution of the whole mass 
  TH1F *hist_whole_mass_2 = new TH1F("hist_whole_mass_2", "Whole mass", 50, 0, 150); 

  //Scalling variable
  Double_t scale;


/////////////////////////////////////////////////////

  //Histogram variables

  //Canvas for all succesful events
  TCanvas* c_2 = new TCanvas("c_2", "Succesful events", 600, 600);

  //Divide that canvas to plot all histograms together
  c_2->Divide(3,3);

/////////////////////////////////////////////////////

  //Lorentz vectors
  //Variables inside loops only exist inside the loops
  //So we need to declare all the lorentz vectors here to use them wherever we want
    TLorentzVector addable_lorentz_wdata;
    TLorentzVector addable_lorentz_jetdata;
    TLorentzVector addable_lorentz_leptondata;
    TLorentzVector addable_lorentz_metdata;
    TLorentzVector addable_total_lorentz_leptondata;
    TLorentzVector addable_total_lorentz_jetdata;
    TLorentzVector addable_lorentz_wholedata;
    TLorentzVector addable_lorentz_empty;


  for(int ientry = 0; ientry < nentries-1; ientry++) 
  {
    //Reset the data
    leptondata->Clear();
    metdata->Clear();
    jetdata->Clear();

    //Reset wholedata and the totals in every event
    addable_lorentz_wholedata=addable_lorentz_empty;
    addable_total_lorentz_leptondata=addable_lorentz_empty;
    addable_total_lorentz_jetdata=addable_lorentz_empty;

    //Reset criteria
    criteria1=0;
    criteria2=0;
    criteria3=0;

    //This line stores the proper data in the TClonesArrays
    tree->GetEntry(ientry);


/////////////////////////////////////////////////////

    //Leptons

    //Selection criteria that avoids empty events
    if((leptondata->GetSize() == lepton_num)||((lepton_num<0) && (leptondata->GetSize() != 0)))
    {

      //Loop through all the leptons
      for (int i = 0; i < leptondata->GetSize(); ++i)
      {

        //If you want all leptons comment the next line, else this chooses only electrons, muons, or their antiparticles
        if((abs(lepPdgId->at(i))!=11)&&(abs(lepPdgId->at(i))!=13)) continue; 

        //Store all the data of the electron in this lorentz vector
        TLorentzVector * lorentz_leptondata = (TLorentzVector *)leptondata->At(i);

        //It's troublesome to do math with pointers, so we create this non pointer variable 
        addable_lorentz_leptondata = *lorentz_leptondata;

        //Get the transverse momentum of that lorentz vector
        mass=addable_lorentz_leptondata.Pt();

        //Get the Eta value of that Lorentz vector
        eta=addable_lorentz_leptondata.Eta();

        //Selection criteria that chooses energy and invariant angle
        if ((mass<lepton_energy || lepton_energy<0) || (abs(eta)>lepton_eta || lepton_eta<0)) continue;

        //For the succesful events
        addable_total_lorentz_leptondata=addable_total_lorentz_leptondata+addable_lorentz_leptondata;

        //We store the transverse momentum of all the leptons in the event
        addable_lorentz_wholedata = addable_lorentz_wholedata + addable_lorentz_leptondata;

        //We keep track of the conditions met 
        criteria1=1;
      }
    }//Closes lepton selection criteria

    /////////////////////////////////////////////////////

    //Met

    //Create a lorentz vector with the metdata of the current entry (it's size is always one)
    TLorentzVector * lorentz_metdata = (TLorentzVector *) metdata->At(0);

    //It's troublesome to do math with pointers
    addable_lorentz_metdata = *lorentz_metdata;

    ///////// Im working on a way to change the previous two lines into a single one

    //Get the invariant transverse energy of that lorentz vector
    mass=addable_lorentz_metdata.Et();

    //Selection criteria
    if ((mass>=met_energy)||(met_energy<0))
    {

      addable_lorentz_wholedata=addable_lorentz_metdata;
      
      //We keep track of the conditions met 
      criteria2=criteria1+1;

      if (criteria2==2)
      {
        /////////////////////////////////////////////////////

        //Thus only if there are succesful leptons we consider the existence of a W boson
        //W boson

        //This will make sense depending on your criteria
        //Remember that by this point whole data has the information of the MET and every lepton that was succesful
        addable_lorentz_wdata = addable_lorentz_wholedata;

        //Get the invariant transverse energy of that lorentz vector
        mass=addable_lorentz_wdata.Mt();
      }
    }


/////////////////////////////////////////////////////

    //Jets

    //Selection criteria that avoids empty events
    if ((jetdata->GetSize()==jet_num)||(jet_num<0 && jetdata->GetSize() != 0))
    {
      //Loop through all the jets
      for (int i = 0; i < jetdata->GetSize(); ++i)
      {
        //Store all the data of the jet in this lorentz vector
        TLorentzVector * lorentz_jetdata = (TLorentzVector *)jetdata->At(i);

        //It's troublesome to do math with pointers
        addable_lorentz_jetdata = *lorentz_jetdata;

        //Get the transverse energy of that lorentz vector
        mass=addable_lorentz_jetdata.Et();

        //Get the Eta value of that Lorentz vector
        eta=addable_lorentz_jetdata.Eta();
 
        if ((mass<jet_energy || jet_energy<0) || (abs(eta)>jet_eta || jet_eta<0)) continue;

          //For the succesful events
          addable_total_lorentz_jetdata=addable_total_lorentz_jetdata+addable_lorentz_jetdata;

          //Add them up
          addable_lorentz_wholedata=addable_lorentz_wholedata+addable_lorentz_jetdata;  

          //We keep track of the conditions met 
          criteria3=criteria2+1;    
      }
    }//Closes jet selection criteria

/////////////////////////////////////////////////////

    //Whole

    if (criteria3==3)
    {
      //Now to fill the histograms of the succesful events

      mass=addable_lorentz_wholedata.Et();
      hist_whole_mass_2->Fill(mass);

      mass=addable_total_lorentz_leptondata.Pt();
      hist_lepton_mass_2->Fill(mass);

      mass=addable_total_lorentz_jetdata.Et();
      hist_jet_mass_2->Fill(mass);

      mass=addable_lorentz_metdata.Et();
      hist_met_mass_2->Fill(mass);

      mass=addable_lorentz_wdata.Mt();
      hist_w_mass_2->Fill(mass);
    }


  }//This closes the main for loop

//Make the image for the succesful events

  c_2->cd(1);
  scale = 1/hist_met_mass_2->Integral();
  hist_met_mass_2->Scale(scale);
  hist_met_mass_2->Draw("H");
  c_2->Update();

  c_2->cd(2);
  scale = 1/hist_lepton_mass_2->Integral();
  hist_lepton_mass_2->Scale(scale);
  hist_lepton_mass_2->Draw("H");
  c_2->Update();

  c_2->cd(3);
  scale = 1/hist_jet_mass_2->Integral();
  hist_jet_mass_2->Scale(scale);
  hist_jet_mass_2->Draw("H");
  c_2->Update();

  c_2->cd(4);
  scale = 1/hist_w_mass_2->Integral();
  hist_w_mass_2->Scale(scale);
  hist_w_mass_2->Draw("H");
  c_2->Update();

  c_2->cd(5);
  scale = 1/hist_whole_mass_2->Integral();
  hist_whole_mass_2->Scale(scale);
  hist_whole_mass_2->Draw("H");
  c_2->Update();

  //Save the image with the name you want
  c_2->SaveAs("test2.pdf");
  c_2->Close();

  // cleanup
  delete file; // automatically deletes "tree" too
  delete lepPdgId;
  delete leptondata;
  delete metdata;
  delete jetdata;
  return 0; 
}