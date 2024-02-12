{
    TCanvas *Hobbes = new TCanvas("Dist","A test",5,5,1200,600);
    Hobbes->cd();
    TPad    *Calvin = new TPad("Calvin","Silly",0.02,0.02,0.99,0.99, 33);
    Calvin->Draw();
    Calvin->cd();
    Calvin->SetGrid();

    TFile *tf = new TFile("IMU.root");

    /*
     * MX {-13:-14}
     * MY {-51:-52}
     * MZ {67.2:68.2}
     * MAG {85.7:86.4}
     */
    TProfile *prof = new TProfile("ABSMAG", "Absolute Magnitude", 
				  215, 0.0, 86000.0, 80.0, 90.0);
    /* Make a profile plot. */
    //IMUTuple->Draw("sqrt(MX*MX+MY*MY+MZ*MZ):UTC","", "PROF");
    IMUTuple->Draw("sqrt(MX*MX+MY*MY+MZ*MZ):UTC>>ABSMAG");
    prof->Draw();
    prof->GetXaxis()->SetTimeDisplay(1);
    prof->GetXaxis()->SetNdivisions(513);
    prof->GetXaxis()->SetTimeFormat("%H:%M:%S");
    prof->GetXaxis()->SetTimeOffset(0,"gmt");

    //TH1 *hbs = IMUData->GetHistogram();
    prof->SetXTitle("Time");
    prof->SetYTitle("Total Field (uT)");
    prof->SetLabelSize(0.03,"X");
    prof->SetLabelSize(0.03,"Y");

}
