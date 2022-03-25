/**
 * Copyright (c) 2021 Shanghai AisinoChip Electronics Technology Co.,Ltd.
 * All rights reserved.
 * @file        acx200t_api.h
 * @ingroup     group_api
 * @brief       ACX200T API Header File
 * @author      Xu Xiaobo
 * @version     V1.0
 * @date        2021-04-08
 */

/**
* @mainpage SE Library
*
* <b>Introduction</b>
*
* This user manual describes the ACX200T API Library
*/

/**
 * @defgroup group_api The API Group
 *
 * This is the API group
 *
 *
 */

#ifndef  __ACX200T_API_H__
#define __ACX200T_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


typedef int (*FUNC_SE_STATUS)();

///正确
#define Err_ok                                                               0

///安全消息版本号信息非法
#define Err_incorrectSecureMessageVersion                                    1

///签名者信息中签名方式非法
#define Err_incorrectSignerType                                              2

///数字证书版本号信息非法
#define Err_incorrectCertVersion                                             3

///数字证书签发者信息非法
#define Err_incorrectCertIssueDigest                                         4

///数字证书中待签主题信息非法
#define Err_incorrectCertSubjectInfo                                         5

///数字证书中待签主题属性信息非法
#define Err_incorrectCertSubjectAttribute                                    6

///数字证书中有效限定信息非法（根据 CA 时间）
#define Err_incorrectCertValidityPeriod                                      7

///数字证书中有效时间信息非法
#define Err_incorrectCertTimeStartAndEnd                                     8

///数字证书父子关系非法
#define Err_incorrectSubcertAuthority                                        9

///证书链非法
#define Err_incorrectCertChain                                               10

///数字证书签名信息非法
#define Err_incorrectCertSignature                                           11

///待签数据中数据产生时间信息非法
#define Err_incorrectTbsDataGenTime                                          12

///待签数据中杂凑算法信息非法
#define Err_incorrectTbsDataHashAlg                                          13

///待签数据中 AID 信息非法
#define Err_incorrectTbsDataItsAid                                           14

///安全消息中签名信息非法
#define Err_incorrectSignedMessageSignature                                  15

///证书类型错误(显性/隐性)
#define Err_incorrectCertType                                                16

///linkvalue错误
#define Err_incorrectCertLinkvalue                                           17

///公钥信息错误
#define Err_incorrectCertPubkey                                              18

///CRA ID 错误
#define Err_craCaId                                                          19

///签名方式非法
#define Err_signedMessageSignWay                                             20

///数字证书的 hash 算法非法
#define Err_incorrectCertHash                                                21

///数字证书的签名类型非法
#define Err_incorrectCertSignatureType                                       22

///无效参数
#define Err_incorrectParam                                                   23

///签发 SecData 时，参数中的证书类型错误
#define Err_certType                                                         24

///OER 编码失败
#define Err_oerEncodeFailed                                                  25

///OER 解码失败
#define Err_oerDecodeFailed                                                  26

///计算摘要失败
#define Err_computeDigestFailed                                              27

///无有效的签名 PC 证书
#define Err_invalidSignPc                                                    28

///签名失败
#define Err_signedFailed                                                     29

///重复消息
#define Err_repeatedMsg                                                      30

///签名证书已被撤销
#define Err_pcHasBeenRevoked                                                 31

///未发现小模式对应的 PC 证书（小模式）
#define Err_notFoundCorrespondPc                                             32

///签名时未找到对应 j 值的证书（签发时）
#define Err_notFoundSpecifyCert                                              33

///安全消息过期
#define Err_signedMessageExpired                                             34

///安全消息过期
#define Err_noSignedData                                                     35

/**
 * @brief SE API Initial
 * @param[in] cb: the SE busy/idle GPIO status callback function, return the gpio value, 0 or 1
 * @param[in] cert_dir: the CERT directory,  e.g. pc.key, pc.cert, ac.key, ac.cert in the directory
 * @param[in] fileName: the SPI device file, e.g.,"/dev/spidev1.0"
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_Init(FUNC_SE_STATUS cb, const char *cert_dir, const char *fileName);

/**
 * @brief Check SE chip working state
 * @param[out] info: the version informate buffer, need 64 bytes
 *              \b if SE chip work well , it can output the version informate,
 *              \b if don't care info, please input NULL
 * @return \b Successful: 0, the SE work well
           \b Failed: the error code
 *
 */
uint32_t SE_FactoryCheck(char *info);

/**
 * @brief Sign the RSU message, output the RSU V2XSecData
 * @param[in] aid: the RSU device's AID
 * @param[in] pdu: the RSU message buffer
 * @param[in] pdu_size: the RSU message length
 * @param[out] spdu: the RSU V2XSecData buffer
 * @param[out] spdu_size: the RSU V2XSecData length
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_SecDataSign_RSU(uint64_t aid, uint8_t *pdu, int32_t pdu_size, uint8_t *spdu, int32_t *spdu_size);

/**
 * @brief Sign the OBU message, output the OBU V2XSecData
 * @param[in] aid: the OBU device's AID
 * @param[in] pdu: the OBU message buffer
 * @param[in] pdu_size: the OBU message length
 * @param[out] spdu: the OBU V2XSecData buffer
 * @param[out] spdu_size: the OBU V2XSecData length
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_SecDataSign_OBU(uint64_t aid, uint8_t *pdu, int32_t pdu_size, uint8_t *spdu, int32_t *spdu_size);

/**
 * @brief Verify the V2XSecData, output the v2x message and AID
 * @param[in] spdu: the V2XSecData buffer
 * @param[in] spdu_size: the V2XSecData length
 * @param[out] pdu: the v2x message buffer
 * @param[out] pdu_size: the v2x message length
 * @param[out] p_pu64AID: the V2XSecData's AID
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_SecDataVerify(uint8_t *spdu, int32_t spdu_size,uint8_t *pdu, int32_t *pdu_size, uint64_t *p_pu64AID);


/**
 * @brief Release the SE API resource
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_SecChipDeinit(void);

/**
 * @brief Set SPI transfer speed
 * @param[in] speed: the new spi transfer speed to set, e.g. 20000000 means 20MHz
 * @return \b Successful: 0
           \b Failed: the error code
 *
 */
int32_t SE_SetSpiSpeed(uint32_t speed);


#ifdef __cplusplus
}
#endif


#endif

