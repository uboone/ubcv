#!/bin/bash

echo "executing run script from cvmfs lantern container"

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/1.3.2/bin/apptainer exec --cleanenv $LANTERN_CONTAINER_PATH /cluster/home/lantern_scripts/run_lantern_workflow_data.sh

echo "Container exited with code $?"

echo "Done!"
