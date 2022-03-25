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
#include <pthread.h>

#include "se_api.h"


#define SPI_1_DEVICE                    "/dev/spidev1.0"
#define SPI_2_DEVICE                    "/dev/spidev0.0"

#define SPI_1_GPIO                      "/sys/class/gpio/gpio101/value"
#define SPI_2_GPIO                      "/sys/class/gpio/gpio169/value"

#define TEST_VERIFY                     1
#define TEST_SIGN                       2


volatile unsigned int counter_1s;
volatile int exit_sigal = 0;


void sigalrm_func(int sig)
{
    counter_1s++;
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

int spi_1_gpio_status(void)
{
    uint8_t buffer[8];
    static int fd = -1;

    if(fd < 0)
    {
        fd = open(SPI_1_GPIO, O_RDONLY);
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

int spi_2_gpio_status(void)
{
    uint8_t buffer[8];
    static int fd = -1;

    if(fd < 0)
    {
        fd = open(SPI_2_GPIO, O_RDONLY);
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

void *sm2_speed_test(void *arg)
{
    uint8_t IDa[] = {
                    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
                    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    };
    uint8_t prvkey[32];
    uint8_t pubkey[64];
    uint8_t sig[64];
    uint8_t msg[256];

    uint32_t test_counter;
    uint32_t error_counter;

    int fd;
    int ret;
    int old_1s_counter;
    int test_type;
    int *param = arg;

    fd = param[0];
    test_type = param[1];

    ret = se_sm2_keygen(fd, prvkey, pubkey);
    if(ret != 0)
    {
        printf("fd %d se_sm2_keygen failed, ret = %d\n", fd, ret);
        exit(0);
    }

    ret = se_sm2_sign(fd, IDa, sizeof(IDa), prvkey, pubkey, msg, sizeof(msg), sig);
    if(ret != 0)
    {
        printf("fd %d se_sm2_sign failed, ret = %d\n", fd, ret);
        exit(0);
    }
    printf("fd %d se_sm2_sign successful, signature:\r\n", fd);
    dbg_printf("", sig, sizeof(sig));

    ret = se_sm2_verify(fd, IDa, sizeof(IDa), pubkey, msg, sizeof(msg), sig);
    if(ret != 0)
    {
        printf("fd %d se_sm2_verify failed, ret = %d\n", fd, ret);
        exit(0);
    }
    printf("fd %d se_sm2_verify successful\r\n", fd);

    error_counter = 0;
    test_counter = 0;
    old_1s_counter = counter_1s;

    alarm(1);

    while(1)
    {
        if(test_type == TEST_SIGN)
        {
            ret = se_sm2_sign(fd, IDa, sizeof(IDa), prvkey, pubkey, msg, sizeof(msg), sig);
            if(ret != 0)
            {
                // printf("fd %d se_sm2_sign failed, ret = %d\n", fd, ret);
                // break;

                error_counter++;
                usleep(10);
            }

            test_counter++;

            if(old_1s_counter != counter_1s)
            {
                printf("fd %d se_sm2_sign speed %d/s error_counter=%d\n",  fd, test_counter, error_counter);
                old_1s_counter = counter_1s;
                test_counter = 0;
                error_counter = 0;
                alarm(1);
            }
        }
        else
        {
            ret = se_sm2_verify(fd, IDa, sizeof(IDa), pubkey, msg, sizeof(msg), sig);
            if(ret != 0)
            {
                // printf("fd %d se_sm2_verify failed, ret = %d\n", fd, ret);
                // break;

                error_counter++;
                usleep(10);
            }

            test_counter++;

            if(old_1s_counter != counter_1s)
            {
                printf("fd %d se_sm2_verify speed %d/s error_counter=%d\n",  fd, test_counter, error_counter);
                old_1s_counter = counter_1s;
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

    exit(0);
}

int main(int argc, char *argv[])
{
    int ret;
    int fd1 = -1;
    int fd2 = -1;

    uint8_t buffer[256];

    uint8_t key[16];
    uint8_t input[64];
    uint8_t iv[16];
    uint8_t output[64];

    uint8_t prvkey[32];
    uint8_t pubkey[64];

    uint8_t msg[130];

    int test_type = -1;

    int dual_spi = 0;
    int arg1[8], arg2[8];

    pthread_t pthread_id1;
    pthread_t pthread_id2;

    SE_SM3_CTX sm3_ctx;
    int speed = -1;

    uint8_t ccm_key[] =
    {
        0xca,0x44,0xef,0x8d,0xf3,0x25,0xab,0xb3,0x8d,0xac,0x37,0x43,0xdd,0x32,0x43,0xdf
    };
    uint8_t ccm_iv[] =
    {
        0x72,0x43,0x52,0x3C,0x65,0x17,0x8B,0x96,0x68,0x37,0xA3,0x6F
    };
    uint8_t ccm_input[] =
    {
        0x03,0x80,0x14,0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xa0,0xa1,0xa2,
        0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,
    };

    if(argc > 1)
    {
        if(strcmp(argv[1], "verify") == 0)
        {
            test_type = TEST_VERIFY;
            printf("test se verify api\r\n");
        }
        else if(strcmp(argv[1], "sign") == 0)
        {
            test_type = TEST_SIGN;
            printf("test se sign api\r\n");
        }
        else if(strcmp(argv[1], "0") == 0)
        {
            //use spi1
            dual_spi = 0;
            printf("se use %s\r\n", SPI_1_DEVICE);
        }
        else if(strcmp(argv[1], "1") == 0)
        {
            //use spi2
            dual_spi = 1;
            printf("se use %s\r\n", SPI_2_DEVICE);
        }
        else if(strcmp(argv[1], "2") == 0)
        {
            //use sp1 & spi2
            dual_spi = 2;
            printf("dual spi test, %s, %s\r\n", SPI_1_DEVICE, SPI_2_DEVICE);
        }
    }
    else
    {
        printf("acx200t_se_demo usage:\r\n");
        printf("./acx200t_se_demo 0|1 15000000 ——  a single test sm2,sm3,sm4, use SPI0|SPI1, SPI CLK 15MHz\r\n");
        printf("./acx200t_se_demo sign|verify 0|1|2 15000000 —— loop test sm2 sign|verify, use SPI0|SPI1|SPI0&SPI1, SPI CLK 15MHz\r\n");
        exit(0);
    }

    if(argc > 2)
    {
        if(strcmp(argv[2], "0") == 0)
        {
            //use spi1
            dual_spi = 0;
            printf("se use %s\r\n", SPI_1_DEVICE);
        }
        else if(strcmp(argv[2], "1") == 0)
        {
            //use spi2
            dual_spi = 1;
            printf("se use %s\r\n", SPI_2_DEVICE);
        }
        else if(strcmp(argv[2], "2") == 0)
        {
            //use sp1 & spi2
            dual_spi = 2;
            printf("dual spi test, %s, %s\r\n", SPI_1_DEVICE, SPI_2_DEVICE);
        }
    }

    printf("dual_spi: %d\n", dual_spi);

    signal(SIGALRM, sigalrm_func);
    signal(SIGINT, sigexit_func);

    if(dual_spi == 0)
    {
        //use spi1
        fd1 = se_api_init(SPI_1_DEVICE, SPI_INF_MODE, spi_1_gpio_status);
        if(fd1 <= 0)
        {
            printf("se_api_init failed: %d, spi_device = %s\n", fd1, SPI_1_DEVICE);
            return 0;
        }
    }
    else if(dual_spi == 1)
    {
        //use spi2
        fd1 = se_api_init(SPI_2_DEVICE, SPI_INF_MODE, spi_2_gpio_status);
        if(fd1 <= 0)
        {
            printf("se_api_init failed: %d, spi_device = %s\n", fd1, SPI_2_DEVICE);
            return 0;
        }
    }
    else if(dual_spi == 2)
    {
        //use spi1 & spi2
        fd1 = se_api_init(SPI_1_DEVICE, SPI_INF_MODE, spi_1_gpio_status);
        if(fd1 <= 0)
        {
            printf("se_api_init failed: %d, spi_device = %s\n", fd1, SPI_1_DEVICE);
            return 0;
        }

        fd2 = se_api_init(SPI_2_DEVICE, SPI_INF_MODE, spi_2_gpio_status);
        if(fd2 <= 0)
        {
            printf("se_api_init failed: %d, spi_device = %s\n", fd2, SPI_2_DEVICE);
            return 0;
        }
    }

    speed = atoi(argv[argc-1]);
    //spi speed must be 10MHz ~ 40MHz
    if((speed <= 40000000) && (speed >= 10000000))
    {
        printf("new spi speed: %d\n", speed);
        ret = set_spi_speed(fd1, speed);
        printf("set_spi_speed ret = %d\r\n", ret);
    }

    ret = get_ver_info(fd1, buffer);
    printf("get_ver_info ret = %d\r\n", ret);
    if(ret == 0)
    {
        printf("the SE version info: %s\r\n", buffer);
    }

    ret = se_sm2_keygen(fd1, prvkey, pubkey);
    printf("se_sm2_keygen ret = %d\r\n", ret);
    if(ret != 0)
    {
        printf("se_sm2_keygen failed: %d \r\n", ret);
        return 0;
    }

    dbg_printf("se_sm2_keygen private key:\r\n", prvkey, 32);
    dbg_printf("se_sm2_keygen public key:\r\n", pubkey, 64);

    get_rand(key, sizeof(key));
    get_rand(iv, sizeof(iv));
    get_rand(input, sizeof(input));

    get_rand(msg, sizeof(msg));

    se_sm3_init(&sm3_ctx);
    se_sm3_update(&sm3_ctx, msg, sizeof(msg)/2);
    se_sm3_update(&sm3_ctx, msg + (sizeof(msg)/2), sizeof(msg)/2);
    se_sm3_finish(&sm3_ctx, output);

    dbg_printf("sm3 input msg: \r\n", msg, sizeof(msg));
    dbg_printf("sm3 se_sm3_update, se_sm3_finish: \r\n", output, 32);

    memset(output, 0, sizeof(output));
    se_sm3_compute(msg, sizeof(msg), output);
    dbg_printf("sm3 se_sm3_compute: \r\n", output, 32);

    dbg_printf("sm4 ecb key: \r\n", key, sizeof(key));
    dbg_printf("sm4 ecb input: \r\n", input, sizeof(input));

    memset(output, 0, sizeof(output));

    ret = se_sm4_ecb_endec(fd1, SM4_ENCRYPT_OP, key, input, sizeof(input), output);
    if(ret != 0)
    {
        printf("se_sm4_ecb_endec SM4_ENCRYPT_OP failed: %d \r\n", ret);
        return 0;
    }
    dbg_printf("sm4 ecb encrypt end: \r\n", output, sizeof(output));

    ret = se_sm4_ecb_endec(fd1, SM4_DECRYPT_OP, key, output, sizeof(output), buffer);
    if(ret != 0)
    {
        printf("se_sm4_ecb_endec SM4_DECRYPT_OP failed: %d \r\n", ret);
        return 0;
    }

    dbg_printf("sm4 ecb decrypt end: \r\n", buffer, sizeof(output));

    memset(buffer, 0, sizeof(buffer));

    dbg_printf("sm4 cbc key: \r\n", key, sizeof(key));
    dbg_printf("sm4 cbc iv: \r\n", iv, sizeof(iv));
    dbg_printf("sm4 cbc input: \r\n", input, sizeof(input));

    memset(output, 0, sizeof(output));

    ret = se_sm4_cbc_endec(fd1, SM4_ENCRYPT_OP, key, iv, input, sizeof(input), output);
    if(ret != 0)
    {
        printf("se_sm4_cbc_endec SM4_ENCRYPT_OP failed: %d \r\n", ret);
        return 0;
    }
    dbg_printf("sm4 cbc encrypt end: \r\n", output, sizeof(output));

    ret = se_sm4_cbc_endec(fd1, SM4_DECRYPT_OP, key, iv, output, sizeof(output), buffer);
    if(ret != 0)
    {
        printf("se_sm4_cbc_endec SM4_DECRYPT_OP failed: %d \r\n", ret);
        return 0;
    }
    dbg_printf("sm4 cbc decrypt end: \r\n", buffer, sizeof(output));

    dbg_printf("sm4 ccm key: \r\n", ccm_key, sizeof(ccm_key));
    dbg_printf("sm4 ccm iv: \r\n", ccm_iv, sizeof(ccm_iv));
    dbg_printf("sm4 ccm input: \r\n", ccm_input, sizeof(ccm_input));

    memset(output, 0, sizeof(output));

    ret = sm4_ccm_endec(fd1, SM4_ENCRYPT_OP, ccm_key, ccm_iv, sizeof(ccm_iv), ccm_input, sizeof(ccm_input), output);
    if(ret != 0)
    {
        printf("sm4_ccm_endec SM4_ENCRYPT_OP failed: %d \r\n", ret);
        return 0;
    }
    //printf encrypted ciphertext and tag
    dbg_printf("sm4 ccm encrypt end: \r\n", output, sizeof(ccm_input) + 16);

    //input ciphertext, exclude tag
    ret = sm4_ccm_endec(fd1, SM4_DECRYPT_OP, ccm_key, ccm_iv, sizeof(ccm_iv), output, sizeof(ccm_input), buffer);
    if(ret != 0)
    {
        printf("sm4_ccm_endec SM4_DECRYPT_OP failed: %d \r\n", ret);
        return 0;
    }

    //printf plaintext and tag
    dbg_printf("sm4 ccm decrypt end: \r\n", buffer, sizeof(ccm_input) + 16);

    if(test_type < 0)
    {
        return 0;
    }

    printf("test start, it can be stop by Ctrol+C\r\n");

    if(dual_spi == 0)
    {
        arg1[0] = fd1;
        arg1[1] = test_type;

        pthread_create(&pthread_id1, 0, sm2_speed_test, &arg1);
        pthread_detach(pthread_id1);
    }
    else if(dual_spi == 1)
    {
        arg1[0] = fd1;
        arg1[1] = test_type;

        pthread_create(&pthread_id2, 0, sm2_speed_test, &arg1);
        pthread_detach(pthread_id2);
    }
    else if(dual_spi == 2)
    {
        arg1[0] = fd1;
        arg1[1] = test_type;

        pthread_create(&pthread_id1, 0, sm2_speed_test, &arg1);
        pthread_detach(pthread_id1);

        arg2[0] = fd2;
        arg2[1] = test_type;

        pthread_create(&pthread_id2, 0, sm2_speed_test, &arg2);
        pthread_detach(pthread_id2);
    }

    while(1)
    {
        sleep(1);

        if(exit_sigal)
        {
            exit(0);
        }
    }
}

