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

    //ABSMAG2D->SetMinimum(83.0);
    //ABSMAG2D->SetMaximum(86.5);

    //ABSMAG2D->GetZaxis()->SetLimits(85.0, 88.0);

}
