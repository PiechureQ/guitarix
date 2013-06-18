declare id "pre 12AU7/ push-pull 6V6"; // in amp tube selector
declare name "pre 12AU7/ push-pull 6V6";

import("music.lib");
import("filter.lib");
import("effect.lib"); 
import("guitarix.lib");
import("maxmsp.lib");

/****************************************************************
 ** Tube Preamp Emulation stage 1 - 2 
 *   12AU7 -> pusch pull 6V6
 */


peak1 = allpassn(4,(-0.2, 0.3, 0.4, 0.5));



gx_drive(drive) = _ <: _ + nonlin(4,4,0.125) * drive * 10 ;

amp = hgroup("stage1", stage1) : 
          component("gxdistortion.dsp").dist2(drive,wet_dry) : 
          hgroup("stage2", stage2)  
          with {
    drive = ampctrl.drive;
    wet_dry = ampctrl.wet_dry;
    preamp = ampctrl.preamp;
/*
    drive = vslider(".gxdistortion.drive[alias]",0.35, 0, 1, 0.01);
    wet_dry = vslider(".gxdistortion.wet_dry[alias]",  100, 0, 100, 1) : /(100) : smoothi(0.999);
    preamp =  vslider(".amp2.stage1.Pregain[alias]",0,-20,20,0.1) : db2linear : smoothi(0.999);
*/
    atten = 0.6;
    stage1 = speakerbp(310.0, 12000.0) :
     *(preamp):*(2.0): (tubestage130_10(TB_12AU7_68k,86.0,2700.0,1.257240) : + ~ (atten*tubestage130_10(TB_12AU7_250k,132.0,1500.0,0.776162))):
    lowpass(1,6531.0) : (tubestage130_10(TB_12AU7_250k,132.0,1500.0,0.776162) : + ~ (atten*tubestage130_10(TB_12AU7_250k,194.0,820.0,0.445487))) : tubestage130_10(TB_12AU7_250k,194.0,820.0,0.445487); 
    stage2 = lowpass(1,6531.0) : *(gain1)  <: ((min(0.7,tubestage(TB_6V6_250k,6531.0,410.0,0.659761))),(max(-0.75,tubestage(TB_6V6_68k,6531.0,410.0,0.664541)))) :> 
    peak1
    with {
        gain1 = ampctrl.gain1;
    /*
        gain1 = vslider(".amp2.stage2.gain1[alias]", 6, -20.0, 20.0, 0.1) : db2linear : smoothi(0.999);
    */
    };
};

process = amp,amp;

