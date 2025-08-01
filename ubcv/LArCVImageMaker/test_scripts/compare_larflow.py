import os,sys
import ROOT as rt
from larcv import larcv

rt.gStyle.SetOptStat(0)

types = ["orig","reduced"]

files = {"orig":"larcv_larflow_origtest.root",
         "reduced":"larcv.root"}

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

runN = nentries["orig"]
if nentries["reduced"]<runN:
    runN = nentries["reduced"]

NROWS=7
#for ientry in range(runN):
for ientry in range(2,3):

    print("ENTRY %02d"%(ientry))

    ev_larflow_v = {}
    for ftype in types:
        iomans[ftype].read_entry(ientry)
        ev_larflow_v[ftype] = iomans[ftype].get_data( larcv.kProductImage2D, "larflow" )

    ev_img = iomans["reduced"].get_data( larcv.kProductImage2D, "wire" )

    event_hists = {}
    entry_canvas = rt.TCanvas("hcanvas_entry%02d"%(ientry),"",600*3,400*NROWS)
    entry_canvas.Divide(3,NROWS)

    for iimg in range(3):
        img = ev_img.at(iimg)
        himg = rt.TH2D("himg_entry%02d_%02d"%(ientry,iimg),"ADC %d"%(iimg),3456,0,3456,1008,0,1008)
        for irow in range(img.meta().rows()):
            for icol in range(img.meta().cols()):
                pixval = img.pixel(irow,icol)
                himg.SetBinContent( icol+1, irow+1, pixval )
        event_hists["himg_entry%02d_%02d"%(ientry,iimg)] = himg
        entry_canvas.cd( iimg+1 )
        himg.Draw("colz")

    entry_canvas.Update()
    
    if NROWS==1:        
        entry_canvas.Draw()
        entry_canvas.SaveAs("compare_larflow_entry%02d.png"%(ientry))
        entry_canvas.Write()
        break

    vdiff = [0,0,0,0,0,0]

    for iimg in range(NROWS-1):
        horig    = rt.TH2D("horig_%02d"%(iimg),"larflow-original[%d]"%(iimg),3456,0,3456,1008,0,1008)
        hreduced = rt.TH2D("hreduced_%02d"%(iimg),"larflow-reduced[%d]"%(iimg),3456,0,3456,1008,0,1008)
        hdiff    = rt.TH2D("hdiff_%02d"%(iimg),"diff[%d]"%(iimg),3456,0,3456,1008,0,1008)
        
        orig    = ev_larflow_v["orig"].at(iimg)
        reduced = ev_larflow_v["reduced"].at(iimg)

        adc = ev_img.at( int(iimg/2) )
        
        #print("  [",iimg,"] ",orig.meta().dump())

        for irow in range(orig.meta().rows()):
            for icol in range(orig.meta().cols()):
                xorig    = orig.pixel(irow,icol)
                xreduced = reduced.pixel(irow,icol)
                xdiff = xreduced-xorig
                pixval = adc.pixel(irow,icol)
                horig.SetBinContent(    icol+1, irow+1, xorig )
                hreduced.SetBinContent( icol+1, irow+1, xreduced)
                hdiff.SetBinContent(    icol+1, irow+1, xdiff )

                if pixval>10.0:
                    #if xorig>-3999:
                    if xdiff*xdiff>0.1:
                        print(f"({icol},{irow}) pixval={pixval} orig={xorig} red={xreduced} diff={xdiff}")
                        vdiff[iimg] += 1


        entry_canvas.cd(3*(iimg+1)+1)
        #horig.GetZaxis().SetRangeUser(-200,200)
        horig.Draw("colz")
        entry_canvas.cd(3*(iimg+1)+2)
        #hreduced.GetZaxis().SetRangeUser(-200,200)
        hreduced.Draw("colz")
        entry_canvas.cd(3*(iimg+1)+3)
        #hdiff.GetZaxis().SetRangeUser(-200,200)
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
