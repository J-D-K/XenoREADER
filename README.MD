# XenoREADER – Open Source Xenogears Disc Image Reader

**XenoREADER** is a work-in-progress open source program, or possibly a library in the future, for reading Xenogears disc images. I started this project after discovering that no open source tools exist for this purpose. The few I have found are either dead, incomplete, non-functional, or closed source and from the early 2000s.

## End Goal

The goal of XenoREADER is to provide a working C library, optionally with a C++ wrapper, to make reading and potentially writing Xenogears image files easier. I also aim to document the disc structure for the community, and possibly provide a GUI in the future.

## Current Progress

**Image Verification** – XenoREADER can open a disc image and verify that it is a valid Xenogears image.

**Disc Identification** – XenoREADER can reliably determine whether the image is Disc 1 or Disc 2.

**Filesystem Parsing** - XenoREADER can read and parse the hidden filesystem Xenogears uses instead of ISO-9660.

**File Extraction** - XenoREADER can extract the contents of a Xenogears image.

## Future Work
The possibility of replacing files (within the constrains of the sectors available) easily.

## Why?
While games like Final Fantasy IX and Chrono Cross have received modern ports, Xenogears has not. This makes Xenogears difficult to mod and work with.

I also haven't done anything this insane and complex in straight C for a while. 