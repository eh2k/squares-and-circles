//faust ./djembe.dsp -cn djembe > ./djembe.hxx

import("stdfaust.lib");

freq = hslider("Freq",0.5,0,1000,0.01):si.smoo;
strikePosition = hslider("Pos",0.5,0,1,0.00001):si.smoo;
strikeSharpness = hslider("Sharpness",0.5,0,5,0.00001):si.smoo;
gain = 3;
trigger = button("trigger");

process = pm.djembe(freq,strikePosition,strikeSharpness,gain,trigger);
