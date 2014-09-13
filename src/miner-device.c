//
//  miner-device.c
//  MultiMiner
//
//  Created by Nathanial Woolls on 8/5/14.
//  Copyright (c) 2014 Nate Woolls. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "weechat-plugin.h"
#include "miner-plugin.h"

#define READ 0 /* Read end of pipe */
#define WRITE 1 /* Write end of pipe */

static
void miner_device_dump_fd(int const fd)
{
	char buffer[4096];
	while (1)
	{
		ssize_t count = read(fd, buffer, sizeof(buffer));
		if (count == -1)
		{
			if (errno == EINTR)
			{
				continue;
			} else
			{
				perror("read");
				exit(1);
			}
		}
		else if (count == 0)
		{
			break;
		}
		else
		{
			buffer[count] = 0;
			weechat_printf(NULL, buffer);
		}
	}
}

static
void miner_dump_command(char * const cmd[])
{
	int stdout_pipes[2];
	int stderr_pipes[2];

	int bytes_read;
	char buf[0x100] = { 0 };
	pid_t pid;

	pipe(stdout_pipes);
	pipe(stderr_pipes);

	pid = fork();

	if (pid == -1)
	{
		/* failed */
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		/* child */

		// child process is writing to pipes, not reading from them
		close(stdout_pipes[READ]);
		close(stderr_pipes[READ]);

		// stdout & stderr should go to the write end of their pipes
		// dup2 closes the original stdout & stderr
		while ((dup2(stderr_pipes[WRITE], STDERR_FILENO) == -1) && (errno == EINTR)) {}
		while ((dup2(stdout_pipes[WRITE], STDOUT_FILENO) == -1) && (errno == EINTR)) {}

		// not needed after dup2
		close(stdout_pipes[WRITE]);
		close(stderr_pipes[WRITE]);

		execvp(cmd[0], cmd);

		// execvp only returns on failure
		perror("execvp");
		_exit(EXIT_FAILURE);
	}

	/* parent */

	// parent process is reading from pipes, not writing to them
	close(stdout_pipes[WRITE]);
	close(stderr_pipes[WRITE]);

	miner_device_dump_fd(stdout_pipes[READ]);
	miner_device_dump_fd(stderr_pipes[READ]);

	// close pipes
	close(stdout_pipes[READ]);
	close(stderr_pipes[READ]);

	/* wait for the child we forked */
	wait(NULL);
}

void miner_device_scan()
{
	char * const cmd[] =
	{
		"bfgminer",
		"-d?",
		"--scrypt",
		"-S", "auto",
		NULL
	};
	miner_dump_command(cmd);
}

void miner_device_probe()
{
	char * const cmd[] =
	{
		"bfgminer",
		"-d?",
		"--scrypt",
		"-S", "all",
		NULL
	};
	miner_dump_command(cmd);
}
