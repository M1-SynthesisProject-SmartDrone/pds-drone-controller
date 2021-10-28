# Drone controller

This app is used to be the bridge between the Android application and the 
drone (px4 drone).

## How to run

Launch the script "./compile.sh" while in this folder (the "drone_controller" one)
and run the application created in the "build folder".

If the script doesn't work for any reason, you can compile yourself by typing :

```
$ sudo mkdir build
$ cd build
$ cmake ..
$ make
```