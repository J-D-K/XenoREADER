# Xenogears Image Notes:
* **Note:** I'm still working on this. I am using a dump of my own game discs to ensure they are clean and untouched.

    1. Xenogears does not use the standard ISO-9660 filesystem outside of what it needs to for the PSX to boot the game.
        * The only two files visible on the disc are its binary `SLUSXX.XXXX` and its configuration for booting.
        * The actual files containing the game's data are spread throughout the disc.
            * The binary and configuration file are accounted for here too. There's no need to actually mount the image like normal. **GOOD**!
        * Sector 24 through 38 contain the table of contents. Nothing is actually named.
        * Sector 40 has something?

    2. **The Table of Contents**:
        * An entry in the table of contents is as such:
            * The first 24bit unsigned int is the sector the entry begins on.
            * The second is a signed 32bit int and is one of following two things:
                * If the entry is a directory, the value will be negative. Inverting this negative value will yield the number of entries inside the directory.
                * If the entry is a file, this will be positive and be the total size of the file.


    3. The sectors themselves contain a 32 byte header that appears to contain the part, "directory" index, and total parts of the file.
        * Files end with blocks of `FF` repeating. This needs to be trimmed when detected.
            * Detecting this might actually be the only way to get the true part count of the file because:
        * Even though a header may specify a file has 9 parts, only 6 or 7 sectors may actually contain data for the file. This may be some kind of alignment strategy for the disc.

    
