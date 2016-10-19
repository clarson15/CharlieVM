# CharlieVM
This is a personal project to write a fully functional processor and assembler.

##Usage
I've included a makefile to compile this from source. It should be cross platform (specifically 32-bit/64-bit systems),but I've only compiled on linux. Assembler takes 2 arguments, input file, output file. Processor can be run with a ROM in the command line or specified immediately after running. 

##Documentation
Take a look at the pdf in the root directory for the processors specs. 

##Todo:
- Interrupt system
- Slow down to fixed frequency
- Comments in ASM language
- Hack up an input system for processor
- Timers
- Second core (already implemented, but removed in the mean-time)
- Primitive OS with CPU scheduler, paging system, & system calls
- Open/Save files (fixed disk size)
- Networking?? (Jesus Christ..)
