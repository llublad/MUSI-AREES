#ifndef _MUSI-CA_H_
#define _MUSI-CA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* This file is auto-generated by the pycert_bearssl tool.  Do not change it manually.
 * Certificates are BearSSL br_x509_trust_anchor format.  Included certs:
 *
 * Index:    0
 * Label:    musi-ca
 * Subject:  1.2.840.113549.1.9.1=lluis.bernat2@estudiant.uib.cat,CN=musi-ca,OU=MUSI,O=UIB,L=Soller,ST=Illes Balears,C=ES
 */

#define TAs_NUM 1

static const unsigned char TA_DN0[] = {
    0x30, 0x81, 0x95, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
    0x13, 0x02, 0x45, 0x53, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04,
    0x08, 0x0c, 0x0d, 0x49, 0x6c, 0x6c, 0x65, 0x73, 0x20, 0x42, 0x61, 0x6c,
    0x65, 0x61, 0x72, 0x73, 0x31, 0x0f, 0x30, 0x0d, 0x06, 0x03, 0x55, 0x04,
    0x07, 0x0c, 0x06, 0x53, 0x6f, 0x6c, 0x6c, 0x65, 0x72, 0x31, 0x0c, 0x30,
    0x0a, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x03, 0x55, 0x49, 0x42, 0x31,
    0x0d, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x04, 0x4d, 0x55,
    0x53, 0x49, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c,
    0x07, 0x6d, 0x75, 0x73, 0x69, 0x2d, 0x63, 0x61, 0x31, 0x2e, 0x30, 0x2c,
    0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16,
    0x1f, 0x6c, 0x6c, 0x75, 0x69, 0x73, 0x2e, 0x62, 0x65, 0x72, 0x6e, 0x61,
    0x74, 0x32, 0x40, 0x65, 0x73, 0x74, 0x75, 0x64, 0x69, 0x61, 0x6e, 0x74,
    0x2e, 0x75, 0x69, 0x62, 0x2e, 0x63, 0x61, 0x74,
};

static const unsigned char TA_EC_CURVE0[] = {
    0x04, 0xfb, 0xfe, 0xc9, 0x28, 0xed, 0x93, 0xf8, 0xcd, 0xc0, 0xea, 0x4d,
    0x4c, 0x88, 0x7a, 0xda, 0x19, 0x64, 0xed, 0x81, 0xa5, 0x62, 0x04, 0x26,
    0xf2, 0x29, 0x58, 0x0b, 0x4d, 0x5b, 0x52, 0x8b, 0x7d, 0x74, 0x46, 0x33,
    0x4d, 0xa2, 0x92, 0xc9, 0x1b, 0x07, 0x10, 0x42, 0xac, 0xbd, 0xc5, 0xf3,
    0xb1, 0xe9, 0x57, 0x02, 0x06, 0xc3, 0xe0, 0x3f, 0xe9, 0xc4, 0xf7, 0x6c,
    0x3c, 0x7a, 0x44, 0xae, 0x3b,
};

static const br_x509_trust_anchor TAs[] = {
    {
        { (unsigned char *)TA_DN0, sizeof TA_DN0 },
        BR_X509_TA_CA,
        {
            BR_KEYTYPE_EC,
            { .ec = {BR_EC_secp256r1, (unsigned char *)TA_EC_CURVE0, sizeof TA_EC_CURVE0}
        }
        }
    },
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef _MUSI-CA_H_ */