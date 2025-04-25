#!/bin/bash

echo "executing run script from cvmfs lantern container"

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/1.3.2/bin/apptainer exec --cleanenv /cvmfs/uboone.opensciencegrid.org/containers/lantern_v2_me_06_03_prod/ /cluster/home/lantern_scripts/run_lantern_workflow_mc.sh

echo "Container exited with code $?"

echo "Done!"
