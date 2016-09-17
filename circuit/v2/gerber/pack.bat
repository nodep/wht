md order

copy /y wht_v2-B_Cu.gbl        order\wht_v2.GBL
copy /y wht_v2-B_Mask.gbs      order\wht_v2.GBS
copy /y wht_v2-B_SilkS.gbo     order\wht_v2.GBO
copy /y wht_v2-Edge_Cuts.gbr   order\wht_v2.GKO
copy /y wht_v2-F_Cu.gtl        order\wht_v2.GTL
copy /y wht_v2-F_Mask.gts      order\wht_v2.GTS
copy /y wht_v2-F_SilkS.gto     order\wht_v2.GTO
copy /y wht_v2.drl             order\wht_v2.TXT

cd order

7za a -tzip WHTorder *.*
