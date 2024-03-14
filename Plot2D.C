{
    TCanvas *Hobbes = new TCanvas("Dist","A test",5,5,1200,600);
    Hobbes->cd();
    TPad    *Calvin = new TPad("Calvin","Silly",0.02,0.02,0.99,0.99, 33);
    Calvin->Draw();
    Calvin->cd();
    Calvin->SetGrid();

    TFile *tf = new TFile("IMU.root");

    Double_t Z_Upper = 0.0;
    Double_t Z_Lower = 0.0;
    Double_t X_Lower = 0.0;
    Double_t X_Upper = 365.0;
    

#if 1
    // Upper and lower limits for 217 Locust. 
    Z_Upper = 85.5;
    //Z_Lower = 82.5;
    Z_Lower = 81.0;
    X_Lower =  0.0;
    X_Upper = 54.0;
#else
    // Upper and lower limits for 62 Kane.
    // Day 63 goes up to 140
    Z_Upper =  97.0;
    Z_Lower =  95.2;
    X_Lower =  54.0;
    X_Upper =  56.0;
#endif

    // Which to plot?
    Int_t index = 0;   // Full scale magnetic field. 
    switch(index)
    {
    case 0:   // Absolute magnitude of field. 
	/*
	 * MX {-13:-14}
	 * MY {-51:-52}
	 * MZ {67.2:68.2}
	 * MAG {85.7:86.4}
	 */
	ABSMAG2D->Draw("SURF2");
	ABSMAG2D->GetYaxis()->SetTimeDisplay(1);
	ABSMAG2D->GetYaxis()->SetNdivisions(513);
	ABSMAG2D->GetYaxis()->SetTimeFormat("%H:%M:%S");
	ABSMAG2D->GetYaxis()->SetTimeOffset(0,"gmt");

	ABSMAG2D->SetXTitle("Day");
	ABSMAG2D->SetYTitle("Time");
	ABSMAG2D->SetZTitle("Total Field (uT)");
	ABSMAG2D->SetLabelSize(0.03,"X");
	ABSMAG2D->SetLabelSize(0.03,"Y");

	ABSMAG2D->SetMinimum(Z_Lower);
	ABSMAG2D->SetMaximum(Z_Upper);

	// Some reason this doesn't work as expected. 
	//ABSMAG2D->GetXaxis()->SetLimits(X_Lower, X_Upper);
	break;
    case 1:   // High res 2D but Z only. 
	Z2D->Draw("SURF2");
	Z2D->GetYaxis()->SetTimeDisplay(1);
	Z2D->GetYaxis()->SetNdivisions(513);
	Z2D->GetYaxis()->SetTimeFormat("%H:%M:%S");
	Z2D->GetYaxis()->SetTimeOffset(0,"gmt");

	Z2D->SetXTitle("Day");
	Z2D->SetYTitle("Time");
	Z2D->SetZTitle("Total Field (uT)");
	Z2D->SetLabelSize(0.03,"X");
	Z2D->SetLabelSize(0.03,"Y");
	Z2D->SetMinimum(65.0);
	Z2D->SetMaximum(67.0);
	break;
    case 2:   // Z Index
	KINDEX->Draw("SURF2");
	KINDEX->GetYaxis()->SetTimeDisplay(1);
	KINDEX->GetYaxis()->SetNdivisions(513);
	KINDEX->GetYaxis()->SetTimeFormat("%H:%M:%S");
	KINDEX->GetYaxis()->SetTimeOffset(0,"gmt");

	KINDEX->SetXTitle("Day");
	KINDEX->SetYTitle("Time");
	KINDEX->SetZTitle("Total Field (uT)");
	KINDEX->SetLabelSize(0.03,"X");
	KINDEX->SetLabelSize(0.03,"Y");
	KINDEX->SetMinimum(2.65);
	KINDEX->SetMaximum(2.72);
	break;
    }

}
