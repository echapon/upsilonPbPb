#!/bin/bash

for cat in dimuPt0005000_dimuY000120 dimuPt0005000_dimuY120240 dimuPt000500_dimuY000240 dimuPt12002000_dimuY000240 dimuPt5001200_dimuY000240; do 
   root -l -b -q Raa2S_Workspace.C\(\"WS_pbpb_${cat}_fsr1_sigma0_ref2_fitres.root\"\,\"WS_pp_${cat}_fsr1_sigma0_ref2_fitres.root\"\,\"WS_combo2S_${cat}_fsr1_sigma0_ref2_fitres.root\"\)
done
