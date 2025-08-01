import os,sys
import ROOT as rt
from larcv import larcv

rt.gStyle.SetOptStat(0)

types = ["orig","reduced"]

files = {"orig":"larcv_set5_larflow.root",
         "reduced":"larcv_set5_larflow_fillcompressed.root"}

"""
We compare the images, pixel-by-pixel, indicating where any differences are.
We also visualize the images.
"""


iomans = {}
nentries = {}
for ftype in types:
    iomans[ftype] = larcv.IOManager(larcv.IOManager.kREAD, ftype, larcv.IOManager.kTickForward )
    iomans[ftype].add_in_file( files[ftype] )
    iomans[ftype].set_verbosity(1)
    iomans[ftype].initialize()
    nentries[ftype] = iomans[ftype].get_n_entries()

fout = rt.TFile("output_compare_larflow.root","recreate")

for ientry in range(nentries["orig"]):

    print("ENTRY %02d"%(ientry))

    ev_larflow_v = {}
    for ftype in types:
        iomans[ftype].read_entry(ientry)
        ev_larflow_v[ftype] = iomans[ftype].get_data( larcv.kProductImage2D, "larflow" )

    event_hists = {}
    entry_canvas = rt.TCanvas("hcanvas_entry%02d"%(ientry),"",600*3,400*6)
    entry_canvas.Divide(3,6)

    vdiff = [0,0,0,0,0,0]

    for iimg in range(6):
        horig    = rt.TH2D("horig_%02d"%(iimg),"",3456,0,3456,1008,0,1008)
        hreduced = rt.TH2D("hreduced_%02d"%(iimg),"",3456,0,3456,1008,0,1008)
        hdiff    = rt.TH2D("hdiff_%02d"%(iimg),"",3456,0,3456,1008,0,1008)
        
        orig    = ev_larflow_v[ftype].at(iimg)
        reduced = ev_larflow_v[ftype].at(iimg)
        
        #print("  [",iimg,"] ",orig.meta().dump())

        for irow in range(orig.meta().rows()):
            for icol in range(orig.meta().cols()):
                xorig    = orig.pixel(irow,icol)
                xreduced = reduced.pixel(irow,icol)
                xdiff = xreduced-xorig
                if xorig>-999:
                    horig.SetBinContent(    icol+1, irow+1, xorig )
                if xreduced>-999:
                    hreduced.SetBinContent( icol+1, irow+1, xreduced)
                hdiff.SetBinContent(    icol+1, irow+1, xdiff )
                if xdiff!=0:
                    vdiff[iimg] += 1


        entry_canvas.cd(3*iimg+1)
        horig.Draw("colz")
        entry_canvas.cd(3*iimg+2)
        hreduced.Draw("colz")
        entry_canvas.cd(3*iimg+3)
        hdiff.Draw("colz")

        event_hists["orig_%02d_%02d"%(ientry,iimg)] = horig
        event_hists["reduced_%02d_%02d"%(ientry,iimg)] = hreduced
        event_hists["diff_%02d_%02d"%(ientry,iimg)] = hdiff

    print("entry %02d: diff = "%(ientry),vdiff)
    entry_canvas.Update()
    entry_canvas.Draw()
    entry_canvas.SaveAs("compare_larflow_entry%02d.png"%(ientry))
    entry_canvas.Write()
    if False:
        #print("[enter] to continue")
        input()
    



print("[enter] to exit")
input()
