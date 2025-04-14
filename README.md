# Apogee Sound System for DJGPP

A DJGPP port of Apogee Sound System audio library (AudioLib) used in Build Engine based games such as Duke Nukem 3D, Rise of the Triades, Blood, Shadow Warrior, NAM, etc.

Can be built with CMake as a static library. Also can be used as a sub-directory to integrate with the target project build.

This version __doesn't__ (yet) supports AWE32 and Gravis UltraSound because they require use of proprietary binary libraries that aren't compatible to DJGPP. However, Gravis Ultrasound libraries are available in the source code form, however, they have GPL incompatible license to be distributed together.

Also, Pro Audio Spectrum and SoundScape support are presented, however, I had no hardware or suitable emulator to verify the work of their code.

## This version also has some improvements
- MIDI synthesizer slightly improved:
  - added suport of CC11-Expression controller.
  - revised the pitch bend formula so it no longer gets messed during a time.
  - On OPL3 chips the Center-Left-Right panning now works as expected (at original version panning didn't used on OPL3 chips).


## Known bugs
- Can't correctly play stereo files (the support wasn't be here officially, old readme contains that note)
- Distorted playback on stereo outputs
