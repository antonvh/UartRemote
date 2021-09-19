from uartremote import *
from utime import ticks_ms
ur = UartRemote(Port.S1)

print("Uart initialized")
print(ur.version)

# Test REPL
ur.repl_activate()
print(ur.repl_run("print('Repl tested')"))
print(ur.repl_run("from uartremote import *"))
print(ur.repl_run("ur=UartRemote()"))

# Start command loop
print(ur.repl_run("ur.loop()",reply=False))
ur.flush()
print(ur.call('echo','repr','Uart command loop tested with echo'))

# Test if we can revert to REPL
print(ur.call('enable repl'))
ur.repl_activate()
print(ur.repl_run("print('Repl tested')"))
print(ur.repl_run("ur.loop()",reply=False))

# Test the speed of the command loop
start = ticks_ms()
for i in range(100):
    q=ur.call('echo','bb',1,2)
elapsed=ticks_ms()-start
print("Commands per second: ",100*1000/elapsed)