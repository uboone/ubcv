#!/bin/bash


echo "<< FILES available >> "
ls -lh

# OUTPUT FILES FROM PREVIOUS STAGE:
#larcv.root
#larlite_opreco.root
#larlite_mcinfo.root

# LARLITE FILES TO MERGE
# -----------------------
LARLITE_FILE_LIST="merged_dlreco.root larlite_opreco.root larlite_mcinfo.root"

echo "LARLITE FILES: ${LARLITE_FILE_LIST}"
echo ""

echo "<< combine larlite files >>"
python $UBDL_BASEDIR/fnal_prod/combine_larlite.py -o $LARLITE_FILE_LIST
echo "<<< Append UBDL Products >>>"
python $UBDL_BASEDIR/fnal_prod/append_supera_products_mc.py merged_dlreco.root larcv.root

echo "<<< cleanup excess root files >>>"
rm -f larlite_opreco.root larlite_mcinfo.root larcv.root larcv_hist.root

