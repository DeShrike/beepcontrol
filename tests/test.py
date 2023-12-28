import sysv_ipc
import struct
import time

queue_id = 1234				# See queueid in configuration
message_type_id = 101		# See messagetypeid in configuration

mq = sysv_ipc.MessageQueue(queue_id, sysv_ipc.IPC_CREAT)

value = 50
duration = 100					# beep duration in ms

while True:

    bytearray1 = struct.pack("ii", duration, value)
    mq.send(bytearray1, True, type=message_type_id)

    print(duration, value)

    value = value + 50
    if value == 2000:
        value = 50

    time.sleep(1)
