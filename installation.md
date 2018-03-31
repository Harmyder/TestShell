First you need to compile YolaFbxImporter.

In properties of Graphics project check that `Additional Include Directories` has proper path to DirectXTK12 include folder. If DirectXTK12 hasn't been properly loaded with the solution you need to remove it and when add again from where it is on your drive. After this you need to add reference to it from Graphics project.

Set `Working Directory` for TestShell to `$(TargetDir)`.

Bullet libs must reside in `./Libs/Bullet`.
