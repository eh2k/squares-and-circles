static void program(FV1 *fv1)
{
    if (fv1->SKP(16, 1))
        goto R002;      // skp	run,1
    fv1->WLDR(0, 0, 0); // wldr	rmp0,0,4096		;initialize predelay
R002:
    fv1->RDAX(20, 2097152);      // rdax	adcl,0.25		;put inputs into predelay
    fv1->RDAX(21, 2097152);      // rdax	adcr,0.25		;headroom space
    fv1->WRA(0, 0);              // wra	pdel,0
    fv1->CHO_RDA(2, 6, 0);       // cho	rda,rmp0,reg|compc,pdel	;get outputs from predelay, interpolated
    fv1->CHO_RDA(2, 0, 1);       // cho	rda,rmp0,0,pdel+1
    fv1->WRA(4101, 0);           // wra	gdel,0			;write predelay output to gate delay memory
    fv1->CHO_RDAL(2);            // cho	rdal,rmp0		;read current predelay pointer
    fv1->RDAX(17, -4194304);     // rdax	pot1,-0.5		;subtract pot for servo control of pointer
    fv1->WRAX(4, 0);             // wrax	rmp0_rate,0		;maintain predelay pointer
    fv1->RDAX(16, 8388608);      // rdax	pot0,1
    fv1->AND(0xf00000);          // and	%01111000_000000000_00000000	;mask upper 4 bits
    fv1->SOF(8388608, -7864320); // sof	1,-15/16
    if (fv1->SKP(2, 38))
        goto R053;             // skp	gez,p1
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 37))
        goto R054;             // skp	gez,p2
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 36))
        goto R055;             // skp	gez,p3
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 35))
        goto R056;             // skp	gez,p4
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 34))
        goto R057;             // skp	gez,p5
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 33))
        goto R058;             // skp	gez,p6
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 32))
        goto R059;             // skp	gez,p7
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 31))
        goto R060;             // skp	gez,p8
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 30))
        goto R061;             // skp	gez,p9
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 29))
        goto R062;             // skp	gez,p10
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 28))
        goto R063;             // skp	gez,p11
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 27))
        goto R064;             // skp	gez,p12
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 26))
        goto R065;             // skp	gez,p13
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 25))
        goto R066;             // skp	gez,p14
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 24))
        goto R067;             // skp	gez,p15
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 23))
        goto R068;             // skp	gez,p16
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 22))
        goto R069;             // skp	gez,p17
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 21))
        goto R070;             // skp	gez,p18
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 20))
        goto R071;             // skp	gez,p19
    fv1->SOF(8388608, 524288); // sof	1,1/16
    if (fv1->SKP(2, 19))
        goto R072; // skp	gez,p20
R053:
    fv1->RDA(18435, 3350528); // rda	gdel+14334,0.4
R054:
    fv1->RDA(18124, 4194304); // rda	gdel+14023,0.5
R055:
    fv1->RDA(17609, 3350528); // rda	gdel+13508,0.4
R056:
    fv1->RDA(17202, 4194304); // rda	gdel+13101,0.5
R057:
    fv1->RDA(16861, 4194304); // rda	gdel+12760,0.5
R058:
    fv1->RDA(16221, 3350528); // rda	gdel+12120,0.4
R059:
    fv1->RDA(15866, 4194304); // rda	gdel+11765,0.5
R060:
    fv1->RDA(15413, 4194304); // rda	gdel+11312,0.5
R061:
    fv1->RDA(14851, 4194304); // rda	gdel+10750,0.5
R062:
    fv1->RDA(14313, 4194304); // rda	gdel+10212,0.5
R063:
    fv1->RDA(13806, 4194304); // rda	gdel+9705,0.5
R064:
    fv1->RDA(13468, 4194304); // rda	gdel+9367,0.5
R065:
    fv1->RDA(13006, 3350528); // rda	gdel+8905,0.4
R066:
    fv1->RDA(12676, 4194304); // rda	gdel+8575,0.5
R067:
    fv1->RDA(12316, 4194304); // rda	gdel+8215,0.5
R068:
    fv1->RDA(11513, 3350528); // rda	gdel+7412,0.4
R069:
    fv1->RDA(11245, 4194304); // rda	gdel+7144,0.5
R070:
    fv1->RDA(10614, 4194304); // rda	gdel+6513,0.5
R071:
    fv1->RDA(9923, 4194304); // rda	gdel+5822,0.5
R072:
    fv1->RDA(9421, 4194304);    // rda	gdel+5320,0.5
    fv1->RDA(9239, 4194304);    // rda	gdel+5138,0.5
    fv1->RDA(8677, 4194304);    // rda	gdel+4576,0.5
    fv1->RDA(8008, 3350528);    // rda	gdel+3907,0.4
    fv1->RDA(7521, 4194304);    // rda	gdel+3420,0.5
    fv1->RDA(7075, 4194304);    // rda	gdel+2974,0.5
    fv1->RDA(6631, 4194304);    // rda	gdel+2530,0.5
    fv1->RDA(6211, 4194304);    // rda	gdel+2110,0.5
    fv1->RDA(5441, 4194304);    // rda	gdel+1340,0.5
    fv1->RDA(5024, 4194304);    // rda	gdel+923,0.5
    fv1->RDA(4601, 4194304);    // rda	gdel+500,0.5
    fv1->RDA(4101, 5029888);    // rda	gdel,0.6
    fv1->WRAX(33, 8388608);     // wrax	gout,1
    fv1->RDA(19336, 5029888);   // rda	lap1#,kap		;spread outputs and smear with allpasses
    fv1->WRAP(19102, -5038080); // wrap	lap1,-kap
    fv1->RDA(19783, 5029888);   // rda	lap2#,kap
    fv1->WRAP(19337, -5038080); // wrap	lap2,-kap
    fv1->RDA(20336, 5029888);   // rda	lap3#,kap
    fv1->WRAP(19784, -5038080); // wrap	lap3,-kap
    fv1->WRAX(22, 0);           // wrax	dacl,0
    fv1->RDAX(33, 8388608);     // rdax	gout,1
    fv1->RDA(20538, 5029888);   // rda	rap1#,kap
    fv1->WRAP(20337, -5038080); // wrap	rap1,-kap
    fv1->RDA(20928, 5029888);   // rda	rap2#,kap
    fv1->WRAP(20539, -5038080); // wrap	rap2,-kap
    fv1->RDA(21556, 5029888);   // rda	rap3#,kap
    fv1->WRAP(20929, -5038080); // wrap	rap3,-kap
    fv1->WRAX(23, 0);           // wrax	dacr,0
    fv1->RDA(6601, -8388608);   // rda	gdel+2500,-1
    fv1->RDFX(34, 4194304);     // rdfx	fil1,0.5
    fv1->WRHX(34, -8388608);    // wrhx	fil1,-1
    fv1->MULX(18);              // mulx	pot2
    fv1->RDA(6601, 8388608);    // rda	gdel+2500,1
    fv1->WRA(6601, 0);          // wra	gdel+2500,0
    fv1->RDA(8601, -8388608);   // rda	gdel+4500,-1
    fv1->RDFX(35, 3355392);     // rdfx	fil2,0.4
    fv1->WRHX(35, -8388608);    // wrhx	fil2,-1
    fv1->MULX(18);              // mulx	pot2
    fv1->RDA(8601, 8388608);    // rda	gdel+4500,1
    fv1->WRA(8601, 0);          // wra	gdel+4500,0
    fv1->RDA(11101, -8388608);  // rda	gdel+7000,-1
    fv1->RDFX(34, 2516480);     // rdfx	fil1,0.3
    fv1->WRHX(34, -8388608);    // wrhx	fil1,-1
    fv1->MULX(18);              // mulx	pot2
    fv1->RDA(11101, 8388608);   // rda	gdel+7000,1
    fv1->WRA(11101, 0);         // wra	gdel+7000,0
    fv1->RDA(14101, -8388608);  // rda	gdel+10000,-1
    fv1->RDFX(34, 1677568);     // rdfx	fil1,0.2
    fv1->WRHX(34, -8388608);    // wrhx	fil1,-1
    fv1->MULX(18);              // mulx	pot2
    fv1->RDA(14101, 8388608);   // rda	gdel+10000,1
    fv1->WRA(14101, 0);         // wra	gdel+10000,0
}
