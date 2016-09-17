md order

copy /y wht_v4-B.Cu.gbl        order\wht_v4.GBL
copy /y wht_v4-B.Mask.gbs      order\wht_v4.GBS
copy /y wht_v4-B.SilkS.gbo     order\wht_v4.GBO
copy /y wht_v4-Edge.Cuts.gm1   order\wht_v4.GKO
copy /y wht_v4-F.Cu.gtl        order\wht_v4.GTL
copy /y wht_v4-F.Mask.gts      order\wht_v4.GTS
copy /y wht_v4-F.SilkS.gto     order\wht_v4.GTO
copy /y wht_v4.drl             order\wht_v4.TXT

cd order

7za a -tzip wht_v4 *.*
