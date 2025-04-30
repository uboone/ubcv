#!/bin/bash

echo "executing run script from cvmfs lantern container"

/cvmfs/oasis.opensciencegrid.org/mis/apptainer/1.3.2/bin/apptainer exec --cleanenv /cvmfs/uboone.opensciencegrid.org/containers/lantern_v2_me_06_03_prod/ /cluster/home/lantern_scripts/run_lantern_workflow_data.sh

echo "Container exited with code $?"

echo "Merging lantern ntuple into unified ntuple"

unified_ntuple=$(find . -maxdepth 1 -type f -name 'reco_stage_2_hist*.root' -printf '%f\n' -quit)

if [[ -f "$unified_ntuple" && -f "flat_ntuple.root" ]]; then
    #make simple root macro to merge ntuples
    cat <<EOF > copy_tree.C
void copy_tree(const char* sourceFile, const char* destFile) {
    TFile *source = TFile::Open(sourceFile);
    TTree *tree = (TTree*)source->Get("EventTree");
    TFile *target = TFile::Open(destFile, "UPDATE");
    target->mkdir("lantern");
    target->cd("lantern");
    tree->CloneTree()->Write("EventTree");
    target->Close();
    source->Close();
}
EOF
    #run merge macro
    root -l -b -q "copy_tree.C(\"flat_ntuple.root\", \"$unified_ntuple\")"
    #clean up
    rm copy_tree.C
else
    echo "Could not find ntuple files to merge!!"
fi

echo "Done!"
