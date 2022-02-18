#!python3

# Run this on a Mac or Linux machine to create/update 'install_uartremote.py'
# Copy the contents of install_uartremote.py into an empty SPIKE Prime project
# And run to install

import binascii, mpy_cross, time
import hashlib

# version control pulled from git commit short
import re, subprocess
# uses native python to run OS command not windows tested
gitCommand = subprocess.run(['git','show','--abbrev-commit'], stdout=subprocess.PIPE).stdout.decode('utf-8')
gitCommand = gitCommand.split('\n', 1)[0]
version = gitCommand.split(' ')[-1]

LIB = '../uartremote.py'
LIB2 = '../SPIKE/uartremote.py' # version tracked file created in working directory
MPY_LIB = '../SPIKE/uartremote.mpy'
INSTALLER = '../SPIKE/install_uartremote.py'

# create uartremote version tracked for distribution
version_tracked=open(LIB,'r').read()
print('Building Installer for uartremote version:',version)
version_code=f"""
    def version(self):
        git_version='{version}'
        if self.DEBUG:print(git_version)

"""
version_tracked = version_tracked + version_code
print('Writing version tracked uartremote...')
with open(LIB2,'w') as f:
    f.write(version_tracked)

# use new copy with version and compile
print('Running MPY cross compile of uartremote...')
mpy_cross.run('-march=armv6',LIB2,'-o', MPY_LIB)
# Should be done in a second!
time.sleep(2)

# Convert .mpy to BASE64 for install.py
mpy_file=open(MPY_LIB,'rb').read()
hash=hashlib.sha256(mpy_file).hexdigest()
ur_b64=binascii.b2a_base64(mpy_file)
mpy_file=open(MPY_LIB,'rb').close()

# compile install_uartremote.py with encoded .mpy
spike_code=f"""import ubinascii, uos, machine,uhashlib
from ubinascii import hexlify
b64=\"\"\"{ur_b64.decode('utf-8')}\"\"\"

def calc_hash(b):
    return hexlify(uhashlib.sha256(b).digest()).decode()

# this is the hash of the compiled uartremote.mpy
hash_gen='{hash}'
git_version='{version}'

uartremote=ubinascii.a2b_base64(b64)
hash_initial=calc_hash(uartremote)

try: # remove any old versions of uartremote library
    uos.remove('/projects/uartremote.py')
    uos.remove('/projects/uartremote.mpy')
except OSError:
    pass

print('writing uartremote.mpy to folder /projects')
with open('/projects/uartremote.mpy','wb') as f:
    f.write(uartremote)
print('Finished writing uartremote.mpy.')
print('Checking hash.')
uartremote_check=open('/projects/uartremote.mpy','rb').read()
hash_check=calc_hash(uartremote_check)

print('Hash generated: ',hash_gen)
print('Git Version: ',git_version)
error=False
if hash_initial != hash_gen:
    print('Failed hash of base64 input : '+hash_initial)
    error=True
if hash_check != hash_gen:
    print('Failed hash of .mpy on SPIKE: '+hash_check)
    error=True

if not error:
    print('Uartremote library written succesfully. Resetting....')
    machine.reset()
else:
    print('Failure in Uartremote library!')

"""
print('Writing install_uartremote.py for deployment to SPIKE...')
with open(INSTALLER,'w') as f:
    f.write(spike_code)

# Done
print('Done!')
