/* 
   Copyright (C) 2012 by Ronnie Sahlberg <ronniesahlberg@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "iscsi.h"
#include "scsi-lowlevel.h"
#include "iscsi-test.h"

int T0203_read16_0blocks(const char *initiator, const char *url)
{ 
	struct iscsi_context *iscsi;
	struct scsi_task *task;
	int ret = 0, lun;

	printf("0203_read16_0blocks:\n");
	printf("====================\n");
	if (show_info) {
		printf("Test that READ16  works correctly when transfer length is 0 blocks.\n");
		printf("1, Read at 0 should work.\n");
		printf("2, Read at one block beyond end-of-lun should fail.\n");
		printf("3, Read at LBA:2^63 should fail.\n");
		printf("4, Read at LBA:-1 should fail.\n");
		printf("\n");
		return 0;
	}

	iscsi = iscsi_context_login(initiator, url, &lun);
	if (iscsi == NULL) {
		printf("Failed to login to target\n");
		return -1;
	}


	printf("READ16 0blocks at LBA:0 ... ");
	task = iscsi_read16_sync(iscsi, lun, 0, 0, block_size, 0, 0, 0, 0, 0);
	if (task == NULL) {
	        printf("[FAILED]\n");
		printf("Failed to send READ16 command: %s\n", iscsi_get_error(iscsi));
		ret = -1;
		goto finished;
	}
	if (task->status != SCSI_STATUS_GOOD) {
	        printf("[FAILED]\n");
		printf("READ16 command: failed with sense. %s\n", iscsi_get_error(iscsi));
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	scsi_free_scsi_task(task);
	printf("[OK]\n");


	printf("READ16 0blocks at LBA:<beyond end-of-disk> ... ");
	task = iscsi_read16_sync(iscsi, lun, num_blocks + 2, 0, block_size, 0, 0, 0, 0, 0);
	if (task == NULL) {
	        printf("[FAILED]\n");
		printf("Failed to send READ16 command: %s\n", iscsi_get_error(iscsi));
		ret = -1;
		goto finished;
	}
	if (task->status == SCSI_STATUS_GOOD) {
	        printf("[FAILED]\n");
		printf("READ16 command: Should fail when reading 0blocks beyond end\n");
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	if (task->status        != SCSI_STATUS_CHECK_CONDITION
	    || task->sense.key  != SCSI_SENSE_ILLEGAL_REQUEST
	    || task->sense.ascq != SCSI_SENSE_ASCQ_LBA_OUT_OF_RANGE) {
	        printf("[FAILED]\n");
		printf("READ16 failed but ascq was wrong. Should have failed with ILLEGAL_REQUEST/LBA_OUT_OF_RANGE. Sense:%s\n", iscsi_get_error(iscsi));
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	scsi_free_scsi_task(task);
	printf("[OK]\n");


	printf("READ16 0blocks at LBA 2^63 ... ");
	task = iscsi_read16_sync(iscsi, lun, 0x8000000000000000, 0, block_size, 0, 0, 0, 0, 0);
	if (task == NULL) {
	        printf("[FAILED]\n");
		printf("Failed to send READ16 command: %s\n", iscsi_get_error(iscsi));
		ret = -1;
		goto finished;
	}
	if (task->status == SCSI_STATUS_GOOD) {
	        printf("[FAILED]\n");
		printf("READ16 command: Should fail when reading 0blocks at 2^63\n");
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	if (task->status        != SCSI_STATUS_CHECK_CONDITION
	    || task->sense.key  != SCSI_SENSE_ILLEGAL_REQUEST
	    || task->sense.ascq != SCSI_SENSE_ASCQ_LBA_OUT_OF_RANGE) {
	        printf("[FAILED]\n");
		printf("READ16 failed but ascq was wrong. Should have failed with ILLEGAL_REQUEST/LBA_OUT_OF_RANGE. Sense:%s\n", iscsi_get_error(iscsi));
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	scsi_free_scsi_task(task);
	printf("[OK]\n");


	printf("READ16 0blocks at LBA -1 ... ");
	task = iscsi_read16_sync(iscsi, lun, -1, 0, block_size, 0, 0, 0, 0, 0);
	if (task == NULL) {
	        printf("[FAILED]\n");
		printf("Failed to send READ16 command: %s\n", iscsi_get_error(iscsi));
		ret = -1;
		goto finished;
	}
	if (task->status == SCSI_STATUS_GOOD) {
	        printf("[FAILED]\n");
		printf("READ16 command: Should fail when reading 0blocks at -1\n");
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	if (task->status        != SCSI_STATUS_CHECK_CONDITION
	    || task->sense.key  != SCSI_SENSE_ILLEGAL_REQUEST
	    || task->sense.ascq != SCSI_SENSE_ASCQ_LBA_OUT_OF_RANGE) {
	        printf("[FAILED]\n");
		printf("READ16 failed but ascq was wrong. Should have failed with ILLEGAL_REQUEST/LBA_OUT_OF_RANGE. Sense:%s\n", iscsi_get_error(iscsi));
		ret = -1;
		scsi_free_scsi_task(task);
		goto finished;
	}
	scsi_free_scsi_task(task);
	printf("[OK]\n");


finished:
	iscsi_logout_sync(iscsi);
	iscsi_destroy_context(iscsi);
	return ret;
}
