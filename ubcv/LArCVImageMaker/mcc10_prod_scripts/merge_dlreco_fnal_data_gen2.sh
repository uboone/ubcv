#!/bin/bash


echo "<< FILES available >> "
ls -lh

# OUTPUT FILES FROM PREVIOUS STAGE:
#larcv.root
#larlite_opreco.root

echo "<<< Append UBDL Products >>>"
mv larlite_opreco.root merged_dlreco.root
python $UBDL_BASEDIR/fnal_prod/append_supera_products_data.py merged_dlreco.root larcv.root

echo "<<< cleanup excess root files >>>"
rm -f larcv.root larcv_hist.root

