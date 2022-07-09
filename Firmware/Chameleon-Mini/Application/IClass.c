/*
 * IClass.c
 *
 *  Created on: 17-05-2020
 *      Author: NVX
 */

#include "ISO15693-A.h"
#include "IClass.h"

#define ICLASS_BLOCK_CSN       0  // CSN
#define ICLASS_BLOCK_CFG       1  // Configuration block
#define ICLASS_BLOCK_EPURSE    2  // e-purse / Card Challenge
#define ICLASS_BLOCK_KD        3  // Kd (Debit Key)
#define ICLASS_BLOCK_KC        4  // Kc (Credit Key)
#define ICLASS_BLOCK_AIA       5  // Application Issuer Area
#define ICLASS_BLOCK_APP1      6  // Start of Application 1 (HID)
#define ICLASS_BLOCK_APP2      19 // Start of Applicaiton 2 (User)

#define ICLASS_FUSE_PERS    0x80 // Personalization Mode
#define ICLASS_FUSE_CRYPT1  0x10 // Crypt1 // 1+1 (crypt1+crypt0) means secured and keys changable
#define ICLASS_FUSE_CRTPT0  0x08 // Crypt0 // 1+0 means secure and keys locked, 0+1 means not secured, 0+0 means disable auth entirely
#define ICLASS_FUSE_CRYPT10 ( ICLASS_FUSE_CRYPT1 | ICLASS_FUSE_CRTPT0 )
#define ICLASS_FUSE_RA      0x01 // Read Access, 1 meanns anonymous read enabled, 0 means must auth to read applicaion

// Low nibble used for command
// High nibble used for options and checksum (MSB)
// The only option we care about in 15693 mode is the key
// which is only used by READCHECK, so for simplicity we
// don't bother breaking down the command and flags into parts
#define ICLASS_CMD_READ_OR_IDENTIFY 0x0C // READ: ADDRESS(1) CRC16(2) -> DATA(8) CRC16(2) _OR_ IDENTIFY: No args -> ASNB(8) CRC16(2)
#define ICLASS_CMD_READ4            0x06 // ADDRESS(1) CRC16(2) -> DATA(32) CRC16(2)
#define ICLASS_CMD_UPDATE           0x87 // ADDRESS(1) DATA(8) SIGN(4)|CRC16(2) -> DATA(8) CRC16(2)
#define ICLASS_CMD_READCHECK_KD     0x88 // ADDRESS(1) -> DATA(8)
#define ICLASS_CMD_READCHECK_KC     0x18 // ADDRESS(1) -> DATA(8)
#define ICLASS_CMD_CHECK            0x05 // CHALLENGE(4) READERSIGNATURE(4) -> CHIPRESPONSE(4)
#define ICLASS_CMD_ACTALL           0x0A // No args -> SOF
#define ICLASS_CMD_ACT              0x8E // No args -> SOF
#define ICLASS_CMD_SELECT           0x81 // ASNB(8)|SERIALNB(8) -> SERIALNB(8) CRC16(2)
#define ICLASS_CMD_DETECT           0x0F // No args -> SERIALNB(8) CRC16(2)
#define ICLASS_CMD_HALT             0x00 // No args -> SOF
#define ICLASS_CMD_PAGESEL          0x84 // PAGE(1) CRC16(2) -> BLOCK1(8) CRC16(2)

#define HAS_MASK(x,b) ( (x&b) == b )

static enum {
    STATE_HALT,
    STATE_IDLE,
    STATE_ACTIVE,
    STATE_SELECTED
} State;

const uint8_t ffBlock[ICLASS_BLOCK_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/*
 * Definition 1 (Cipher state). A cipher state of iClass s is an element of F 40/2
 * consisting of the following four components:
 *   1. the left register l = (l 0 . . . l 7 ) ∈ F 8/2 ;
 *   2. the right register r = (r 0 . . . r 7 ) ∈ F 8/2 ;
 *   3. the top register t = (t 0 . . . t 15 ) ∈ F 16/2 .
 *   4. the bottom register b = (b 0 . . . b 7 ) ∈ F 8/2 .
 */
typedef struct {
    uint8_t l;
    uint8_t r;
    uint8_t b;
    uint16_t t;
} CipherState_t;

uint8_t CurrentCSN[ICLASS_CSN_SIZE];
uint8_t CurrentKeyBlockNum; // Used as current csn in loclass reader attack mode
uint8_t CurrentKey[ICLASS_BLOCK_SIZE];
CipherState_t CipherState;

#define NUM_CSNS 9
// CSNs from Proxmark3 repo
static const uint8_t loclassCSNs[ICLASS_CSN_SIZE * NUM_CSNS] PROGMEM = {
    0x01, 0x0A, 0x0F, 0xFF, 0xF7, 0xFF, 0x12, 0xE0,
    0x0C, 0x06, 0x0C, 0xFE, 0xF7, 0xFF, 0x12, 0xE0,
    0x10, 0x97, 0x83, 0x7B, 0xF7, 0xFF, 0x12, 0xE0,
    0x13, 0x97, 0x82, 0x7A, 0xF7, 0xFF, 0x12, 0xE0,
    0x07, 0x0E, 0x0D, 0xF9, 0xF7, 0xFF, 0x12, 0xE0,
    0x14, 0x96, 0x84, 0x76, 0xF7, 0xFF, 0x12, 0xE0,
    0x17, 0x96, 0x85, 0x71, 0xF7, 0xFF, 0x12, 0xE0,
    0xCE, 0xC5, 0x0F, 0x77, 0xF7, 0xFF, 0x12, 0xE0,
    0xD2, 0x5A, 0x82, 0xF8, 0xF7, 0xFF, 0x12, 0xE0
};

static void loclassSetCSN(void) {
    memcpy_P(CurrentCSN, loclassCSNs + (CurrentKeyBlockNum * ICLASS_CSN_SIZE), ICLASS_CSN_SIZE);
    MemoryWriteBlock(CurrentCSN, ICLASS_BLOCK_CSN * ICLASS_BLOCK_SIZE, ICLASS_CSN_SIZE);
}

// Crypto borrowed from Proxmark3 implementation and tweaked to run fast enough
// https://github.com/RfidResearchGroup/proxmark3/blob/e550f8ccc85b745e3961a096b5e3a602adaa5034/armsrc/optimized_cipher.c
static const uint8_t opt_select_LUT[256] = {
    00, 03, 02, 01, 02, 03, 00, 01, 04, 07, 07, 04, 06, 07, 05, 04,
    01, 02, 03, 00, 02, 03, 00, 01, 05, 06, 06, 05, 06, 07, 05, 04,
    06, 05, 04, 07, 04, 05, 06, 07, 06, 05, 05, 06, 04, 05, 07, 06,
    07, 04, 05, 06, 04, 05, 06, 07, 07, 04, 04, 07, 04, 05, 07, 06,
    06, 05, 04, 07, 04, 05, 06, 07, 02, 01, 01, 02, 00, 01, 03, 02,
    03, 00, 01, 02, 00, 01, 02, 03, 07, 04, 04, 07, 04, 05, 07, 06,
    00, 03, 02, 01, 02, 03, 00, 01, 00, 03, 03, 00, 02, 03, 01, 00,
    05, 06, 07, 04, 06, 07, 04, 05, 05, 06, 06, 05, 06, 07, 05, 04,
    02, 01, 00, 03, 00, 01, 02, 03, 06, 05, 05, 06, 04, 05, 07, 06,
    03, 00, 01, 02, 00, 01, 02, 03, 07, 04, 04, 07, 04, 05, 07, 06,
    02, 01, 00, 03, 00, 01, 02, 03, 02, 01, 01, 02, 00, 01, 03, 02,
    03, 00, 01, 02, 00, 01, 02, 03, 03, 00, 00, 03, 00, 01, 03, 02,
    04, 07, 06, 05, 06, 07, 04, 05, 00, 03, 03, 00, 02, 03, 01, 00,
    01, 02, 03, 00, 02, 03, 00, 01, 05, 06, 06, 05, 06, 07, 05, 04,
    04, 07, 06, 05, 06, 07, 04, 05, 04, 07, 07, 04, 06, 07, 05, 04,
    01, 02, 03, 00, 02, 03, 00, 01, 01, 02, 02, 01, 02, 03, 01, 00
};

__attribute__((optimize("-O3"))) INLINE void opt_successor(const uint8_t *k, CipherState_t *s, uint8_t y) {
    uint16_t Tt = s->t & 0xc533;
    Tt = Tt ^ (Tt >> 1);
    Tt = Tt ^ (Tt >> 4);
    Tt = Tt ^ (Tt >> 10);
    Tt = Tt ^ (Tt >> 8);

    s->t = (s->t >> 1);
    s->t |= (Tt ^ (s->r >> 7) ^ (s->r >> 3)) << 15;

    uint8_t opt_B = s->b;
    opt_B ^= s->b >> 6;
    opt_B ^= s->b >> 5;
    opt_B ^= s->b >> 4;

    s->b = s->b >> 1;
    s->b |= (opt_B ^ s->r) << 7;

    uint8_t opt_select = opt_select_LUT[s->r] & 0x04;
    opt_select |= (opt_select_LUT[s->r] ^ ((Tt ^ y) << 1)) & 0x02;
    opt_select |= (opt_select_LUT[s->r] ^ Tt) & 0x01;

    uint8_t r = s->r;
    s->r = (k[opt_select] ^ s->b) + s->l;
    s->l = s->r + r;
}

__attribute__((optimize("-O3"))) INLINE void opt_suc(const uint8_t *k, CipherState_t *s, const uint8_t *in) {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t head;
        head = in[i];
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);

        head >>= 1;
        opt_successor(k, s, head);
    }
}

__attribute__((optimize("-O3"))) INLINE void opt_output(const uint8_t *k, CipherState_t *s, uint8_t *buffer) {
    for (uint8_t times = 0; times < 4; times++) {
        uint8_t bout = 0;
        bout |= (s->r & 0x4) >> 2;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) >> 1;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4);
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) << 1;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) << 2;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) << 3;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) << 4;
        opt_successor(k, s, 0);
        bout |= (s->r & 0x4) << 5;
        opt_successor(k, s, 0);
        buffer[times] = bout;
    }
}

/*
 * The tag MAC can be divided (both can, but no point in dividing the reader mac) into
 * two functions, since the first 8 bytes are known, we can pre-calculate the state
 * reached after feeding CC to the cipher.
 * @param cc_p
 * @param div_key_p
 * @return the cipher state
 */
static CipherState_t IClassDoTagMAC1(uint8_t *cc_p, const uint8_t *div_key_p) {
    CipherState_t _init = {
        ((div_key_p[0] ^ 0x4c) + 0xEC) & 0xFF,// l
        ((div_key_p[0] ^ 0x4c) + 0x21) & 0xFF,// r
        0x4c, // b
        0xE012 // t
    };
    opt_suc(div_key_p, &_init, cc_p);
    return _init;
}

/*
 * The second part of the tag MAC calculation, since the CC is already calculated into the state,
 * this function is fed only the NR, and internally feeds the remaining 32 0-bits to generate the tag
 * MAC response.
 * @param _init - precalculated cipher state
 * @param nr - the reader challenge
 * @param mac - where to store the MAC
 * @param div_key_p - the key to use
 */
__attribute__((optimize("-O3"))) static void IClassDoTagMAC2(CipherState_t _init, uint8_t *nr, uint8_t mac[4], const uint8_t *div_key_p) {
    opt_suc(div_key_p, &_init, nr);
    opt_output(div_key_p, &_init, mac);
}

// 0x06 input should return 0x4556
static uint16_t iClassCRC16(void *buf, uint16_t size) {
    uint16_t reg = 0xE012;
    uint8_t i, j;

    uint8_t *DataPtr = (uint8_t *)buf;

    for (i = 0; i < size; i++) {
        reg = reg ^ *DataPtr++;
        for (j = 0; j < 8; j++) {
            if (reg & 0x0001) {
                reg = (reg >> 1) ^ ISO15693_CRC16_POLYNORMAL;
            } else {
                reg = (reg >> 1);
            }
        }
    }

    return reg;
}

static void iClassAppendCRC(uint8_t *buf, uint16_t size) {
    uint16_t crc = iClassCRC16(buf, size);

    buf[size] = crc & 0xFF;
    buf[size + 1] = crc >> 8;
}

// Borrowed from Proxmark3 repo
static void makeAntiCollCsn(const uint8_t *original_csn, uint8_t *rotated_csn) {
    for (uint8_t i = 0; i < ICLASS_BLOCK_SIZE; i++) {
        rotated_csn[i] = (original_csn[i] >> 3) | (original_csn[(i + 1) % 8] << 5);
    }
}

static void initCipherState(void) {
    uint8_t ePurse[ICLASS_BLOCK_SIZE];
    MemoryReadBlock(ePurse, ICLASS_BLOCK_EPURSE * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
    MemoryReadBlock(CurrentKey, CurrentKeyBlockNum * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);

    CipherState = IClassDoTagMAC1(ePurse, CurrentKey);
}

void IClassAppInit(void) {
    MemoryReadBlock(CurrentCSN, ICLASS_BLOCK_CSN * ICLASS_BLOCK_SIZE, ICLASS_CSN_SIZE);
    DetectionMode = false;
    CurrentKeyBlockNum = 0; // Force IClassAppReset to re-init cipher state
    IClassAppReset();
}

void IClassDetectionInit(void) {
    State = STATE_IDLE;

    DetectionMode = true;
    CurrentKeyBlockNum = 0;

    // Setup card

    // block 0
    loclassSetCSN();

    // block 1
    const uint8_t confBlock[ICLASS_BLOCK_SIZE] = {0x12, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0xFF, 0x3C};
    MemoryWriteBlock(confBlock, ICLASS_BLOCK_CFG * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);

    // block 2
    const uint8_t ePurse[ICLASS_BLOCK_SIZE] = {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    MemoryWriteBlock(ePurse, ICLASS_BLOCK_EPURSE * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);

    // block 3 is Kd and block 4 is Kc, neither are relevant in this mode

    // block 5
    MemoryWriteBlock(ffBlock, ICLASS_BLOCK_AIA * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);

    DetectionLogToFlash(LOG_INFO_SYSTEM_BOOT, NULL, 0);
}

void IClassAppReset(void) {
    State = STATE_IDLE;

    if (DetectionMode == false && CurrentKeyBlockNum != ICLASS_BLOCK_KD) {
        CurrentKeyBlockNum = ICLASS_BLOCK_KD;
        initCipherState();
    }
}

uint16_t IClassAppProcess(uint8_t *FrameBuf, uint16_t FrameBytes) {
    uint8_t keyBlockNum = ICLASS_BLOCK_KD;

    switch (FrameBuf[0]) {
        case ICLASS_CMD_ACTALL: // No args
            if (FrameBytes != 1) {
                return ISO15693_APP_NO_RESPONSE;
            }

            if (State != STATE_HALT) {
                State = STATE_ACTIVE;
            }
            return ISO15693_APP_SOF_ONLY;
        case ICLASS_CMD_ACT: // No args
            if (FrameBytes != 1 || State != STATE_ACTIVE) {
                return ISO15693_APP_NO_RESPONSE;
            }

            return ISO15693_APP_SOF_ONLY;
        case ICLASS_CMD_HALT: // No args
            if (FrameBytes != 1 || State != STATE_SELECTED) {
                return ISO15693_APP_NO_RESPONSE;
            }

            State = STATE_HALT;
            return ISO15693_APP_SOF_ONLY;
        case ICLASS_CMD_READ_OR_IDENTIFY:
            if (FrameBytes == 1 && State == STATE_ACTIVE) { // ICLASS_CMD_IDENTIFY
                // ASNB(8) CRC16(2)
                makeAntiCollCsn(CurrentCSN, FrameBuf);
                iClassAppendCRC(FrameBuf, ICLASS_CSN_SIZE);
                return ICLASS_CSN_SIZE+2;
            } else if (FrameBytes == 4 && State == STATE_SELECTED) { // ICLASS_CMD_READ ADDRESS(1) CRC16(2)
                if (FrameBuf[1] >= ICLASS_BLOCK_NUM) {
                    return ISO15693_APP_NO_RESPONSE;
                }

                // TODO: Check CRC?
                // TODO: Check auth?

                // DATA(8) CRC16(2)
                if (FrameBuf[1] == ICLASS_BLOCK_KD || FrameBuf[1] == ICLASS_BLOCK_KD) {
                    // Reading Kd or Kc blocks always returns FF's
                    memcpy(FrameBuf, ffBlock, ICLASS_BLOCK_SIZE);
                } else {
                    MemoryReadBlock(FrameBuf, FrameBuf[1] * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
                }
                iClassAppendCRC(FrameBuf, ICLASS_BLOCK_SIZE);
                return ICLASS_BLOCK_SIZE+2;
            }

            return ISO15693_APP_NO_RESPONSE;
        case ICLASS_CMD_READ4: // ADDRESS(1) CRC16(2)
            if (FrameBytes != 4 || State != STATE_SELECTED || FrameBuf[1]+4 >= ICLASS_BLOCK_NUM) {
                return ISO15693_APP_NO_RESPONSE;
            }

            // TODO: Check CRC?
            // TODO: Check auth?

            uint8_t blockNum = FrameBuf[1];

            // DATA(32) CRC16(2)
            MemoryReadBlock(FrameBuf, blockNum * ICLASS_BLOCK_SIZE, ICLASS_READ4_SIZE);
            if (blockNum == 4) {
                // Kc is block 4, so just redact first block of response
                memcpy(FrameBuf, ffBlock, ICLASS_BLOCK_SIZE);
            } else if (blockNum < 4) {
                // Kd is block 3
                uint8_t *kdOffset = FrameBuf+((3-blockNum)*ICLASS_BLOCK_SIZE);
                memcpy(kdOffset, ffBlock, ICLASS_BLOCK_SIZE);
                if (blockNum != 0) {
                    // Redact Kc
                    memcpy(kdOffset+ICLASS_BLOCK_SIZE, ffBlock, ICLASS_BLOCK_SIZE);
                }
            }
            iClassAppendCRC(FrameBuf, ICLASS_READ4_SIZE);
            return ICLASS_READ4_SIZE + 2;
        case ICLASS_CMD_SELECT: // ASNB(8)|SERIALNB(8)
            if (FrameBytes != 9) {
                return ISO15693_APP_NO_RESPONSE;
            }

            uint8_t selectCsn[ICLASS_CSN_SIZE];
            if (State == STATE_HALT || State == STATE_IDLE) {
                memcpy(selectCsn, CurrentCSN, ICLASS_CSN_SIZE);
            } else {
                makeAntiCollCsn(CurrentCSN, selectCsn);
            }

            if (memcmp(FrameBuf+1, selectCsn, ICLASS_CSN_SIZE)) {
                if (State == STATE_ACTIVE) {
                    State = STATE_IDLE;
                } else if (State == STATE_SELECTED) {
                    State = STATE_HALT;
                }

                return ISO15693_APP_NO_RESPONSE;
            }

            State = STATE_SELECTED;

            // SERIALNB(8) CRC16(2)
            memcpy(FrameBuf, CurrentCSN, ICLASS_CSN_SIZE);
            iClassAppendCRC(FrameBuf, ICLASS_CSN_SIZE);

            return ICLASS_CSN_SIZE+2;
        case ICLASS_CMD_READCHECK_KC: // ADDRESS(1)
            keyBlockNum = ICLASS_BLOCK_KC;
            // fallthrough
        case ICLASS_CMD_READCHECK_KD: // ADDRESS(1)
            if (FrameBytes != 2 || FrameBuf[1] != ICLASS_BLOCK_EPURSE || State != STATE_SELECTED) {
                return ISO15693_APP_NO_RESPONSE;
            }

            if (CurrentKeyBlockNum != keyBlockNum && !DetectionMode) {
                CurrentKeyBlockNum = keyBlockNum;
                initCipherState();
            }

            // DATA(8)
            MemoryReadBlock(FrameBuf, FrameBuf[1] * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
            return ICLASS_BLOCK_SIZE;
        case ICLASS_CMD_CHECK: // CHALLENGE(4) READERSIGNATURE(4)
            if (FrameBytes != 9 || State != STATE_SELECTED) {
                return ISO15693_APP_NO_RESPONSE;
            }

            if (DetectionMode) {
                // LOCLASS Reader attack mode
                // Save <CSN 8><CC 8><NR 4><MAC 4>
                uint8_t loclassLog[24];
                MemoryReadBlock(loclassLog, ICLASS_BLOCK_CSN * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
                MemoryReadBlock(loclassLog + ICLASS_BLOCK_SIZE, ICLASS_BLOCK_EPURSE * ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
                memcpy(loclassLog + (ICLASS_BLOCK_SIZE * 2), FrameBuf + 1, 8); // Copy CHALLENGE (nr) and READERSIGNATURE (mac)
                DetectionLogToFlash(LOG_INFO_APP_AUTHING, loclassLog, sizeof(loclassLog));

                // Rotate to the next CSN
                CurrentKeyBlockNum = (CurrentKeyBlockNum + 1) % NUM_CSNS;
                loclassSetCSN();

                State = STATE_IDLE;

                return ISO15693_APP_NO_RESPONSE;
            }

            // TODO: Validate READERSIGNATURE?

            // CHIPRESPONSE(4)
            ISO15693StartEarlySend(false, 4);
            // for speed reasons, IClassDoTagMAC2 requires 4 bytes of trailing 00's after the NR
            memset(FrameBuf + 5, 0, 4);
            IClassDoTagMAC2(CipherState, FrameBuf + 1, FrameBuf, CurrentKey);
            return ISO15693_APP_EARLY_SEND;
        case ICLASS_CMD_UPDATE: // ADDRESS(1) DATA(8) SIGN(4)|CRC16(2)
            if ((FrameBytes != 12 && FrameBytes != 14) || State != STATE_SELECTED) {
                return ISO15693_APP_NO_RESPONSE;
            }

            if (FrameBuf[1] >= ICLASS_BLOCK_NUM) {
                return ISO15693_APP_NO_RESPONSE;
            }

            uint8_t cfgBlock[ICLASS_BLOCK_SIZE];
            MemoryReadBlock(cfgBlock, ICLASS_BLOCK_CFG*ICLASS_BLOCK_SIZE, ICLASS_BLOCK_SIZE);
            bool persMode = HAS_MASK(cfgBlock[7], ICLASS_FUSE_PERS);

            if ((FrameBuf[1] == ICLASS_BLOCK_CSN) // CSN is always read only
                || (!persMode && !HAS_MASK(cfgBlock[3], 0x80)) // Chip is in RO mode, no updated possible (even ePurse)
                || (!persMode && FrameBuf[1] == ICLASS_BLOCK_AIA) // AIA can only be set in personalisation mode
                || (!persMode && (FrameBuf[1] == ICLASS_BLOCK_KD || FrameBuf[1] == ICLASS_BLOCK_KC) && (!HAS_MASK(cfgBlock[7], ICLASS_FUSE_CRYPT10)))
               ) {
                return ISO15693_APP_NO_RESPONSE; // TODO: Is this the right response?
            }

            if (FrameBuf[1] >= 6 && FrameBuf[1] <= 12) {
                if (!HAS_MASK(cfgBlock[3], 1 << (FrameBuf[1] - 6))) { // bit0 is block6, up to bit6 being block12
                    // Block is marked as read-only, deny writing
                    return ISO15693_APP_NO_RESPONSE; // TODO: Is this the right response?
                }
            }

            // TODO: Check CRC/SIGN depending on if in secure mode
            // Check correct key
            // -> Kd only allows decrementing e-Purse
            // -> per-app controlled by key access config
            //bool keyAccess = HAS_MASK(cfgBlock[5], 0x01);
            // -> must auth with that key to change it

            uint8_t blockOffset = FrameBuf[1] * ICLASS_BLOCK_SIZE;
            uint8_t block[ICLASS_BLOCK_SIZE];
            switch (FrameBuf[1]) {
            case ICLASS_BLOCK_CFG:
                block[0] = cfgBlock[0]; // Applications Limit
                block[1] = cfgBlock[1] & FrameBuf[3]; // OTP
                block[2] = cfgBlock[2] & FrameBuf[4]; // OTP
                block[3] = cfgBlock[3] & FrameBuf[5];// Block Write Lock
                block[4] = cfgBlock[4]; // Chip Config
                block[5] = cfgBlock[5]; // Memory Config
                block[6] = FrameBuf[8]; // EAS
                block[7] = cfgBlock[7]; // Fuses

                // Some parts allow w (but not e) if in persMode
                if (persMode) {
                    block[0] &= FrameBuf[2]; // Applications Limit
                    block[4] &= FrameBuf[6]; // Chip Config
                    block[5] &= FrameBuf[7]; // Memory Config
                    block[7] &= FrameBuf[9]; // Fuses
                } else {
                    // Fuses allows setting Crypt1/0 from 1 to 0 only during application mode
                    block[7] &= FrameBuf[9] | ~ICLASS_FUSE_CRYPT10;
                }
                break;
            case ICLASS_BLOCK_EPURSE:
                // ePurse updates swap first and second half of the block each update
                memcpy(block+4, FrameBuf+2, 4);
                memcpy(block, FrameBuf+6, 4);
                break;
            case ICLASS_BLOCK_KD:
            case ICLASS_BLOCK_KC:
                if (!persMode) {
                    MemoryReadBlock(block, blockOffset, ICLASS_BLOCK_SIZE);
                    for (uint8_t i = 0; i < sizeof(ICLASS_BLOCK_SIZE); i++)
                        block[i] ^= FrameBuf[i+2];
                    break;
                }
                // fallthrough to default case when personalisation mode
            default:
                memcpy(block, FrameBuf+2, ICLASS_BLOCK_SIZE);
                break;
            }

            MemoryWriteBlock(block, blockOffset, ICLASS_BLOCK_SIZE);

            if ((FrameBuf[1] == CurrentKeyBlockNum || FrameBuf[1] == ICLASS_BLOCK_EPURSE) && !DetectionMode)
                initCipherState();

            // DATA(8) CRC16(2)
            if (FrameBuf[1] == ICLASS_BLOCK_KD || FrameBuf[1] == ICLASS_BLOCK_KD) {
                // Key updates always return FF's
                memcpy(FrameBuf, ffBlock, ICLASS_BLOCK_SIZE);
            } else {
                memcpy(FrameBuf, block, ICLASS_BLOCK_SIZE);
            }
            iClassAppendCRC(FrameBuf, ICLASS_BLOCK_SIZE);
            return ICLASS_BLOCK_SIZE+2;
        case ICLASS_CMD_PAGESEL: // PAGE(1) CRC16(2)
            // Chips with a single page do not answer to this command
            // BLOCK1(8) CRC16(2)
            return ISO15693_APP_NO_RESPONSE;
        case ICLASS_CMD_DETECT:
            // TODO
            return ISO15693_APP_NO_RESPONSE;
        default:
            return ISO15693_APP_NO_RESPONSE;
    }
}

void IClassGetCsn(ConfigurationUidType uid) {
    MemoryReadBlock(uid, ICLASS_BLOCK_CSN * ICLASS_BLOCK_SIZE, ICLASS_CSN_SIZE);
}

void IClassSetCsn(const ConfigurationUidType uid) {
    memcpy(CurrentCSN, uid, ICLASS_CSN_SIZE);
    MemoryWriteBlock(uid, ICLASS_BLOCK_CSN * ICLASS_BLOCK_SIZE, ICLASS_CSN_SIZE);
}
