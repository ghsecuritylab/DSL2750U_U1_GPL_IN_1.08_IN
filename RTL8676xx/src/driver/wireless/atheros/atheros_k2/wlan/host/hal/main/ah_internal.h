/*
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 * Copyright (c) 2002-2005 Atheros Communications, Inc.
 * All rights reserved.
 *
 * $Id: //depot/sw/carriersrc/branches/magpie_host_dev/split_src/host/hal/main/ah_internal.h#16 $
 */

#ifndef _ATH_AH_INTERAL_H_
#define _ATH_AH_INTERAL_H_

#include <asf_bitmap.h>
#define IEEE80211_AMPDU_LIMIT_MAX   (64 * 1024 - 1)
/*
 * Atheros Device Hardware Access Layer (HAL).
 *
 * Internal definitions.
 */
#define AH_NULL 0
#define AH_MIN(a,b) ((a)<(b)?(a):(b))
#define AH_MAX(a,b) ((a)>(b)?(a):(b))


/*
 * Remove const in a way that keeps the compiler happy.
 * This works for gcc but may require other magic for
 * other compilers (not sure where this should reside).
 * Note that uintptr_t is C99.
 */
#ifndef __DECONST
#if __WORDSIZE == 64
typedef unsigned long a_int32_t a_uintptr_t;
#else
typedef a_uint32_t a_uintptr_t;
#endif
#define __DECONST(type, var)    ((type)(a_uintptr_t)(const void *)(var))
#endif

typedef enum {
    START_ADHOC_NO_11A,     /* don't use 802.11a channel */
    START_ADHOC_PER_11D,        /* 802.11a + 802.11d ad-hoc */
    START_ADHOC_IN_11A,     /* 802.11a ad-hoc */
    START_ADHOC_IN_11B,     /* 802.11b ad-hoc */
} START_ADHOC_OPTION;

#include <adf_os_prepack.h>
typedef adf_os_packed_pre struct {
    a_uint8_t    id;     /* element ID */
    a_uint8_t    len;        /* total length in bytes */
    a_uint8_t    cc[3];      /* ISO CC+(I)ndoor/(O)utdoor */
    struct {
        a_uint8_t schan;     /* starting channel */
        a_uint8_t nchan;     /* number channels */
        a_uint8_t maxtxpwr;
    } band[4];          /* up to 4 sub bands */
}  adf_os_packed_post COUNTRY_INFO_LIST;
#include <adf_os_postpack.h>

typedef struct {
    a_uint16_t   start;      /* first register */
    a_uint16_t   end;        /* ending register or zero */
} HAL_REGRANGE;

/*
 * Transmit power scale factor.
 *
 * NB: This is not public because we want to discourage the use of
 *     scaling; folks should use the tx power limit interface.
 */
typedef enum {
    HAL_TP_SCALE_MAX    = 0,        /* no scaling (default) */
    HAL_TP_SCALE_50     = 1,        /* 50% of max (-3 dBm) */
    HAL_TP_SCALE_25     = 2,        /* 25% of max (-6 dBm) */
    HAL_TP_SCALE_12     = 3,        /* 12% of max (-9 dBm) */
    HAL_TP_SCALE_MIN    = 4,        /* min, but still on */
} HAL_TP_SCALE;

typedef enum {
    HAL_CAP_RADAR       = 0,        /* Radar capability */
    HAL_CAP_AR      = 1,        /* AR capability */
} HAL_PHYDIAG_CAPS;

/*
 * Enable/disable strong signal fast diversity
 */

#define HAL_CAP_STRONG_DIV  2
/*
 * NF Cal history buffer
 */
#define HAL_NF_CAL_HIST_MAX     5

#if 0 /*MAGPIE_MERLIN */

#define HAL_DUAL_CHAIN      6
#define HAL_DUAL_CHAIN_11G		2

/*
 * NF Cal history buffer
 */
#define AR_PHY_CCA_FILTERWINDOW_LENGTH_INIT     3
#define AR_PHY_CCA_FILTERWINDOW_LENGTH          5
#else

#define HAL_DUAL_CHAIN      6

#endif

typedef struct {
    a_int16_t     nfCalBuffer[HAL_NF_CAL_HIST_MAX];
    a_uint8_t      currIndex;
    a_uint8_t    windowIndicator;
    a_int16_t     privNF;
} HAL_NFCAL_HIST;

/*
 * Internal form of a HAL_CHANNEL.  Note that the structure
 * must be defined such that you can cast references to a
 * HAL_CHANNEL so don't shuffle the first two members.
 */
typedef struct {
    a_uint16_t   channel;    /* NB: must be first for casting */
    a_uint32_t   channelFlags;
    a_uint8_t    privFlags;
    a_int8_t      maxRegTxPower;
    a_int8_t      maxTxPower;
    a_int8_t      minTxPower; /* as above... */
    a_uint8_t    regClassId; /* Regulatory class id */

    HAL_BOOL    bssSendHere;
    a_uint8_t    gainI;
    HAL_BOOL    iqCalValid;
    HAL_BOOL    oneTimeCalsDone;
    a_int8_t      iCoff;
    a_int8_t      qCoff;
    a_int16_t     rawNoiseFloor;
    a_int16_t     finalNoiseFloor;
    a_int8_t      antennaMax;
    a_uint64_t   regDmnFlags;    /* Flags for channel use in reg */
    a_uint8_t   conformanceTestLimit;   /* conformance test limit from reg domain */
    a_uint16_t   mainSpur;       /* cached spur value for this cahnnel */
    a_uint64_t   ah_tsf_last;    /* tsf @ which time accured is computed */
    a_uint64_t   ah_channel_time;    /* time on the channel  */
    a_uint64_t   dfsTsf; /* Tsf when channel leaves NOL */
#ifndef MAGPIE_MERLIN
    HAL_NFCAL_HIST  nfCalHist[HAL_DUAL_CHAIN];
#endif
} HAL_CHANNEL_INTERNAL;

typedef struct {
    a_uint32_t   halChanSpreadSupport        : 1,
            halSleepAfterBeaconBroken   : 1,
            halCompressSupport      : 1,
            halBurstSupport         : 1,
            halFastFramesSupport        : 1,
            halChapTuningSupport        : 1,
            halTurboGSupport        : 1,
            halTurboPrimeSupport        : 1,
            halXrSupport            : 1,
            halMicAesCcmSupport     : 1,
            halMicCkipSupport       : 1,
            halMicTkipSupport       : 1,
            halCipherAesCcmSupport      : 1,
            halCipherCkipSupport        : 1,
            halCipherTkipSupport        : 1,
            halPSPollBroken         : 1,
            halVEOLSupport          : 1,
            halBssIdMaskSupport     : 1,
            halMcastKeySrchSupport      : 1,
            halTsfAddSupport        : 1,
            halChanHalfRate         : 1,
            halChanQuarterRate      : 1,
            halHTSupport            : 1,
            halGTTSupport           : 1,
            halFastCCSupport        : 1,
            halExtChanDfsSupport    : 1,
            halUseCombinedRadarRssi : 1,
            halCSTSupport           : 1,
            halRifsRxSupport        : 1,
            halRifsTxSupport        : 1,
#ifdef MAGPIE_MERLIN
            halforcePpmSupport      : 1,
            halAutoSleepSupport     : 1,
            hal4kbSplitTransSupport : 1,
            halEnhancedPmSupport    : 1,
            halMbssidAggrSupport    : 1,
            halTkipWepHtRateSupport : 1,
#endif
            halRfSilentSupport      : 1;
    a_uint32_t   halWirelessModes;
    a_uint16_t   halTotalQueues;
    a_uint16_t   halKeyCacheSize;
    a_uint16_t   halLow5GhzChan, halHigh5GhzChan;
    a_uint16_t   halLow2GhzChan, halHigh2GhzChan;
    a_uint16_t   halNumMRRetries;
    a_uint8_t    halTxChainMask;
    a_uint8_t    halRxChainMask;
    a_uint16_t   halRtsAggrLimit;
    a_uint16_t   halJapanRegCap;
    a_uint8_t    halNumGpioPins;
} HAL_CAPABILITIES;

#if defined(MAGPIE_MERLIN) || defined(K2)
/*****
** HAL Configuration
**
** This type contains all of the "factory default" configuration
** items that may be changed during initialization or operation.
** These were formerly global variables which are now PER INSTANCE
** values that are used to control the operation of the specific
** HAL instance
*/

typedef struct {
    a_int32_t         ath_hal_dma_beacon_response_time;
    a_int32_t         ath_hal_sw_beacon_response_time;
    a_int32_t         ath_hal_additional_swba_backoff;
    a_int32_t         ath_hal_6mb_ack;
    a_int32_t         ath_hal_cwmIgnoreExtCCA;
#ifdef ATH_FORCE_BIAS
    /* workaround Fowl bug for orientation sensitivity */
    a_int32_t         ath_hal_forceBias;
    a_int32_t         ath_hal_forceBiasAuto ;
#endif
    a_uint8_t    ath_hal_pciePowerSaveEnable;
    a_uint8_t    ath_hal_pcieL1SKPEnable;
    a_uint8_t    ath_hal_pcieClockReq;
    a_uint32_t   ath_hal_pcieWaen; /****************/
    a_int32_t         ath_hal_pciePowerReset;
	a_uint8_t	ath_hal_analogShiftRegWAR;
    a_uint8_t    ath_hal_htEnable;
#ifdef ATH_SUPERG_DYNTURBO
    a_int32_t         ath_hal_disableTurboG;
#endif
    a_uint32_t   ath_hal_ofdmTrigLow;
    a_uint32_t   ath_hal_ofdmTrigHigh;
    a_uint32_t   ath_hal_cckTrigHigh;
    a_uint32_t   ath_hal_cckTrigLow;
    a_uint32_t   ath_hal_enableANI;
    a_uint8_t    ath_hal_noiseImmunityLvl;
    a_uint32_t   ath_hal_ofdmWeakSigDet;
    a_uint32_t   ath_hal_cckWeakSigThr;
    a_uint8_t    ath_hal_spurImmunityLvl;
    a_uint8_t    ath_hal_firStepLvl;
    a_int8_t      ath_hal_rssiThrHigh;
    a_int8_t      ath_hal_rssiThrLow;
    a_uint16_t   ath_hal_diversityControl;
    a_uint16_t   ath_hal_antennaSwitchSwap;
    a_int32_t         ath_hal_rifs_enable;
    a_int32_t         ath_hal_fastClockEnable;

#if AH_DEBUG
    a_int32_t         ath_hal_debug;
#endif
#define         SPUR_DISABLE        0
#define         SPUR_ENABLE_IOCTL   1
#define         SPUR_ENABLE_EEPROM  2
#define         AR_EEPROM_MODAL_SPURS   5
#define         AR_SPUR_5413_1      1640    /* Freq 2464 */
#define         AR_SPUR_5413_2      1200    /* Freq 2420 */
#define         AR_NO_SPUR      0x8000
#define         AR_BASE_FREQ_2GHZ   2300
#define         AR_BASE_FREQ_5GHZ   4900
#define         AR_SPUR_FEEQ_BOUND_HT40  19
#define         AR_SPUR_FEEQ_BOUND_HT20  10

    a_int32_t         ath_hal_spurMode; 
    a_uint16_t   ath_hal_spurChans[AR_EEPROM_MODAL_SPURS][2];
    #ifdef K2
    a_uint8_t    ath_hal_defaultAntCfg;
    #endif
} HAL_OPS_CONFIG;
#endif


typedef enum {
        DFS_UNINIT_DOMAIN   = 0,    /* Uninitialized dfs domain */
        DFS_FCC_DOMAIN      = 1,    /* FCC3 dfs domain */
        DFS_ETSI_DOMAIN     = 2,    /* ETSI dfs domain */
        DFS_MKK4_DOMAIN     = 3,    /* Japan dfs domain */
} HAL_DFS_DOMAIN;

/*
 * The ``private area'' follows immediately after the ``public area''
 * in the data structure returned by ath_hal_attach.  Private data are
 * used by device-independent code such as the regulatory domain support.
 * This data is not resident in the public area as a client may easily
 * override them and, potentially, bypass access controls.  In general,
 * code within the HAL should never depend on data in the public area.
 * Instead any public data needed internally should be shadowed here.
 *
 * When declaring a device-specific ath_hal data structure this structure
 * is assumed to at the front; e.g.
 *
 *  struct ath_hal_5212 {
 *      struct ath_hal_private  ah_priv;
 *      ...
 *  };
 *
 * It might be better to manage the method pointers in this structure
 * using an indirect pointer to a read-only data structure but this would
 * disallow class-style method overriding (and provides only minimally
 * better protection against client alteration).
 */
struct ath_hal_private {
    struct ath_hal  h;          /* public area */

    /* NB: all methods go first to simplify initialization */
    HAL_BOOL    (*ah_getChannelEdges)(struct ath_hal*,
                a_uint32_t channelFlags,
                a_uint16_t *lowChannel, a_uint16_t *highChannel);
    void        (*ah_enableRadarDetection)(struct ath_hal*);
    a_uint32_t       (*ah_getWirelessModes)(struct ath_hal*);
    HAL_BOOL    (*ah_eepromRead)(struct ath_hal *, a_uint32_t, a_uint16_t *);
    HAL_BOOL    (*ah_eepromWrite)(struct ath_hal *, a_uint32_t, a_uint16_t);
    HAL_STATUS  (*ah_eepromAttach)(struct ath_hal *);
    HAL_STATUS  (*ah_eepromDetach)(struct ath_hal *);

    /* EEprom Cal and Board setup */
    HAL_BOOL    (*ah_eepromSetBoardValues)(struct ath_hal *,
                     HAL_CHANNEL_INTERNAL *);
    a_uint16_t   (*ah_eepromGetSpurChan)(struct ath_hal *, a_uint16_t, HAL_BOOL);
    HAL_BOOL    (*ah_setTransmitPower)(struct ath_hal *, 
                    HAL_CHANNEL_INTERNAL *, a_uint16_t *);
    HAL_BOOL    (*ah_getEepromCapabilityInfo)(struct ath_hal *, a_uint16_t *);
    HAL_BOOL    (*ah_getEepromNoiseFloorThresh)(struct ath_hal *,
                    const HAL_CHANNEL_INTERNAL *, a_int16_t *);
    HAL_BOOL    (*ah_gpioCfgOutput)(struct ath_hal *, a_uint32_t gpio);
    HAL_BOOL    (*ah_gpioCfgInput)(struct ath_hal *, a_uint32_t gpio);
    a_uint32_t   (*ah_gpioGet)(struct ath_hal *, a_uint32_t gpio);
    HAL_BOOL    (*ah_gpioSet)(struct ath_hal *,
                a_uint32_t gpio, a_uint32_t val);
    void        (*ah_gpioSetIntr)(struct ath_hal*, a_uint32_t, a_uint32_t);
    HAL_BOOL    (*ah_getChipPowerLimits)(struct ath_hal *,
                HAL_CHANNEL *, a_uint32_t);

    /*
     * Device revision information.
     */
    a_uint16_t   ah_devid;       /* PCI device ID */
    a_uint16_t   ah_subvendorid;     /* PCI subvendor ID */
    a_uint32_t   ah_macVersion;      /* MAC version id */
    a_uint16_t   ah_macRev;      /* MAC revision */
    a_uint16_t   ah_phyRev;      /* PHY revision */
    a_uint16_t   ah_analog5GhzRev;   /* 2GHz radio revision */
    a_uint16_t   ah_analog2GhzRev;   /* 5GHz radio revision */
    a_uint32_t   ah_flags;           /* misc flags */

    HAL_OPMODE  ah_opmode;      /* operating mode from reset */
    HAL_CAPABILITIES ah_caps;       /* device capabilities */
    a_uint32_t   ah_diagreg;     /* user-specified AR_DIAG_SW */
    a_int16_t     ah_powerLimit;      /* tx power cap */
    a_uint16_t   ah_maxPowerLevel;   /* calculated max tx power */
    a_uint32_t       ah_tpScale;     /* tx power scale factor */

    /*
     * State for regulatory domain handling.
     */
    HAL_REG_DOMAIN  ah_currentRD;       /* Current regulatory domain */
    HAL_REG_DOMAIN  ah_currentRDExt;    /* David Quan Memorial regulatory extension */
    HAL_CTRY_CODE   ah_countryCode;     /* current country code */
    HAL_DFS_DOMAIN  ah_dfsDomain;       /* current dfs domain */
    START_ADHOC_OPTION ah_adHocMode;    /* ad-hoc mode handling */
    HAL_BOOL    ah_commonMode;      /* common mode setting */
    /* NB: 802.11d stuff is not currently used */
    HAL_BOOL    ah_cc11d;       /* 11d country code */
    COUNTRY_INFO_LIST ah_cc11dInfo;     /* 11d country code element */
    HAL_CHANNEL_INTERNAL ah_channels[256];  /* calculated channel list */
    a_uint32_t       ah_nchan;       /* valid channels in list */
    HAL_CHANNEL_INTERNAL *ah_curchan;   /* current channel */

    a_uint8_t        ah_coverageClass;   /* coverage class */
    HAL_BOOL        ah_regdomainUpdate;     /* regdomain is updated? */
    a_uint64_t       ah_tsf_channel;         /* tsf @ which last channel change happened */
    /*
     * RF Silent handling; setup according to the EEPROM.
     */
    a_uint16_t  ah_rfsilent;        /* GPIO pin + polarity */
    HAL_BOOL    ah_rfkillEnabled;   /* enable/disable RfKill */
    HAL_BOOL    ah_cwCalRequire;
    HAL_BOOL    ah_isPciExpress; /* XXX: only used for ar5212 MAC(Condor/Hawk/Swan) */
    a_int16_t   ah_txPowerTableBase;  /* tx power written to the power per rate registers is relative to this value (dBm)*/ 

#if defined(MAGPIE_MERLIN) || defined(K2)
    HAL_OPS_CONFIG   ah_config;     /* Operating Configuration */
#endif
    HAL_NFCAL_HIST  nfCalHist[HAL_DUAL_CHAIN];
};
#define AH_PRIVATE(_ah) ((struct ath_hal_private *)(_ah))

#define ath_hal_getChannelEdges(_ah, _cf, _lc, _hc) \
    AH_PRIVATE(_ah)->ah_getChannelEdges(_ah, _cf, _lc, _hc)
#define ath_hal_enableRadarDetection(_ah) \
    AH_PRIVATE(_ah)->ah_enableRadarDetection(_ah)
#define ath_hal_getWirelessModes(_ah) \
    AH_PRIVATE(_ah)->ah_getWirelessModes(_ah)
#define ath_hal_eepromRead(_ah, _off, _data) \
    AH_PRIVATE(_ah)->ah_eepromRead(_ah, _off, _data)
#define ath_hal_eepromWrite(_ah, _off, _data) \
    AH_PRIVATE(_ah)->ah_eepromWrite(_ah, _off, _data)
#define ath_hal_gpioCfgOutput(_ah, _gpio) \
    AH_PRIVATE(_ah)->ah_gpioCfgOutput(_ah, _gpio)
#define ath_hal_gpioCfgInput(_ah, _gpio) \
    AH_PRIVATE(_ah)->ah_gpioCfgInput(_ah, _gpio)
#define ath_hal_gpioGet(_ah, _gpio) \
    AH_PRIVATE(_ah)->ah_gpioGet(_ah, _gpio)
#define ath_hal_gpioSet(_ah, _gpio, _val) \
    AH_PRIVATE(_ah)->ah_gpioGet(_ah, _gpio, _val)
#define ath_hal_gpioSetIntr(_ah, _gpio, _ilevel) \
    AH_PRIVATE(_ah)->ah_gpioSetIntr(_ah, _gpio, _ilevel)
#define ath_hal_getpowerlimits(_ah, _chans, _nchan) \
    AH_PRIVATE(_ah)->ah_getChipPowerLimits(_ah, _chans, _nchan)
#ifdef ATH_FORCE_PPM
#define ath_hal_ppmGetRssiDump(_ah) \
    AH_PRIVATE(_ah)->ah_ppmGetRssiDump(_ah)
#define ath_hal_ppmArmTrigger(_ah) \
    AH_PRIVATE(_ah)->ah_ppmArmTrigger(_ah)
#define ath_hal_ppmGetTrigger(_ah) \
    AH_PRIVATE(_ah)->ah_ppmGetTrigger(_ah)
#define ath_hal_ppmForce(_ah) \
    AH_PRIVATE(_ah)->ah_ppmForce(_ah)
#define ath_hal_ppmUnForce(_ah) \
    AH_PRIVATE(_ah)->ah_ppmUnForce(_ah)
#define ath_hal_ppmGetReg(_ah, _arg) \
    AH_PRIVATE(_ah)->ah_ppmGetReg(_ah, _arg)
#endif /* ATH_FORCE_PPM */

#if !defined(_NET_IF_IEEE80211_H_) && !defined(_NET80211__IEEE80211_H_)
/*
 * Stuff that would naturally come from _ieee80211.h
 */
#define IEEE80211_ADDR_LEN      6

#define IEEE80211_WEP_KEYLEN            5   /* 40bit */
#define IEEE80211_WEP_IVLEN         3   /* 24bit */
#define IEEE80211_WEP_KIDLEN            1   /* 1 octet */
#define IEEE80211_WEP_CRCLEN            4   /* CRC-32 */

#define IEEE80211_CRC_LEN           4

#define IEEE80211_MTU               1500
#define IEEE80211_MAX_LEN           (2300 + IEEE80211_CRC_LEN + \
    (IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN + IEEE80211_WEP_CRCLEN))

enum {
    IEEE80211_T_DS,         /* direct sequence spread spectrum */
    IEEE80211_T_FH,         /* frequency hopping */
    IEEE80211_T_OFDM,       /* frequency division multiplexing */
    IEEE80211_T_TURBO,      /* high rate DS */
    IEEE80211_T_HT,         /* HT - full GI */
    IEEE80211_T_MAX
};
#define IEEE80211_T_CCK IEEE80211_T_DS  /* more common nomenclatur */
#endif /* _NET_IF_IEEE80211_H_ */

/* NB: these are defined privately until XR support is announced */
enum {
    ATHEROS_T_XR    = IEEE80211_T_MAX,  /* extended range */
};

#define HAL_COMP_BUF_MAX_SIZE   9216       /* 9K */
#define HAL_COMP_BUF_ALIGN_SIZE 512

#define HAL_TXQ_USE_LOCKOUT_BKOFF_DIS   0x00000001

#define INIT_AIFS       2
#define INIT_CWMIN      15
#define INIT_CWMIN_11B      31
#define INIT_CWMAX      1023
#define INIT_SH_RETRY       10
#define INIT_LG_RETRY       10
#define INIT_SSH_RETRY      32
#define INIT_SLG_RETRY      32

typedef struct {
    a_uint32_t   tqi_ver;        /* HAL TXQ verson */
    HAL_TX_QUEUE    tqi_type;       /* hw queue type*/
    HAL_TX_QUEUE_SUBTYPE tqi_subtype;   /* queue subtype, if applicable */
    HAL_TX_QUEUE_FLAGS tqi_qflags;      /* queue flags */
    a_uint32_t   tqi_priority;
    a_uint32_t   tqi_aifs;       /* aifs */
    a_uint32_t   tqi_cwmin;      /* cwMin */
    a_uint32_t   tqi_cwmax;      /* cwMax */
    a_uint16_t   tqi_shretry;        /* frame short retry limit */
    a_uint16_t   tqi_lgretry;        /* frame long retry limit */
    a_uint32_t   tqi_cbrPeriod;
    a_uint32_t   tqi_cbrOverflowLimit;
    a_uint32_t   tqi_burstTime;
    a_uint32_t   tqi_readyTime;
    a_uint32_t   tqi_physCompBuf;
    a_uint32_t   tqi_intFlags;       /* flags for internal use */
} HAL_TX_QUEUE_INFO;

extern  HAL_BOOL ath_hal_setTxQProps(struct ath_hal *ah,
        HAL_TX_QUEUE_INFO *qi, const HAL_TXQ_INFO *qInfo);
extern  HAL_BOOL ath_hal_getTxQProps(struct ath_hal *ah,
        HAL_TXQ_INFO *qInfo, const HAL_TX_QUEUE_INFO *qi);

typedef enum {
    HAL_ANI_PRESENT,            /* is ANI support present */
    HAL_ANI_NOISE_IMMUNITY_LEVEL,       /* set level */
    HAL_ANI_OFDM_WEAK_SIGNAL_DETECTION, /* enable/disable */
    HAL_ANI_CCK_WEAK_SIGNAL_THR,        /* enable/disable */
    HAL_ANI_FIRSTEP_LEVEL,          /* set level */
    HAL_ANI_SPUR_IMMUNITY_LEVEL,        /* set level */
    HAL_ANI_MODE,               /* 0 => manual, 1 => auto */
    HAL_ANI_PHYERR_RESET,           /* reset phy error stats */
} HAL_ANI_CMD;

#define HAL_NOISE_IMMUNE_MAX        4   /* Max noise immunity level */
#define HAL_SPUR_IMMUNE_MAX     7   /* Max spur immunity level for AP */
#define HAL_SPUR_IMMUNE_MAX_STA 2   /* Max spur immunity level for STA */
#define HAL_FIRST_STEP_MAX      2   /* Max first step level */

#define HAL_ANI_OFDM_TRIG_HIGH      500
#define HAL_ANI_OFDM_TRIG_LOW       200
#define HAL_ANI_CCK_TRIG_HIGH       200
#define HAL_ANI_CCK_TRIG_LOW        100
#define HAL_ANI_NOISE_IMMUNE_LVL    HAL_NOISE_IMMUNE_MAX
#define HAL_ANI_USE_OFDM_WEAK_SIG   AH_TRUE
#define HAL_ANI_CCK_WEAK_SIG_THR    AH_FALSE
#define HAL_ANI_FIRSTEP_LVL     0
#define HAL_ANI_RSSI_THR_HIGH       40
#define HAL_ANI_RSSI_THR_LOW        7
#define HAL_ANI_PERIOD          100
#define HAL_ANI_SPUR_IMMUNE_LVL     7
#define HAL_SPUR_IMMUNE         2

#define HAL_SPUR_VAL_MASK       0x3FFF
#define HAL_SPUR_CHAN_WIDTH     87

#define HAL_BIN_WIDTH_BASE_100HZ    3125
#define HAL_BIN_WIDTH_TURBO_100HZ   6250
#define HAL_MAX_BINS_ALLOWED        28

#define CHANNEL_XR_A    (CHANNEL_A | CHANNEL_XR)
#define CHANNEL_XR_G    (CHANNEL_PUREG | CHANNEL_XR)
#define CHANNEL_XR_T    (CHANNEL_T | CHANNEL_XR)

/*
 * A    = 5GHZ|OFDM
 * T    = 5GHZ|OFDM|TURBO
 * XR_T = 2GHZ|OFDM|XR
 *
 * IS_CHAN_A(T) or IS_CHAN_A(XR_T) will return TRUE.  This is probably
 * not the default behavior we want.  We should migrate to a better mask --
 * perhaps CHANNEL_ALL.
 *
 * For now, IS_CHAN_G() masks itself with CHANNEL_108G.
 *
 */

#define IS_CHAN_A(_c)   ((((_c)->channelFlags & CHANNEL_A) == CHANNEL_A) || \
             (((_c)->channelFlags & CHANNEL_A_HT20) == CHANNEL_A_HT20))
#define IS_CHAN_B(_c)   (((_c)->channelFlags & CHANNEL_B) == CHANNEL_B)
#define IS_CHAN_G(_c)   ((((_c)->channelFlags & (CHANNEL_108G|CHANNEL_G)) == CHANNEL_G) || \
             (((_c)->channelFlags & CHANNEL_G_HT20) == CHANNEL_G_HT20))
#define IS_CHAN_108G(_c)(((_c)->channelFlags & CHANNEL_108G) == CHANNEL_108G)
#define IS_CHAN_T(_c)   (((_c)->channelFlags & CHANNEL_T) == CHANNEL_T)
#define IS_CHAN_X(_c)   (((_c)->channelFlags & CHANNEL_X) == CHANNEL_X)
#define IS_CHAN_PUREG(_c) \
    (((_c)->channelFlags & CHANNEL_PUREG) == CHANNEL_PUREG)
#define IS_CHAN_NA(_c)  (((_c)->channelFlags & CHANNEL_A_HT20) == CHANNEL_A_HT20)
#define IS_CHAN_NG(_c)  (((_c)->channelFlags & CHANNEL_G_HT20) == CHANNEL_G_HT20)

#define IS_CHAN_TURBO(_c)   (((_c)->channelFlags & CHANNEL_TURBO) != 0)
#define IS_CHAN_CCK(_c)     (((_c)->channelFlags & CHANNEL_CCK) != 0)
#define IS_CHAN_OFDM(_c)    (((_c)->channelFlags & CHANNEL_OFDM) != 0)
#define IS_CHAN_XR(_c)      (((_c)->channelFlags & CHANNEL_XR) != 0)
#define IS_CHAN_5GHZ(_c)    (((_c)->channelFlags & CHANNEL_5GHZ) != 0)
#define IS_CHAN_2GHZ(_c)    (((_c)->channelFlags & CHANNEL_2GHZ) != 0)
#define IS_CHAN_PASSIVE(_c) (((_c)->channelFlags & CHANNEL_PASSIVE) != 0)
#define IS_CHAN_HALF_RATE(_c)   (((_c)->channelFlags & CHANNEL_HALF) != 0)
#define IS_CHAN_QUARTER_RATE(_c) (((_c)->channelFlags & CHANNEL_QUARTER) != 0)
#define IS_CHAN_HT(_c)      (((_c)->channelFlags & CHANNEL_HT20) != 0)
#define IS_CHAN_HT20(_c)    (((_c)->channelFlags & CHANNEL_HT20) != 0)
#define IS_CHAN_HT40(_c)    (((_c)->channelFlags & CHANNEL_HT40) != 0)

#define IS_CHAN_IN_PUBLIC_SAFETY_BAND(_c) ((_c) > 4940 && (_c) < 4990)

/*
 * Deduce if the host cpu has big- or litt-endian byte order.
 */
static adf_os_inline a_int32_t
isBigEndian(void)
{
    union {
        a_int32_t i;
        char c[4];
    } u;
    u.i = 1;
    return (u.c[0] == 0);
}

/* unalligned little endian access */
#define LE_READ_2(p)                            \
    ((a_uint16_t)                            \
     ((((const a_uint8_t *)(p))[0]    ) | (((const a_uint8_t *)(p))[1]<< 8)))
#define LE_READ_4(p)                            \
    ((a_uint32_t)                            \
     ((((const a_uint8_t *)(p))[0]    ) | (((const a_uint8_t *)(p))[1]<< 8) |\
      (((const a_uint8_t *)(p))[2]<<16) | (((const a_uint8_t *)(p))[3]<<24)))

/*
 * Register manipulation macros that expect bit field defines
 * to follow the convention that an _S suffix is appended for
 * a shift count, while the field mask has no suffix.
 */
#define SM(_v, _f)  (((_v) << _f##_S) & _f)
#define MS(_v, _f)  (((_v) & _f) >> _f##_S)
#define OS_REG_RMW(_a, _r, _set, _clr)    \
        OS_REG_WRITE(_a, _r, (OS_REG_READ(_a, _r) & ~(_clr)) | (_set))
#define OS_REG_RMW_FIELD(_a, _r, _f, _v) \
    OS_REG_WRITE(_a, _r, \
        (OS_REG_READ(_a, _r) &~ _f) | (((_v) << _f##_S) & _f))
#define OS_REG_READ_FIELD(_a, _r, _f) \
        (((OS_REG_READ(_a, _r) & _f) >> _f##_S))
#define OS_REG_SET_BIT(_a, _r, _f) \
    OS_REG_WRITE(_a, _r, OS_REG_READ(_a, _r) | _f)
#define OS_REG_CLR_BIT(_a, _r, _f) \
    OS_REG_WRITE(_a, _r, OS_REG_READ(_a, _r) &~ _f)

/*
 * Regulatory domain support.
 */

/*
 * Return the max allowed antenna gain based on the current
 * regulatory domain.
 */
extern  a_uint32_t ath_hal_getantennareduction(struct ath_hal *,
        HAL_CHANNEL *, a_int8_t twiceGain);

a_uint8_t
ath_hal_getantennaallowed(struct ath_hal *ah, HAL_CHANNEL *chan);

/*
 * Return the test group for the specific channel based on
 * the current regulator domain.
 */
extern  a_uint8_t ath_hal_getctl(struct ath_hal *, HAL_CHANNEL *);
/*
 * Return whether or not a noise floor check is required
 * based on the current regulatory domain for the specified
 * channel.
 */
extern  a_uint32_t ath_hal_getnfcheckrequired(struct ath_hal *, HAL_CHANNEL *);
/*
 * Map a public channel definition to the corresponding
 * internal data structure.  This implicitly specifies
 * whether or not the specified channel is ok to use
 * based on the current regulatory domain constraints.
 */
extern  HAL_CHANNEL_INTERNAL *ath_hal_checkchannel(struct ath_hal *,
        const HAL_CHANNEL *);

/* system-configurable parameters */
extern  a_int32_t ath_hal_dma_beacon_response_time;   /* in TU's */
extern  a_int32_t ath_hal_sw_beacon_response_time;    /* in TU's */
extern  a_int32_t ath_hal_additional_swba_backoff;    /* in TU's */
extern  a_int32_t ath_hal_6mb_ack;
extern  a_int32_t ath_hal_clksel;
extern  a_int32_t ath_hal_soft_eeprom;
extern  a_int32_t ath_hal_enableTPC;
extern  a_int32_t ath_hal_maxTPC;
extern  a_int32_t ath_hal_cwmIgnoreExtCCA;
#ifdef ATH_FORCE_BIAS
extern  a_int32_t ath_hal_forceBias;
extern  a_int32_t ath_hal_forceBiasAuto;
#endif
extern  a_uint8_t ath_hal_thresh62;
extern  a_uint8_t ath_hal_thresh62_ext;
extern  a_int32_t ath_hal_rifs_enable;
#ifdef MAGPIE_MERLIN
extern  a_int32_t ath_hal_desc_tpc;
extern  a_int32_t ath_hal_redchn_maxpwr;
extern  a_int32_t ath_hal_rifs_enable;
#endif

/* wait for the register contents to have the specified value */
extern  HAL_BOOL ath_hal_wait(struct ath_hal *, a_uint32_t reg,
        a_uint32_t mask, a_uint32_t val);

/* return the first n bits in val reversed */
extern  a_uint32_t ath_hal_reverseBits(a_uint32_t val, a_uint32_t n);

/* printf interfaces */
void ath_hal_printf(struct ath_hal *, const char*, ...);
void ath_hal_vprintf(struct ath_hal *, const char*, __va_list);

extern  const char* ath_hal_ether_sprintf(const a_uint8_t *mac);

/* Math interfaces */
extern a_uint32_t ath_hal_sqrt(a_uint32_t x);

/* allocate and free memory */
extern  void *ath_hal_malloc(adf_os_size_t);
extern  void ath_hal_free(void *);

/* common debugging interfaces */
#ifdef AH_DEBUG
//extern  a_int32_t ath_hal_debug;
extern void HALDEBUG(struct ath_hal *ah, const char* fmt, ...);
extern void HALDEBUGn(struct ath_hal *ah, a_int32_t level, const char* fmt, ...);
#else
#define HALDEBUG(_ah, _fmt, ...)
#define HALDEBUGn(_ah, _level, _fmt, ...)
#endif /* AH_DEBUG */

#ifdef MAGPIE_MERLIN
/*
** Prototype for factory initialization function
*/
extern void __ahdecl ath_hal_factory_defaults(struct ath_hal_private *ap);
extern  a_uint32_t ath_hal_get_curmode(struct ath_hal *, HAL_CHANNEL_INTERNAL *); 

#endif

/*
 * Register logging definitions shared with ardecode.
 */
#include "ah_decode.h"

/*
 * Common assertion interface.  Note: it is a bad idea to generate
 * an assertion failure for any recoverable event.  Instead catch
 * the violation and, if possible, fix it up or recover from it; either
 * with an error return value or a diagnostic messages.  System software
 * does not panic unless the situation is hopeless.
 */
#ifdef AH_ASSERT
#define HALASSERT(_x) do {                  \
    adf_os_assert(_x)                       \
} while (0)
#else
#define HALASSERT(_x)
#endif /* AH_ASSERT */

/*
 * Convert between microseconds and core system clocks.
 */
extern  a_uint32_t ath_hal_mac_clks(struct ath_hal *ah, HAL_HT_CWM *htcwm, a_uint32_t usecs);
extern  a_uint32_t ath_hal_mac_usec(struct ath_hal *ah, HAL_HT_CWM *htcwm, a_uint32_t clks);

/*
 * Generic get/set capability support.  Each chip overrides
 * this routine to support chip-specific capabilities.
 */
extern  HAL_STATUS ath_hal_getcapability(struct ath_hal *ah,
        HAL_CAPABILITY_TYPE type, a_uint32_t capability,
        a_uint32_t *result);
extern  HAL_BOOL ath_hal_setcapability(struct ath_hal *ah,
        HAL_CAPABILITY_TYPE type, a_uint32_t capability,
        a_uint32_t setting, HAL_STATUS *status);

/*
 * Diagnostic interface.  This is an open-ended interface that
 * is opaque to applications.  Diagnostic programs use this to
 * retrieve internal data structures, etc.  There is no guarantee
 * that calling conventions for calls other than HAL_DIAG_REVS
 * are stable between HAL releases; a diagnostic application must
 * use the HAL revision information to deal with ABI/API differences.
 */
enum {
    HAL_DIAG_REVS       = 0,    /* MAC/PHY/Radio revs */
    HAL_DIAG_EEPROM     = 1,    /* EEPROM contents */
    HAL_DIAG_EEPROM_EXP_11A = 2,    /* EEPROM 5112 power exp for 11a */
    HAL_DIAG_EEPROM_EXP_11B = 3,    /* EEPROM 5112 power exp for 11b */
    HAL_DIAG_EEPROM_EXP_11G = 4,    /* EEPROM 5112 power exp for 11g */
    HAL_DIAG_ANI_CURRENT    = 5,    /* ANI current channel state */
    HAL_DIAG_ANI_OFDM   = 6,    /* ANI OFDM timing error stats */
    HAL_DIAG_ANI_CCK    = 7,    /* ANI CCK timing error stats */
    HAL_DIAG_ANI_STATS  = 8,    /* ANI statistics */
    HAL_DIAG_RFGAIN     = 9,    /* RfGain GAIN_VALUES */
    HAL_DIAG_RFGAIN_CURSTEP = 10,   /* RfGain GAIN_OPTIMIZATION_STEP */
    HAL_DIAG_PCDAC      = 11,   /* PCDAC table */
    HAL_DIAG_TXRATES    = 12,   /* Transmit rate table */
    HAL_DIAG_REGS       = 13,   /* Registers */
    HAL_DIAG_ANI_CMD    = 14,   /* ANI issue command */
    HAL_DIAG_SETKEY     = 15,   /* Set keycache backdoor */
    HAL_DIAG_RESETKEY   = 16,   /* Reset keycache backdoor */
    HAL_DIAG_EEREAD     = 17,   /* Read EEPROM word */
    HAL_DIAG_EEWRITE    = 18,   /* Write EEPROM word */
    HAL_DIAG_TXCONT     = 19,   /* TX99 settings */
    HAL_DIAG_SET_RADAR  = 20,   /* Set Radar thresholds */
    HAL_DIAG_GET_RADAR  = 21,   /* Get Radar thresholds */
    HAL_DIAG_USENOL     = 22,   /* Turn on/off the use of the radar NOL */
    HAL_DIAG_GET_USENOL = 23,   /* Get status of the use of the radar NOL */
    HAL_DIAG_REGREAD    = 24,   /* Reg reads */
    HAL_DIAG_REGWRITE   = 25,   /* Reg writes */
    HAL_DIAG_GET_REGBASE= 26,   /* Get register base */
    HAL_DIAG_DMADBG = 27,   /* Dump DMA Regs */
};

/*
 * Device revision information.
 */
typedef struct {
    a_uint16_t   ah_devid;       /* PCI device ID */
    a_uint16_t   ah_subvendorid;     /* PCI subvendor ID */
    a_uint32_t   ah_macVersion;      /* MAC version id */
    a_uint16_t   ah_macRev;      /* MAC revision */
    a_uint16_t   ah_phyRev;      /* PHY revision */
    a_uint16_t   ah_analog5GhzRev;   /* 2GHz radio revision */
    a_uint16_t   ah_analog2GhzRev;   /* 5GHz radio revision */
} HAL_REVS;

/*
 * Argument payload for HAL_DIAG_SETKEY.
 */
typedef struct {
    HAL_KEYVAL  dk_keyval;
    a_uint16_t   dk_keyix;   /* key index */
    a_uint8_t    dk_mac[IEEE80211_ADDR_LEN];
    a_int32_t     dk_xor;     /* XOR key data */
} HAL_DIAG_KEYVAL;

/*
 * Argument payload for HAL_DIAG_EEWRITE.
 */
typedef struct {
    a_uint16_t   ee_off;     /* eeprom offset */
    a_uint16_t   ee_data;    /* write data */
} HAL_DIAG_EEVAL;

typedef struct {
    a_uint32_t     offset;       /* reg offset */
    a_uint32_t val;          /* reg value  */
} HAL_DIAG_REGVAL;

extern  HAL_BOOL ath_hal_getdiagstate(struct ath_hal *ah, a_int32_t request,
            const void *args, a_uint32_t argsize,
            void **result, a_uint32_t *resultsize);

/*
 * Setup a h/w rate table for use.
 */
extern  void ath_hal_setupratetable(struct ath_hal *ah, HAL_RATE_TABLE *rt);

/*
 * The following are for direct integration of Atheros code.
 */
typedef enum {
    WIRELESS_MODE_11a   = 0,
    WIRELESS_MODE_TURBO = 1,
    WIRELESS_MODE_11b   = 2,
    WIRELESS_MODE_11g   = 3,
    WIRELESS_MODE_108g  = 4,
    WIRELESS_MODE_XR    = 5,
    WIRELESS_MODE_11na  = 6,
    WIRELESS_MODE_11ng  = 7,
    WIRELESS_MODE_MAX
} WIRELESS_MODE;

#ifdef TODO_MAGPIE
extern  WIRELESS_MODE ath_hal_chan2wmode(struct ath_hal *, const HAL_CHANNEL *);
#endif

#define FRAME_DATA      2   /* Data frame */
#define SUBT_DATA_CFPOLL    2   /* Data + CF-Poll */
#define SUBT_NODATA_CFPOLL  6   /* No Data + CF-Poll */
#define WLAN_CTRL_FRAME_SIZE    (2+2+6+4)   /* ACK+FCS */

#define MAX_REG_ADD_COUNT   129

#ifdef AH_PRIVATE_DIAG
void
ar5212_ContTxMode(struct ath_hal *ah, struct ath_desc *ds, a_int32_t mode);
void
ar5416_ContTxMode_10(struct ath_hal *ah, struct ath_desc *ds, a_int32_t mode);
void
ar5416_ContTxMode_20(struct ath_hal *ah, struct ath_desc *ds, a_int32_t mode);
#endif

#endif /* _ATH_AH_INTERAL_H_ */
