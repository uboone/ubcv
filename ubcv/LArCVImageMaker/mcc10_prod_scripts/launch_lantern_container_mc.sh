#!/bin/bash

echo "executing run script from cvmfs lantern container"

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/1.3.2/bin/apptainer exec --cleanenv $LANTERN_CONTAINER_PATH /cluster/home/lantern_scripts/run_lantern_workflow_mc.sh

echo "Container exited with code $?"

echo "Checking output lantern ntuple"

#make simple root macro to validate lantern ntuple
cat << 'EOF' > check_lantern.C
void check_lantern() {
    TFile *f = TFile::Open("flat_ntuple.root");
    if (!f || f->IsZombie()) {
        std::cerr << "lantern ntuple is missing or corrupted" << std::endl;
        gSystem->Exit(1);
    }
    TTree *tree = (TTree*)f->Get("EventTree");
    if (!tree) {
        std::cerr << "EventTree not found in lantern ntuple" << std::endl;
        gSystem->Exit(1);
    }
    f->Close();
    gSystem->Exit(0);
}
EOF

#run check lantern macro
root -l -b -q check_lantern.C
status=$?
#clean up
rm check_lantern.C

#force job crash if lantern ntuple is missing/corrupted
if [ $status -ne 0 ]; then
    echo "detected lantern ntuple file error; exiting with code 1"
    exit 1
else
    echo "lantern ntuple file validation passed"
fi

echo "Done!"
