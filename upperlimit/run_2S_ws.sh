#!/bin/bash

# dir
dir=ws_tight_20150313/
nosyst="false"

# pt fits
dir2=$dir/ptFits
for cat in dimuPt000500 dimuPt12002000 dimuPt5001200; do 
   ppfile=`ls $dir2/WS*pp*$cat*root`
   pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
   nice root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}_$nosyst.root\"\,$nosyst\)
   mv c1.pdf $dir2/c1_${cat}_$nosyst.pdf
   mv c2.pdf $dir2/c2_${cat}_$nosyst.pdf
   mv cpoi.pdf $dir2/cpoi_${cat}_$nosyst.pdf
done

# rapidity fits
dir2=$dir/rapidityFits
for cat in dimuY000120 dimuY120240; do 
   ppfile=`ls $dir2/WS*pp*$cat*root`
   pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
   nice root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}_$nosyst.root\"\,$nosyst\)
   mv c1.pdf $dir2/c1_${cat}_$nosyst.pdf
   mv c2.pdf $dir2/c2_${cat}_$nosyst.pdf
   mv cpoi.pdf $dir2/cpoi_${cat}_$nosyst.pdf
done

# # centrality fits
# dir2=$dir/centralityFits
# for cat in cent0M5 cent5M10 cent10M20 cent20M30 cent30M40 cent40M50 cent50M100; do 
#    ppfile=`ls $dir2/WS*pp*root`
#    pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
#    nice root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}_$nosyst.root\"\,$nosyst\)
#    mv c1.pdf $dir2/c1_${cat}_$nosyst.pdf
#    mv c2.pdf $dir2/c2_${cat}_$nosyst.pdf
#    mv cpoi.pdf $dir2/cpoi_${cat}_$nosyst.pdf
# done
