# Do not use this version of UartRemote: use [MPY-Robot installer](https://github.com/antonvh/mpy-robot-tools/blob/master/Installer/install_mpy_robot_tools.py)


## IMPORTANT NOTICE
Uartremote only works on SPIKE Legacy (2.0) or Robot Inventor firmware. If you have a yellow SPIKE hub, with SPIKE 3.0, I suggest one of these options:

1. Use [Pupremote](https://github.com/antonvh/PUPRemote) + [Pybricks](https://code.pybricks.com). (Best)
2. Flash Robot Inventor firmware with that LEGO app and keep using uartremote 
3. Figure out how to emulate a lego sensor with LPF2 (part of of Pupremote) and keep using SPIKE 3.0 firmware. (very hard)

I'm a bit frustrated that LEGO removed so many options in their 3.0 firmware, but I can't change that.

## Background
The SPIKE IDE checks the filesystem of the Spike Prime. If it sees any non-system files in the root directory, it triggeres a firmware update. After the firmware update, the non-system files will be deleted. However, files that reside in the `/project`  will not be deleted after a firmware update.
## Installation of uartremote.py or uartremote.mpy
Open a new Python project in the LEGO Education SPIKE Prime IDE and paste in the content of the file `install_uartremote.py` that can be found in this directory. Execute the script. Open the console in the IDE. After executing it should show:

```
[10:28:55.389] > writing uartremote.mpy to folder /projects[10:28:55.584] > Finished writing uartremote.mpy.
[10:28:55.610] > Checking hash.[10:28:55.686] > Hash generated:  <hash>
[10:28:55.704] > Uartremote library written succesfully. Resetting....
```

The timestanps will be different on your system and `<hash>` will show the sha-256 hash value.

Now the uartremote.mpy library is copied to the `/project` directory.

To use the library include the following line in your python code:

```from project.uartremote import *```

## Creating the install file
If you have the `mpy-cross` cross compile tool installed, just do this in the SPIKE directory:
`./create_install_file.py`

## Errors while installing uartremote library
If you see any errors when running the `install_uartremote.py` script, these will be related to wrong hashes. 
- If the hash of the base64 decoded string is not the same as the hash of the initial uartremote.mpy file you will see:
```
Failed hash of base64 input : <hash>
```
- if the hash of the uartremote.mpy file written locally to the hub's filesystem differs from the initial hash you will see:
```
Failed hash of .mpy on SPIKE: <hash>
```
In both cases you can try again by copying the `install_uartremote.py` again in to an empty Lego Spike project and rerun the code.

## Github Actions for generating SPIKE install file
Using the aGithub Action workflow, the `uartremote.py` is integrated in the the `install_uartremote.py` script automatically, every time that a change in `uartremote.py` is commited.


