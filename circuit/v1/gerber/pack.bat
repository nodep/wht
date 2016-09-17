md order

copy /y wht-B_Cu.gbl        order\wht.GBL
copy /y wht-B_Mask.gbs      order\wht.GBS
copy /y wht-B_SilkS.gbo     order\wht.GBO
copy /y wht-Edge_Cuts.gbr   order\wht.GKO
copy /y wht-F_Cu.gtl        order\wht.GTL
copy /y wht-F_Mask.gts      order\wht.GTS
copy /y wht-F_SilkS.gto     order\wht.GTO
copy /y wht.drl             order\wht.TXT

cd order

7za a -tzip WHTorder *.*
