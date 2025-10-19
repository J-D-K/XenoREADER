# Xenogears Image Notes:
* **Note:** I'm still working on this. I am using a dump of my own game discs to ensure they are clean and untouched.

    1. Xenogears does not use the standard ISO-9660 filesystem outside of what it needs to for the PSX to boot the game.
        * The only two files visible on the disc are its binary `SLUSXX.XXXX` and its configuration for booting.
        * The actual files containing the game's data are spread throughout the disc.
            * The binary and configuration file are accounted for here too. There's no need to actually mount the image like normal.
                * This is actually very nice. Now I don't need to add extra code to extract the binary. _It just happens._ It's usually `FILE_0004.bin` in the root directory.

    2. **The Table of Contents/Hidden Filesystem**:
        * The hidden filesystem begins in sector 24 and is 16 sectors (32KB) large.
        * Each entry is 7 bytes long.
            * The first 24 bits are the sector where the file/directory begin.
            * The next 32bits are a signed 32bit int.
                * If negative, it's a directory and inverting the value gets you the number of files the directory contains.
                * If positive, it's the size of the file in bytes.
        * Once an entry with sector `0xFFFFFF` is encountered, you have hit the end of the hidden file system.

    
