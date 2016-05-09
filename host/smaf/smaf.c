/*
 * Copyright (c) 2016, Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <smaf_api.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <ta_smaf_test.h>
#include <err.h>
#include <tee_client_api.h>

#define LENGTH 1024*16

int main(void)
{
	int ret;
	int fd;
	void *data;
	char reference[LENGTH+1];
	char *pt;
	int i;
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SMAF_TEST_UUID;
	uint32_t err_origin;
	TEEC_SharedMemory sharedMem;

	/* open the smaf device */
	if (smaf_open()) {
		printf("Can't open /dev/smaf\n");
		return;
	}

	/* create the buffer */
	ret = smaf_create_buffer(LENGTH+1, O_CLOEXEC | O_RDWR, "smaf-optee-allocator", &fd);
	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return 1;
	}

	/* map the buffer - real allocation */
	data = mmap(0, LENGTH+1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		printf("%s mmap() failed\n", __func__);
		return 1;
	}
	memset(data, 0, LENGTH+1);

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	sharedMem.buffer = data;
	sharedMem.size = LENGTH;
	sharedMem.flags = TEEC_MEM_INPUT;
	res = TEEC_RegisterSharedMemory(&ctx, &sharedMem);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_RegisterSharedMemory failed with 0x%x ", res);

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].memref.parent = &sharedMem;
	op.params[0].memref.size = LENGTH;
	op.params[0].memref.offset = 0;

	res = TEEC_InvokeCommand(&sess, TA_SMAF_CMD_WRITE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);

	pt = data;
	if (pt[0] != 0xFF)
		printf("Wrong value - is %d\n", pt[0]);
	else
		printf("Correct value - is %d\n", pt[0]);


	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	/* unmap the buffer */
	munmap(data, LENGTH+1);
	close(fd);

	smaf_close();

	printf("Done\n");
	return 0;
}
