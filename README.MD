# XenoReader – Open Source Xenogears Disc Image Reader

**XenoReader** is a work-in-progress open source program, or possibly a library in the future, for reading Xenogears disc images. I started this project after discovering that no open source tools exist for this purpose. The few available tools are closed source and date back to 2005 or possibly earlier.

## End Goal

The goal of XenoReader is to provide a working C library, optionally with a C++ wrapper, to make reading and potentially writing Xenogears image files easier. I also aim to document the disc structure for the community, and possibly provide a GUI in the future.

## Current Progress

**Image Verification** – XenoReader can open a disc image and verify that it is a valid Xenogears image.

**Disc Identification** – XenoReader can reliably determine whether the image is Disc 1 or Disc 2.

**Sector Reading** - XenoReader can read sectors from the image into easy-to-use `Sector` structs that make their contents easier to understand and access.

## Future Work
Further work is needed to reverse engineer the filesystem. Xenogears, like several other Squaresoft RPGs from this era, does not use a standard ISO-9660 filesystem beyond the main binary and configuration file. This makes accessing and modifying game data challenging. **As of now, anything in this repo is me making guesses and reverse engineering what I can.**

## Why?
While games like Final Fantasy IX and Chrono Cross have received modern ports, Xenogears has not. This makes Xenogears difficult to mod and work with.