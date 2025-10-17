# Xenogears Image Notes:
* **Note:** I'm still working on this. I am using a dump of my own game discs to ensure they are clean and untouched.

    1. Xenogears does not use the standard ISO-9660 filesystem outside of what it needs to for the PSX to boot the game.
        * The only two files visible on the disc are its binary `SLUSXX.XXXX` and its configuration for booting.
        * The actual files containing the game's data are spread throughout the disc.
        * Sector 24, and possibly 25, contain the data needed to find these files, but not completely. They seem to contain data pointing to where the files for a particular "directory" start. There is another piece of data after the sector,
        but I'm not entirely sure what it is yet.
        * Data appears to be compressed.

    2. The sectors themselves contain a 32 byte header that appears to contain the part, "directory" index, and total parts of the file.
        * Files end with blocks of `FF` repeating. This needs to be trimmed when detected.
            * Detecting this might actually be the only way to get the true part count of the file because:
        * Even though a header may specify a file has 9 parts, only 6 or 7 sectors may actually contain data for the file. This may be some kind of alignment strategy for the disc.

    
