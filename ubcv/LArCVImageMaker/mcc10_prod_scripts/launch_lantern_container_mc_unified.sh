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

echo "Merging lantern ntuple into unified ntuple"

unified_ntuple=$(find . -maxdepth 1 -type f -name 'reco_stage_2_hist*.root' -printf '%f\n' -quit)

if [[ -f "$unified_ntuple" && -f "flat_ntuple.root" ]]; then
    #make simple root macro to merge ntuples
    cat <<EOF > copy_tree.C
void copy_tree(const char* sourceFile, const char* destFile) {
    TFile *source = TFile::Open(sourceFile);
    TTree *tree = (TTree*)source->Get("EventTree");
    TTree *pottree = (TTree*)source->Get("potTree");
    TFile *target = TFile::Open(destFile, "UPDATE");
    target->mkdir("lantern");
    target->cd("lantern");
    tree->CloneTree()->Write("EventTree");
    pottree->CloneTree()->Write("potTree");
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
