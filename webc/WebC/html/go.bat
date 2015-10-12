..\bin\gmake.exe
move "*.c" ".\source"
move "*.h" ".\include"
cd chrome
..\..\bin\gmake.exe
move "*.c" ".\..\source"
move "*.h" ".\..\include"
cd ..\Images
..\..\bin\gmake.exe
move "*.c" ".\..\source"
move "*.h" ".\..\include"
cd..
