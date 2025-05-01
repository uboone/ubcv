#!/bin/bash


echo "<< FILES available >> "
ls -lh

# for debug: get file that goes into the dl portion of the workflow
#echo "COPY OUTPUT OF DLMERGED STAGE TO SCRATCH"
#ifdh cp -D Physics*_ubdl.root /pnfs/uboone/scratch/users/tmw/v10_04_07_05/intermediate_outputs/


# OUTPUT FILES FROM PREVIOUS STAGE:
#larcv.root
#larlite_opreco.root

echo "<<< Append UBDL Products >>>"
mv larlite_opreco.root merged_dlreco.root
python3 $UBDL_BASEDIR/fnal_prod/append_supera_products_data.py merged_dlreco.root larcv.root

echo "<<< cleanup excess root files >>>"
rm -f larcv.root larcv_hist.root

