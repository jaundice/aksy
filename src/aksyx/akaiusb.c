#include <stdio.h>
#include <string.h>
#include <usb.h>
#include <assert.h>
#include <errno.h>
#include "akaiusb.h"

#ifdef _POSIX_SOURCE
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <time.h>
#endif

/* Checks whether buffer is an ok reply (0x41 0x6b 0x61 0x49) */
inline int 
z48_reply_ok(unsigned char* buffer)
{
	if (buffer[0] != 0x41) return 0;
	if (buffer[1] != 0x6b) return 0;
	if (buffer[2] != 0x61) return 0;
	if (buffer[3] != 0x49) return 0;
	return 1;
}

#ifdef _POSIX_SOURCE
void
print_transfer_stats(struct timeval t1, struct timeval t2, int bytes_transferred)
{
	float elapsed, kbps;
	gettimeofday(&t2, NULL); // timeval, timezone struct
	// get elapsed time in seconds. 
	elapsed = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)/1000000.0f;
	kbps = bytes_transferred/(1024*elapsed);
	printf("Transfered %i bytes in elapsed %6f (%6f kB/s)\n", bytes_transferred, elapsed, kbps); 
}
#endif

int akai_usb_device_init(akai_usb_device akai_dev)
{
    struct usb_bus *bus;
    struct usb_device *dev;
    char usb_product_id;

    usb_init();

    usb_find_busses();
    usb_find_devices();

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
       for (dev = bus->devices; dev; dev = dev->next)
       {
         if (dev->descriptor.idVendor == VENDOR_ID)
         {
             usb_product_id =  (unsigned char)dev->descriptor.idProduct;

             if (usb_product_id != Z48 && usb_product_id != S56K)
             {
                free(akai_dev);
                return AKAI_UNSUPPORTED_DEVICE;
             }

             /* found the akai z48 or s56k */
             akai_dev->dev = usb_open(dev);
             akai_dev->id = usb_product_id;
             if (! akai_dev->dev)
             {
                free(akai_dev);
                return AKAI_NO_USB_HANDLE;
             }

             return usb_claim_interface(akai_dev->dev, 0);
          }
       }
    }

    return AKAI_NO_SAMPLER_FOUND;
}

int akai_usb_device_close(akai_usb_device akai_dev)
{
    int rc;
    rc = usb_release_interface(akai_dev->dev, 0);
    rc = usb_close (akai_dev->dev)|rc;
    free (akai_dev);
    return rc;
}

int akai_usb_device_exec_sysex(akai_usb_device akai_dev,  
    char *sysex, int sysex_length, 
    char *result_buff, int result_buff_length, int timeout) 
{
    usb_bulk_write(akai_dev->dev, EP_OUT, sysex, sysex_length, timeout);
    return usb_bulk_read(akai_dev->dev, EP_IN, result_buff, result_buff_length, timeout);

}

int akai_usb_device_get_handle_by_name(akai_usb_device akai_dev,
    char* name, char* handle)
{
    printf("GETHANDLE\n");
    unsigned char section;
    unsigned char *extension, *sysex, *data;
    int retval;
    int name_length = strlen(name);
    if (name_length < 4)
    {
        /* invalid name */
        return -1;
    }

    extension = (unsigned char*) calloc(3, sizeof(unsigned char));
    strncpy(extension, name + name_length-3, 3);
    printf("extension, %s\n", extension);
    if (strcasecmp(extension, "akm"))
    {
        section = '\x18';
    }
    else if (strcasecmp(extension, "wav"))
    {
        section = '\x1c';
    }
    else if (strcasecmp(extension, "akp"))
    {
        section = '\x14';
    }
    else if (strcasecmp(extension, "mid"))
    {
        section = '\x28';
    }
    else
    {
        /* invalid name */
        return -1;
    }

    /* request: \x10\x08\x00\xf0\x47 <device> <section, command, name, \xf7 */
    sysex = (unsigned char*) calloc(name_length+11, sizeof(unsigned char));
    int sysex_length = name_length+11;
    memcpy(sysex, "\x10", 5 * sizeof(unsigned char));
    memcpy(sysex+1, &sysex_length, 1 * sizeof(unsigned char));
    memcpy(sysex+2, "\x00\xf0\x47", 5 * sizeof(unsigned char));
    memcpy(sysex+5, &akai_dev->id, 1 * sizeof(unsigned char));
    memcpy(sysex+6, "\x00", 1 * sizeof(unsigned char));
    memcpy(sysex+7, &section, 1 * sizeof(unsigned char));
    memcpy(sysex+8, "\x08", 1 * sizeof(unsigned char));
    memcpy(sysex+9, name, name_length * sizeof(unsigned char));
    memcpy(sysex+9 + name_length, "\x00\xf7", 2 * sizeof(unsigned char));
		int i = 0;
		for (; i < name_length+11 ; i++)
			printf("%02x ", sysex[i]);
		printf("\n");
    /* success reply: \xf0\x47 <device> <section, command, <reply_ok> <4 byte handle>, \xf7 */
    /* error reply: \xf0\x47 <device> <section, command, <reply_error>, \xf7 */
    if (!usb_bulk_write(akai_dev->dev, EP_OUT, sysex, name_length + 11, USB_TIMEOUT))
    {
        retval = AKAI_TRANSMISSION_ERROR;
    }
    else
    {
        data = (unsigned char*) calloc(10, sizeof(unsigned char));
        if (usb_bulk_read(akai_dev->dev, EP_IN, data, 11, USB_TIMEOUT) < 0)
        {
            retval = AKAI_TRANSMISSION_ERROR;
        }
        else
        {
            if (data[5] == SYSEX_ERROR)
            {
                printf("ERROR\n");
                retval = AKAI_SYSEX_ERROR;
            }
            else if (data[5] == SYSEX_REPLY)
            {
                printf("SUCCESS\n");
                memcpy(handle, data+6, 4*sizeof(unsigned char));
                retval = AKAI_SUCCESS;
            }
            else
            {
                retval = AKAI_SYSEX_UNEXPECTED;
            }
        }
        free(data);
    }
    free(sysex);
    return retval;
}

int akai_usb_device_get(akai_usb_device akai_dev, char *src_filename, 
    char *dest_filename, int location, int timeout)
{
    unsigned char *command, *data, *handle;
	int block_size = 4096*4, bytes_transferred = 0, rc = 0, retval = 0;
    int src_filename_length = strlen(src_filename) + 1;
#ifdef _POSIX_SOURCE
	struct timeval t1, t2;
#endif
    FILE *dest_file;

	dest_file = fopen(dest_filename, "w+"); 

    if (!dest_file)
    {
        return errno;
    }

    /* create get request */
    if (location == Z48_MEMORY)
    {
        handle = (unsigned char*) calloc(4, sizeof(unsigned char));
        if (!akai_usb_device_get_handle_by_name(akai_dev, src_filename, handle)) 
        {
            retval = errno;
            free(handle);
            return retval;
        }
        else
        {
            command = (unsigned char*) calloc(4+1, sizeof(unsigned char));
            command[0] = Z48_MEMORY_GET;
            memcpy(command+1, handle, 5 * sizeof(unsigned char));
            retval = usb_bulk_write(akai_dev->dev, EP_OUT, command, 5, timeout);
        }
    }
    else
    {
        command = (unsigned char*) calloc(src_filename_length+1, sizeof(unsigned char));
        command[0] = Z48_DISK_GET;
        memcpy(command+1, src_filename, src_filename_length * sizeof(unsigned char));
        retval = usb_bulk_write(akai_dev->dev, EP_OUT, command, src_filename_length+1, timeout);
    }

    free(command);

    if (retval)
    {
       return retval;
    }

    data = calloc(block_size, sizeof(unsigned char));
#ifdef _POSIX_SOURCE
    gettimeofday(&t1, NULL); // timeval, timezone struct
#endif

    do
    {
        rc = usb_bulk_read(akai_dev->dev, EP_IN, data, block_size, timeout);  

        if (rc == block_size)
        {
            bytes_transferred+= rc;

            /* write to file */
            fwrite(data, sizeof(unsigned char), rc, dest_file);

            /* sent continue request */
            usb_bulk_write(akai_dev->dev, EP_OUT, "\x00", 1, USB_TIMEOUT);  
        }	
        else if (rc == 8)
        {
            /* get the number of bytes to read */
#ifdef _DEBUG
            printf("Current block size: %i. Bytes read now: %i, Total bytes read: %i. Advertised: %i\n", 
                block_size, rc, bytes_transferred, GET_BYTES_TRANSFERRED(data));
#endif
            if (bytes_transferred > 0) 
            {
                block_size = GET_BLOCK_SIZE(data);
                if (block_size == 0)
                {
                    /* file transfer completed */
                    break;
                }
            }
        }
        else if (rc == 4 && z48_reply_ok(data))	
        {
            continue;
        }
        else
        {
            printf("At bulk read: Unexpected reply, rc %i or unexpected end of transmission.\n", rc);
            retval = AKAI_TRANSMISSION_ERROR;
        }

    } while(rc > 0);

    fclose(dest_file);
    free(data);

#ifdef _POSIX_SOURCE
    print_transfer_stats(t1, t2, bytes_transferred);
#endif

    return retval? retval: AKAI_SUCCESS;
}

/* uploads a file to the sampler. */
int akai_usb_device_put(akai_usb_device akai_dev, 
    char *src_filename, char *dest_filename, int timeout)
{
	unsigned char *buf, *command, *reply_buf;
    struct stat* st;
	char destination = 0x0;
	int filesize, rc, blocksize = 4096 * 4, transferred = 0, bytes_read = 0;
    int dest_filename_length = strlen(dest_filename) + 1; 
	FILE* fp;
			
#ifdef _POSIX_SOURCE
	struct timeval t1, t2;
#endif

    /* Get file info */
    st = (struct stat*)malloc(sizeof(struct stat));
    rc = stat(src_filename, st);

    if (rc < 0)
    {
        free(st);
        return rc;
    }

    filesize = st->st_size;
    //  read in st->st_blksize ???
    free(st);
    buf = calloc(filesize, sizeof(unsigned char));

    fp = fopen(src_filename, "r");

    if (!fp)
    {
        return errno;
    }

    printf("File name to upload %s, Size of file: %i bytes\n", dest_filename, filesize);
    /* create 'put' command: 0x41, byte size and the name of the file to transfer */
    command = (unsigned char*) calloc(dest_filename_length+5,  sizeof(unsigned char));
    command[0] = (destination)?Z48_DISK_PUT:Z48_MEMORY_PUT;
    command[1] = filesize >> 24;
    command[2] = filesize >> 16;
    command[3] = filesize >> 8;
    command[4] = filesize;
    memcpy(command+5, dest_filename, dest_filename_length * sizeof(unsigned char));

    rc = usb_bulk_write(akai_dev->dev, EP_OUT, command, dest_filename_length+5, 1000); 

    reply_buf = (unsigned char*) calloc(64, sizeof(unsigned char));

#ifdef _POSIX_SOURCE
  	gettimeofday(&t1, NULL); // timeval, timezone struct
#endif

	do 
	{
		rc = usb_bulk_read(akai_dev->dev, EP_IN, reply_buf, 64, 1000); 

#ifdef _DEBUG
        printf("return code: %i\n", rc);
		int i = 0;
		for (; i < rc; i++)
			printf("%02x ", reply_buf[i]);
		printf("\n");
#endif

        if (rc == 1) continue;

		if (rc == 4 && z48_reply_ok(reply_buf))	
		{
			continue;
		}
		else if (rc == 8)
		{

			blocksize = GET_BLOCK_SIZE(reply_buf);	
            transferred = GET_BYTES_TRANSFERRED(reply_buf);
#ifdef _DEBUG
            printf("blocksize: %i\n", blocksize);
            printf("transferred: %i\n", transferred);
#endif
			if (transferred == filesize) 
			{
                // continue reading last reply 
				continue;
			}
		}
		else if (rc == 5)
		{
			break; // finished TODO: check contents of buffer...
		}

		/* check is probably not necessary */
		if (! feof(fp))
		{
            fseek(fp, transferred, 0);
			bytes_read = fread(buf, sizeof(unsigned char), blocksize, fp);
#ifdef _DEBUG
            printf("writing %i bytes\n", bytes_read);
#endif
			usb_bulk_write(akai_dev->dev, EP_OUT, buf, bytes_read, 1000); 
		}

		/* continue */
		usb_bulk_write(akai_dev->dev, EP_OUT, "\x00", 1, 1000); 
	} while(rc > 0);

	fclose(fp);
	free(reply_buf);
	free(buf);

	if (rc < 0)
	{
		return AKAI_TRANSMISSION_ERROR;
	}

#ifdef _POSIX_SOURCE
	print_transfer_stats(t1, t2, transferred);
#endif
    // XXX: AKAI_SUCCESS;
	return 1;
}


