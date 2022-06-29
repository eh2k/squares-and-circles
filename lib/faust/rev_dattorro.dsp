//faust ./rev_dattorro.dsp -cn rev_dattorro > ./rev_dattorro.hxx

import("stdfaust.lib");

dw = hslider("[0]D/W",0.5,0,1,0.0001):si.smoo;
decay = hslider("[1]Decay",0.5,0,1,0.0001):si.smoo;
damping = hslider("[2]Damp",0.5,0,1,0.0001):si.smoo;
bw = hslider("[3]BW",0.5,0,1,0.0001):si.smoo;
diffd= hslider("[4]DDiff",0.5,0.01,0.99,0.0001):si.smoo;
diffi = hslider("[5]IDiff",0.5,0.01,0.99,0.0001):si.smoo;
pre_delay = 24;

process = _,_ <: (_,_ <:re.dattorro_rev(pre_delay, bw, diffi, diffi, decay, diffd, diffd, damping) : (*(dw),*(dw))), *(1-dw), *(1-dw) :> _,_;
