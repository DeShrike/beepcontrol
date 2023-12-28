#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "beepcontrol.h"
#include "config.h"

static volatile int keepRunning = 1;

// sudo apt-get install wiringpi
// gcc -lwiringPi
// gcc -lwiringPiDev

#define CONFIG_FILE         "/etc/beepcontrol.ini"

#define delayMilliseconds   delay

#define PINMODE(p, m)           if (p != 0) { pinMode(p, m); }
#define PULLUPDNCONTROL(p, m)   if (p != 0) { pullUpDnControl(p, m); }
#define DIGITALWRITE(p, m)      if (p != 0) { digitalWrite(p, m); }
#define DIGITALREAD(p)          (p != 0 ? digitalRead(p) : 0)

#define STEPDELAY   1000

configuration* config;

void intHandler(int dummy)
{
    keepRunning = 0;
}

void cleanup()
{
    printf("Cleanup\n");

    DIGITALWRITE(config->beeperPin, LOW);
    delayMilliseconds(100);
    PINMODE(config->beeperPin, INPUT);
}

void loop()
{
    int msqid;
	t_data data;

	if (-1 == (msqid = msgget((key_t)config->queueId, IPC_CREAT | 0666)))
	{
		perror("msgget() failed");
		return;
	}

    while (keepRunning)
    {
		if (-1 == msgrcv(msqid, &data, sizeof(t_data) - sizeof(long), 0, IPC_NOWAIT))
		{
			perror("msgrcv() failed");
            delayMilliseconds(STEPDELAY);
			continue;
		}

		printf("\n");
		printf("New message received\n");
        printf("Data Type: %ld\n", data.data_type);

        printf("Raw data: ");
		for(int i = 0; i < BUFF_SIZE; i++)
		{
			printf("%02X ", data.data_buff[i]);
		}

		printf("\n");

		if (data.data_type == config->messageTypeId)
		{
            int iValue = 0;
            int iDuration = 0;
			memcpy(&iDuration, data.data_buff, sizeof(int));
			memcpy(&iValue, data.data_buff + sizeof(int), sizeof(int));
            printf("Beep Duration: %d ms\n", iDuration);
            printf("Beep Value   : %d\n", iValue);

            pwmWrite(config->beeperPin, iValue);
            delayMilliseconds(iDuration);
            pwmWrite(config->beeperPin, 0);
		}

        delayMilliseconds(STEPDELAY);
    }
}

void start()
{
    signal(SIGKILL, intHandler);
    signal(SIGINT, intHandler);
    signal(SIGTERM, intHandler);
    signal(SIGHUP, intHandler);

    wiringPiSetupGpio();  // setup wih Broadcom numbering

    // set pin to PWM
    PINMODE(config->beeperPin, PWM_OUTPUT);

    // set the PWM mode to Mark Space
    pwmSetMode(PWM_MODE_MS);

    // delay a moment to let hardware settings settle.
    delay(100);

    pwmWrite(config->beeperPin, 0);

    printf("Running! Press Ctrl+C to quit. \n");

    loop();

    printf("\nStopping\n");
    cleanup();
}

void show_config(configuration* config)
{
    printf("Config loaded from '%s':\n", CONFIG_FILE);

    printf("     Beeper Pin: %d \n", config->beeperPin);
    printf("       Queue ID: %d \n", config->queueId);
    printf("Message Type ID: %d \n", config->messageTypeId);
}

int main(int argc, char* argv[])
{
    config = read_config(CONFIG_FILE);
    if (config == NULL)
    {
        fprintf(stderr, "Can't load '%s'\n", CONFIG_FILE);
        return EXIT_FAILURE;
    }

    show_config(config);

    if (config->beeperPin != 18 && config->beeperPin != 12 && config->beeperPin != 13 && config->beeperPin != 19)
    {
        fprintf(stderr, "Bad configuration in '%s'\n", CONFIG_FILE);
        fprintf(stderr, "Pin number must be 18 or 12 (for PWM0) or 13 or 19 (for PWM1). (BCM numbering)\n");
        free_config(config);
        return EXIT_FAILURE;
    }

    start();

    free_config(config);

    return EXIT_SUCCESS;
}
