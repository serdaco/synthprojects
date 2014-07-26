Synthprojects
=============

Some synth projects, using arduino


Bicycle drummer
===============

Uses an arduino nano, a dreamblaster s1 synth module, and a battery powered speaker :  It plays a rock drum pattern, synchronized to the tempo of the bicycle wheel. 

How it works : the Arduino measures the period of the reed switch pulse, and adjusts the tempo of a drum sequence accordingly.  This drum sequence is sent by TTL level midi commands to a dreamblaster S1 module. The S1 is a low power waveblaster compatible module, it normally uses 5V but can be powered with 9V just as well. If the cycle period grows longer than 5 seconds, the sequence is stopped. As soon as a new period is detected the sequence is started and/or the tempo is adjusted. 


Drone Beat
===============

Arduino example code : drum and bass sequence with potmeter resonant filter control 


Arduino Nano Dreamblaster S1 hardware shield
===============

Kicad project files for the Arduino Nano shield from http://www.serdashop.com/ArduinoNanoSynthShield
The Kicad project uses the excellent component library from Walter Lain : http://smisioto.no-ip.org/elettronica/kicad/kicad-en.htm
