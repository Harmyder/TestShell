To build the project you should set appropriate include and library directories. It must be done in executable project (TestShell), not in the lib.

`Project`->`Properties`->`Configuration Properties`->`C/C++`->`General`
Insert into `Additional Include Directories` directory to Bullet's includes on my PC it is `C:\Yola\PhysicsEngines\bullet3-2.86.1\src;`
C:\Yola\PhysicsEngines\bullet3-2.86.1\src;

To build libraries you could confer with http://bulletphysics.org/mediawiki-1.5.8/index.php/Creating_a_project_from_scratch

`Project`->`Properties`->`Configuration Properties`->`Linker`->`Library Directories`
Insert in Debug and Release modes respectively
C:\Yola\PhysicsEngines\bullet_build\lib\Debug;
C:\Yola\PhysicsEngines\bullet_build\lib\Release;
