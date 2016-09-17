md order

copy /y wht_v3-B_Cu.gbl        order\wht_v3.GBL
copy /y wht_v3-B_Mask.gbs      order\wht_v3.GBS
copy /y wht_v3-B_SilkS.gbo     order\wht_v3.GBO
copy /y wht_v3-Edge_Cuts.gbr   order\wht_v3.GKO
copy /y wht_v3-F_Cu.gtl        order\wht_v3.GTL
copy /y wht_v3-F_Mask.gts      order\wht_v3.GTS
copy /y wht_v3-F_SilkS.gto     order\wht_v3.GTO
copy /y wht_v3.drl             order\wht_v3.TXT

cd order

7za a -tzip wht_v3 *.*
