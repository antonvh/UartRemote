#!python3

# Run this on a Mac or Linux machine to create/update 'install_uartremote.py'
# Copy the contents of install_uartremote.py into an empty SPIKE Prime project
# And run to install

import binascii, mpy_cross, time
import hashlib

LIB = '../uartremote.py'
MPY_LIB = '../SPIKE/uartremote.mpy'
INSTALLER = '../SPIKE/install_uartremote.py'

mpy_cross.run('-march=armv6',LIB,'-o', MPY_LIB)

# Should be done in a second!
time.sleep(2)

mpy_file=open(MPY_LIB,'rb').read()
hash=hashlib.sha256(mpy_file).hexdigest()
ur_b64=binascii.b2a_base64(mpy_file)

spike_code=f"""import ubinascii, uos, machine,uhashlib
from ubinascii import hexlify
b64=\"\"\"{ur_b64.decode('utf-8')}\"\"\"

def calc_hash(b):
    return hexlify(uhashlib.sha256(b).digest()).decode()

# this is the hash of the compiled uartremote.mpy
hash_gen='{hash}'

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
with open(INSTALLER,'w') as f:
    f.write(spike_code)
