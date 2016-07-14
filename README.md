# MTCS

Public repository of the Mercator Telescope Control System software.

### Restrictions with respect to the private repository of the MTCS software:
 - The public repository is only periodically updated. 
 - The public repository does not contain TwinCAT licence files
 - The public repository does not contain the [SLALIB positional astronomy library](http://star-www.rl.ac.uk/)
 
 
### To compile the TwinCAT C++ part of the MTCS project, you need to install SLALIB:
- copy slalib.h, slamac.h and all *.c files to the MTCS/Slalib/slalib folder
- replace all mathematical functions by their real-time TwinCAT counterparts
   - sin -> sin_
   - cos -> cos_
   - tan -> tan_
   - atan2 -> atan2_
   - fabs -> fabs_ 
   - ...
- This can be done semi-automatically by the 'Find and Replace' functionality of Visual Studio.
