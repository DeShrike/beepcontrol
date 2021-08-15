# beepcontrol

Linux daemon to control a piezo electric beeper from Raspberry Pi using GPIO PWM.

Uses Message Queue based Interprocess Communication (IPC)

## Dependencies

- Python >= 3.6
- sysv_ipc (python package)
- WiringPi (C library)

## Build & Install

```
sudo apt-get install wiringPi
make
make install	# install daemon
make enable		# enable at boot
make start
```

## Configuration

Configuration is in `/etc/beepcontrol.ini`

```
[beeper]
pin=13
queueid=1234
messagetypeid=101
```

queueid: The ID of the Message Queue to use. An integer > 0.

messagetypeid: The ID of the message to use. An integer > 0.

pin: the pin number where the beeper is connected. (Broadcom numbering (BCM))

Should be one of the following:

(PWM0)  1 = WiringPi, *18* = BCM, 12 = Board

(PWM0) 26 = WiringPi, *12* = BCM, 32 = Board

(PWM1) 23 = WiringPi, *13* = BCM, 33 = Board

(PWM1) 24 = WiringPi, *19* = BCM, 35 = Board (Not on Pi Zero ?)

## Usage

### From C

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define  QUEUEID   1234			// See queueid in configuration
#define  MESSAGETYPEID   101	// See messagetypeid in configuration
#define  BUFF_SIZE   16

#include "type_definitions.h"

typedef struct
{
	long  data_type;
	char  data_buff[BUFF_SIZE];
} t_data;

int      msqid;
t_data   data;

if (-1 == (msqid = msgget((key_t)QUEUEID, IPC_CREAT | 0666)))
{
	perror("msgget() failed");
	exit(1);
}

int value = 100;	// beep duration in ms
data.data_type = MESSAGETYPEID;
memcpy(data.data_buff, &value, sizeof(int));
if (-1 == msgsnd(msqid, &data, sizeof(t_data) - sizeof(long), 0))
{
	perror("msgsnd() failed");
	exit(1);
}
```

### From Python

```
import sysv_ipc

queue_id = 1234				# See queueid in configuration
message_type_id = 101		# See messagetypeid in configuration

mq = sysv_ipc.MessageQueue(queue_id, sysv_ipc.IPC_CREAT)

value = 100					# beep duration in ms

bytearray1 = struct.pack("i", value)
mq.send(bytearray1, True, type=message_type_id)
```
