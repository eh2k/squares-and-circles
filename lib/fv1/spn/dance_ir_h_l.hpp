static void program(FV1 *fv1)
{
    fv1->RDAX(16, 16768768);     // rdax	pot0,1.999	;get pot0, clip the upper half of pot0's range.
    fv1->WRAX(34, 0);            // wrax	kmix,0		;write the output mix value
    fv1->RDAX(16, -8388608);     // rdax	pot0,-1		;get pot0 again, 0 to -1
    fv1->SOF(8388608, 8380160);  // sof	1,0.999		;now +1 to 0
    fv1->SOF(16768768, 0);       // sof	1.999,0		;now +1 until midpint, then decreases to 0
    fv1->WRAX(33, 0);            // wrax	kin,0		;write the input attenuator value
    fv1->RDAX(16, 8388608);      // rdax	pot0,1		;get pot0 again
    fv1->WRAX(32, 8388608);      // wrax	krt,1		;save in krt, keep in ACC
    fv1->SOF(8388608, -4194304); // sof	1,-0.5		;subtract 1/2
    if (fv1->SKP(2, 2))
        goto R012;        // skp	gez,2		;skp if pot is in upper half of range
    fv1->SOF(0, 4194304); // sof	0,0.5		;load accumulator with +0.5
    fv1->WRAX(32, 0);     // wrax	krt,0		;overwrite if pot is in lower half of range
R012:
    fv1->AND(0x0);               // clr
    fv1->RDAX(17, 8388608);      // rdax	pot1,1		;get pot1
    fv1->SOF(4194304, -4194304); // sof	0.5,-0.5	;ranges -0.5 to 0
    fv1->EXP(8388608, 0);        // exp	1,0
    fv1->WRAX(43, 0);            // wrax	kfh,0		;write to HP filter control
    fv1->RDAX(18, 8388608);      // rdax	pot2,1		;get pot2
    fv1->SOF(4194304, -4194304); // sof	0.5,-0.5	;ranges -0.5 to 0
    fv1->EXP(8388608, 0);        // exp	1,0
    fv1->WRAX(44, 0);            // wrax	kfl,0		;write to LP filter control
    fv1->RDAX(17, -8388608);     // rdax	pot1,-1
    fv1->SOF(8388608, 8380160);  // sof	1,0.999		;ranges +1 to 0
    fv1->WRAX(45, 8388608);      // wrax	temp,1
    fv1->MULX(45);               // mulx	temp
    fv1->MULX(45);               // mulx	temp
    fv1->WRAX(50, 0);            // wrax	hbyp,0
    fv1->RDAX(18, 8388608);      // rdax	pot2,1		;read pot2 (LP) again
    fv1->MULX(18);               // mulx 	pot2
    fv1->MULX(18);               // mulx	pot2
    fv1->MULX(18);               // mulx	pot2
    fv1->WRAX(51, 0);            // wrax	lbyp,0
    fv1->RDAX(20, 2097152);      // rdax	adcl,0.25
    fv1->RDAX(21, 2097152);      // rdax	adcr,0.25	;get inputs, leave headroom
    fv1->MULX(33);               // mulx	kin		;scale by input attenuator
    fv1->RDA(202, 5029888);      // rda	ap1#,kap	;4 all passes:
    fv1->WRAP(0, -5038080);      // wrap	ap1,-kap
    fv1->RDA(744, 5029888);      // rda	ap2#,kap
    fv1->WRAP(203, -5038080);    // wrap	ap2,-kap
    fv1->RDA(1902, 5029888);     // rda	ap3#,kap
    fv1->WRAP(745, -5038080);    // wrap	ap3,-kap
    fv1->RDA(3806, 5029888);     // rda	ap4#,kap
    fv1->WRAP(1903, -5038080);   // wrap	ap4,-kap
    fv1->WRAX(45, 0);            // wrax	temp,0		;write ap output to temp reg
    fv1->RDA(26831, 8388608);    // rda	del2#,1
    fv1->MULX(32);               // mulx	krt
    fv1->RDAX(45, 8388608);      // rdax	temp,1
    fv1->RDA(6011, 5029888);     // rda	dap1a#,kap
    fv1->WRAP(3807, -5038080);   // wrap	dap1a,-kap
    fv1->RDA(9313, 5029888);     // rda	dap1b#,kap
    fv1->WRAP(6012, -5038080);   // wrap	dap1b,-kap
    fv1->WRA(9314, 0);           // wra	del1,0
    fv1->RDA(13770, 8388608);    // rda	del1#,1
    fv1->MULX(32);               // mulx	krt
    fv1->RDAX(45, 8388608);      // rdax	temp,1
    fv1->RDA(17303, 5029888);    // rda	dap2a#,kap
    fv1->WRAP(13771, -5038080);  // wrap	dap2a,-kap
    fv1->RDA(20505, 5029888);    // rda	dap2b#,kap
    fv1->WRAP(17304, -5038080);  // wrap	dap2b,-kap
    fv1->WRA(20506, 0);          // wra	del2,0
    fv1->RDAX(20, -8388608);     // rdax	adcl,-1
    fv1->RDA(9314, 12582912);    // rda	del1,1.5
    fv1->MULX(16);               // mulx	pot0
    fv1->RDAX(20, 8388608);      // rdax	adcl,1
    fv1->WRAX(46, 0);            // wrax	rmixl,0
    fv1->RDAX(21, -8388608);     // rdax	adcr,-1
    fv1->RDA(20506, 12582912);   // rda	del2,1.5
    fv1->MULX(16);               // mulx	pot0
    fv1->RDAX(21, 8388608);      // rdax	adcr,1
    fv1->WRAX(47, 0);            // wrax	rmixr,0
    fv1->RDAX(35, 8388608);      // rdax	hpal,1
    fv1->MULX(43);               // mulx	kfh
    fv1->RDAX(36, 8388608);      // rdax	hpbl,1
    fv1->WRAX(36, -8388608);     // wrax	hpbl,-1
    fv1->RDAX(35, -1677824);     // rdax	hpal,kqh
    fv1->RDAX(46, 8388608);      // rdax	rmixl,1
    fv1->WRAX(48, 8388608);      // wrax	hpoutl,1	;HP output
    fv1->MULX(43);               // mulx	kfh
    fv1->RDAX(35, 8388608);      // rdax	hpal,1
    fv1->WRAX(35, 0);            // wrax	hpal,0
    fv1->RDAX(39, 8388608);      // rdax	hpar,1
    fv1->MULX(43);               // mulx	kfh
    fv1->RDAX(40, 8388608);      // rdax	hpbr,1
    fv1->WRAX(40, -8388608);     // wrax	hpbr,-1
    fv1->RDAX(39, -1677824);     // rdax	hpar,kqh
    fv1->RDAX(47, 8388608);      // rdax	rmixr,1
    fv1->WRAX(49, 8388608);      // wrax	hpoutr,1	;HP output
    fv1->MULX(43);               // mulx	kfh
    fv1->RDAX(39, 8388608);      // rdax	hpar,1
    fv1->WRAX(39, 0);            // wrax	hpar,0
    fv1->RDAX(48, -8388608);     // rdax	hpoutl,-1
    fv1->RDAX(46, 8388608);      // rdax	rmixl,1
    fv1->MULX(50);               // mulx	hbyp
    fv1->RDAX(48, 8388608);      // rdax	hpoutl,1
    fv1->WRAX(48, 0);            // wrax	hpoutl,0
    fv1->RDAX(49, -8388608);     // rdax	hpoutr,-1
    fv1->RDAX(47, 8388608);      // rdax	rmixr,1
    fv1->MULX(50);               // mulx	hbyp
    fv1->RDAX(49, 8388608);      // rdax	hpoutr,1
    fv1->WRAX(49, 0);            // wrax	hpoutr,0
    fv1->RDAX(37, 8388608);      // rdax	lpal,1
    fv1->MULX(44);               // mulx	kfl
    fv1->RDAX(38, 8388608);      // rdax	lpbl,1
    fv1->WRAX(38, -8388608);     // wrax	lpbl,-1
    fv1->RDAX(37, -1677824);     // rdax	lpal,kql
    fv1->RDAX(48, 8388608);      // rdax	hpoutl,1
    fv1->MULX(44);               // mulx	kfl
    fv1->RDAX(37, 8388608);      // rdax	lpal,1
    fv1->WRAX(37, 0);            // wrax	lpal,0
    fv1->RDAX(41, 8388608);      // rdax	lpar,1
    fv1->MULX(44);               // mulx	kfl
    fv1->RDAX(42, 8388608);      // rdax	lpbr,1
    fv1->WRAX(42, -8388608);     // wrax	lpbr,-1
    fv1->RDAX(41, -1677824);     // rdax	lpar,kql
    fv1->RDAX(49, 8388608);      // rdax	hpoutr,1
    fv1->MULX(44);               // mulx	kfl
    fv1->RDAX(41, 8388608);      // rdax	lpar,1
    fv1->WRAX(41, 0);            // wrax	lpar,0
    fv1->RDAX(38, -8388608);     // rdax	lpbl,-1
    fv1->RDAX(48, 8388608);      // rdax	hpoutl,1
    fv1->MULX(51);               // mulx	lbyp
    fv1->RDAX(38, 8388608);      // rdax	lpbl,1
    fv1->WRAX(22, 0);            // wrax	dacl,0
    fv1->RDAX(42, -8388608);     // rdax	lpbr,-1
    fv1->RDAX(49, 8388608);      // rdax	hpoutr,1
    fv1->MULX(51);               // mulx	lbyp
    fv1->RDAX(42, 8388608);      // rdax	lpbr,1
    fv1->WRAX(23, 0);            // wrax	dacr,0
}
