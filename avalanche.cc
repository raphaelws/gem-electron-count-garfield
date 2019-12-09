/**
 * avalanche.cc
 * General program flow based on example code from the Garfield++ website.
 *
 * Demonstrates electron avalanche and induced signal readout with
 * 2D finite-element visualization in Garfield++ with a LEM.
 * LEM parameters are from:
 * C. Shalem et. al. Nucl. Instr. Meth. A, 558, 475 (2006).
 *
*/
#include <iostream>
#include <fstream>

#include <cmath>

#include <TCanvas.h>
#include <TApplication.h>
#include <TFile.h>

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/ComponentElmer.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/Plotting.hh"
#include "Garfield/ViewFEMesh.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/GarfieldConstants.hh"
#include "Garfield/Random.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/TrackHeed.hh"

using namespace Garfield;

int main(int argc, char* argv[]) {
   // PARTE DO CÓDIGO QUE FAZ A PREPARACAO

   //CONTO AQUI O NÚMERO DE LINHAS PARA VER QUANTAS ENERGIA TEMOS NO
   // ARQUIVO DE ENTRADA
   int nlinhas = 0;
  std::ifstream energia;
   energia.open("energias.txt");
   if (energia.is_open()) {
   std::string linha;
   while (getline(energia,linha)) {
   nlinhas+=1;
 }
 energia.close();
}


  float somael [nlinhas] = {0};



 // ----------------------------------------------------------
 //-------------------------------------------------------------
  //MUDE AQUI O NÚMERO DE LANAMENTOS QUE QUER FAZER PARA CADA ENERGIA
  float nlanca = 100;

//-------------------------------------------------------------
//------------------------------------------------------------



//inicio do LOOp que produz os elétrons a partir dos fótons
  TApplication app("app", &argc, argv);
  for (int j=0; j<=nlanca; j++){

  //TApplication app("app", &argc, argv);

  // Set relevant LEM parameters.
  // LEM thickness in cm
  const double lem_th = 0.04;
  // Copper thickness
  const double lem_cpth = 0.0035;
  // LEM pitch in cm
  const double lem_pitch = 0.07;
  // X-width of drift simulation will cover between +/- axis_x
  const double axis_x = 0.1;
  // Y-width of drift simulation will cover between +/- axis_y
  const double axis_y = 0.1;
  const double axis_z = 0.25 + lem_th / 2 + lem_cpth;


  // Define the medium.
  MediumMagboltz* gas = new MediumMagboltz();
  // Set the temperature (K)
  gas->SetTemperature(293.15);
  // Set the pressure (Torr)
  gas->SetPressure(740.);
  // Allow for drifting in this medium
  gas->EnableDrift();
  // Specify the gas mixture (Ar/CO2 70:30)
  gas->SetComposition("ar", 70., "co2", 30.);

  // Import an Elmer-created field map.
  ComponentElmer* elm = new ComponentElmer(
      "gemcell/mesh.header", "gemcell/mesh.elements", "gemcell/mesh.nodes",
      "gemcell/dielectrics.dat", "gemcell/gemcell.result", "cm");
  elm->EnablePeriodicityX();
  elm->EnableMirrorPeriodicityY();
  elm->SetMedium(0, gas);
  // Import the weighting field for the readout electrode.
  elm->SetWeightingField("gemcell/gemcell_WTlel.result", "wtlel");

  // Set up a sensor object.
  Sensor* sensor = new Sensor();
  sensor->AddComponent(elm);
  sensor->SetArea(-axis_x, -axis_y, -axis_z, axis_x, axis_y, axis_z);
  sensor->AddElectrode(elm, "wtlel");
  // Set the signal binning.
  const double tEnd = 500.0;
  const int nsBins = 500;
  sensor->SetTimeWindow(0., tEnd / nsBins, nsBins);

  // Create an avalanche object
  AvalancheMicroscopic* aval = new AvalancheMicroscopic();
  aval->SetSensor(sensor);
  aval->SetCollisionSteps(100);
  aval->EnableSignalCalculation();

  // Set up the object for drift line visualization.
  ViewDrift* viewDrift = new ViewDrift();
  viewDrift->SetArea(-axis_x, -axis_y, -axis_z, axis_x, axis_y, axis_z);
  aval->EnablePlotting(viewDrift);

  //LANÇAMENTO DO FÓTON PELO HEED COMEÇA AQUI-----------------------------------------------------
  std::ifstream energia;
  energia.open("energias.txt");
  //energia.open ("file.txt");
//  file in("file.txt");

//LENDO AS ENERGIAS
  int local = 0;
  if (energia.is_open()) {
  std::string linha;
  float energyp;
  while (getline(energia,linha)) {
  float energyp = std::stof(linha.c_str());



  //std::cout << "Lançando com energia " << energyp << std::endl;

    // now we loop back and get the next line in 'str'

  const double xp = 0.;
  const double yp = 0.;
  const double zp = 0.07;
  const double tp = 0.;
  const double dxp = 0;
  const double dyp = 0;
  const double dzp = -1;
  //const double energyp = 6000.;
  int nel = 0;
  //número de lançamentos por energia
  //int nlanca = 2;
  float soma = 0;

  //for (int j=0; j<nlanca; j++){
  TrackHeed* track = new TrackHeed();
  track -> SetSensor(sensor);
  track -> EnableElectricField();
  track->EnablePlotting(viewDrift);

  int count = 0;
   while (nel == 0) {
   track -> TransportPhoton(xp, yp, zp, tp, energyp, dxp, dyp, dzp, nel);
    count += 1;
  //  std::cout << "Tentando: " << count << "\n";

}
//std::cout<< "Foram produzidos: " << nel << "elétrons \n";

//SOMANDO O NÚMERO DE ELÉTRONS PARA DEPOIS TIRAR A MEDIA FAZENDO
//SOMA / N DE LANÇAMENTOS
somael[local]+=nel;
std::cout << "Energia: " << energyp << "- --- " << "número de elétrons: " << nel << std::endl;

//INDICE DO VETOR
local+=1;

//----------------------------------------------------------------------------
//AVALANCHE COMENTADA POIS NÃO É NECESSÁRIA AGORA
// for (int i=0; i<nel; i++){
//   std::cout<<"Iniciando avalanche: " << i << "\n";
//   double xe, ye, ze, tel, energyel, dxe, dye, dze;
//   track -> GetElectron(i, xe, ye, ze, tel, energyel, dxe, dye, dze);
//       aval->AvalancheElectron(xe, ye, ze, tel, energyel, dxe, dye, dze);
// }
//float media =soma / nlanca;
//std::cout<<"Media do lançamento para " << energyp << "igual a: " << media << "\n";
//}
//comentario do nlanca ^
//---------------------------------------------------------------------------------------


}
//chave do while ^
energia.close();
}
//chave do if is open

// TERMINA AQUI ----------------------------------------------------------------------
  //std::cout << "fim da avalanche  " << i;
  //}
  // Extract the calculated signal.

  //--------------------------------------------------------------------------
  double bscale = tEnd / nsBins;  // time per bin
  double sum = 0.;  // to keep a running sum of the integrated signal

  // Create ROOT histograms of the signal and a file in which to store them.
  TFile* f = new TFile("avalanche_signals.root", "RECREATE");
  TH1F* hS = new TH1F("hh", "hh", nsBins, 0, tEnd);        // total signal
  TH1F* hInt = new TH1F("hInt", "hInt", nsBins, 0, tEnd);  // integrated signal

  // Fill the histograms with the signals.
  //  Note that the signals will be in C/(ns*binWidth), and we will divide by e
  // to give a signal in e/(ns*binWidth).
  //  The total signal is then the integral over all bins multiplied by the bin
  // width in ns.
  for (int i = 0; i < nsBins; i++) {
    double wt = sensor->GetSignal("wtlel", i) / ElementaryCharge;
    sum += wt;
    hS->Fill(i * bscale, wt);
    hInt->Fill(i * bscale, sum);
  }

  // Write the histograms to the TFile.
  hS->Write();
  hInt->Write();
  f->Close();

  // Plot the signal.
  // const bool plotSignal = true;
  // if (plotSignal) {
  //   TCanvas* cSignal = new TCanvas("signal", "Signal");
  //   ViewSignal* vSignal = new ViewSignal();
  //   vSignal->SetSensor(sensor);
  //   vSignal->SetCanvas(cSignal);
  //   vSignal->PlotSignal("wtlel");
  // }

  // Plot the geometry, field and drift lines.
  // TCanvas* cGeom = new TCanvas("geom", "Geometry/Avalanche/Fields");
  // cGeom->SetLeftMargin(0.14);
  // const bool plotContours = true;
  // if (plotContours) {
  //   ViewField* vf = new ViewField();
  //   vf->SetSensor(sensor);
  //   vf->SetCanvas(cGeom);
  //   vf->SetArea(-axis_x, -axis_y, axis_x, axis_y);
  //   vf->SetNumberOfContours(40);
  //   vf->SetNumberOfSamples2d(30, 30);
  //   vf->SetPlane(0, -1, 0, 0, 0, 0);
  //   vf->PlotContour("v");
  // }

  // Set up the object for FE mesh visualization.
  // ViewFEMesh* vFE = new ViewFEMesh();
  // vFE->SetArea(-axis_x, -axis_z, -axis_y, axis_x, axis_z, axis_y);
  // vFE->SetCanvas(cGeom);
  // vFE->SetComponent(elm);
  // vFE->SetPlane(0, -1, 0, 0, 0, 0);
  // vFE->SetFillMesh(true);
  // vFE->SetColor(1, kGray);
  // vFE->SetColor(2, kYellow + 3);
  // vFE->SetColor(3, kYellow + 3);
  // if (!plotContours) {
  //   vFE->EnableAxes();
  //   vFE->SetXaxisTitle("x (cm)");
  //   vFE->SetYaxisTitle("z (cm)");
  //   vFE->SetViewDrift(viewDrift);
  //   vFE->Plot();
  // }

  //app.Run(kTRUE);

}

//LIMPANDO O QUE ESTIVER ESCRITO NO ARQUIVO ANTERIORMENTE
std::ofstream eFile ("media-eletron.txt",std::ofstream::out | std::ofstream::trunc);
 std::ofstream efile ("media-eletron.txt",std::ofstream::app);
//TIRANDO A MÉDIA E SALVANDO NO ARQUIVO DE SAÍDA
 for (int k=0; k<nlinhas; k++){

  eFile << somael[k]/nlanca << std::endl;
  std::cout << somael[k] <<" dividido por " << nlanca << "= " << somael[k]/nlanca << std::endl; 
}
std::cout << "Fim do programa" << std::endl;


//  app.Run(kTRUE);
  return 0;

}
