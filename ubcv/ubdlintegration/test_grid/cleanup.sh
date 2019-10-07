#!/bin/bash

for weightfile in mcc8_mrcnn_plane*.pt; do
    if [ -f $weightfile ]; then
        rm $weightfile
    fi
done

for weightfile in sparseinfill_*plane_test.tar; do
    if [ -f $weightfile ]; then
        rm $weightfile
    fi
done

#rm mcc8_mrcnn_plane*.pt
#rm sparseinfill_*plane_test.tar
