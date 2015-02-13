######################################################################################
#							DOWNLOAD AND INSTALL GSL SOFTWARE						 #
######################################################################################

1. Download GSL library from http://gnuwin32.sourceforge.net/downlinks/gsl-src-zip.php 
2. Unizip to <project_dir>\GSL (MAKE SURE THAT THERE ARE NO SPACES IN TE PATH OF YOUR PROJECT DIRECTORY)

######################################################################################
#						BUILD GSL LIBRARIES FOR VISUAL STUDIO						 #
######################################################################################

1. Launch VISUAL C++ STUDIO
2. Open project <project_dir>\GSL\src\gsl\1.8\gsl-1.8\VC8\libgsl.sln
3. Continue with convert and build all projects

 ######################################################################################
#								HOW TO USE GSL LIBRARY								 #
######################################################################################

1. Launch VISUAL STUDIO
2. Open your project
3. Go to the menu "Project" and select "properties"
4. Expand "Configuration Properties" and go to "VC++ directories"
5. Click on the field "Executable Directories" and paste the following entries:
	- <project_dir>\GSL\src\gsl\1.8\gsl-1.8\VC8\libgsl\Debug-DLL
	- <project_dir>\GSL\src\gsl\1.8\gsl-1.8\VC8\libgslcblas\Debug-DLL
6. Click on the field "Include Directories" and paste the following entries:
	- <project_dir>\GSL\src\gsl\1.8\gsl-1.8
7. Click on the field "Library Directories" and paste the following entries:
	- <project_dir>\GSL\src\gsl\1.8\gsl-1.8\VC8\libgsl\Debug-DLL
	- <project_dir>\GSL\src\gsl\1.8\gsl-1.8\VC8\libgslcblas\Debug-DLL
8. Expand "Configuration Properties". Expand "Linker" and select "Input".
9. Select the field "Additional Dependencies" and append the following entries:
	- libgsl_dll_d.lib
	- libgslcblas_dll_d.lib
10. Expand "Configuration Properties". Expand "C/C++" and select Prepocessor.
11. Select the field "Prepocessor Definitions" and add the following entries:
	- GSL_DLL
12. Copy the DLLs: libgslcblas_d.dll and libgsl_d.dll into your project directory: <project_dir>\Debug
