# C_Linux_Raytracer
Clone:
The repo must be cloned with --recursive as it uses stbimage as a submodule

Build
1) First clean up Texture.o which is in PROJECT_DIR*/src
2) Clean up the executables in PROJECT_DIR*/
This must be done as I'm developing on PowerPC so all these binaries are probably not going to run on your machine.
3) Edit the build script to uncomment out the gcc command that rebuilds Texture.o or add Texture.c to the main gcc command.

Texture.c is being compiled as a seperate object file becuse it's really slow to compile on my old PPC machine and i'm not modifying it much so I compile it once, modify the rest of the project and then link against the .o file

