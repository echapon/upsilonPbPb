#!/bin/bash

# dir
dir=ws_2S_04mar2015/

# pt fits
dir2=$dir/ptFits
for cat in dimuPt000500 dimuPt12002000 dimuPt5001200; do 
   ppfile=`ls $dir2/WS*pp*$cat*root`
   pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
   root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}.root\"\)
   mv c1.pdf $dir2/c1_${cat}.pdf
   mv c2.pdf $dir2/c2_${cat}.pdf
   mv cpoi.pdf $dir2/cpoi_${cat}.pdf
done

# rapidity fits
dir2=$dir/rapidityFits
for cat in dimuY000120 dimuY120240; do 
   ppfile=`ls $dir2/WS*pp*$cat*root`
   pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
   root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}.root\"\)
   mv c1.pdf $dir2/c1_${cat}.pdf
   mv c2.pdf $dir2/c2_${cat}.pdf
   mv cpoi.pdf $dir2/cpoi_${cat}.pdf
done

# rapidity fits
dir2=$dir/centralityFits
for cat in cent0M5 cent5M10 cent10M20 cent20M30 cent30M40 cent40M50 cent50M100; do 
   ppfile=`ls $dir2/WS*pp*root`
   pbpbfile=`ls $dir2/WS*pbpb*$cat*root`
   root -l -b -q Raa2S_Workspace.C\(\"${pbpbfile}\"\,\"${ppfile}\"\,\"$dir2/WS_combo2S_${cat}.root\"\)
   mv c1.pdf $dir2/c1_${cat}.pdf
   mv c2.pdf $dir2/c2_${cat}.pdf
   mv cpoi.pdf $dir2/cpoi_${cat}.pdf
done
