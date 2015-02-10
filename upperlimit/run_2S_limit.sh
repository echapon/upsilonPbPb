#!/bin/bash

for cat in dimuPt0005000_dimuY000120 dimuPt0005000_dimuY120240 dimuPt000500_dimuY000240 dimuPt12002000_dimuY000240 dimuPt5001200_dimuY000240; do 
   root -l -b -q runLimit_RaaNS_Workspace.C'("WS_combo2S_'${cat}'_fsr1_sigma0_ref2_fitres.root","raa2")'
   mv results.txt results_2S_${cat}.txt
   mv c1.pdf c1_2S_${cat}.pdf
   mv c1.root c1_2S_${cat}.root
   mv c2.pdf c2_2S_${cat}.pdf
   mv c2.root c2_2S_${cat}.root
done
