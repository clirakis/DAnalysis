{
    TCanvas *Hobbes = new TCanvas("Dist","A test",5,5,1200,600);
    Hobbes->cd();
    TPad    *Calvin = new TPad("Calvin","Silly",0.02,0.02,0.99,0.99, 33);
    Calvin->Draw();
    Calvin->cd();
    Calvin->SetGrid();

    TFile *tf = new TFile("Sunspots.root");

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
}
