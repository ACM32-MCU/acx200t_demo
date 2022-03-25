/**
 * Copyright (c) 2021 Shanghai AisinoChip Electronics Technology Co.,Ltd.
 * All rights reserved.
 * @file        se_api.h
 * @ingroup     group_api
 * @brief       SE API Header File
 * @author      Xu Xiaobo
 * @version     V1.0
 * @date        2021-04-08
 */

/**
 * @defgroup group_api The API Group
 *
 * This is the API group
 *
 *
 */

#ifndef  __SE_API_H__
#define __SE_API_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>



///SPI Interface Mode
#define SPI_INF_MODE                        1

///Net Interface Mode
#define NET_INF_MODE                        2

///Check SE GPIO Status Function Define
typedef int (*FUNC_SE_STATUS)();

typedef struct
{
    unsigned long total[2];
    unsigned long state[8];
    unsigned char buffer[64];
    unsigned char ipad[64];
    unsigned char opad[64];
}
SE_SM3_CTX;

///SM4 Encrypt Opteration
#define SM4_ENCRYPT_OP                        0

///SM4 Decrypt Opteration
#define SM4_DECRYPT_OP                        1

/**
 * @brief SE API Init
 * @param[in] path_or_ip: the SPI device path or the Server IP, e.g. "/dev/spidev1.0"
 * @param[in] inf_type: the SPI interface \link SPI_INF_MODE \endlink or Net interface type \link NET_INF_MODE \endlink
 * @param[in] se_status: the SE IO status callback function
 * @return \b Successful: the spi device fd or socket fd
           \b Failed: the error code
 *
 */
int se_api_init(char *path_or_ip, int inf_type, FUNC_SE_STATUS se_status);

/**
 * @brief SE generate a sm2 key pair
 * @param[in] fd: the spi device fd or socket fd
 * @param[out] prv_key: store the sm2 private key buffer
 * @param[out] pub_key: store the sm2 public key buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm2_keygen(int fd, uint8_t *prv_key, uint8_t *pub_key);

/**
 * @brief SE do sm2 signature
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] IDa: the sm2 IDa data buffer
 * @param[in] IDa_len: the sm2 IDa data length
 * @param[in] prv_key: the sm2 signature private key buffer
 * @param[in] pub_key: the sm2 signature public key buffer
 * @param[in] msg: the message data buffer
 * @param[in] msg_len: the message data length
 * @param[out] signature: store sm2 signature buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm2_sign(int fd, uint8_t *IDa, int IDa_len, uint8_t *prv_key, uint8_t *pub_key,
                                  uint8_t *msg, int msg_len, uint8_t *signature);

/**
 * @brief SE do sm2 verify signature
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] IDa: the sm2 IDa data buffer
 * @param[in] IDa_len: the sm2 IDa data length
 * @param[in] pub_key: the sm2 signature public key buffer
 * @param[in] msg: the message data buffer
 * @param[in] msg_len: the message data length
 * @param[in] signature: the sm2 signature buffer to verify
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm2_verify(int fd, uint8_t *IDa, int IDa_len, uint8_t *pub_key,
                                  uint8_t *msg, int msg_len, uint8_t *signature);

/**
 * @brief SE do sm2 public key decompress
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] compress_flag: the compress flag, 0 or 1
 * @param[in] pub_key_x: the sm2 public key x buffer
 * @param[out] pub_key_y: output the the sm2 public key y buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int sm2_pubkey_decompress(int fd, uint8_t compress_flag, uint8_t *pub_key_x, uint8_t *pub_key_y);

/**
 * @brief Import sm2 public key for SE verify test
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] pub_key: the import sm2 public key buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int import_sm2_pubkey(int fd, uint8_t *pub_key);

/**
 * @brief Export sm2 public key for SE sign test
 * @param[in] fd: the spi device fd or socket fd
 * @param[out] pub_key: the export sm2 public key buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int export_sm2_pubkey(int fd, uint8_t *pub_key);

/**
 * @brief SE sm2 signature test
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] msg: the message data buffer
 * @param[in] msg_len: the message data length
 * @param[out] signature: the sm2 signature buffer by sign
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int test_sm2_sign(int fd, uint8_t *msg, int msg_len, uint8_t *signature);

/**
 * @brief SE sm2 verify test
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] msg: the message data buffer
 * @param[in] msg_len: the message data length
 * @param[in] signature: the sm2 signature buffer to verify
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int test_sm2_verify(int fd, uint8_t *msg, int msg_len, uint8_t *signature);

/**
 * @brief Get SE version information
 * @param[in] fd: the spi device fd or socket fd
 * @param[out] ver_info: the SE version information buffer
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int get_ver_info(int fd, uint8_t *ver_info);

/**
 * @brief SE init sm3 context
 * @param[in] ctx: the pointer of SE sm3 context
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm3_init(SE_SM3_CTX *ctx);

/**
 * @brief SE update the sm3 context
 * @param[in] ctx: the pointer of SE sm3 context
 * @param[in] input: the data buffer to update sm3 operation
 * @param[in] len: the data length to update sm3 context operation
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm3_update(SE_SM3_CTX *ctx, uint8_t *input, uint32_t len);

/**
 * @brief SE finish the sm3 operation
 * @param[in] ctx: the pointer of SE sm3 context
 * @param[out] output: the data buffer to store sm3 result
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm3_finish(SE_SM3_CTX *ctx, uint8_t *output);

/**
 * @brief SE compute the data's sm3 hash
 * @param[in] input: the data buffer to compute sm3 hash
 * @param[in] len: the data length of the data
 * @param[out] output: the data buffer to store the sm3 result
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm3_compute(uint8_t *input, uint32_t len, uint8_t *output);

/**
 * @brief SE do sm4 ECB encrypt/decrypt
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] op: the sm4 ECB encrypt \link SM4_ENCRYPT_OP \endlink or decrypt \link SM4_DECRYPT_OP \endlink operation
 * @param[in] key: the sm4 key buffer
 * @param[in] input: the data buffer to encrypt or decrypt
 * @param[in] len: the data length, must be multiples of 16, the maximum is 160
 * @param[out] output: the buffer to store the sm4 ECB result
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm4_ecb_endec(int fd, uint8_t op, uint8_t *key,
                                  uint8_t *input, int len, uint8_t *output);

/**
 * @brief SE do sm4 CBC encrypt/decrypt
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] op: the sm4 ECB encrypt \link SM4_ENCRYPT_OP \endlink or decrypt \link SM4_DECRYPT_OP \endlink operation
 * @param[in] key: the sm4 key buffer
 * @param[in] iv: the sm4 CBC IV buffer
 * @param[in] input: the data buffer to encrypt or decrypt
 * @param[in] len: the data length, must be multiples of 16, the maximum is 160
 * @param[out] output: the buffer to store the sm4 CBC result
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int se_sm4_cbc_endec(int fd, uint8_t op, uint8_t *key, uint8_t *iv,
                                  uint8_t *input, int len, uint8_t *output);

/**
 * @brief SE do sm4 CCM encrypt/decrypt
 * @param[in] fd: the spi device fd or socket fd
 * @param[in] op: the sm4 ECB encrypt \link SM4_ENCRYPT_OP \endlink or decrypt \link SM4_DECRYPT_OP \endlink operation
 * @param[in] key: the sm4 key buffer
 * @param[in] iv: the sm4 CCM IV buffer
 * @param[in] iv_len: the sm4 CCM IV length
 * @param[in] input: the data buffer to encrypt or decrypt
 * @param[in] len: the data length, the maximum is 160
 * @param[out] output: the buffer to store the sm4 CCM result, include cihpertext/plaintext and tag
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int sm4_ccm_endec(int fd, uint8_t op, uint8_t *key, uint8_t *iv, uint8_t iv_len,
                                  uint8_t *input, int len, uint8_t *output);

/**
 * @brief Set SPI transfer speed
 * @param[in] fd: the spi device fd
 * @param[in] speed: the new spi transfer speed to set
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int set_spi_speed(int fd, uint32_t speed);

#ifdef __cplusplus
}
#endif


#endif

