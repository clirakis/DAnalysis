{
    TCanvas *Hobbes = new TCanvas("Dist","A test",5,5,1200,600);
    Hobbes->cd();
    TPad    *Calvin = new TPad("Calvin","Silly",0.02,0.02,0.99,0.99, 33);
    Calvin->Draw();
    Calvin->cd();
    Calvin->SetGrid();

    TFile *tf = new TFile("IMU.root");
    Bool_t Relative = kTRUE;

    IMUData->Draw("AP");
    if(!Relative)
    {
	IMUData->GetXaxis()->SetTimeDisplay(1);
	IMUData->GetXaxis()->SetNdivisions(503);
	IMUData->GetXaxis()->SetTimeFormat("%Y-%m-%d %H:%M");
	IMUData->GetXaxis()->SetTimeOffset(0,"gmt");
    }
    else
    {
	IMUData->GetXaxis()->SetTimeDisplay(1);
	IMUData->GetXaxis()->SetNdivisions(513);
	IMUData->GetXaxis()->SetTimeFormat("%H:%M:%S");
	IMUData->GetXaxis()->SetTimeOffset(0,"gmt");
    }

    TH1 *hbs = IMUData->GetHistogram();
    hbs->SetXTitle("Time");
    hbs->SetYTitle("Total Field (uT)");
    hbs->SetLabelSize(0.03,"X");
    hbs->SetLabelSize(0.03,"Y");
}
