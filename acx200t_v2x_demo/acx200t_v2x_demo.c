#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>

#include "acx200t_api.h"

#define CERT_DIR                        "./cert_list/"


#define SPI_DEVICE                      "/dev/spidev1.0"
#define SE_GPIO                         "/sys/class/gpio/gpio101/value"

#define PDU_LEN                         256
#define TEST_VERIFY                     1
#define TEST_SIGN_OBU                   2
#define TEST_SIGN_RSU                   3

#define OBU_AID                         111
#define RSU_AID                         3620


volatile int timeout_1s = 0;
volatile int exit_sigal = 0;

void sigalrm_func(int sig)
{
    timeout_1s = 1;
}

void sigexit_func(int sig)
{
    exit_sigal = 1;
}

static void get_rand(unsigned char *output, int len)
{
    int i;
    struct timeval tv;

    usleep(1);
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    for(i=0; i<len; i++)
    {
        output[i] = rand() &0xFF;
    }
}

static void dbg_printf(char *title, unsigned char *msg, int len)
{
    int i;

    if(title != NULL)
    {
        printf("%s", title);
    }

    for (i = 0; i < len; i++)
    {
        printf("%02X%s", msg[i], ((i+1) % 16 == 0) ? "\n":" ");
    }
    printf("\n");

    fflush(stdout);
}

int se_gpio_status(void)
{
    uint8_t buffer[8];
    static int fd = -1;

    if(fd < 0)
    {
        fd = open(SE_GPIO, O_RDONLY);
        if(fd < 0)
        {
            return -1;
        }
    }
    else
    {
        lseek(fd, 0, SEEK_SET);
    }

    read(fd, buffer, 1);

    return (buffer[0] == '0') ? 0 : 1;
}

int main(int argc, char *argv[])
{
    char buffer[256];

    uint8_t pdu[PDU_LEN];
    uint8_t spdu[2048];

    int ret;
    int pdu_size;
    int spdu_size;
    uint64_t aid;

    uint32_t test_counter;
    uint32_t error_counter;

    int test_type = TEST_SIGN_OBU;
    int speed;

    if(argc > 1)
    {
        if(strcmp(argv[1], "verify") == 0)
        {
            test_type = TEST_VERIFY;
            printf("test SE_SecDataVerify api\r\n");
        }
        else if(strcmp(argv[1], "sign") == 0)
        {
            if(argc > 2)
            {
                if(strcmp(argv[2], "obu") == 0)
                {
                    test_type = TEST_SIGN_OBU;
                    aid = OBU_AID;
                    printf("test SE_SecDataSign_OBU api\r\n");
                }
                else if(strcmp(argv[2], "rsu") == 0)
                {
                    test_type = TEST_SIGN_RSU;
                    aid = RSU_AID;
                    printf("test SE_SecDataSign_RSU api\r\n");
                }
                else
                {
                    printf("sign data type error: %s, please use obu or rsu\r\n", argv[2]);
                    return 0;
                }
            }
            else
            {
                printf("no sign data type specified, use default obu\r\n");
                printf("test SE_SecDataSign_OBU api\r\n");
                aid = OBU_AID;
            }
        }
    }
    else
    {
        printf("usage: ./acx200t_v2x_demo verify|sign speed\r\n");
        printf("./acx200t_v2x_demo verify => test SE_SecDataVerify api\r\n");
        printf("./acx200t_v2x_demo sign obu|rsu => test SE_SecDataSign_OBU api\r\n");
        printf("./acx200t_v2x_demo verify|sign speed => test in speed SPI CLK, e.g. 20000000\r\n");
        return 0;
    }

    if(access(CERT_DIR, R_OK))
    {
        printf("the cert dir: %s not exist, please copy the cert_list dir to there.\r\n", CERT_DIR);
        return 0;
    }

    signal(SIGALRM, sigalrm_func);
    signal(SIGINT, sigexit_func);

    ret = SE_Init(se_gpio_status, CERT_DIR, SPI_DEVICE);
    if(ret != 0)
    {
        printf("SE_Init failed: ret = %d\r\n", ret);
        return ret;
    }

    speed = atoi(argv[argc-1]);
    //spi speed must be 10MHz ~ 60MHz
    if((speed <= 40000000) && (speed >= 10000000))
    {
        printf("new spi speed: %dHz\n", speed);
        ret = SE_SetSpiSpeed(speed);
        printf("set_spi_speed ret = %d\r\n", ret);
    }

    //if input NULL to SE_FactoryCheck, then no message output
    ret = SE_FactoryCheck(buffer);
    printf("SE_FactoryCheck ret = %d\r\n", ret);
    if(ret == 0)
    {
        printf("the SE Info: %s\r\n", buffer);
    }

    get_rand(pdu, PDU_LEN);

    printf("test start, it can be stop by Ctrol+C\r\n");

    pdu_size = sizeof(pdu);

    ret = SE_SecDataSign_OBU(aid, pdu, pdu_size, spdu, &spdu_size);
    if(ret != 0)
    {
        printf("SE_SecDataSign_OBU ret: %d\r\n", ret);
        return 0;
    }
    dbg_printf("SE_SecDataSign_OBU spdu:\r\n", spdu, spdu_size);

    test_counter = 0;
    error_counter = 0;
    alarm(1);

    while(1)
    {
        if(test_type == TEST_VERIFY)
        {
            ret = SE_SecDataVerify(spdu, spdu_size, pdu, &pdu_size, &aid);
            if(ret != 0)
            {
                error_counter++;
                usleep(10);

                // printf("SE_SecDataVerify failed ret = %d\r\n", ret);
                // break;
            }
            test_counter++;

            if(timeout_1s != 0)
            {
                usleep(1000);
                printf("test SE_SecDataVerify speed %d/s error_counter=%d\n", test_counter, error_counter);
                timeout_1s = 0;
                test_counter = 0;
                error_counter = 0;
                alarm(1);
            }
        }
        else if(test_type == TEST_SIGN_OBU)
        {
            ret = SE_SecDataSign_OBU(aid, pdu, pdu_size, spdu, &spdu_size);
            if(ret != 0)
            {
                error_counter++;
                usleep(10);
            }
            test_counter++;

            if(timeout_1s != 0)
            {
                usleep(1000);
                printf("test SE_SecDataSign_OBU speed %d/s error_counter=%d\n", test_counter, error_counter);
                timeout_1s = 0;
                test_counter = 0;
                error_counter = 0;
                alarm(1);
            }
        }
        else
        {
            ret = SE_SecDataSign_RSU(aid, pdu, pdu_size, spdu, &spdu_size);
            if(ret != 0)
            {
                error_counter++;
                usleep(10);
            }
            test_counter++;

            if(timeout_1s != 0)
            {
                usleep(1000);
                printf("test SE_SecDataSign_RSU speed %d/s error_counter=%d\n", test_counter, error_counter);
                timeout_1s = 0;
                test_counter = 0;
                error_counter = 0;
                alarm(1);
            }
        }

        if(exit_sigal)
        {
            printf("test all counter: %d, exit\n", test_counter);
            exit(0);
        }
    }
}

