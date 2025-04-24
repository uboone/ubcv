#!/bin/bash

echo "executing run script from cvmfs lantern container"

SCRIPT_DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
/cvmfs/oasis.opensciencegrid.org/mis/apptainer/1.3.2/bin/apptainer exec --cleanenv /cvmfs/uboone.opensciencegrid.org/containers/lantern_v2_me_06_03_prod/ "$SCRIPT_DIR/run_lantern_workflow_data.sh"

echo "Container exited with code $?"

echo "Done!"
