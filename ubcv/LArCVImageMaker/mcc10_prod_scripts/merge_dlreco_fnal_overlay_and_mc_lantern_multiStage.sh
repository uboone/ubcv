#!/bin/bash

# To be run after the output of
# - for overlay: mcc10_dlreco_w_wirecell_driver_overlay_lantern.fcl
# - for mc: mcc10_dlreco_w_wirecell_driver_mc_lantern.fcl

echo "<< FILES available >> "
ls -lh *.root

# (lantern dlreco)
#ubdlout=`ls -1 *_ubdl.root | tail -n 1`
#echo "COPY OUTPUT OF DLMERGED STAGE TO SCRATCH"
#ifdh cp -D Physics*_ubdl.root /pnfs/uboone/scratch/users/tmw/v10_04_07_05/intermediate_outputs/

# OUTPUT FILES FROM PREVIOUS STAGE:
#larcv.root
#larlite_opreco.root
#larlite_mcinfo.root

# opreco contents
#TFile**larlite_opreco.root
#TFile*larlite_opreco.root
#KEY: TTree larlite_id_tree;1LArLite Event ID Tree
#KEY: TTree daqheadertimeuboone_daq_tree;1daqheadertimeuboone Tree by daq
# KEY: TTree crthit_crthitcorr_tree;1crthit Tree by crthitcorr
# KEY: TTree crttrack_crttrack_tree;1crttrack Tree by crttrack
# KEY: TTree ophit_ophitBeam::DLDeploy_tree;1ophit Tree by ophitBeam::DLDeploy
# KEY: TTree ophit_ophitBeamCalib::DLDeploy_tree;1ophit Tree by ophitBeamCalib::DLDeploy
# KEY: TTree ophit_ophitBeamNoRemap::DLDeploy_tree;1ophit Tree by ophitBeamNoRemap::DLDeploy
# KEY: TTree ophit_ophitCosmicCalib::DLDeploy_tree;1ophit Tree by ophitCosmicCalib::DLDeploy
# KEY: TTree opflash_simpleFlashBeam_tree;1opflash Tree by simpleFlashBeam
# KEY: TTree opflash_simpleFlashBeamLowPE_tree;1opflash Tree by simpleFlashBeamLowPE
# KEY: TTree opflash_simpleFlashCosmic_tree;1opflash Tree by simpleFlashCosmic
# KEY: TTree opflash_wcopflash:beam:DLDeploy_tree;1opflash Tree by wcopflash:beam:DLDeploy
# KEY: TTree opflash_wcopflash:cosmic:DLDeploy_tree;1opflash Tree by wcopflash:cosmic:DLDeploy
# KEY: TTree trigger_triggersim_tree;1trigger Tree by triggersim
# KEY: TTree ass_simpleFlashBeam_tree;1ass Tree by simpleFlashBeam
# KEY: TTree ass_simpleFlashBeamLowPE_tree;1ass Tree by simpleFlashBeamLowPE
# KEY: TTree ass_simpleFlashCosmic_tree;1ass Tree by simpleFlashCosmic
# KEY: TTree ass_wcopflash:beam:DLDeploy_tree;1ass Tree by wcopflash:beam:DLDeploy
# KEY: TTree ass_wcopflash:cosmic:DLDeploy_tree;1ass Tree by wcopflash:cosmic:DLDeploy
# KEY: TTree opdigit_wcopflash:MergedBeam:DLDeploy_tree;1opdigit Tree by wcopflash:MergedBeam:DLDeploy
# KEY: TTree opdigit_wcopflash:MergedCosmic:DLDeploy_tree;1opdigit Tree by wcopflash:MergedCosmic:DLDeploy
# KEY: TTree swtrigger_swtrigger_tree;1swtrigger Tree by swtrigger

# larcv objects: "chstatus_wire_tree", "image2d_ancestor_tree", "image2d_instance_tree", "image2d_larflow_tree", "image2d_segment_tree", "image2d_thrumu_tree", "image2d_wire_tree", "partroi_segment_tree"
rm -f extract_opreco.root
rm -f extract_larcv.root

# We want to rename complicated names
shopt -s expand_aliases
xrootcp=`which rootcp` # have to do this because of broken env call at top of rootcp
python3 ${xrootcp} larlite_opreco.root:daqheadertimeuboone_daq_tree            extract_opreco.root
python3 ${xrootcp} larlite_opreco.root:swtrigger_swtrigger_tree                extract_opreco.root
python3 ${xrootcp} larlite_opreco.root:trigger_triggersim_tree                 extract_opreco.root
python3 ${xrootcp} larlite_opreco.root:crthit_crthitcorr_tree                  extract_opreco.root
python3 ${xrootcp} larlite_opreco.root:crttrack_crttrack_tree                  extract_opreco.root
#python3 ${xrootcp} larlite_opreco.root:opflash_simpleFlashBeam_tree   extract_opreco.root:opflash_simpleFlashBeam_tree
#python3 ${xrootcp} larlite_opreco.root:opflash_simpleFlashCosmic_tree extract_opreco.root:opflash_simpleFlashCosmic_tree
#python3 ${xrootcp} larlite_opreco.root:opflash_wcopflash*beam_tree    extract_opreco.root:opflash_wcopflashbeam_tree
#python3 ${xrootcp} larlite_opreco.root:opflash_wcopflash*cosmic_tree  extract_opreco.root:opflash_wcopflashcosmic_tree
#python3 ${xrootcp} larlite_opreco.root:opdigit_wcopflash*MergedBeam_tree   extract_opreco.root:opdigit_wcopflashbeam_tree
#python3 ${xrootcp} larlite_opreco.root:opdigit_wcopflash*MergedCosmic_tree extract_opreco.root:opdigit_wcopflashcosmic_tree
python3 ${xrootcp} larlite_opreco.root:opflash_*_tree   extract_opreco.root
python3 ${xrootcp} larlite_opreco.root:opdigit_*_tree   extract_opreco.root

# extract particular larcv tres
# KEY: TTreechstatus_wire_tree;1wire tree
# KEY: TTreeimage2d_ancestor_tree;1ancestor tree
# KEY: TTreeimage2d_instance_tree;1instance tree
# KEY: TTreeimage2d_larflow_tree;1larflow tree
# KEY: TTreeimage2d_segment_tree;1segment tree
# KEY: TTreeimage2d_thrumu_tree;1thrumu tree
# KEY: TTreeimage2d_wire_tree;1wire tree
# KEY: TTreepartroi_segment_tree;1segment tree

python3 ${xrootcp} larcv.root:image2d_wire_tree       extract_larcv.root
python3 ${xrootcp} larcv.root:image2d_thrumu_tree     extract_larcv.root
python3 ${xrootcp} larcv.root:chstatus_wire_tree      extract_larcv.root

python3 ${xrootcp} larcv_mc3.root:image2d_ancestor_tree   extract_larcv.root
python3 ${xrootcp} larcv_mc3.root:image2d_instance_tree   extract_larcv.root
python3 ${xrootcp} larcv_mc2.root:image2d_segment_tree    extract_larcv.root
python3 ${xrootcp} larcv_mc5.root:image2d_larflow_tree    extract_larcv.root
python3 ${xrootcp} larcv_mc4.root:partroi_segment_tree    extract_larcv.root

hadd merged_dlreco.root larlite_mcinfo.root extract_opreco.root extract_larcv.root

# LARLITE FILES TO MERGE
# -----------------------
# LARLITE_FILE_LIST="merged_dlreco.root larlite_opreco.root larlite_mcinfo.root"

# echo "LARLITE FILES: ${LARLITE_FILE_LIST}"
# echo ""

# echo "<< combine larlite files >>"
# python $UBDL_BASEDIR/fnal_prod/combine_larlite.py -o $LARLITE_FILE_LIST
# echo "<<< Append UBDL Products >>>"
# python $UBDL_BASEDIR/fnal_prod/append_supera_products_mc.py merged_dlreco.root larcv.root

echo "<<< cleanup excess root files >>>"
CMD="rm -f larlite_opreco.root larlite_mcinfo.root larcv.root larcv_mc2.root larcv_mc3.root larcv_mc4.root larcv_mc5.root larcv_hist.root extract_opreco.root extract_larcv.root"
echo $CMD
$CMD
