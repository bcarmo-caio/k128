## Author
Caio Renato Bedulli do Carmo

## K128
K128 is a simple program to encrypt a file using K128-CBC method.  
This project has been built as an exercise in cryptography for a  
course in introduction to cryptography in 2012 and since then I have not  
improved it.  
crproj2012.pdf is a file describing (pt-BR) how this project should be  
written.  

This program comes with no warranty whatsoever. Use it at you risk.  

## License
Copyright (C) 2012 Caio Renato Bedulli do Carmo  
This program is free software: you can redistribute it and/or modify it under  
the terms of the GNU General Public License as published by the Free Software  
Foundation, either version 3 of the License, or (at your option) any later  
version.

## Tests
Tests have been done using files of small (less than 128 bytes), medium (some  
kylobytes and megabytes) and large (some gigabytes) files using the following  
machine:  
Linux  2.6.32-5-686 #1 SMP Mon Mar 26 05:20:33 UTC 2012 i686 GNU/Linux  
Debian Stable (Squeeze)  

lscpu:  
Architecture:          i686  
CPU op-mode(s):        32-bit, 64-bit  
CPU(s):                2  
Thread(s) per core:    1  
Core(s) per socket:    2  
CPU socket(s):         1  
Vendor ID:             GenuineIntel  
CPU family:            6  
Model:                 23  
Stepping:              10  
CPU MHz:               1200.000  
Virtualization:        VT-x  
L1d cache:             32K  
L1i cache:             32K  
L2 cache:              1024K  
