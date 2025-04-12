# Apogee Sound System for DJGPP

A DJGPP port of Apogee Sound System audio library (AudioLib) used in Build Engine based games such as Duke Nukem 3D, Rise of the Triades, Blood, Shadow Warrior, NAM, etc.

Can be built with CMake as a static library. Also can be used as a sub-directory to integrate with the target project build.

This version __doesn't__ (yet) supports AWE32, Gravis UltraSound, SoundScape, and Pro Audio Spectrum due to various problems such as:
- AWE32, GUS, and SoundScape require use of proprietary binary libraries that aren't compatible to DJGPP. However, Gravis Ultrasound libraries are available in the source code form, however, they have GPL incompatible license to be distributed together.
- Pro Audio Spectrum support didn't yet polished enough to work.

## This version also has some improvements
- MIDI synthesizer slightly improved:
  - added suport of CC11-Expression controller.
  - revised the pitch bend formula so it no longer gets messed during a time.
