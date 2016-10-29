/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rtmp.h

    Abstract:
    Miniport generic portion header file

    Revision History:
    Who         When          What
    --------    ----------    ----------------------------------------------
    Paul Lin    2002-08-01    created
    James Tan   2002-09-06    modified (Revise NTCRegTable)
    John Chang  2004-09-06    modified for RT2600     
*/
#ifndef __RTMP_H__
#define __RTMP_H__

#include "link_list.h"
#include "spectrum_def.h"

#ifdef MLME_EX
#include "mlme_ex_def.h"
#endif // MLME_EX //

#ifdef CONFIG_AP_SUPPORT
#include "ap_autoChSel.h"
#endif // CONFIG_AP_SUPPORT //

#undef AP_WSC_INCLUDED
#undef STA_WSC_INCLUDED
#undef WSC_INCLUDED

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
#define AP_WSC_INCLUDED
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //


#if defined(AP_WSC_INCLUDED) || defined(STA_WSC_INCLUDED)
#define WSC_INCLUDED
#endif

#ifdef WSC_INCLUDED
#include "wsc.h"
#endif // WSC_INCLUDED //

#ifdef MAT_SUPPORT
#include "mat.h"
#endif // MAT_SUPPORT //



//#define DBG		1

//#define DBG_DIAGNOSE		1

#if defined(CONFIG_AP_SUPPORT) && defined(CONFIG_STA_SUPPORT)
#define IF_DEV_CONFIG_OPMODE_ON_AP(_pAd)	if(_pAd->OpMode == OPMODE_AP)
#define IF_DEV_CONFIG_OPMODE_ON_STA(_pAd)	if(_pAd->OpMode == OPMODE_STA)
#else
#define IF_DEV_CONFIG_OPMODE_ON_AP(_pAd)
#define IF_DEV_CONFIG_OPMODE_ON_STA(_pAd)
#endif

#define VIRTUAL_IF_INC(__pAd) ((__pAd)->VirtualIfCnt++)
#define VIRTUAL_IF_DEC(__pAd) ((__pAd)->VirtualIfCnt--)
#define VIRTUAL_IF_NUM(__pAd) ((__pAd)->VirtualIfCnt)

#ifdef RT2870
////////////////////////////////////////////////////////////////////////////
// The TX_BUFFER structure forms the transmitted USB packet to the device
////////////////////////////////////////////////////////////////////////////
typedef struct __TX_BUFFER{
	union	{
		UCHAR			WirelessPacket[TX_BUFFER_NORMSIZE];
		HEADER_802_11	NullFrame;
		PSPOLL_FRAME	PsPollPacket;
		RTS_FRAME		RTSFrame;
	}field;
	UCHAR			Aggregation[4];  //Buffer for save Aggregation size.
} TX_BUFFER, *PTX_BUFFER;

typedef struct __HTTX_BUFFER{
	union	{
		UCHAR			WirelessPacket[MAX_TXBULK_SIZE];
		HEADER_802_11	NullFrame;
		PSPOLL_FRAME	PsPollPacket;
		RTS_FRAME		RTSFrame;
	}field;
	UCHAR			Aggregation[4];  //Buffer for save Aggregation size.
} HTTX_BUFFER, *PHTTX_BUFFER;


// used to track driver-generated write irps 
typedef struct _TX_CONTEXT
{
	PVOID			pAd;		//Initialized in MiniportInitialize
	PURB			pUrb;			//Initialized in MiniportInitialize
	PIRP			pIrp;			//used to cancel pending bulk out.
									//Initialized in MiniportInitialize
	PTX_BUFFER		TransferBuffer;	//Initialized in MiniportInitialize
	ULONG			BulkOutSize;
	UCHAR			BulkOutPipeId;
	UCHAR			SelfIdx;
	BOOLEAN			InUse;
	BOOLEAN			bWaitingBulkOut; // at least one packet is in this TxContext, ready for making IRP anytime.
	BOOLEAN			bFullForBulkOut; // all tx buffer are full , so waiting for tx bulkout.
	BOOLEAN			IRPPending;
	BOOLEAN			LastOne;
	BOOLEAN			bAggregatible;
	UCHAR			Header_802_3[LENGTH_802_3];
	UCHAR			Rsv[2];
	ULONG			DataOffset;
	UINT			TxRate;
	dma_addr_t		data_dma;		// urb dma on linux

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_AP_SUPPORT
	USHORT			Wcid;
#endif // UAPSD_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
}	TX_CONTEXT, *PTX_CONTEXT, **PPTX_CONTEXT;


// used to track driver-generated write irps 
typedef struct _HT_TX_CONTEXT
{
	PVOID			pAd;		//Initialized in MiniportInitialize
	PURB			pUrb;			//Initialized in MiniportInitialize
	PIRP			pIrp;			//used to cancel pending bulk out.
									//Initialized in MiniportInitialize
	PHTTX_BUFFER	TransferBuffer;	//Initialized in MiniportInitialize
	ULONG			BulkOutSize;	// Indicate the total bulk-out size in bytes in one bulk-transmission
	UCHAR			BulkOutPipeId;
	BOOLEAN			IRPPending;
	BOOLEAN			LastOne;
	BOOLEAN			bCurWriting;
	BOOLEAN			bRingEmpty;
	BOOLEAN			bCopySavePad;
	UCHAR			SavedPad[8];
	UCHAR			Header_802_3[LENGTH_802_3];
	ULONG			CurWritePosition;		// Indicate the buffer offset which packet will be inserted start from.
	ULONG			CurWriteRealPos;		// Indicate the buffer offset which packet now are writing to.
	ULONG			NextBulkOutPosition;	// Indicate the buffer start offset of a bulk-transmission
	ULONG			ENextBulkOutPosition;	// Indicate the buffer end offset of a bulk-transmission
	UINT			TxRate;
	dma_addr_t		data_dma;		// urb dma on linux
}	HT_TX_CONTEXT, *PHT_TX_CONTEXT, **PPHT_TX_CONTEXT;


//
// Structure to keep track of receive packets and buffers to indicate
// receive data to the protocol.
//
typedef struct _RX_CONTEXT
{
	PUCHAR				TransferBuffer; 
	PVOID				pAd;
	PIRP				pIrp;//used to cancel pending bulk in.
	PURB				pUrb;
	//These 2 Boolean shouldn't both be 1 at the same time. 
	ULONG				BulkInOffset;	// number of packets waiting for reordering . 
//	BOOLEAN				ReorderInUse;	// At least one packet in this buffer are in reordering buffer and wait for receive indication
	BOOLEAN				bRxHandling;	// Notify this packet is being process now.
	BOOLEAN				InUse;			// USB Hardware Occupied. Wait for USB HW to put packet. 
	BOOLEAN				Readable;		// Receive Complete back. OK for driver to indicate receiving packet. 
	BOOLEAN				IRPPending;		// TODO: To be removed
	atomic_t			IrpLock;
	NDIS_SPIN_LOCK		RxContextLock;
	dma_addr_t			data_dma;		// urb dma on linux
}	RX_CONTEXT, *PRX_CONTEXT;
#endif // RT2870 //


//
//  NDIS Version definitions
//
#ifdef  NDIS50_MINIPORT
#define RTMP_NDIS_MAJOR_VERSION     5
#define RTMP_NDIS_MINOR_VERSION     0
#endif

#ifdef  NDIS51_MINIPORT
#define RTMP_NDIS_MAJOR_VERSION     5
#define RTMP_NDIS_MINOR_VERSION     1
#endif

extern  char    NIC_VENDOR_DESC[];
extern  int     NIC_VENDOR_DESC_LEN;

extern  unsigned char   SNAP_AIRONET[];
extern  unsigned char   CipherSuiteCiscoCCKM[];
extern  unsigned char   CipherSuiteCiscoCCKMLen;
extern	unsigned char	CipherSuiteCiscoCCKM24[];
extern	unsigned char	CipherSuiteCiscoCCKM24Len;
extern  unsigned char   CipherSuiteCCXTkip[];
extern  unsigned char   CipherSuiteCCXTkipLen;
extern  unsigned char   CISCO_OUI[];
extern  UCHAR	BaSizeArray[4];

extern UCHAR BROADCAST_ADDR[MAC_ADDR_LEN];
extern UCHAR MULTICAST_ADDR[MAC_ADDR_LEN];
extern UCHAR ZERO_MAC_ADDR[MAC_ADDR_LEN];
extern ULONG BIT32[32];
extern UCHAR BIT8[8];
extern char* CipherName[];
extern char* MCSToMbps[];
extern UCHAR	 RxwiMCSToOfdmRate[12];
extern UCHAR SNAP_802_1H[6];
extern UCHAR SNAP_BRIDGE_TUNNEL[6];
extern UCHAR SNAP_AIRONET[8];
extern UCHAR CKIP_LLC_SNAP[8];
extern UCHAR EAPOL_LLC_SNAP[8];
extern UCHAR EAPOL[2];
extern UCHAR IPX[2];
extern UCHAR APPLE_TALK[2];
extern UCHAR RateIdToPlcpSignal[12]; // see IEEE802.11a-1999 p.14
extern UCHAR	 OfdmRateToRxwiMCS[];
extern UCHAR OfdmSignalToRateId[16] ;
extern UCHAR default_cwmin[4];
extern UCHAR default_cwmax[4];
extern UCHAR default_sta_aifsn[4];
extern UCHAR MapUserPriorityToAccessCategory[8];

extern USHORT RateUpPER[];
extern USHORT RateDownPER[];
extern UCHAR  Phy11BNextRateDownward[];
extern UCHAR  Phy11BNextRateUpward[];
extern UCHAR  Phy11BGNextRateDownward[];
extern UCHAR  Phy11BGNextRateUpward[];
extern UCHAR  Phy11ANextRateDownward[];
extern UCHAR  Phy11ANextRateUpward[];
extern CHAR   RssiSafeLevelForTxRate[];
extern UCHAR  RateIdToMbps[];
extern USHORT RateIdTo500Kbps[];

extern UCHAR  CipherSuiteWpaNoneTkip[];
extern UCHAR  CipherSuiteWpaNoneTkipLen;

extern UCHAR  CipherSuiteWpaNoneAes[];
extern UCHAR  CipherSuiteWpaNoneAesLen;

extern UCHAR  SsidIe;
extern UCHAR  SupRateIe;
extern UCHAR  ExtRateIe;

#ifdef DOT11_N_SUPPORT
extern UCHAR  HtCapIe;
extern UCHAR  AddHtInfoIe;
extern UCHAR  NewExtChanIe;
#ifdef DOT11N_DRAFT3
extern UCHAR  ExtHtCapIe;
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //

extern UCHAR  ErpIe;
extern UCHAR  DsIe;
extern UCHAR  TimIe;
extern UCHAR  WpaIe;
extern UCHAR  Wpa2Ie;
extern UCHAR  IbssIe;
extern UCHAR  Ccx2Ie;
extern UCHAR  WapiIe;

extern UCHAR  WPA_OUI[];
extern UCHAR  RSN_OUI[];
extern UCHAR  WAPI_OUI[];
extern UCHAR  WME_INFO_ELEM[];
extern UCHAR  WME_PARM_ELEM[];
extern UCHAR  Ccx2QosInfo[];
extern UCHAR  Ccx2IeInfo[];
extern UCHAR  RALINK_OUI[];
extern UCHAR  PowerConstraintIE[];


extern UCHAR  RateSwitchTable[];
extern UCHAR  RateSwitchTable11B[];
extern UCHAR  RateSwitchTable11G[];
extern UCHAR  RateSwitchTable11BG[];

#ifdef DOT11_N_SUPPORT
extern UCHAR  RateSwitchTable11BGN1S[];
extern UCHAR  RateSwitchTable11BGN2S[];
extern UCHAR  RateSwitchTable11BGN2SForABand[];
extern UCHAR  RateSwitchTable11N1S[];
extern UCHAR  RateSwitchTable11N2S[];
extern UCHAR  RateSwitchTable11N2SForABand[];

#endif // DOT11_N_SUPPORT //

#define	MAXSEQ		(0xFFF)

#ifdef RALINK_ATE
typedef	struct _ATE_INFO {
	UCHAR	Mode;
	CHAR	TxPower0;
	CHAR	TxPower1;
	CHAR    TxAntennaSel;
	CHAR    RxAntennaSel;
	TXWI_STRUC  TxWI; 	  // TXWI
	USHORT	QID;
	UCHAR	Addr1[MAC_ADDR_LEN];
	UCHAR	Addr2[MAC_ADDR_LEN];
	UCHAR	Addr3[MAC_ADDR_LEN];
	UCHAR	Channel;
	UINT32	TxLength;
	UINT32	TxCount;
	UINT32	TxDoneCount; // Tx DMA Done
	UINT32	RFFreqOffset;
	BOOLEAN	bRxFer;
	BOOLEAN	bQATxStart; // Have compiled QA in and use it to ATE tx.
	BOOLEAN	bQARxStart;	// Have compiled QA in and use it to ATE rx.
	UINT32	RxTotalCnt;
	UINT32	RxCntPerSec;

	CHAR	LastSNR0;             // last received SNR
	CHAR    LastSNR1;             // last received SNR for 2nd  antenna
	CHAR    LastRssi0;            // last received RSSI
	CHAR    LastRssi1;            // last received RSSI for 2nd  antenna
	CHAR    LastRssi2;            // last received RSSI for 3rd  antenna
	CHAR    AvgRssi0;             // last 8 frames' average RSSI
	CHAR    AvgRssi1;             // last 8 frames' average RSSI
	CHAR    AvgRssi2;             // last 8 frames' average RSSI
	SHORT   AvgRssi0X8;           // sum of last 8 frames' RSSI
	SHORT   AvgRssi1X8;           // sum of last 8 frames' RSSI
	SHORT   AvgRssi2X8;           // sum of last 8 frames' RSSI

	UINT32	NumOfAvgRssiSample;

#ifdef RALINK_28xx_QA
	// Tx frame
#ifdef RT2870
	/* not used in RT2860 */
	TXINFO_STRUC		TxInfo; // TxInfo
#endif // RT2870 //
	USHORT		HLen; // Header Length
	USHORT		PLen; // Pattern Length
	UCHAR 		Header[32]; // Header buffer
	UCHAR		Pattern[32]; // Pattern buffer
	USHORT		DLen; // Data Length
	USHORT		seq;
	UINT32		CID;
	pid_t 		AtePid;
	// counters
	UINT32		U2M;
	UINT32		OtherData;
	UINT32		Beacon;
	UINT32		OtherCount;
	UINT32		TxAc0;
	UINT32		TxAc1;
	UINT32		TxAc2;
	UINT32		TxAc3;
	UINT32		TxHCCA;
	UINT32		TxMgmt;
	UINT32		RSSI0;
	UINT32		RSSI1;
	UINT32		RSSI2;
	UINT32		SNR0;
	UINT32		SNR1;
	// control
	//UINT32		Repeat; // Tx Cpu count
	UCHAR		TxStatus; // task Tx status // 0 --> task is idle, 1 --> task is running
#endif // RALINK_28xx_QA //
}	ATE_INFO, *PATE_INFO;

#ifdef RALINK_28xx_QA
struct ate_racfghdr {
 	UINT32		magic_no;
	USHORT		command_type;
	USHORT		command_id;
	USHORT		length;
	USHORT		sequence;
	USHORT		status;
	UCHAR		data[2046];
}  __attribute__((packed));
#endif // RALINK_28xx_QA //
#endif // RALINK_ATE //

#ifdef DOT11_N_SUPPORT
struct reordering_mpdu
{
	struct reordering_mpdu	*next;
	PNDIS_PACKET			pPacket;		/* coverted to 802.3 frame */
	int						Sequence;		/* sequence number of MPDU */
	BOOLEAN					bAMSDU;
};

struct reordering_list
{
	struct reordering_mpdu *next;
	int 	qlen;
};

struct reordering_mpdu_pool
{
	PVOID					mem;
	NDIS_SPIN_LOCK			lock;
	struct reordering_list 	freelist;
};
#endif // DOT11_N_SUPPORT //

typedef struct 	_RSSI_SAMPLE {
	CHAR			LastRssi0;             // last received RSSI
	CHAR			LastRssi1;             // last received RSSI
	CHAR			LastRssi2;             // last received RSSI
	CHAR			AvgRssi0;
	CHAR			AvgRssi1;
	CHAR			AvgRssi2;
	SHORT			AvgRssi0X8;
	SHORT			AvgRssi1X8;
	SHORT			AvgRssi2X8;
} RSSI_SAMPLE;

//
//  Queue structure and macros
//
typedef struct  _QUEUE_ENTRY    {
	struct _QUEUE_ENTRY     *Next;
}   QUEUE_ENTRY, *PQUEUE_ENTRY;

// Queue structure
typedef struct  _QUEUE_HEADER   {
	PQUEUE_ENTRY    Head;
	PQUEUE_ENTRY    Tail;
	ULONG           Number;
}   QUEUE_HEADER, *PQUEUE_HEADER;

#define InitializeQueueHeader(QueueHeader)              \
{                                                       \
	(QueueHeader)->Head = (QueueHeader)->Tail = NULL;   \
	(QueueHeader)->Number = 0;                          \
}

#define RemoveHeadQueue(QueueHeader)                \
(QueueHeader)->Head;                                \
{                                                   \
	PQUEUE_ENTRY pNext;                             \
	if ((QueueHeader)->Head != NULL)				\
	{												\
		pNext = (QueueHeader)->Head->Next;          \
		(QueueHeader)->Head = pNext;                \
		if (pNext == NULL)                          \
			(QueueHeader)->Tail = NULL;             \
		(QueueHeader)->Number--;                    \
	}												\
}

#define InsertHeadQueue(QueueHeader, QueueEntry)            \
{                                                           \
		((PQUEUE_ENTRY)QueueEntry)->Next = (QueueHeader)->Head; \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
		if ((QueueHeader)->Tail == NULL)                        \
			(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);   \
		(QueueHeader)->Number++;                                \
}

#define InsertTailQueue(QueueHeader, QueueEntry)                \
{                                                               \
	((PQUEUE_ENTRY)QueueEntry)->Next = NULL;                    \
	if ((QueueHeader)->Tail)                                    \
		(QueueHeader)->Tail->Next = (PQUEUE_ENTRY)(QueueEntry); \
	else                                                        \
		(QueueHeader)->Head = (PQUEUE_ENTRY)(QueueEntry);       \
	(QueueHeader)->Tail = (PQUEUE_ENTRY)(QueueEntry);           \
	(QueueHeader)->Number++;                                    \
}

//
//  Macros for flag and ref count operations
//
#define RTMP_SET_FLAG(_M, _F)       ((_M)->Flags |= (_F))
#define RTMP_CLEAR_FLAG(_M, _F)     ((_M)->Flags &= ~(_F))
#define RTMP_CLEAR_FLAGS(_M)        ((_M)->Flags = 0)
#define RTMP_TEST_FLAG(_M, _F)      (((_M)->Flags & (_F)) != 0)
#define RTMP_TEST_FLAGS(_M, _F)     (((_M)->Flags & (_F)) == (_F))

#define OPSTATUS_SET_FLAG(_pAd, _F)     ((_pAd)->CommonCfg.OpStatusFlags |= (_F))
#define OPSTATUS_CLEAR_FLAG(_pAd, _F)   ((_pAd)->CommonCfg.OpStatusFlags &= ~(_F))
#define OPSTATUS_TEST_FLAG(_pAd, _F)    (((_pAd)->CommonCfg.OpStatusFlags & (_F)) != 0)

#define CLIENT_STATUS_SET_FLAG(_pEntry,_F)      ((_pEntry)->ClientStatusFlags |= (_F))
#define CLIENT_STATUS_CLEAR_FLAG(_pEntry,_F)    ((_pEntry)->ClientStatusFlags &= ~(_F))
#define CLIENT_STATUS_TEST_FLAG(_pEntry,_F)     (((_pEntry)->ClientStatusFlags & (_F)) != 0)

#define RX_FILTER_SET_FLAG(_pAd, _F)    ((_pAd)->CommonCfg.PacketFilter |= (_F))
#define RX_FILTER_CLEAR_FLAG(_pAd, _F)  ((_pAd)->CommonCfg.PacketFilter &= ~(_F))
#define RX_FILTER_TEST_FLAG(_pAd, _F)   (((_pAd)->CommonCfg.PacketFilter & (_F)) != 0)


#define CKIP_KP_ON(_p)				((((_p)->StaCfg.CkipFlag) & 0x10) && ((_p)->StaCfg.bCkipCmicOn == TRUE))
#define CKIP_CMIC_ON(_p)			((((_p)->StaCfg.CkipFlag) & 0x08) && ((_p)->StaCfg.bCkipCmicOn == TRUE))


#define INC_RING_INDEX(_idx, _RingSize)    \
{                                          \
    (_idx) = (_idx+1) % (_RingSize);       \
}

// We will have a cost down version which mac version is 0x3090xxxx
#define IS_RT3090(_pAd)				((((_pAd)->MACVersion & 0xffff0000) == 0x30710000) || (((_pAd)->MACVersion & 0xffff0000) == 0x30900000))

#define IS_RT3070(_pAd)				(((_pAd)->MACVersion & 0xffff0000) == 0x30700000)
#define IS_RT3071(_pAd)				(((_pAd)->MACVersion & 0xffff0000) == 0x30710000)
#define IS_RT2070(_pAd)				(((_pAd)->RfIcType == RFIC_2020) || ((_pAd)->EFuseTag == 0x27))

#define IS_RT30xx(_pAd)				(((_pAd)->MACVersion & 0xfff00000) == 0x30700000)

#define RING_PACKET_INIT(_TxRing, _idx)    \
{                                          \
    _TxRing->Cell[_idx].pNdisPacket = NULL;                              \
    _TxRing->Cell[_idx].pNextNdisPacket = NULL;                              \
}

#define TXDT_INIT(_TxD)    \
{                                          \
	NdisZeroMemory(_TxD, TXD_SIZE);	\
	_TxD->DMADONE = 1;                              \
}

//Set last data segment
#define RING_SET_LASTDS(_TxD, _IsSD0)    \
{                                          \
    if (_IsSD0) {_TxD->LastSec0 = 1;}     \
    else {_TxD->LastSec1 = 1;}     \
}

// Increase TxTsc value for next transmission
// TODO: 
// When i==6, means TSC has done one full cycle, do re-keying stuff follow specs
// Should send a special event microsoft defined to request re-key
#define INC_TX_TSC(_tsc)                                \
{                                                       \
    int i=0;                                            \
    while (++_tsc[i] == 0x0)                            \
    {                                                   \
        i++;                                            \
        if (i == 6)                                     \
            break;                                      \
    }                                                   \
}

#ifdef DOT11_N_SUPPORT
// StaActive.SupportedHtPhy.MCSSet is copied from AP beacon.  Don't need to update here.
#define COPY_HTSETTINGS_FROM_MLME_AUX_TO_ACTIVE_CFG(_pAd)                                 \
{                                                                                       \
	_pAd->StaActive.SupportedHtPhy.ChannelWidth = _pAd->MlmeAux.HtCapability.HtCapInfo.ChannelWidth;      \
	_pAd->StaActive.SupportedHtPhy.MimoPs = _pAd->MlmeAux.HtCapability.HtCapInfo.MimoPs;      \
	_pAd->StaActive.SupportedHtPhy.GF = _pAd->MlmeAux.HtCapability.HtCapInfo.GF;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor20 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor20;      \
	_pAd->StaActive.SupportedHtPhy.ShortGIfor40 = _pAd->MlmeAux.HtCapability.HtCapInfo.ShortGIfor40;      \
	_pAd->StaActive.SupportedHtPhy.TxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.TxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.RxSTBC = _pAd->MlmeAux.HtCapability.HtCapInfo.RxSTBC;      \
	_pAd->StaActive.SupportedHtPhy.ExtChanOffset = _pAd->MlmeAux.AddHtInfo.AddHtInfo.ExtChanOffset;      \
	_pAd->StaActive.SupportedHtPhy.RecomWidth = _pAd->MlmeAux.AddHtInfo.AddHtInfo.RecomWidth;      \
	_pAd->StaActive.SupportedHtPhy.OperaionMode = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.OperaionMode;      \
	_pAd->StaActive.SupportedHtPhy.NonGfPresent = _pAd->MlmeAux.AddHtInfo.AddHtInfo2.NonGfPresent;      \
	NdisMoveMemory((_pAd)->MacTab.Content[BSSID_WCID].HTCapability.MCSSet, (_pAd)->StaActive.SupportedPhyInfo.MCSSet, sizeof(UCHAR) * 16);\
}

#define COPY_AP_HTSETTINGS_FROM_BEACON(_pAd, _pHtCapability)                                 \
{                                                                                       \
	_pAd->MacTab.Content[BSSID_WCID].AMsduSize = (UCHAR)(_pHtCapability->HtCapInfo.AMsduSize);	\
	_pAd->MacTab.Content[BSSID_WCID].MmpsMode= (UCHAR)(_pHtCapability->HtCapInfo.MimoPs);	\
	_pAd->MacTab.Content[BSSID_WCID].MaxRAmpduFactor = (UCHAR)(_pHtCapability->HtCapParm.MaxRAmpduFactor);	\
}
#endif // DOT11_N_SUPPORT //

//
// MACRO for 32-bit PCI register read / write
//
// Usage : RTMP_IO_READ32(
//              PRTMP_ADAPTER pAd,
//              ULONG Register_Offset,
//              PULONG  pValue)
//
//         RTMP_IO_WRITE32(
//              PRTMP_ADAPTER pAd,
//              ULONG Register_Offset,
//              ULONG Value)
//

//
// BBP & RF are using indirect access. Before write any value into it.
// We have to make sure there is no outstanding command pending via checking busy bit.
//
#define MAX_BUSY_COUNT  100         // Number of retry before failing access BBP & RF indirect register
//

#ifdef RT2870
#define RTMP_RF_IO_WRITE32(_A, _V)                 RTUSBWriteRFRegister(_A, _V)
#define RTMP_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)   RTUSBReadBBPRegister(_A, _I, _pV)
#define RTMP_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)   RTUSBWriteBBPRegister(_A, _I, _V)

#define BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)			RTUSBWriteBBPRegister(_A, _I, _V)
#define BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)   		RTUSBReadBBPRegister(_A, _I, _pV)
#endif // RT2870 //

#ifdef RT30xx
#define RTMP_RF_IO_READ8_BY_REG_ID(_A, _I, _pV)    RT30xxReadRFRegister(_A, _I, _pV)
#define RTMP_RF_IO_WRITE8_BY_REG_ID(_A, _I, _V)    RT30xxWriteRFRegister(_A, _I, _V)
#endif // RT30xx //

#define     MAP_CHANNEL_ID_TO_KHZ(ch, khz)  {               \
                switch (ch)                                 \
                {                                           \
                    case 1:     khz = 2412000;   break;     \
                    case 2:     khz = 2417000;   break;     \
                    case 3:     khz = 2422000;   break;     \
                    case 4:     khz = 2427000;   break;     \
                    case 5:     khz = 2432000;   break;     \
                    case 6:     khz = 2437000;   break;     \
                    case 7:     khz = 2442000;   break;     \
                    case 8:     khz = 2447000;   break;     \
                    case 9:     khz = 2452000;   break;     \
                    case 10:    khz = 2457000;   break;     \
                    case 11:    khz = 2462000;   break;     \
                    case 12:    khz = 2467000;   break;     \
                    case 13:    khz = 2472000;   break;     \
                    case 14:    khz = 2484000;   break;     \
                    case 36:  /* UNII */  khz = 5180000;   break;     \
                    case 40:  /* UNII */  khz = 5200000;   break;     \
                    case 44:  /* UNII */  khz = 5220000;   break;     \
                    case 48:  /* UNII */  khz = 5240000;   break;     \
                    case 52:  /* UNII */  khz = 5260000;   break;     \
                    case 56:  /* UNII */  khz = 5280000;   break;     \
                    case 60:  /* UNII */  khz = 5300000;   break;     \
                    case 64:  /* UNII */  khz = 5320000;   break;     \
                    case 149: /* UNII */  khz = 5745000;   break;     \
                    case 153: /* UNII */  khz = 5765000;   break;     \
                    case 157: /* UNII */  khz = 5785000;   break;     \
                    case 161: /* UNII */  khz = 5805000;   break;     \
                    case 165: /* UNII */  khz = 5825000;   break;     \
                    case 100: /* HiperLAN2 */  khz = 5500000;   break;     \
                    case 104: /* HiperLAN2 */  khz = 5520000;   break;     \
                    case 108: /* HiperLAN2 */  khz = 5540000;   break;     \
                    case 112: /* HiperLAN2 */  khz = 5560000;   break;     \
                    case 116: /* HiperLAN2 */  khz = 5580000;   break;     \
                    case 120: /* HiperLAN2 */  khz = 5600000;   break;     \
                    case 124: /* HiperLAN2 */  khz = 5620000;   break;     \
                    case 128: /* HiperLAN2 */  khz = 5640000;   break;     \
                    case 132: /* HiperLAN2 */  khz = 5660000;   break;     \
                    case 136: /* HiperLAN2 */  khz = 5680000;   break;     \
                    case 140: /* HiperLAN2 */  khz = 5700000;   break;     \
                    case 34:  /* Japan MMAC */   khz = 5170000;   break;   \
                    case 38:  /* Japan MMAC */   khz = 5190000;   break;   \
                    case 42:  /* Japan MMAC */   khz = 5210000;   break;   \
                    case 46:  /* Japan MMAC */   khz = 5230000;   break;   \
                    case 184: /* Japan */   khz = 4920000;   break;   \
                    case 188: /* Japan */   khz = 4940000;   break;   \
                    case 192: /* Japan */   khz = 4960000;   break;   \
                    case 196: /* Japan */   khz = 4980000;   break;   \
                    case 208: /* Japan, means J08 */   khz = 5040000;   break;   \
                    case 212: /* Japan, means J12 */   khz = 5060000;   break;   \
                    case 216: /* Japan, means J16 */   khz = 5080000;   break;   \
                    default:    khz = 2412000;   break;     \
                }                                           \
            }

#define     MAP_KHZ_TO_CHANNEL_ID(khz, ch)  {               \
                switch (khz)                                \
                {                                           \
                    case 2412000:    ch = 1;     break;     \
                    case 2417000:    ch = 2;     break;     \
                    case 2422000:    ch = 3;     break;     \
                    case 2427000:    ch = 4;     break;     \
                    case 2432000:    ch = 5;     break;     \
                    case 2437000:    ch = 6;     break;     \
                    case 2442000:    ch = 7;     break;     \
                    case 2447000:    ch = 8;     break;     \
                    case 2452000:    ch = 9;     break;     \
                    case 2457000:    ch = 10;    break;     \
                    case 2462000:    ch = 11;    break;     \
                    case 2467000:    ch = 12;    break;     \
                    case 2472000:    ch = 13;    break;     \
                    case 2484000:    ch = 14;    break;     \
                    case 5180000:    ch = 36;  /* UNII */  break;     \
                    case 5200000:    ch = 40;  /* UNII */  break;     \
                    case 5220000:    ch = 44;  /* UNII */  break;     \
                    case 5240000:    ch = 48;  /* UNII */  break;     \
                    case 5260000:    ch = 52;  /* UNII */  break;     \
                    case 5280000:    ch = 56;  /* UNII */  break;     \
                    case 5300000:    ch = 60;  /* UNII */  break;     \
                    case 5320000:    ch = 64;  /* UNII */  break;     \
                    case 5745000:    ch = 149; /* UNII */  break;     \
                    case 5765000:    ch = 153; /* UNII */  break;     \
                    case 5785000:    ch = 157; /* UNII */  break;     \
                    case 5805000:    ch = 161; /* UNII */  break;     \
                    case 5825000:    ch = 165; /* UNII */  break;     \
                    case 5500000:    ch = 100; /* HiperLAN2 */  break;     \
                    case 5520000:    ch = 104; /* HiperLAN2 */  break;     \
                    case 5540000:    ch = 108; /* HiperLAN2 */  break;     \
                    case 5560000:    ch = 112; /* HiperLAN2 */  break;     \
                    case 5580000:    ch = 116; /* HiperLAN2 */  break;     \
                    case 5600000:    ch = 120; /* HiperLAN2 */  break;     \
                    case 5620000:    ch = 124; /* HiperLAN2 */  break;     \
                    case 5640000:    ch = 128; /* HiperLAN2 */  break;     \
                    case 5660000:    ch = 132; /* HiperLAN2 */  break;     \
                    case 5680000:    ch = 136; /* HiperLAN2 */  break;     \
                    case 5700000:    ch = 140; /* HiperLAN2 */  break;     \
                    case 5170000:    ch = 34;  /* Japan MMAC */   break;   \
                    case 5190000:    ch = 38;  /* Japan MMAC */   break;   \
                    case 5210000:    ch = 42;  /* Japan MMAC */   break;   \
                    case 5230000:    ch = 46;  /* Japan MMAC */   break;   \
                    case 4920000:    ch = 184; /* Japan */  break;   \
                    case 4940000:    ch = 188; /* Japan */  break;   \
                    case 4960000:    ch = 192; /* Japan */  break;   \
                    case 4980000:    ch = 196; /* Japan */  break;   \
                    case 5040000:    ch = 208; /* Japan, means J08 */  break;   \
                    case 5060000:    ch = 212; /* Japan, means J12 */  break;   \
                    case 5080000:    ch = 216; /* Japan, means J16 */  break;   \
                    default:         ch = 1;     break;     \
                }                                           \
            }

//
// Common fragment list structure -  Identical to the scatter gather frag list structure
//
//#define RTMP_SCATTER_GATHER_ELEMENT         SCATTER_GATHER_ELEMENT 
//#define PRTMP_SCATTER_GATHER_ELEMENT        PSCATTER_GATHER_ELEMENT 
#define NIC_MAX_PHYS_BUF_COUNT              8

typedef struct _RTMP_SCATTER_GATHER_ELEMENT {
    PVOID		Address;
    ULONG		Length;
    PULONG		Reserved;
} RTMP_SCATTER_GATHER_ELEMENT, *PRTMP_SCATTER_GATHER_ELEMENT;


typedef struct _RTMP_SCATTER_GATHER_LIST {
    ULONG  NumberOfElements;
    PULONG Reserved;
    RTMP_SCATTER_GATHER_ELEMENT Elements[NIC_MAX_PHYS_BUF_COUNT];
} RTMP_SCATTER_GATHER_LIST, *PRTMP_SCATTER_GATHER_LIST;

//
//  Some utility macros
//
#ifndef min
#define min(_a, _b)     (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b)     (((_a) > (_b)) ? (_a) : (_b))
#endif

#define GET_LNA_GAIN(_pAd)	((_pAd->LatchRfRegs.Channel <= 14) ? (_pAd->BLNAGain) : ((_pAd->LatchRfRegs.Channel <= 64) ? (_pAd->ALNAGain0) : ((_pAd->LatchRfRegs.Channel <= 128) ? (_pAd->ALNAGain1) : (_pAd->ALNAGain2))))

#define INC_COUNTER64(Val)          (Val.QuadPart++)

#define INFRA_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_INFRA_ON))
#define ADHOC_ON(_p)                (OPSTATUS_TEST_FLAG(_p, fOP_STATUS_ADHOC_ON))
#define MONITOR_ON(_p)              (((_p)->StaCfg.BssType) == BSS_MONITOR)
#define IDLE_ON(_p)                 (!INFRA_ON(_p) && !ADHOC_ON(_p))

// Check LEAP & CCKM flags
#define LEAP_ON(_p)                 (((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP)
#define LEAP_CCKM_ON(_p)            ((((_p)->StaCfg.LeapAuthMode) == CISCO_AuthModeLEAP) && ((_p)->StaCfg.LeapAuthInfo.CCKM == TRUE))

// if orginal Ethernet frame contains no LLC/SNAP, then an extra LLC/SNAP encap is required 
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_START(_pBufVA, _pExtraLlcSnapEncap)		\
{																\
	if (((*(_pBufVA + 12) << 8) + *(_pBufVA + 13)) > 1500)		\
	{															\
		_pExtraLlcSnapEncap = SNAP_802_1H;						\
		if (NdisEqualMemory(IPX, _pBufVA + 12, 2) || 			\
			NdisEqualMemory(APPLE_TALK, _pBufVA + 12, 2))		\
		{														\
			_pExtraLlcSnapEncap = SNAP_BRIDGE_TUNNEL;			\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}

// New Define for new Tx Path.
#define EXTRA_LLCSNAP_ENCAP_FROM_PKT_OFFSET(_pBufVA, _pExtraLlcSnapEncap)	\
{																\
	if (((*(_pBufVA) << 8) + *(_pBufVA + 1)) > 1500)			\
	{															\
		_pExtraLlcSnapEncap = SNAP_802_1H;						\
		if (NdisEqualMemory(IPX, _pBufVA, 2) || 				\
			NdisEqualMemory(APPLE_TALK, _pBufVA, 2))			\
		{														\
			_pExtraLlcSnapEncap = SNAP_BRIDGE_TUNNEL;			\
		}														\
	}															\
	else														\
	{															\
		_pExtraLlcSnapEncap = NULL;								\
	}															\
}


#define MAKE_802_3_HEADER(_p, _pMac1, _pMac2, _pType)                   \
{                                                                       \
    NdisMoveMemory(_p, _pMac1, MAC_ADDR_LEN);                           \
    NdisMoveMemory((_p + MAC_ADDR_LEN), _pMac2, MAC_ADDR_LEN);          \
    NdisMoveMemory((_p + MAC_ADDR_LEN * 2), _pType, LENGTH_802_3_TYPE); \
}

// if pData has no LLC/SNAP (neither RFC1042 nor Bridge tunnel), keep it that way.
// else if the received frame is LLC/SNAP-encaped IPX or APPLETALK, preserve the LLC/SNAP field 
// else remove the LLC/SNAP field from the result Ethernet frame
// Patch for WHQL only, which did not turn on Netbios but use IPX within its payload
// Note:
//     _pData & _DataSize may be altered (remove 8-byte LLC/SNAP) by this MACRO
//     _pRemovedLLCSNAP: pointer to removed LLC/SNAP; NULL is not removed
#define CONVERT_TO_802_3(_p8023hdr, _pDA, _pSA, _pData, _DataSize, _pRemovedLLCSNAP)      \
{                                                                       \
    char LLC_Len[2];                                                    \
                                                                        \
    _pRemovedLLCSNAP = NULL;                                            \
    if (NdisEqualMemory(SNAP_802_1H, _pData, 6)  ||                     \
        NdisEqualMemory(SNAP_BRIDGE_TUNNEL, _pData, 6))                 \
    {                                                                   \
        PUCHAR pProto = _pData + 6;                                     \
                                                                        \
        if ((NdisEqualMemory(IPX, pProto, 2) || NdisEqualMemory(APPLE_TALK, pProto, 2)) &&  \
            NdisEqualMemory(SNAP_802_1H, _pData, 6))                    \
        {                                                               \
            LLC_Len[0] = (UCHAR)(_DataSize / 256);                      \
            LLC_Len[1] = (UCHAR)(_DataSize % 256);                      \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);          \
        }                                                               \
        else                                                            \
        {                                                               \
            MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, pProto);           \
            _pRemovedLLCSNAP = _pData;                                  \
            _DataSize -= LENGTH_802_1_H;                                \
            _pData += LENGTH_802_1_H;                                   \
        }                                                               \
    }                                                                   \
    else                                                                \
    {                                                                   \
        LLC_Len[0] = (UCHAR)(_DataSize / 256);                          \
        LLC_Len[1] = (UCHAR)(_DataSize % 256);                          \
        MAKE_802_3_HEADER(_p8023hdr, _pDA, _pSA, LLC_Len);              \
    }                                                                   \
}

#define SWITCH_AB( _pAA, _pBB)    \
{                                                                           \
    PVOID pCC;                                                          \
    pCC = _pBB;                                                 \
    _pBB = _pAA;                                                 \
    _pAA = pCC;                                                 \
}

// Enqueue this frame to MLME engine
// We need to enqueue the whole frame because MLME need to pass data type
// information from 802.11 header
#ifdef RT2870
#define REPORT_MGMT_FRAME_TO_MLME(_pAd, Wcid, _pFrame, _FrameSize, _Rssi0, _Rssi1, _Rssi2, _PlcpSignal)        \
{                                                                                       \
    UINT32 High32TSF=0, Low32TSF=0;                                                          \
    MlmeEnqueueForRecv(_pAd, Wcid, High32TSF, Low32TSF, (UCHAR)_Rssi0, (UCHAR)_Rssi1,(UCHAR)_Rssi2,_FrameSize, _pFrame, (UCHAR)_PlcpSignal);   \
}
#endif // RT2870 //

#ifdef RT30xx
//Need to collect each ant's rssi concurrently
//rssi1 is report to pair2 Ant and rss2 is reprot to pair1 Ant when 4 Ant
#define COLLECT_RX_ANTENNA_AVERAGE_RSSI(_pAd, _rssi1, _rssi2)					\
{																				\
	SHORT	AvgRssi;															\
	UCHAR	UsedAnt;															\
	if (_pAd->RxAnt.EvaluatePeriod == 0)									\
	{																		\
		UsedAnt = _pAd->RxAnt.Pair1PrimaryRxAnt;							\
		AvgRssi = _pAd->RxAnt.Pair1AvgRssi[UsedAnt];						\
		if (AvgRssi < 0)													\
			AvgRssi = AvgRssi - (AvgRssi >> 3) + _rssi1;					\
		else																\
			AvgRssi = _rssi1 << 3;											\
		_pAd->RxAnt.Pair1AvgRssi[UsedAnt] = AvgRssi;						\
	}																		\
	else																	\
	{																		\
		UsedAnt = _pAd->RxAnt.Pair1SecondaryRxAnt;							\
		AvgRssi = _pAd->RxAnt.Pair1AvgRssi[UsedAnt];						\
		if ((AvgRssi < 0) && (_pAd->RxAnt.FirstPktArrivedWhenEvaluate))		\
			AvgRssi = AvgRssi - (AvgRssi >> 3) + _rssi1;					\
		else																\
		{																	\
			_pAd->RxAnt.FirstPktArrivedWhenEvaluate = TRUE;					\
			AvgRssi = _rssi1 << 3;											\
		}																	\
		_pAd->RxAnt.Pair1AvgRssi[UsedAnt] = AvgRssi;						\
		_pAd->RxAnt.RcvPktNumWhenEvaluate++;								\
	}																		\
}
#endif // RT30xx //


#define NDIS_QUERY_BUFFER(_NdisBuf, _ppVA, _pBufLen)                    \
    NdisQueryBuffer(_NdisBuf, _ppVA, _pBufLen)

#define MAC_ADDR_EQUAL(pAddr1,pAddr2)           RTMPEqualMemory((PVOID)(pAddr1), (PVOID)(pAddr2), MAC_ADDR_LEN)
#define SSID_EQUAL(ssid1, len1, ssid2, len2)    ((len1==len2) && (RTMPEqualMemory(ssid1, ssid2, len1)))

//
// Check if it is Japan W53(ch52,56,60,64) channel.
//
#define JapanChannelCheck(channel)  ((channel == 52) || (channel == 56) || (channel == 60) || (channel == 64))



//
// Register set pair for initialzation register set definition
//
typedef struct  _RTMP_REG_PAIR
{
	ULONG   Register;
	ULONG   Value;
} RTMP_REG_PAIR, *PRTMP_REG_PAIR;

typedef struct  _REG_PAIR
{
	UCHAR   Register;
	UCHAR   Value;
} REG_PAIR, *PREG_PAIR;

//
// Register set pair for initialzation register set definition
//
typedef struct  _RTMP_RF_REGS
{
	UCHAR   Channel;
	ULONG   R1;
	ULONG   R2;
	ULONG   R3;
	ULONG   R4;
} RTMP_RF_REGS, *PRTMP_RF_REGS;

typedef struct _FREQUENCY_ITEM {
	UCHAR	Channel;
	UCHAR	N;
	UCHAR	R;
	UCHAR	K;
} FREQUENCY_ITEM, *PFREQUENCY_ITEM;

//
//  Data buffer for DMA operation, the buffer must be contiguous physical memory
//  Both DMA to / from CPU use the same structure.
//
typedef struct  _RTMP_DMABUF
{
	ULONG                   AllocSize;
	PVOID                   AllocVa;            // TxBuf virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // TxBuf physical address
} RTMP_DMABUF, *PRTMP_DMABUF;


typedef	union	_HEADER_802_11_SEQ{
#ifdef RT_BIG_ENDIAN
    struct {
   	USHORT			Sequence:12;
	USHORT			Frag:4;
    }   field;
#else
    struct {
	USHORT			Frag:4;
	USHORT			Sequence:12;
    }   field;
#endif
    USHORT           value;
}	HEADER_802_11_SEQ, *PHEADER_802_11_SEQ;

//
//  Data buffer for DMA operation, the buffer must be contiguous physical memory
//  Both DMA to / from CPU use the same structure.
//
typedef struct  _RTMP_REORDERBUF
{
	BOOLEAN			IsFull;
	PVOID                   AllocVa;            // TxBuf virtual address
	UCHAR			Header802_3[14];
	HEADER_802_11_SEQ			Sequence;	//support compressed bitmap BA, so no consider fragment in BA
	UCHAR 		DataOffset;
	USHORT 		Datasize;
	ULONG                   AllocSize;
#ifdef RT2870
	PUCHAR					AllocPa; 
#endif // RT2870 //
}   RTMP_REORDERBUF, *PRTMP_REORDERBUF;

//
// Control block (Descriptor) for all ring descriptor DMA operation, buffer must be 
// contiguous physical memory. NDIS_PACKET stored the binding Rx packet descriptor
// which won't be released, driver has to wait until upper layer return the packet
// before giveing up this rx ring descriptor to ASIC. NDIS_BUFFER is assocaited pair
// to describe the packet buffer. For Tx, NDIS_PACKET stored the tx packet descriptor
// which driver should ACK upper layer when the tx is physically done or failed.
//
typedef struct _RTMP_DMACB
{
	ULONG                   AllocSize;          // Control block size
	PVOID                   AllocVa;            // Control block virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // Control block physical address
	PNDIS_PACKET pNdisPacket;
	PNDIS_PACKET pNextNdisPacket;

	RTMP_DMABUF             DmaBuf;             // Associated DMA buffer structure
} RTMP_DMACB, *PRTMP_DMACB;

typedef struct _RTMP_TX_BUF
{
	PQUEUE_ENTRY    Next;
	UCHAR           Index;
	ULONG                   AllocSize;          // Control block size
	PVOID                   AllocVa;            // Control block virtual address
	NDIS_PHYSICAL_ADDRESS   AllocPa;            // Control block physical address
} RTMP_TXBUF, *PRTMP_TXBUF;

typedef struct _RTMP_RX_BUF
{
	BOOLEAN           InUse;
	ULONG           	ByBaRecIndex;
	RTMP_REORDERBUF	MAP_RXBuf[MAX_RX_REORDERBUF];
} RTMP_RXBUF, *PRTMP_RXBUF;
typedef struct _RTMP_TX_RING
{
	RTMP_DMACB  Cell[TX_RING_SIZE];
	UINT32		TxCpuIdx;	
	UINT32		TxDmaIdx;   
	UINT32		TxSwFreeIdx; 	// software next free tx index
} RTMP_TX_RING, *PRTMP_TX_RING;

typedef struct _RTMP_RX_RING
{
	RTMP_DMACB  Cell[RX_RING_SIZE];
	UINT32		RxCpuIdx;
	UINT32		RxDmaIdx;
	INT32		RxSwReadIdx; 	// software next read index
} RTMP_RX_RING, *PRTMP_RX_RING;

typedef struct _RTMP_MGMT_RING
{
	RTMP_DMACB  Cell[MGMT_RING_SIZE];
	UINT32		TxCpuIdx;
	UINT32		TxDmaIdx;
	UINT32		TxSwFreeIdx; // software next free tx index
} RTMP_MGMT_RING, *PRTMP_MGMT_RING;

//
//  Statistic counter structure
//
typedef struct _COUNTER_802_3
{
	// General Stats
	ULONG       GoodTransmits;
	ULONG       GoodReceives;
	ULONG       TxErrors;
	ULONG       RxErrors;
	ULONG       RxNoBuffer;

	// Ethernet Stats
	ULONG       RcvAlignmentErrors;
	ULONG       OneCollision;
	ULONG       MoreCollisions;

} COUNTER_802_3, *PCOUNTER_802_3;

typedef struct _COUNTER_802_11 {
	ULONG           Length;
	LARGE_INTEGER   LastTransmittedFragmentCount;
	LARGE_INTEGER   TransmittedFragmentCount;
	LARGE_INTEGER   MulticastTransmittedFrameCount;
	LARGE_INTEGER   FailedCount;
	LARGE_INTEGER   RetryCount;
	LARGE_INTEGER   MultipleRetryCount;
	LARGE_INTEGER   RTSSuccessCount;
	LARGE_INTEGER   RTSFailureCount;
	LARGE_INTEGER   ACKFailureCount;
	LARGE_INTEGER   FrameDuplicateCount;
	LARGE_INTEGER   ReceivedFragmentCount;
	LARGE_INTEGER   MulticastReceivedFrameCount;
	LARGE_INTEGER   FCSErrorCount;
} COUNTER_802_11, *PCOUNTER_802_11;

typedef struct _COUNTER_RALINK {
	ULONG           TransmittedByteCount;   // both successful and failure, used to calculate TX throughput
	ULONG           ReceivedByteCount;      // both CRC okay and CRC error, used to calculate RX throughput
	ULONG           BeenDisassociatedCount;
	ULONG           BadCQIAutoRecoveryCount;
	ULONG           PoorCQIRoamingCount;
	ULONG           MgmtRingFullCount;
	ULONG           RxCountSinceLastNULL;
	ULONG           RxCount;
	ULONG           RxRingErrCount;
	ULONG           KickTxCount;
	ULONG           TxRingErrCount;
	LARGE_INTEGER   RealFcsErrCount;
	ULONG           PendingNdisPacketCount;

	ULONG           OneSecOsTxCount[NUM_OF_TX_RING];
	ULONG           OneSecDmaDoneCount[NUM_OF_TX_RING];
	UINT32          OneSecTxDoneCount;
	ULONG           OneSecRxCount;
	UINT32          OneSecTxAggregationCount;
	UINT32          OneSecRxAggregationCount;

	UINT32   		OneSecFrameDuplicateCount;

#ifdef RT2870
	ULONG           OneSecTransmittedByteCount;   // both successful and failure, used to calculate TX throughput
#endif // RT2870 //

	UINT32          OneSecTxNoRetryOkCount;
	UINT32          OneSecTxRetryOkCount;
	UINT32          OneSecTxFailCount;
	UINT32          OneSecFalseCCACnt;      // CCA error count, for debug purpose, might move to global counter
	UINT32          OneSecRxOkCnt;          // RX without error
	UINT32          OneSecRxOkDataCnt;      // unicast-to-me DATA frame count
	UINT32          OneSecRxFcsErrCnt;      // CRC error
	UINT32          OneSecBeaconSentCnt;
	UINT32          LastOneSecTotalTxCount; // OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount
	UINT32          LastOneSecRxOkDataCnt;  // OneSecRxOkDataCnt
	ULONG		DuplicateRcv;
	ULONG		TxAggCount;
	ULONG		TxNonAggCount;
	ULONG		TxAgg1MPDUCount;
	ULONG		TxAgg2MPDUCount;
	ULONG		TxAgg3MPDUCount;
	ULONG		TxAgg4MPDUCount;
	ULONG		TxAgg5MPDUCount;
	ULONG		TxAgg6MPDUCount;
	ULONG		TxAgg7MPDUCount;
	ULONG		TxAgg8MPDUCount;
	ULONG		TxAgg9MPDUCount;
	ULONG		TxAgg10MPDUCount;
	ULONG		TxAgg11MPDUCount;
	ULONG		TxAgg12MPDUCount;
	ULONG		TxAgg13MPDUCount;
	ULONG		TxAgg14MPDUCount;
	ULONG		TxAgg15MPDUCount;
	ULONG		TxAgg16MPDUCount;

	LARGE_INTEGER       TransmittedOctetsInAMSDU; 
	LARGE_INTEGER       TransmittedAMSDUCount; 
	LARGE_INTEGER       ReceivedOctesInAMSDUCount; 
	LARGE_INTEGER       ReceivedAMSDUCount; 
	LARGE_INTEGER       TransmittedAMPDUCount;
	LARGE_INTEGER       TransmittedMPDUsInAMPDUCount;
	LARGE_INTEGER       TransmittedOctetsInAMPDUCount; 		
	LARGE_INTEGER       MPDUInReceivedAMPDUCount;
} COUNTER_RALINK, *PCOUNTER_RALINK;

typedef struct _PID_COUNTER {
	ULONG           TxAckRequiredCount;      // CRC error
	ULONG           TxAggreCount;
	ULONG           TxSuccessCount; // OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount
	ULONG		LastSuccessRate;
} PID_COUNTER, *PPID_COUNTER;

typedef struct _COUNTER_DRS {
	// to record the each TX rate's quality. 0 is best, the bigger the worse.
	USHORT          TxQuality[MAX_STEP_OF_TX_RATE_SWITCH];
	UCHAR           PER[MAX_STEP_OF_TX_RATE_SWITCH];
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
	ULONG           CurrTxRateStableTime; // # of second in current TX rate
	BOOLEAN         fNoisyEnvironment;
	BOOLEAN         fLastSecAccordingRSSI;
	UCHAR           LastSecTxRateChangeAction; // 0: no change, 1:rate UP, 2:rate down
	UCHAR			LastTimeTxRateChangeAction; //Keep last time value of LastSecTxRateChangeAction
	ULONG			LastTxOkCount;
} COUNTER_DRS, *PCOUNTER_DRS;

//
//  Arcfour Structure Added by PaulWu
//
typedef struct  _ARCFOUR
{
	UINT            X;
	UINT            Y;
	UCHAR           STATE[256];
} ARCFOURCONTEXT, *PARCFOURCONTEXT;

// MIMO Tx parameter, ShortGI, MCS, STBC, etc.  these are fields in TXWI too. just copy to TXWI.
typedef struct  _RECEIVE_SETTING {
#ifdef RT_BIG_ENDIAN
	USHORT		MIMO:1;
	USHORT		OFDM:1;
	USHORT		rsv:3;	
	USHORT		STBC:2;	//SPACE 
	USHORT		ShortGI:1;
	USHORT		Mode:2;	//channel bandwidth 20MHz or 40 MHz
	USHORT   	NumOfRX:2;                 // MIMO. WE HAVE 3R 
#else
	USHORT   	NumOfRX:2;                 // MIMO. WE HAVE 3R 
	USHORT		Mode:2;	//channel bandwidth 20MHz or 40 MHz
	USHORT		ShortGI:1;
	USHORT		STBC:2;	//SPACE 
	USHORT		rsv:3;	 
	USHORT		OFDM:1;
	USHORT		MIMO:1;
#endif
 } RECEIVE_SETTING, *PRECEIVE_SETTING;

// Shared key data structure
typedef struct  _WEP_KEY {
	UCHAR   KeyLen;                     // Key length for each key, 0: entry is invalid
	UCHAR   Key[MAX_LEN_OF_KEY];        // right now we implement 4 keys, 128 bits max
} WEP_KEY, *PWEP_KEY;

typedef struct _CIPHER_KEY {
	UCHAR   Key[16];            // right now we implement 4 keys, 128 bits max
	UCHAR   RxMic[8];			// make alignment 
	UCHAR   TxMic[8];
	UCHAR   TxTsc[6];           // 48bit TSC value
	UCHAR   RxTsc[6];           // 48bit TSC value
	UCHAR   CipherAlg;          // 0-none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128
	UCHAR   KeyLen; 
            // Key length for each key, 0: entry is invalid
	UCHAR   Type;               // Indicate Pairwise/Group when reporting MIC error
} CIPHER_KEY, *PCIPHER_KEY;

typedef struct _BBP_TUNING_STRUCT {
	BOOLEAN     Enable;
	UCHAR       FalseCcaCountUpperBound;  // 100 per sec
	UCHAR       FalseCcaCountLowerBound;  // 10 per sec
	UCHAR       R17LowerBound;            // specified in E2PROM
	UCHAR       R17UpperBound;            // 0x68 according to David Tung
	UCHAR       CurrentR17Value;
} BBP_TUNING, *PBBP_TUNING;

typedef struct _SOFT_RX_ANT_DIVERSITY_STRUCT {
	UCHAR     EvaluatePeriod;		 // 0:not evalute status, 1: evaluate status, 2: switching status
	UCHAR     EvaluateStableCnt;
	UCHAR     Pair1PrimaryRxAnt;     // 0:Ant-E1, 1:Ant-E2
	UCHAR     Pair1SecondaryRxAnt;   // 0:Ant-E1, 1:Ant-E2
	UCHAR     Pair2PrimaryRxAnt;     // 0:Ant-E3, 1:Ant-E4
	UCHAR     Pair2SecondaryRxAnt;   // 0:Ant-E3, 1:Ant-E4
	SHORT     Pair1AvgRssi[2];       // AvgRssi[0]:E1, AvgRssi[1]:E2
	SHORT     Pair2AvgRssi[2];       // AvgRssi[0]:E3, AvgRssi[1]:E4
	SHORT     Pair1LastAvgRssi;      // 
	SHORT     Pair2LastAvgRssi;      // 
	ULONG     RcvPktNumWhenEvaluate;
	BOOLEAN   FirstPktArrivedWhenEvaluate;
	RALINK_TIMER_STRUCT    RxAntDiversityTimer;
} SOFT_RX_ANT_DIVERSITY, *PSOFT_RX_ANT_DIVERSITY;

typedef struct _LEAP_AUTH_INFO {
	BOOLEAN         Enabled;        //Ture: Enable LEAP Authentication
	BOOLEAN         CCKM;           //Ture: Use Fast Reauthentication with CCKM
	UCHAR           Reserve[2];
	UCHAR           UserName[256];  //LEAP, User name
	ULONG           UserNameLen;
	UCHAR           Password[256];  //LEAP, User Password
	ULONG           PasswordLen;
} LEAP_AUTH_INFO, *PLEAP_AUTH_INFO;

typedef struct {
	UCHAR        Addr[MAC_ADDR_LEN];
	UCHAR        ErrorCode[2];  //00 01-Invalid authentication type
								//00 02-Authentication timeout
								//00 03-Challenge from AP failed
								//00 04-Challenge to AP failed
	BOOLEAN      Reported;
} ROGUEAP_ENTRY, *PROGUEAP_ENTRY;

typedef struct {
	UCHAR               RogueApNr;
	ROGUEAP_ENTRY       RogueApEntry[MAX_LEN_OF_BSS_TABLE];
} ROGUEAP_TABLE, *PROGUEAP_TABLE;

typedef struct {
	BOOLEAN     Enable;
	UCHAR       Delta;
	BOOLEAN     PlusSign;
} CCK_TX_POWER_CALIBRATE, *PCCK_TX_POWER_CALIBRATE;

//
// Receive Tuple Cache Format
//
typedef struct  _TUPLE_CACHE    {
	BOOLEAN         Valid;
	UCHAR           MacAddress[MAC_ADDR_LEN];
	USHORT          Sequence; 
	USHORT          Frag;
} TUPLE_CACHE, *PTUPLE_CACHE;

//
// Fragment Frame structure
//
typedef struct  _FRAGMENT_FRAME {
	PNDIS_PACKET    pFragPacket;
	ULONG       RxSize;
	USHORT      Sequence;
	USHORT      LastFrag;
	ULONG       Flags;          // Some extra frame information. bit 0: LLC presented
} FRAGMENT_FRAME, *PFRAGMENT_FRAME;


//
// Packet information for NdisQueryPacket
//
typedef struct  _PACKET_INFO    {
	UINT            PhysicalBufferCount;    // Physical breaks of buffer descripor chained
	UINT            BufferCount ;           // Number of Buffer descriptor chained
	UINT            TotalPacketLength ;     // Self explained
	PNDIS_BUFFER    pFirstBuffer;           // Pointer to first buffer descriptor
} PACKET_INFO, *PPACKET_INFO;

//
// Tkip Key structure which RC4 key & MIC calculation
//
typedef struct  _TKIP_KEY_INFO  {
	UINT        nBytesInM;  // # bytes in M for MICKEY
	ULONG       IV16;
	ULONG       IV32;   
	ULONG       K0;         // for MICKEY Low
	ULONG       K1;         // for MICKEY Hig
	ULONG       L;          // Current state for MICKEY
	ULONG       R;          // Current state for MICKEY
	ULONG       M;          // Message accumulator for MICKEY
	UCHAR       RC4KEY[16];
	UCHAR       MIC[8];
} TKIP_KEY_INFO, *PTKIP_KEY_INFO;

//
// Private / Misc data, counters for driver internal use
//
typedef struct  __PRIVATE_STRUC {
	UINT       SystemResetCnt;         // System reset counter
	UINT       TxRingFullCnt;          // Tx ring full occurrance number
	UINT       PhyRxErrCnt;            // PHY Rx error count, for debug purpose, might move to global counter
	// Variables for WEP encryption / decryption in rtmp_wep.c
	UINT       FCSCRC32;
	ARCFOURCONTEXT  WEPCONTEXT;
	// Tkip stuff
	TKIP_KEY_INFO   Tx;
	TKIP_KEY_INFO   Rx;
} PRIVATE_STRUC, *PPRIVATE_STRUC;

// structure to tune BBP R66 (BBP TUNING)
typedef struct _BBP_R66_TUNING {
	BOOLEAN     bEnable;
	USHORT      FalseCcaLowerThreshold;  // default 100
	USHORT      FalseCcaUpperThreshold;  // default 512
	UCHAR       R66Delta;
	UCHAR       R66CurrentValue;
	BOOLEAN		R66LowerUpperSelect; //Before LinkUp, Used LowerBound or UpperBound as R66 value.
} BBP_R66_TUNING, *PBBP_R66_TUNING;

// structure to store channel TX power
typedef struct _CHANNEL_TX_POWER {
	USHORT     RemainingTimeForUse;		//unit: sec
	UCHAR      Channel;
#ifdef DOT11N_DRAFT3
	BOOLEAN       bEffectedChannel;	// For BW 40 operating in 2.4GHz , the "effected channel" is the channel that is covered in 40Mhz.
#endif // DOT11N_DRAFT3 //
	CHAR       Power;
	CHAR       Power2;
	UCHAR      MaxTxPwr;
	UCHAR      DfsReq;
} CHANNEL_TX_POWER, *PCHANNEL_TX_POWER;

// structure to store 802.11j channel TX power
typedef struct _CHANNEL_11J_TX_POWER {
	UCHAR      Channel;
	UCHAR      BW;	// BW_10 or BW_20
	CHAR       Power;
	CHAR       Power2;
	USHORT     RemainingTimeForUse;		//unit: sec
} CHANNEL_11J_TX_POWER, *PCHANNEL_11J_TX_POWER;

typedef enum _ABGBAND_STATE_ {
	UNKNOWN_BAND,
	BG_BAND,
	A_BAND,
} ABGBAND_STATE;

typedef struct _MLME_STRUCT {
	STATE_MACHINE_FUNC      ActFunc[ACT_FUNC_SIZE];
	// Action 
	STATE_MACHINE           ActMachine;

#ifdef WSC_INCLUDED
	STATE_MACHINE			WscMachine;
	STATE_MACHINE_FUNC		WscFunc[WSC_FUNC_SIZE];
#endif // WSC_INCLUDED //

#ifdef QOS_DLS_SUPPORT
	STATE_MACHINE			DlsMachine;
	STATE_MACHINE_FUNC      DlsFunc[DLS_FUNC_SIZE];
#endif // QOS_DLS_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
	// AP state machines
	STATE_MACHINE           ApCntlMachine;
	STATE_MACHINE           ApAssocMachine;
	STATE_MACHINE           ApAuthMachine;
	STATE_MACHINE           ApAuthRspMachine;
	STATE_MACHINE           ApSyncMachine;
	STATE_MACHINE           ApWpaMachine;
	STATE_MACHINE_FUNC      ApAssocFunc[AP_ASSOC_FUNC_SIZE];
	STATE_MACHINE_FUNC		ApDlsFunc[DLS_FUNC_SIZE];
	STATE_MACHINE_FUNC      ApAuthFunc[AP_AUTH_FUNC_SIZE];
	STATE_MACHINE_FUNC      ApAuthRspFunc[AP_AUTH_RSP_FUNC_SIZE];
	STATE_MACHINE_FUNC      ApSyncFunc[AP_SYNC_FUNC_SIZE];
	STATE_MACHINE_FUNC      ApWpaFunc[AP_WPA_FUNC_SIZE];
#ifdef APCLI_SUPPORT
	STATE_MACHINE_EX       ApCliAuthMachine;
	STATE_MACHINE_EX       ApCliAssocMachine;
	STATE_MACHINE_EX       ApCliCtrlMachine;
	STATE_MACHINE_EX       ApCliSyncMachine;
	STATE_MACHINE_EX       ApCliWpaPskMachine;

	STATE_MACHINE_FUNC_EX  ApCliAuthFunc[APCLI_AUTH_FUNC_SIZE];
	STATE_MACHINE_FUNC_EX  ApCliAssocFunc[APCLI_ASSOC_FUNC_SIZE];
	STATE_MACHINE_FUNC_EX  ApCliCtrlFunc[APCLI_CTRL_FUNC_SIZE];
	STATE_MACHINE_FUNC_EX  ApCliSyncFunc[APCLI_SYNC_FUNC_SIZE];
//	STATE_MACHINE_FUNC_EX  ApCliWpaPskFunc[APCLI_WPA_PSK_FUNC_SIZE];
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	
	

	ULONG                   ChannelQuality;  // 0..100, Channel Quality Indication for Roaming
	ULONG                   Now32;           // latch the value of NdisGetSystemUpTime()
	ULONG                   LastSendNULLpsmTime;

	BOOLEAN                 bRunning;
	NDIS_SPIN_LOCK          TaskLock;
	MLME_QUEUE              Queue;

	UINT                    ShiftReg;

	RALINK_TIMER_STRUCT     PeriodicTimer;
	RALINK_TIMER_STRUCT     APSDPeriodicTimer;
	RALINK_TIMER_STRUCT     LinkDownTimer;
	RALINK_TIMER_STRUCT     LinkUpTimer;
	ULONG                   PeriodicRound;
	ULONG                   OneSecPeriodicRound;

	UCHAR					RealRxPath;
	BOOLEAN					bLowThroughput;
	BOOLEAN					bEnableAutoAntennaCheck;
	RALINK_TIMER_STRUCT		RxAntEvalTimer;

#ifdef RT30xx
	UCHAR CaliBW40RfR24;
	UCHAR CaliBW20RfR24;
#endif // RT30xx //
	
} MLME_STRUCT, *PMLME_STRUCT;

// structure for radar detection and channel switch
typedef struct _RADAR_DETECT_STRUCT {
    //BOOLEAN		IEEE80211H;			// 0: disable, 1: enable IEEE802.11h
	UCHAR		CSCount;			//Channel switch counter
	UCHAR		CSPeriod;			//Channel switch period (beacon count)
	UCHAR		RDCount;			//Radar detection counter
	UCHAR		RDMode;				//Radar Detection mode
	UCHAR		RDDurRegion;		//Radar detection duration region
	UCHAR		BBPR16;
	UCHAR		BBPR17;
	UCHAR		BBPR18;
	UCHAR		BBPR21;
	UCHAR		BBPR22;
	UCHAR		BBPR64;
	ULONG		InServiceMonitorCount; // unit: sec
	UINT8		DfsSessionTime;
	BOOLEAN		bFastDfs;
	UINT8		ChMovingTime;
	UINT8		LongPulseRadarTh;
} RADAR_DETECT_STRUCT, *PRADAR_DETECT_STRUCT;

#ifdef CARRIER_DETECTION_SUPPORT
typedef enum CD_STATE_n
{
	CD_NORMAL,
	CD_SILENCE,
	CD_MAX_STATE
} CD_STATE;

typedef struct CARRIER_DETECTION_s
{
	BOOLEAN					Enable;
	UINT8					CDSessionTime;
	UINT8					CDPeriod;
	CD_STATE				CD_State;
} CARRIER_DETECTION, *PCARRIER_DETECTION;
#endif // CARRIER_DETECTION_SUPPORT //

typedef enum _REC_BLOCKACK_STATUS
{	
    Recipient_NONE=0,
	Recipient_USED,
	Recipient_HandleRes,
    Recipient_Accept
} REC_BLOCKACK_STATUS, *PREC_BLOCKACK_STATUS;

typedef enum _ORI_BLOCKACK_STATUS
{
    Originator_NONE=0,
	Originator_USED,
    Originator_WaitRes,
    Originator_Done
} ORI_BLOCKACK_STATUS, *PORI_BLOCKACK_STATUS;

#ifdef DOT11_N_SUPPORT
typedef struct _BA_ORI_ENTRY{
	UCHAR   Wcid;
	UCHAR   TID;
	UCHAR   BAWinSize;
	UCHAR   Token;
// Sequence is to fill every outgoing QoS DATA frame's sequence field in 802.11 header.
	USHORT	Sequence;
	USHORT	TimeOutValue;
	ORI_BLOCKACK_STATUS  ORI_BA_Status;
	RALINK_TIMER_STRUCT ORIBATimer;
	PVOID	pAdapter;
} BA_ORI_ENTRY, *PBA_ORI_ENTRY;

typedef struct _BA_REC_ENTRY {
	UCHAR   Wcid;
	UCHAR   TID;
	UCHAR   BAWinSize;	// 7.3.1.14. each buffer is capable of holding a max AMSDU or MSDU.
	//UCHAR	NumOfRxPkt;
	//UCHAR    Curindidx; // the head in the RX reordering buffer
	USHORT		LastIndSeq;
//	USHORT		LastIndSeqAtTimer;
	USHORT		TimeOutValue;
	RALINK_TIMER_STRUCT RECBATimer;
	ULONG		LastIndSeqAtTimer;
	ULONG		nDropPacket;
	ULONG		rcvSeq;
	REC_BLOCKACK_STATUS  REC_BA_Status;
//	UCHAR	RxBufIdxUsed;
	// corresponding virtual address for RX reordering packet storage.
	//RTMP_REORDERDMABUF MAP_RXBuf[MAX_RX_REORDERBUF]; 
	NDIS_SPIN_LOCK          RxReRingLock;                 // Rx Ring spinlock
//	struct _BA_REC_ENTRY *pNext;
	PVOID	pAdapter;
	struct reordering_list	list;
} BA_REC_ENTRY, *PBA_REC_ENTRY;


typedef struct {
	ULONG		numAsRecipient;		// I am recipient of numAsRecipient clients. These client are in the BARecEntry[]
	ULONG		numAsOriginator;	// I am originator of 	numAsOriginator clients. These clients are in the BAOriEntry[]
	BA_ORI_ENTRY       BAOriEntry[MAX_LEN_OF_BA_ORI_TABLE];
	BA_REC_ENTRY       BARecEntry[MAX_LEN_OF_BA_REC_TABLE];
} BA_TABLE, *PBA_TABLE;

//For QureyBATableOID use;
typedef struct  PACKED _OID_BA_REC_ENTRY{
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   BaBitmap;   // if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize
	UCHAR   rsv; 
	UCHAR   BufSize[8];
	REC_BLOCKACK_STATUS	REC_BA_Status[8];
} OID_BA_REC_ENTRY, *POID_BA_REC_ENTRY;

//For QureyBATableOID use;
typedef struct  PACKED _OID_BA_ORI_ENTRY{
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   BaBitmap;  // if (BaBitmap&(1<<TID)), this session with{MACAddr, TID}exists, so read BufSize[TID] for BufferSize, read ORI_BA_Status[TID] for status
	UCHAR   rsv; 
	UCHAR   BufSize[8];
	ORI_BLOCKACK_STATUS  ORI_BA_Status[8];
} OID_BA_ORI_ENTRY, *POID_BA_ORI_ENTRY;

typedef struct _QUERYBA_TABLE{
	OID_BA_ORI_ENTRY       BAOriEntry[32];
	OID_BA_REC_ENTRY       BARecEntry[32];
	UCHAR   OriNum;// Number of below BAOriEntry
	UCHAR   RecNum;// Number of below BARecEntry
} QUERYBA_TABLE, *PQUERYBA_TABLE;

typedef	union	_BACAP_STRUC	{
#ifdef RT_BIG_ENDIAN
	struct	{
		UINT32     :4;
		UINT32     b2040CoexistScanSup:1;		//As Sta, support do 2040 coexistence scan for AP. As Ap, support monitor trigger event to check if can use BW 40MHz.
		UINT32     bHtAdhoc:1;			// adhoc can use ht rate.
		UINT32     MMPSmode:2;	// MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable
		UINT32     AmsduSize:1;	// 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]	= { 3839, 7935};
		UINT32     AmsduEnable:1;	//Enable AMSDU transmisstion
		UINT32		MpduDensity:3;	
		UINT32		Policy:2;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use
		UINT32		AutoBA:1;	// automatically BA  	
		UINT32		TxBAWinLimit:8;
		UINT32		RxBAWinLimit:8;
	}	field;
#else
	struct	{
		UINT32		RxBAWinLimit:8;
		UINT32		TxBAWinLimit:8;
		UINT32		AutoBA:1;	// automatically BA  	
		UINT32		Policy:2;	// 0: DELAY_BA 1:IMMED_BA  (//BA Policy subfiled value in ADDBA frame)   2:BA-not use
		UINT32		MpduDensity:3;	
		UINT32       	AmsduEnable:1;	//Enable AMSDU transmisstion
		UINT32       	AmsduSize:1;	// 0:3839, 1:7935 bytes. UINT  MSDUSizeToBytes[]	= { 3839, 7935};
		UINT32       	MMPSmode:2;	// MIMO power save more, 0:static, 1:dynamic, 2:rsv, 3:mimo enable
		UINT32       	bHtAdhoc:1;			// adhoc can use ht rate.
		UINT32       	b2040CoexistScanSup:1;		//As Sta, support do 2040 coexistence scan for AP. As Ap, support monitor trigger event to check if can use BW 40MHz.
		UINT32       	:4;	
	}	field;
#endif
	UINT32			word;
} BACAP_STRUC, *PBACAP_STRUC;
#endif // DOT11_N_SUPPORT //

//This structure is for all 802.11n card InterOptibilityTest action. Reset all Num every n second.  (Details see MLMEPeriodic)
typedef	struct	_IOT_STRUC	{
	UCHAR			Threshold[2];
	UCHAR			ReorderTimeOutNum[MAX_LEN_OF_BA_REC_TABLE];	// compare with threshold[0]
	UCHAR			RefreshNum[MAX_LEN_OF_BA_REC_TABLE];	// compare with threshold[1]
	ULONG			OneSecInWindowCount;
	ULONG			OneSecFrameDuplicateCount;
	ULONG			OneSecOutWindowCount;
	UCHAR			DelOriAct;
	UCHAR			DelRecAct;
	UCHAR			RTSShortProt;
	UCHAR			RTSLongProt;
	BOOLEAN			bRTSLongProtOn;
} IOT_STRUC, *PIOT_STRUC;

// This is the registry setting for 802.11n transmit setting.  Used in advanced page.
typedef union _REG_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
 struct {
         UINT32  rsv:13;
		 UINT32  EXTCHA:2;
		 UINT32  HTMODE:1;
		 UINT32  TRANSNO:2; 
		 UINT32  STBC:1; //SPACE 
		 UINT32  ShortGI:1;
		 UINT32  BW:1; //channel bandwidth 20MHz or 40 MHz
		 UINT32  TxBF:1; // 3*3
		 UINT32  rsv0:10;
		 //UINT32  MCS:7;                 // MCS
         //UINT32  PhyMode:4;             
    } field;
#else
 struct {
         //UINT32  PhyMode:4;                
         //UINT32  MCS:7;                 // MCS
		 UINT32  rsv0:10;
		 UINT32  TxBF:1;
         UINT32  BW:1; //channel bandwidth 20MHz or 40 MHz
         UINT32  ShortGI:1;
         UINT32  STBC:1; //SPACE 
         UINT32  TRANSNO:2; 
         UINT32  HTMODE:1;
         UINT32  EXTCHA:2;
         UINT32  rsv:13;
    } field;
#endif
 UINT32   word;
} REG_TRANSMIT_SETTING, *PREG_TRANSMIT_SETTING;

typedef union  _DESIRED_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
	struct	{
			USHORT		rsv:3;	 
			USHORT		FixedTxMode:2;			// If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode.
			USHORT		PhyMode:4;	
			USHORT   	MCS:7;                 // MCS
	}	field;
#else
	struct	{
			USHORT   	MCS:7;                 	// MCS
			USHORT		PhyMode:4;				
			USHORT	 	FixedTxMode:2;			// If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode.
			USHORT		rsv:3;	 	 
	}	field;
#endif
	USHORT		word;
 } DESIRED_TRANSMIT_SETTING, *PDESIRED_TRANSMIT_SETTING;

typedef struct {
	BOOLEAN		IsRecipient;
	UCHAR   MACAddr[MAC_ADDR_LEN];
	UCHAR   TID;
	UCHAR   nMSDU;
	USHORT   TimeOut;
	BOOLEAN bAllTid;  // If True, delete all TID for BA sessions with this MACaddr.
} OID_ADD_BA_ENTRY, *POID_ADD_BA_ENTRY;

//
// Multiple SSID structure
//
#define WLAN_MAX_NUM_OF_TIM			((MAX_LEN_OF_MAC_TABLE >> 3) + 1) /* /8 + 1 */
#define WLAN_CT_TIM_BCMC_OFFSET		0 /* unit: 32B */

/* clear bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_CLEAR(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] &= ~BIT8[0];

/* set bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_SET(apidx) \
	pAd->ApCfg.MBSSID[apidx].TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] |= BIT8[0];

/* clear a station PS TIM bit */
#define WLAN_MR_TIM_BIT_CLEAR(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] &= (~BIT8[bit_offset]); }

/* set a station PS TIM bit */
#define WLAN_MR_TIM_BIT_SET(ad_p, apidx, wcid) \
	{	UCHAR tim_offset = wcid >> 3; \
		UCHAR bit_offset = wcid & 0x7; \
		ad_p->ApCfg.MBSSID[apidx].TimBitmaps[tim_offset] |= BIT8[bit_offset]; }

#ifdef RT2870
#define BEACON_BITMAP_MASK		0xff
typedef struct _BEACON_SYNC_STRUCT_
{
	UCHAR        			BeaconBuf[HW_BEACON_MAX_COUNT][HW_BEACON_OFFSET];
	UCHAR					BeaconTxWI[HW_BEACON_MAX_COUNT][TXWI_SIZE];
	ULONG 					TimIELocationInBeacon[HW_BEACON_MAX_COUNT];
	ULONG					CapabilityInfoLocationInBeacon[HW_BEACON_MAX_COUNT];
	BOOLEAN					EnableBeacon;		// trigger to enable beacon transmission.
	UCHAR					BeaconBitMap;		// NOTE: If the MAX_MBSSID_NUM is larger than 8, this parameter need to change.
	UCHAR					DtimBitOn;			// NOTE: If the MAX_MBSSID_NUM is larger than 8, this parameter need to change.
}BEACON_SYNC_STRUCT;
#endif // RT2870 //

typedef struct _MULTISSID_STRUCT {
	UCHAR								Bssid[MAC_ADDR_LEN];
    UCHAR                               SsidLen;
    CHAR                                Ssid[MAX_LEN_OF_SSID];
    USHORT                              CapabilityInfo;
    
    PNET_DEV                   			MSSIDDev;

	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;
	NDIS_802_11_WEP_STATUS              WepStatus;
	NDIS_802_11_WEP_STATUS				GroupKeyWepStatus;
	WPA_MIX_PAIR_CIPHER					WpaMixPairCipher;

	ULONG								TxCount;
	ULONG								RxCount;
	ULONG								ReceivedByteCount;
	ULONG								TransmittedByteCount;
	ULONG								RxErrorCount;
	ULONG								RxDropCount;

	HTTRANSMIT_SETTING					HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.	
	RT_HT_PHY_INFO						DesiredHtPhyInfo;
	DESIRED_TRANSMIT_SETTING        	DesiredTransmitSetting; // Desired transmit setting. this is for reading registry setting only. not useful.
	BOOLEAN								bAutoTxRateSwitch;
	
	//CIPHER_KEY                          SharedKey[SHARE_KEY_NUM]; // ref pAd->SharedKey[BSS][4]
	UCHAR                               DefaultKeyId;

	UCHAR								TxRate;       // RATE_1, RATE_2, RATE_5_5, RATE_11, ...
	UCHAR     							DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];// OID_802_11_DESIRED_RATES
	UCHAR								DesiredRatesIndex;
	UCHAR     							MaxTxRate;            // RATE_1, RATE_2, RATE_5_5, RATE_11

//	ULONG           					TimBitmap;      // bit0 for broadcast, 1 for AID1, 2 for AID2, ...so on
//    ULONG           					TimBitmap2;     // b0 for AID32, b1 for AID33, ... and so on
	UCHAR								TimBitmaps[WLAN_MAX_NUM_OF_TIM];

    // WPA
    UCHAR                               GMK[32];
    UCHAR                               PMK[32];
	UCHAR								GTK[32];
    BOOLEAN                             IEEE8021X;
    BOOLEAN                             PreAuth;
    UCHAR                               GNonce[32];
    UCHAR                               PortSecured;
    NDIS_802_11_PRIVACY_FILTER          PrivacyFilter;
    UCHAR                               BANClass3Data;
    ULONG                               IsolateInterStaTraffic;

    UCHAR                               RSNIE_Len[2];
    UCHAR                               RSN_IE[2][MAX_LEN_OF_RSNIE];


    UCHAR                   			TimIELocationInBeacon;
    UCHAR                   			CapabilityInfoLocationInBeacon;
#ifdef CONFIG_AP_SUPPORT
    NDIS_AP_802_11_PMKID		PMKIDCache;
#endif // CONFIG_AP_SUPPORT //
    // outgoing BEACON frame buffer and corresponding TXWI
	// PTXWI_STRUC                           BeaconTxWI; // 
    CHAR                                BeaconBuf[MAX_BEACON_SIZE]; // NOTE: BeaconBuf should be 4-byte aligned

    BOOLEAN                             bHideSsid;
	UINT16								StationKeepAliveTime; // unit: second

    USHORT                              VLAN_VID;
    USHORT                              VLAN_Priority;

    RT_802_11_ACL						AccessControlList;

	// EDCA Qos
    BOOLEAN								bWmmCapable;	// 0:disable WMM, 1:enable WMM
    BOOLEAN								bDLSCapable;	// 0:disable DLS, 1:enable DLS

	UCHAR           					DlsPTK[64];		// Due to windows dirver count on meetinghouse to handle 4-way shake
	
	// For 802.1x daemon setting per BSS
	UCHAR								radius_srv_num;
	RADIUS_SRV_INFO						radius_srv_info[MAX_RADIUS_SRV_NUM];
	
#ifdef RTL865X_SOC
	unsigned int						mylinkid;
#endif 

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
	WSC_LV_INFO            WscIEBeacon;
   	WSC_LV_INFO            WscIEProbeResp;
	WSC_CTRL	           WscControl;
#endif // WSC_AP_SUPPORT //

#ifdef IGMP_SNOOP_SUPPORT
	BOOLEAN								IgmpSnoopEnable;	// 0: disable, 1: enable.
#endif // IGMP_SNOOP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	UINT32					RcvdConflictSsidCount;					
	UINT32					RcvdSpoofedAssocRespCount;	
	UINT32					RcvdSpoofedReassocRespCount;	
	UINT32					RcvdSpoofedProbeRespCount;
	UINT32					RcvdSpoofedBeaconCount;
	UINT32					RcvdSpoofedDisassocCount;	
	UINT32					RcvdSpoofedAuthCount;
	UINT32					RcvdSpoofedDeauthCount;	
	UINT32					RcvdSpoofedUnknownMgmtCount;	
	UINT32					RcvdReplayAttackCount;

	CHAR					RssiOfRcvdConflictSsid;					
	CHAR					RssiOfRcvdSpoofedAssocResp;	
	CHAR					RssiOfRcvdSpoofedReassocResp;	
	CHAR					RssiOfRcvdSpoofedProbeResp;
	CHAR					RssiOfRcvdSpoofedBeacon;
	CHAR					RssiOfRcvdSpoofedDisassoc;	
	CHAR					RssiOfRcvdSpoofedAuth;
	CHAR					RssiOfRcvdSpoofedDeauth;	
	CHAR					RssiOfRcvdSpoofedUnknownMgmt;	
	CHAR					RssiOfRcvdReplayAttack;		
	
	BOOLEAN					bBcnSntReq;
	UCHAR					BcnBufIdx;
} MULTISSID_STRUCT, *PMULTISSID_STRUCT;



#ifdef DOT11N_DRAFT3
typedef enum _BSS2040COEXIST_FLAG{
	BSS_2040_COEXIST_DISABLE = 0,
	BSS_2040_COEXIST_TIMER_FIRED  = 1,
	BSS_2040_COEXIST_INFO_SYNC = 2,
	BSS_2040_COEXIST_INFO_NOTIFY = 4,
}BSS2040COEXIST_FLAG;
#endif // DOT11N_DRAFT3 //

// configuration common to OPMODE_AP as well as OPMODE_STA
typedef struct _COMMON_CONFIG {

	BOOLEAN		bCountryFlag;
	UCHAR		CountryCode[3];
	UCHAR		Geography;
	UCHAR       CountryRegion;      // Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel
	UCHAR       CountryRegionForABand;	// Enum of country region for A band
	UCHAR       PhyMode;            // PHY_11A, PHY_11B, PHY_11BG_MIXED, PHY_ABG_MIXED
	USHORT      Dsifs;              // in units of usec
	ULONG       PacketFilter;       // Packet filter for receiving

	CHAR        Ssid[MAX_LEN_OF_SSID]; // NOT NULL-terminated
	UCHAR       SsidLen;               // the actual ssid length in used
	UCHAR       LastSsidLen;               // the actual ssid length in used
	CHAR        LastSsid[MAX_LEN_OF_SSID]; // NOT NULL-terminated
	UCHAR		LastBssid[MAC_ADDR_LEN];

	UCHAR       Bssid[MAC_ADDR_LEN];
	USHORT      BeaconPeriod;       
	UCHAR       Channel;            
	UCHAR       CentralChannel;    	// Central Channel when using 40MHz is indicating. not real channel.        

#if 0	// move to STA_ADMIN_CONFIG
	UCHAR       DefaultKeyId;

	NDIS_802_11_PRIVACY_FILTER          PrivacyFilter;  // PrivacyFilter enum for 802.1X
	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;       // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;
	NDIS_802_11_WEP_STATUS				OrigWepStatus;	// Original wep status set from OID

	// Add to support different cipher suite for WPA2/WPA mode
	NDIS_802_11_ENCRYPTION_STATUS		GroupCipher;		// Multicast cipher suite
	NDIS_802_11_ENCRYPTION_STATUS		PairCipher;			// Unicast cipher suite
	BOOLEAN								bMixCipher;			// Indicate current Pair & Group use different cipher suites
	USHORT								RsnCapability;
	
	NDIS_802_11_WEP_STATUS              GroupKeyWepStatus;
#endif

	UCHAR       SupRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       SupRateLen;
	UCHAR       ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
	UCHAR       ExtRateLen;
	UCHAR       DesireRate[MAX_LEN_OF_SUPPORTED_RATES];      // OID_802_11_DESIRED_RATES
	UCHAR       MaxDesiredRate;
	UCHAR       ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];

	ULONG       BasicRateBitmap;        // backup basic ratebitmap

	BOOLEAN		bAPSDCapable;
	BOOLEAN		bInServicePeriod;
	BOOLEAN		bAPSDAC_BE;
	BOOLEAN		bAPSDAC_BK;
	BOOLEAN		bAPSDAC_VI;
	BOOLEAN		bAPSDAC_VO;
	BOOLEAN		bNeedSendTriggerFrame;
	BOOLEAN		bAPSDForcePowerSave;	// Force power save mode, should only use in APSD-STAUT
	ULONG		TriggerTimerCount;
	UCHAR		MaxSPLength;
	UCHAR		BBPCurrentBW;	// BW_10, 	BW_20, BW_40
	// move to MULTISSID_STRUCT for MBSS 
	//HTTRANSMIT_SETTING	HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.	
	REG_TRANSMIT_SETTING        RegTransmitSetting; //registry transmit setting. this is for reading registry setting only. not useful.
	//UCHAR       FixedTxMode;              // Fixed Tx Mode (CCK, OFDM), for HT fixed tx mode (GF, MIX) , refer to RegTransmitSetting.field.HTMode
	UCHAR       TxRate;                 // Same value to fill in TXD. TxRate is 6-bit 
	UCHAR       MaxTxRate;              // RATE_1, RATE_2, RATE_5_5, RATE_11
	UCHAR       TxRateIndex;            // Tx rate index in RateSwitchTable
	UCHAR       TxRateTableSize;        // Valid Tx rate table size in RateSwitchTable
	//BOOLEAN		bAutoTxRateSwitch;
	UCHAR       MinTxRate;              // RATE_1, RATE_2, RATE_5_5, RATE_11
	UCHAR       RtsRate;                // RATE_xxx
	HTTRANSMIT_SETTING	MlmeTransmit;   // MGMT frame PHY rate setting when operatin at Ht rate.
	UCHAR       MlmeRate;               // RATE_xxx, used to send MLME frames
	UCHAR       BasicMlmeRate;          // Default Rate for sending MLME frames

	USHORT      RtsThreshold;           // in unit of BYTE
	USHORT      FragmentThreshold;      // in unit of BYTE

	UCHAR       TxPower;                // in unit of mW
	ULONG       TxPowerPercentage;      // 0~100 %
	ULONG       TxPowerDefault;         // keep for TxPowerPercentage

#ifdef DOT11_N_SUPPORT
	BACAP_STRUC        BACapability; //   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0
	BACAP_STRUC        REGBACapability; //   NO USE = 0XFF  ;  IMMED_BA =1  ;  DELAY_BA=0
#endif // DOT11_N_SUPPORT //
	IOT_STRUC		IOTestParm;	// 802.11n InterOpbility Test Parameter;
	ULONG       TxPreamble;             // Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto
	BOOLEAN     bUseZeroToDisableFragment;     // Microsoft use 0 as disable 
	ULONG       UseBGProtection;        // 0: auto, 1: always use, 2: always not use
	BOOLEAN     bUseShortSlotTime;      // 0: disable, 1 - use short slot (9us)
	BOOLEAN     bEnableTxBurst;         // 1: enble TX PACKET BURST, 0: disable TX PACKET BURST
	BOOLEAN     bAggregationCapable;      // 1: enable TX aggregation when the peer supports it
	BOOLEAN     bPiggyBackCapable;		// 1: enable TX piggy-back according MAC's version
	BOOLEAN     bIEEE80211H;			// 1: enable IEEE802.11h spec.
	ULONG		DisableOLBCDetect;		// 0: enable OLBC detect; 1 disable OLBC detect 

#ifdef DOT11_N_SUPPORT
	BOOLEAN				bRdg;
#endif // DOT11_N_SUPPORT //
	BOOLEAN             bWmmCapable;        // 0:disable WMM, 1:enable WMM
	QOS_CAPABILITY_PARM APQosCapability;    // QOS capability of the current associated AP
	EDCA_PARM           APEdcaParm;         // EDCA parameters of the current associated AP
	QBSS_LOAD_PARM      APQbssLoad;         // QBSS load of the current associated AP
	UCHAR               AckPolicy[4];       // ACK policy of the specified AC. see ACK_xxx
	// a bitmap of BOOLEAN flags. each bit represent an operation status of a particular 
	// BOOLEAN control, either ON or OFF. These flags should always be accessed via
	// OPSTATUS_TEST_FLAG(), OPSTATUS_SET_FLAG(), OP_STATUS_CLEAR_FLAG() macros.
	// see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition
	ULONG               OpStatusFlags;

	BOOLEAN				NdisRadioStateOff; //For HCT 12.0, set this flag to TRUE instead of called MlmeRadioOff.       
	ABGBAND_STATE		BandState;		// For setting BBP used on B/G or A mode.
	BOOLEAN				bRxAntDiversity; // 0:disable, 1:enable Software Rx Antenna Diversity.

	// IEEE802.11H--DFS.
	RADAR_DETECT_STRUCT	RadarDetect;

#ifdef CARRIER_DETECTION_SUPPORT
	CARRIER_DETECTION		CarrierDetect;
#endif // CARRIER_DETECTION_SUPPORT //

#ifdef DOT11_N_SUPPORT
	// HT
	UCHAR			BASize;		// USer desired BAWindowSize. Should not exceed our max capability
	//RT_HT_CAPABILITY	SupportedHtPhy;
	RT_HT_CAPABILITY	DesiredHtPhy;
	HT_CAPABILITY_IE		HtCapability;	
	ADD_HT_INFO_IE		AddHTInfo;	// Useful as AP.
	//This IE is used with channel switch announcement element when changing to a new 40MHz.
	//This IE is included in channel switch ammouncement frames 7.4.1.5, beacons, probe Rsp.
	NEW_EXT_CHAN_IE	NewExtChanOffset;	//7.3.2.20A, 1 if extension channel is above the control channel, 3 if below, 0 if not present

#ifdef DOT11N_DRAFT3
	UCHAR					Bss2040CoexistFlag;		// bit 0: bBssCoexistTimerRunning, bit 1: NeedSyncAddHtInfo.
	RALINK_TIMER_STRUCT	Bss2040CoexistTimer;

	//This IE is used for 20/40 BSS Coexistence.
	BSS_2040_COEXIST_IE		BSS2040CoexistInfo;
	// ====== 11n D3.0 =======================>
	USHORT					Dot11OBssScanPassiveDwell;				// Unit : TU. 5~1000
	USHORT					Dot11OBssScanActiveDwell;				// Unit : TU. 10~1000
	USHORT					Dot11BssWidthTriggerScanInt;			// Unit : Second
	USHORT					Dot11OBssScanPassiveTotalPerChannel;	// Unit : TU. 200~10000
	USHORT					Dot11OBssScanActiveTotalPerChannel;	// Unit : TU. 20~10000
	USHORT					Dot11BssWidthChanTranDelayFactor;
	USHORT					Dot11OBssScanActivityThre;				// Unit : percentage
	
	ULONG					Dot11BssWidthChanTranDelay;			// multiple of (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor)
	ULONG					CountDownCtr;	// CountDown Counter from (Dot11BssWidthTriggerScanInt * Dot11BssWidthChanTranDelayFactor)
	
	NDIS_SPIN_LOCK          TriggerEventTabLock;
	BSS_2040_COEXIST_IE		LastBSSCoexist2040;
	BSS_2040_COEXIST_IE		BSSCoexist2040;
	TRIGGER_EVENT_TAB		TriggerEventTab;
	UCHAR					ChannelListIdx;
	// <====== 11n D3.0 =======================
	BOOLEAN					bOverlapScanning;
#endif // DOT11N_DRAFT3 //

    BOOLEAN                 bHTProtect;
    BOOLEAN                 bMIMOPSEnable;
    BOOLEAN					bBADecline;
	BOOLEAN					bDisableReordering;
	BOOLEAN					bForty_Mhz_Intolerant;
	BOOLEAN					bExtChannelSwitchAnnouncement;
	BOOLEAN					bRcvBSSWidthTriggerEvents;
	ULONG					LastRcvBSSWidthTriggerEventsTime;

	UCHAR					TxBASize;	
#endif // DOT11_N_SUPPORT //

	// Enable wireless event
	BOOLEAN				bWirelessEvent;
	BOOLEAN				bWiFiTest;				// Enable this parameter for WiFi test		

	// Tx & Rx Stream number selection
	UCHAR				TxStream;
	UCHAR				RxStream;

	// transmit phy mode, trasmit rate for Multicast.
#ifdef MCAST_RATE_SPECIFIC
	UCHAR				McastTransmitMcs;
	UCHAR				McastTransmitPhyMode;
#endif // MCAST_RATE_SPECIFIC //

	BOOLEAN     		bHardwareRadio;     // Hardware controlled Radio enabled

#ifdef RT2870
	BOOLEAN     		bMultipleIRP;       // Multiple Bulk IN flag
	UCHAR       		NumOfBulkInIRP;     // if bMultipleIRP == TRUE, NumOfBulkInIRP will be 4 otherwise be 1
 	RT_HT_CAPABILITY	SupportedHtPhy;
	ULONG				MaxPktOneTxBulk;
	UCHAR				TxBulkFactor;
	UCHAR				RxBulkFactor;

	BEACON_SYNC_STRUCT	*pBeaconSync;
	RALINK_TIMER_STRUCT	BeaconUpdateTimer;
	UINT32				BeaconAdjust;
	UINT32				BeaconFactor;
	UINT32				BeaconRemain;
#endif // RT2870 //

#ifdef WSC_INCLUDED
	/* WSC hardware push button function 0811 */
	UINT8					WscHdrPshBtnCheckCount;
#endif // WSC_INCLUDED //

 	NDIS_SPIN_LOCK			MeasureReqTabLock;
	PMEASURE_REQ_TAB		pMeasureReqTab;

	NDIS_SPIN_LOCK			TpcReqTabLock;
	PTPC_REQ_TAB			pTpcReqTab;

	// transmit phy mode, trasmit rate for Multicast.
#ifdef MCAST_RATE_SPECIFIC
	HTTRANSMIT_SETTING		MCastPhyMode;
#endif // MCAST_RATE_SPECIFIC //

#ifdef SINGLE_SKU
	UINT16					DefineMaxTxPwr;
#endif // SINGLE_SKU //


} COMMON_CONFIG, *PCOMMON_CONFIG;



#ifdef RT2870
typedef struct   RT_ADD_PAIRWISE_KEY_ENTRY {
        NDIS_802_11_MAC_ADDRESS         MacAddr;
        USHORT                          MacTabMatchWCID;        // ASIC
        CIPHER_KEY      CipherKey;
} RT_ADD_PAIRWISE_KEY_ENTRY,*PRT_ADD_PAIRWISE_KEY_ENTRY;
#endif // RT2870 //

// ----------- start of AP --------------------------
// AUTH-RSP State Machine Aux data structure
typedef struct _AP_MLME_AUX {
	UCHAR               Addr[MAC_ADDR_LEN];
	USHORT              Alg;
	CHAR                Challenge[CIPHER_TEXT_LEN];
} AP_MLME_AUX, *PAP_MLME_AUX;

// structure to define WPA Group Key Rekey Interval
typedef struct PACKED _RT_802_11_WPA_REKEY {
	ULONG ReKeyMethod;          // mechanism for rekeying: 0:disable, 1: time-based, 2: packet-based
	ULONG ReKeyInterval;        // time-based: seconds, packet-based: kilo-packets
} RT_WPA_REKEY,*PRT_WPA_REKEY, RT_802_11_WPA_REKEY, *PRT_802_11_WPA_REKEY;

typedef struct _MAC_TABLE_ENTRY {
	//Choose 1 from ValidAsWDS and ValidAsCLI  to validize.
	BOOLEAN		ValidAsCLI;		// Sta mode, set this TRUE after Linkup,too.
	BOOLEAN		ValidAsWDS;	// This is WDS Entry. only for AP mode.
	BOOLEAN		ValidAsApCli;   //This is a AP-Client entry, only for AP mode which enable AP-Client functions.
	BOOLEAN		ValidAsMesh;
	BOOLEAN		ValidAsDls;	// This is DLS Entry. only for STA mode.
	BOOLEAN		isCached;
	BOOLEAN		bIAmBadAtheros;	// Flag if this is Atheros chip that has IOT problem.  We need to turn on RTS/CTS protection.

	UCHAR         	EnqueueEapolStartTimerRunning;  // Enqueue EAPoL-Start for triggering EAP SM
	//jan for wpa
	// record which entry revoke MIC Failure , if it leaves the BSS itself, AP won't update aMICFailTime MIB
	UCHAR           CMTimerRunning;
	UCHAR           apidx;			// MBSS number 
	UCHAR           RSNIE_Len;
	UCHAR           RSN_IE[MAX_LEN_OF_RSNIE];
	UCHAR           ANonce[LEN_KEY_DESC_NONCE];
	UCHAR           R_Counter[LEN_KEY_DESC_REPLAY];
	UCHAR           PTK[64];
	UCHAR           ReTryCounter;   
	RALINK_TIMER_STRUCT                 RetryTimer;
	RALINK_TIMER_STRUCT					EnqueueStartForPSKTimer;	// A timer which enqueue EAPoL-Start for triggering PSK SM
	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;   // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;
	AP_WPA_STATE    WpaState;
	GTK_STATE       GTKState;
	USHORT          PortSecured;
	NDIS_802_11_PRIVACY_FILTER  PrivacyFilter;      // PrivacyFilter enum for 802.1X
	CIPHER_KEY      PairwiseKey;
	PVOID           pAd;
    INT				PMKID_CacheIdx;
    UCHAR			PMKID[LEN_PMKID];


	UCHAR           Addr[MAC_ADDR_LEN];
	UCHAR           PsMode;
	SST             Sst;
	AUTH_STATE      AuthState; // for SHARED KEY authentication state machine used only
	BOOLEAN			IsReassocSta;	// Indicate whether this is a reassociation procedure
	USHORT          Aid;
	USHORT          CapabilityInfo;
	UCHAR           LastRssi;
	ULONG           NoDataIdleCount;
	UINT16			StationKeepAliveCount; // unit: second
	ULONG           PsQIdleCount;
	QUEUE_HEADER    PsQueue;

	UINT32			StaConnectTime;		// the live time of this station since associated with AP 

#ifdef CONFIG_AP_SUPPORT
#ifdef UAPSD_AP_SUPPORT
    /* 0:AC_BK, 1:AC_BE, 2:AC_VI, 3:AC_VO */
    BOOLEAN			bAPSDCapablePerAC[4];
    UCHAR			MaxSPLength;

    BOOLEAN         bAPSDAllAC; /* 1: all AC are U-APSD */

    QUEUE_HEADER    UAPSDQueue[WMM_NUM_OF_AC]; /* queue for each U-APSD */
    USHORT          UAPSDQIdleCount;           /* U-APSD queue timeout */

    PQUEUE_ENTRY    pUAPSDEOSPFrame;    /* the last U-APSD frame */
    USHORT          UAPSDTxNum;         /* total U-APSD frame number */
    BOOLEAN         bAPSDFlagEOSPOK;    /* 1: EOSP is tx by ASIC */
    BOOLEAN         bAPSDFlagSPStart;   /* 1: SP is started */

	/* need to use unsigned long, because time parameters in OS is defined as
		unsigned long */
	unsigned long	UAPSDTimeStampLast;	/* unit: 1000000/OS_HZ */
	BOOLEAN			bAPSDFlagSpRoughUse;/* 1: use rough SP (default: accurate) */

	/* we will set the flag when PS-poll frame is received and
		clear it when statistics handle.
		if the flag is set when PS-poll frame is received then calling
		statistics handler to clear it. */
	BOOLEAN			bAPSDFlagLegacySent;/* 1: Legacy PS sent but
											yet statistics handle */

#ifdef RT2870
	UINT32			UAPSDTagOffset[WMM_NUM_OF_AC];
#endif // RT2870 //
#endif // UAPSD_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

#ifdef DOT11_N_SUPPORT
	BOOLEAN			bSendBAR;
	USHORT			NoBADataCountDown;

	UINT32   		CachedBuf[16];		// UINT (4 bytes) for alignment
	UINT			TxBFCount; // 3*3
#endif // DOT11_N_SUPPORT //
	UINT			FIFOCount;
	UINT			DebugFIFOCount;
	UINT			DebugTxCount;
    BOOLEAN			bDlsInit;


//====================================================
//WDS entry needs these
// rt2860 add this. if ValidAsWDS==TRUE, MatchWDSTabIdx is the index in WdsTab.MacTab
	UINT			MatchWDSTabIdx;
	UCHAR           MaxSupportedRate;
	UCHAR           CurrTxRate;
	UCHAR           CurrTxRateIndex;
	// to record the each TX rate's quality. 0 is best, the bigger the worse.
	USHORT          TxQuality[MAX_STEP_OF_TX_RATE_SWITCH];
//	USHORT          OneSecTxOkCount;
	UINT32			OneSecTxNoRetryOkCount;
	UINT32          OneSecTxRetryOkCount;
	UINT32          OneSecTxFailCount;
	UINT32			ContinueTxFailCnt;
	UINT32          CurrTxRateStableTime; // # of second in current TX rate
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
//====================================================

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	UINT			MatchAPCLITabIdx; // indicate the index in ApCfg.ApCliTab. 
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //



	BOOLEAN         fNoisyEnvironment;
	BOOLEAN			fLastSecAccordingRSSI;
	UCHAR           LastSecTxRateChangeAction; // 0: no change, 1:rate UP, 2:rate down
	CHAR			LastTimeTxRateChangeAction; //Keep last time value of LastSecTxRateChangeAction	
	ULONG			LastTxOkCount;
	UCHAR           PER[MAX_STEP_OF_TX_RATE_SWITCH];

	// a bitmap of BOOLEAN flags. each bit represent an operation status of a particular 
	// BOOLEAN control, either ON or OFF. These flags should always be accessed via
	// CLIENT_STATUS_TEST_FLAG(), CLIENT_STATUS_SET_FLAG(), CLIENT_STATUS_CLEAR_FLAG() macros.
	// see fOP_STATUS_xxx in RTMP_DEF.C for detail bit definition. fCLIENT_STATUS_AMSDU_INUSED
	ULONG           ClientStatusFlags;

	// TODO: Shall we move that to DOT11_N_SUPPORT???
	HTTRANSMIT_SETTING	HTPhyMode, MaxHTPhyMode, MinHTPhyMode;// For transmit phy setting in TXWI.
	
#ifdef DOT11_N_SUPPORT
	// HT EWC MIMO-N used parameters
	USHORT		RXBAbitmap;	// fill to on-chip  RXWI_BA_BITMASK in 8.1.3RX attribute entry format
	USHORT		TXBAbitmap;	// This bitmap as originator, only keep in software used to mark AMPDU bit in TXWI 
	USHORT		TXAutoBAbitmap;
	USHORT		BADeclineBitmap;
	USHORT		BARecWcidArray[NUM_OF_TID];	// The mapping wcid of recipient session. if RXBAbitmap bit is masked
	USHORT		BAOriWcidArray[NUM_OF_TID]; // The mapping wcid of originator session. if TXBAbitmap bit is masked
	USHORT		BAOriSequence[NUM_OF_TID]; // The mapping wcid of originator session. if TXBAbitmap bit is masked

	// 802.11n features.
	UCHAR		MpduDensity;
	UCHAR		MaxRAmpduFactor;
	UCHAR		AMsduSize;
	UCHAR		MmpsMode;	// MIMO power save more. 

	HT_CAPABILITY_IE		HTCapability;

#ifdef DOT11N_DRAFT3
	UCHAR		BSS2040CoexistenceMgmtSupport;
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //

	BOOLEAN		bAutoTxRateSwitch;

	UCHAR       RateLen;
	struct _MAC_TABLE_ENTRY *pNext;
    USHORT      TxSeq[NUM_OF_TID];
	USHORT		NonQosDataSeq;

	RSSI_SAMPLE	RssiSample;

#ifdef CONFIG_AP_SUPPORT
#ifdef WSC_AP_SUPPORT
    BOOLEAN                 bWscCapable;
    BOOLEAN                 EnqueueEapolStartTimerForWscRunning; 
    RALINK_TIMER_STRUCT     EnqueueEapolStartTimerForWsc;
    UCHAR                   Receive_EapolStart_EapRspId;
#endif // WSC_AP_SUPPORT //
#endif // CONFIG_AP_SUPPORT //
	UINT32			TXMCSExpected[16];
	UINT32			TXMCSSuccessful[16];
	UINT32			TXMCSFailed[16];
	UINT32			TXMCSAutoFallBack[16][16];

} MAC_TABLE_ENTRY, *PMAC_TABLE_ENTRY;

typedef struct _MAC_TABLE {
	USHORT			Size;
	MAC_TABLE_ENTRY *Hash[HASH_TABLE_SIZE];
	MAC_TABLE_ENTRY Content[MAX_LEN_OF_MAC_TABLE];
	QUEUE_HEADER    McastPsQueue;
	ULONG           PsQIdleCount;
	BOOLEAN         fAnyStationInPsm;   
	BOOLEAN         fAnyStationBadAtheros;	// Check if any Station is atheros 802.11n Chip.  We need to use RTS/CTS with Atheros 802,.11n chip. 
	BOOLEAN			fAnyTxOPForceDisable;	// Check if it is necessary to disable BE TxOP
	BOOLEAN			fAllStationAsRalink; 	// Check if all stations are ralink-chipset 
#ifdef DOT11_N_SUPPORT	
	BOOLEAN         fAnyStationIsLegacy;	// Check if I use legacy rate to transmit to my BSS Station/
	BOOLEAN         fAnyStationNonGF;		// Check if any Station can't support GF.
	BOOLEAN         fAnyStation20Only;		// Check if any Station can't support GF.
	BOOLEAN			fAnyStationMIMOPSDynamic; // Check if any Station is MIMO Dynamic
	BOOLEAN         fAnyBASession;   // Check if there is BA session.  Force turn on RTS/CTS 
#endif // DOT11_N_SUPPORT //
} MAC_TABLE, *PMAC_TABLE;

#ifdef DOT11_N_SUPPORT
#define IS_HT_STA(_pMacEntry)	\
	(_pMacEntry->MaxHTPhyMode.field.MODE >= MODE_HTMIX)

#define IS_HT_RATE(_pMacEntry)	\
	(_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX)

#define PEER_IS_HT_RATE(_pMacEntry)	\
	(_pMacEntry->HTPhyMode.field.MODE >= MODE_HTMIX)
#endif // DOT11_N_SUPPORT //

typedef struct _WDS_ENTRY {
	BOOLEAN         Valid;
	UCHAR           Addr[MAC_ADDR_LEN];
	ULONG           NoDataIdleCount;
	struct _WDS_ENTRY *pNext;
} WDS_ENTRY, *PWDS_ENTRY;

typedef struct  _WDS_TABLE_ENTRY {
	USHORT			Size;
	UCHAR           WdsAddr[MAC_ADDR_LEN];
	WDS_ENTRY       *Hash[HASH_TABLE_SIZE];
	WDS_ENTRY       Content[MAX_LEN_OF_MAC_TABLE];
	UCHAR           MaxSupportedRate;
	UCHAR           CurrTxRate;
	USHORT          TxQuality[MAX_LEN_OF_SUPPORTED_RATES];
	USHORT          OneSecTxOkCount;
	USHORT          OneSecTxRetryOkCount;
	USHORT          OneSecTxFailCount;
	ULONG           CurrTxRateStableTime; // # of second in current TX rate
	UCHAR           TxRateUpPenalty;      // extra # of second penalty due to last unstable condition
} WDS_TABLE_ENTRY, *PWDS_TABLE_ENTRY;

typedef struct _RT_802_11_WDS_ENTRY {
	PNET_DEV			dev;
	UCHAR				Valid;
	UCHAR				PhyMode;
	UCHAR				PeerWdsAddr[MAC_ADDR_LEN];
	UCHAR				MacTabMatchWCID;	// ASIC
	NDIS_802_11_WEP_STATUS  WepStatus;
	UCHAR					KeyIdx;
	CIPHER_KEY          	WdsKey;
	HTTRANSMIT_SETTING				HTPhyMode, MaxHTPhyMode, MinHTPhyMode;
	RT_HT_PHY_INFO					DesiredHtPhyInfo;
	BOOLEAN							bAutoTxRateSwitch;
	DESIRED_TRANSMIT_SETTING       	DesiredTransmitSetting; // Desired transmit setting.
} RT_802_11_WDS_ENTRY, *PRT_802_11_WDS_ENTRY;

typedef struct _WDS_TABLE {
	UCHAR               Mode;
	ULONG               Size;
	RT_802_11_WDS_ENTRY	WdsEntry[MAX_WDS_ENTRY];
} WDS_TABLE, *PWDS_TABLE;

typedef struct _APCLI_STRUCT {
	PNET_DEV				dev;
#ifdef RTL865X_SOC
	unsigned int            mylinkid;
#endif
	BOOLEAN                 Enable;	// Set it as 1 if the apcli interface was configured to "1"  or by iwpriv cmd "ApCliEnable"
	BOOLEAN                 Valid;	// Set it as 1 if the apcli interface associated success to remote AP.
	UCHAR					MacTabWCID;	//WCID value, which point to the entry of ASIC Mac table.
	UCHAR                   SsidLen;
	CHAR                    Ssid[MAX_LEN_OF_SSID];

	UCHAR                   CfgSsidLen;
	CHAR                    CfgSsid[MAX_LEN_OF_SSID];
	UCHAR                   CfgApCliBssid[ETH_LENGTH_OF_ADDRESS];
	UCHAR                   CurrentAddress[ETH_LENGTH_OF_ADDRESS];

	ULONG                   ApCliRcvBeaconTime;

	ULONG                   CtrlCurrState;
	ULONG                   SyncCurrState;
	ULONG                   AuthCurrState;
	ULONG                   AssocCurrState;
	ULONG					WpaPskCurrState;

	USHORT                  AuthReqCnt;
	USHORT                  AssocReqCnt;

	ULONG                   ClientStatusFlags;
	UCHAR                   MpduDensity;

	NDIS_802_11_AUTHENTICATION_MODE     AuthMode;   // This should match to whatever microsoft defined
	NDIS_802_11_WEP_STATUS              WepStatus;

	// Add to support different cipher suite for WPA2/WPA mode
	NDIS_802_11_ENCRYPTION_STATUS		GroupCipher;		// Multicast cipher suite
	NDIS_802_11_ENCRYPTION_STATUS		PairCipher;			// Unicast cipher suite
	BOOLEAN								bMixCipher;			// Indicate current Pair & Group use different cipher suites
	USHORT								RsnCapability;
	
	UCHAR		PSK[100];				// reserve PSK key material
	UCHAR       PSKLen;
	UCHAR       PMK[32];                // WPA PSK mode PMK
	//UCHAR       PTK[64];                // WPA PSK mode PTK
	UCHAR		GTK[32];				// GTK from authenticator

	//CIPHER_KEY		PairwiseKey;
	CIPHER_KEY      SharedKey[SHARE_KEY_NUM];
	UCHAR           DefaultKeyId;

	// WPA 802.1x port control, WPA_802_1X_PORT_SECURED, WPA_802_1X_PORT_NOT_SECURED
	//UCHAR       PortSecured;
    
	// store RSN_IE built by driver
	UCHAR		RSN_IE[MAX_LEN_OF_RSNIE];  // The content saved here should be convert to little-endian format.
	UCHAR		RSNIE_Len; 

	// For WPA countermeasures
	ULONG       LastMicErrorTime;   // record last MIC error time
	//ULONG       MicErrCnt;          // Should be 0, 1, 2, then reset to zero (after disassoiciation).
	BOOLEAN                 bBlockAssoc; // Block associate attempt for 60 seconds after counter measure occurred.

	// For WPA-PSK supplicant state
	//WPA_STATE   	WpaState;           // Default is SS_NOTUSE
	//UCHAR       	ReplayCounter[8];
	//UCHAR       	ANonce[32];         // ANonce for WPA-PSK from authenticator
	UCHAR       	SNonce[32];         // SNonce for WPA-PSK
	UCHAR			GNonce[32];			// GNonce for WPA-PSK from authenticator
	
#ifdef WSC_AP_SUPPORT
	WSC_CTRL	           WscControl;
#endif // WSC_AP_SUPPORT //	

	HTTRANSMIT_SETTING				HTPhyMode, MaxHTPhyMode, MinHTPhyMode;
	RT_HT_PHY_INFO					DesiredHtPhyInfo;
	BOOLEAN							bAutoTxRateSwitch;
	DESIRED_TRANSMIT_SETTING       	DesiredTransmitSetting; // Desired transmit setting.
} APCLI_STRUCT, *PAPCLI_STRUCT;

#ifdef CONFIG_AP_SUPPORT
typedef struct _AP_ADMIN_CONFIG {
	USHORT          CapabilityInfo;
	// Multiple SSID
    UCHAR               BssidNum;
	UCHAR				MacMask;	
    MULTISSID_STRUCT    MBSSID[MAX_MBSSID_NUM];
	ULONG				IsolateInterStaTrafficBTNBSSID;

#ifdef APCLI_SUPPORT
	UCHAR				ApCliInfRunned;				// Number of  ApClient interface which was running. value from 0 to MAX_APCLI_INTERFACE
	APCLI_STRUCT		ApCliTab[MAX_APCLI_NUM];		//AP-client 
#endif // APCLI_SUPPORT //

	// for wpa
	RALINK_TIMER_STRUCT         CounterMeasureTimer;

	UCHAR           CMTimerRunning;
	UCHAR           BANClass3Data;
	LARGE_INTEGER   aMICFailTime;
	LARGE_INTEGER   PrevaMICFailTime;
	ULONG           MICFailureCounter;

	// for Group Rekey 
	RT_WPA_REKEY    WPAREKEY;
	ULONG           REKEYCOUNTER;
	RALINK_TIMER_STRUCT     REKEYTimer;
	UCHAR           REKEYTimerRunning;

	RSSI_SAMPLE		RssiSample;
	ULONG           NumOfAvgRssiSample;

	BOOLEAN         bAutoChannelAtBootup;  // 0: disable, 1: enable
	BOOLEAN         bIsolateInterStaTraffic;
	BOOLEAN         bHideSsid;

	// temporary latch for Auto channel selection
	CHAR            AutoChannel_MaxRssi;  // max RSSI during Auto Channel Selection period
	UCHAR           AutoChannel_Channel;  // channel number during Auto Channel Selection

//	Remove to MBSSID
//	UCHAR           RSNIE_Len[2];
//	UCHAR           RSN_IE[2][MAX_LEN_OF_RSNIE];

	UCHAR           Key_Counter[32];  //jan for wpa

	ULONG			PMKCachePeriod;		

	UCHAR           TimIELocationInBeacon;
	UCHAR           CapabilityInfoLocationInBeacon;
	UCHAR           DtimCount;      // 0.. DtimPeriod-1
	UCHAR           DtimPeriod;     // default = 3
	UCHAR           ErpIeContent;
	ULONG           LastOLBCDetectTime;
	ULONG			LastNoneHTOLBCDetectTime;
	ULONG       	LastScanTime;       // Record last scan time for issue BSSID_SCAN_LIST

	// For 802.1x daemon setting
	UINT32			own_ip_addr;
	UINT32			retry_interval;
	UINT32			session_timeout_interval;
	UCHAR 			EAPifname[IFNAMSIZ];		// indicate as the binding interface for EAP negotiation.
	UCHAR			EAPifname_len;
	UCHAR 			PreAuthifname[IFNAMSIZ];	// indicate as the binding interface for WPA2 Pre-authentication.
	UCHAR			PreAuthifname_len;

	// for CCX 1.0
#if 0	// AP doesn't need CCX variables
	BOOLEAN         bCkipOn;
	UCHAR           CkipFlag;
	UCHAR           GIV[3];  //for CCX iv
	UCHAR           RxSEQ[4];
	UCHAR           TxSEQ[4];
	UCHAR           CKIPMIC[4];
#endif

	// EDCA parameters to be announced to its local BSS
	EDCA_PARM       BssEdcaParm;

	// rt2860c support unlimited piggyback. should delete this boolean.
	BOOLEAN					bOneClientUsePiggyBack;	// This parameter didn't need anymore in RT2860.

	RALINK_TIMER_STRUCT		ApQuickResponeForRateUpTimer;
	BOOLEAN					ApQuickResponeForRateUpTimerRunning;

#ifdef IDS_SUPPORT
	// intrusion detection parameter	
	BOOLEAN					IdsEnable;
	UINT32					AuthFloodThreshold;		// Authentication frame flood threshold
	UINT32					AssocReqFloodThreshold;		// Association request frame flood threshold
	UINT32					ReassocReqFloodThreshold;	// Re-association request frame flood threshold
	UINT32					ProbeReqFloodThreshold;		// Probe request frame flood threshold
	UINT32					DisassocFloodThreshold;		// Disassociation frame flood threshold
	UINT32					DeauthFloodThreshold;		// Deauthentication frame flood threshold
	UINT32					EapReqFooldThreshold;		// EAP request frame flood threshold

	UINT32					RcvdAuthCount;	
	UINT32					RcvdAssocReqCount;		
	UINT32					RcvdReassocReqCount;	
	UINT32					RcvdProbeReqCount;		
	UINT32					RcvdDisassocCount;		
	UINT32					RcvdDeauthCount;		
	UINT32					RcvdEapReqCount;	

	RALINK_TIMER_STRUCT		IDSTimer;
	BOOLEAN					IDSTimerRunning;
#endif // IDS_SUPPORT //

#ifdef DOT11N_DRAFT3
	// =====for 11n D3.0 ==============================
	BOOLEAN					bBW40Permitted;
	BOOLEAN					bBssOverLapDetection;
	BOOLEAN					bBssCoexistMgmt;
#endif // DOT11N_DRAFT3 //

	ULONG					EntryLifeCheck;
} AP_ADMIN_CONFIG, *PAP_ADMIN_CONFIG;

#ifdef IGMP_SNOOP_SUPPORT
typedef enum _IGMP_GROUP_TYPE
{
	MODE_IS_INCLUDE = 1,
	MODE_IS_EXCLUDE,
	CHANGE_TO_INCLUDE_MODE,
	CHANGE_TO_EXCLUDE_MODE,
	ALLOW_NEW_SOURCES,
	BLOCK_OLD_SOURCES
} IgmpGroupType;

typedef enum _MULTICAST_FILTER_ENTRY_TYPE
{
	MCAT_FILTER_STATIC = 0,
	MCAT_FILTER_DYNAMIC,
} MulticastFilterEntryType;

typedef struct _MEMBER_ENTRY
{
	struct _MEMBER_ENTRY *pNext;
	UCHAR Addr[MAC_ADDR_LEN];
//	USHORT Aid;
} MEMBER_ENTRY, *PMEMBER_ENTRY;

typedef struct _MULTICAST_FILTER_TABLE_ENTRY
{
	BOOLEAN Valid;
	MulticastFilterEntryType type; // 0: static, 1: dynamic.
	UINT  lastTime;
	PNET_DEV net_dev;
	UCHAR Addr[MAC_ADDR_LEN];
	LIST_HEADER MemberList;
	struct _MULTICAST_FILTER_TABLE_ENTRY *pNext;
} MULTICAST_FILTER_TABLE_ENTRY, *PMULTICAST_FILTER_TABLE_ENTRY;

typedef struct _MULTICAST_FILTER_TABLE
{
	UCHAR Size;
	PMULTICAST_FILTER_TABLE_ENTRY Hash[MAX_LEN_OF_MULTICAST_FILTER_HASH_TABLE];
	MULTICAST_FILTER_TABLE_ENTRY Content[MAX_LEN_OF_MULTICAST_FILTER_TABLE];
	NDIS_SPIN_LOCK MulticastFilterTabLock;
	NDIS_SPIN_LOCK FreeMemberPoolTabLock;
	MEMBER_ENTRY freeMemberPool[FREE_MEMBER_POOL_SIZE];
	LIST_HEADER freeEntryList;
} MULTICAST_FILTER_TABLE, *PMULTICAST_FILTER_TABLE;
#endif // IGMP_SNOOP_SUPPORT //

#endif // CONFIG_AP_SUPPORT //
// ----------- end of AP ----------------------------

#ifdef BLOCK_NET_IF
typedef struct _BLOCK_QUEUE_ENTRY
{
	BOOLEAN SwTxQueueBlockFlag;
	LIST_HEADER NetIfList;
} BLOCK_QUEUE_ENTRY, *PBLOCK_QUEUE_ENTRY;
#endif // BLOCK_NET_IF //


struct wificonf
{
	BOOLEAN	bShortGI;
	BOOLEAN bGreenField;
};


#ifdef ETH_CONVERT_SUPPORT

#define ETH_CONVERT_NODE_MAX 256

// Ethernet Convertor operation mode definitions.
typedef enum{
	ETH_CONVERT_MODE_DISABLE = 0,
	ETH_CONVERT_MODE_DONGLE = 1,	// Multiple client support, dispatch to AP by device mac address.
	ETH_CONVERT_MODE_CLONE = 2,		// Single client support, dispatch to AP by client's mac address.
	ETH_CONVERT_MODE_HYBRID = 3,	// Multiple client supprot, dispatch to AP by client's mac address.
}ETH_CONVERT_MODE;


typedef struct _ETH_CONVERT_STRUCT_
{
	UCHAR		EthCloneMac[MAC_ADDR_LEN];	//Only meanful when ECMode = Clone/Hybrid mode.
	UCHAR		ECMode;						// 0 = Disable, 1 = Dongle mode, 2 = Clone mode, 3 = Hybrid mode.
	BOOLEAN		CloneMacVaild;				// 1 if the CloneMac is valid for connection. 0 if not valid.
//	UINT32		nodeCount;					// the number of nodes which connect to Internet via us.
	UCHAR       SSIDStr[MAX_LEN_OF_SSID];
	UCHAR		SSIDStrLen;
	BOOLEAN		macAutoLearn;				//0: disabled, 1: enabled.
}ETH_CONVERT_STRUCT;
#endif // ETH_CONVERT_SUPPORT //


typedef struct _INF_PCI_CONFIG
{
	PUCHAR                  CSRBaseAddress;     // PCI MMIO Base Address, all access will use
}INF_PCI_CONFIG;

typedef struct _INF_USB_CONFIG
{
	UINT                BulkInEpAddr;		// bulk-in endpoint address
	UINT                BulkOutEpAddr[6];	// bulk-out endpoint address
	
}INF_USB_CONFIG;

#ifdef IKANOS_VX_1X0
	typedef void (*IkanosWlanTxCbFuncP)(void *, void *);

	struct IKANOS_TX_INFO
	{
		struct net_device *netdev;
		IkanosWlanTxCbFuncP *fp;
	};
#endif // IKANOS_VX_1X0 //



#ifdef DBG_DIAGNOSE
#define DIAGNOSE_TIME	10   // 10 sec
typedef struct _RtmpDiagStrcut_
{	// Diagnosis Related element
	unsigned char		inited;
	unsigned char 	qIdx;
	unsigned char 	ArrayStartIdx;
	unsigned char		ArrayCurIdx;
	// Tx Related Count
	USHORT			TxDataCnt[DIAGNOSE_TIME];
	USHORT			TxFailCnt[DIAGNOSE_TIME];				
//	USHORT			TxDescCnt[DIAGNOSE_TIME][16];		// TxDesc queue length in scale of 0~14, >=15
	USHORT			TxDescCnt[DIAGNOSE_TIME][24]; // 3*3	// TxDesc queue length in scale of 0~14, >=15
//	USHORT			TxMcsCnt[DIAGNOSE_TIME][16];			// TxDate MCS Count in range from 0 to 15, step in 1.
	USHORT			TxMcsCnt[DIAGNOSE_TIME][24]; // 3*3
	USHORT			TxSWQueCnt[DIAGNOSE_TIME][9];		// TxSwQueue length in scale of 0, 1, 2, 3, 4, 5, 6, 7, >=8

	USHORT			TxAggCnt[DIAGNOSE_TIME];
	USHORT			TxNonAggCnt[DIAGNOSE_TIME];
//	USHORT			TxAMPDUCnt[DIAGNOSE_TIME][16];		// 10 sec, TxDMA APMDU Aggregation count in range from 0 to 15, in setp of 1.
	USHORT			TxAMPDUCnt[DIAGNOSE_TIME][24]; // 3*3 // 10 sec, TxDMA APMDU Aggregation count in range from 0 to 15, in setp of 1.
	USHORT			TxRalinkCnt[DIAGNOSE_TIME];			// TxRalink Aggregation Count in 1 sec scale.
	USHORT			TxAMSDUCnt[DIAGNOSE_TIME];			// TxAMSUD Aggregation Count in 1 sec scale.

	// Rx Related Count
	USHORT			RxDataCnt[DIAGNOSE_TIME];			// Rx Total Data count.
	USHORT			RxCrcErrCnt[DIAGNOSE_TIME];
//	USHORT			RxMcsCnt[DIAGNOSE_TIME][16];		// Rx MCS Count in range from 0 to 15, step in 1.
	USHORT			RxMcsCnt[DIAGNOSE_TIME][24]; // 3*3
}RtmpDiagStruct;
#endif // DBG_DIAGNOSE //


//
//  The miniport adapter structure
//
typedef struct _RTMP_ADAPTER
{
	PVOID					OS_Cookie;	// save specific structure relative to OS
	PNET_DEV				net_dev;
	ULONG					VirtualIfCnt;



	NDIS_SPIN_LOCK          irq_lock;
	UCHAR                   irq_disabled;

#ifdef RT2870
/*****************************************************************************************/
/*      USB related parameters                                                           */
/*****************************************************************************************/
	struct usb_config_descriptor		*config;
	UINT								BulkInEpAddr;		// bulk-in endpoint address
	UINT								BulkOutEpAddr[6];	// bulk-out endpoint address

	UINT								NumberOfPipes;
	USHORT								BulkOutMaxPacketSize;
	USHORT								BulkInMaxPacketSize;

	//======Control Flags
	LONG                    	PendingIoCount;	
	ULONG						BulkFlags;
	BOOLEAN                     bUsbTxBulkAggre;	// Flags for bulk out data priority


	//======Timer Thread
	RT2870_TIMER_QUEUE		TimerQ;
	NDIS_SPIN_LOCK			TimerQLock;


	//======Cmd Thread 
	CmdQ					CmdQ;
	NDIS_SPIN_LOCK			CmdQLock;				// CmdQLock spinlock

	BOOLEAN					TimerFunc_kill;
	BOOLEAN					mlme_kill;
	

	//======Semaphores (event)
	struct semaphore			mlme_semaphore;			/* to sleep thread on	*/
	struct semaphore			RTUSBCmd_semaphore;		/* to sleep thread on	*/
	struct semaphore			RTUSBTimer_semaphore;
#ifdef INF_AMAZON_SE
	struct semaphore			UsbVendorReq_semaphore;
	PVOID						UsbVendorReqBuf;
#endif // INF_AMAZON_SE //
#ifdef WINBOND
        struct semaphore                        Winbond_UsbVendorReq_semaphore;
        PVOID                                   Winbond_UsbVendorReqBuf;
#endif // WINBOND //

	struct completion			TimerQComplete;
	struct completion			mlmeComplete;
	struct completion			CmdQComplete;
	wait_queue_head_t			*wait;	

	//======Lock for 2870 ATE
#ifdef RALINK_ATE
	NDIS_SPIN_LOCK			GenericLock;		// ATE Tx/Rx generic spinlock
#endif // RALINK_ATE //

#endif // RT2870 //


/*****************************************************************************************/
	/*      Both PCI/USB related parameters                                                  */
/*****************************************************************************************/
	

/*****************************************************************************************/
/*      Tx related parameters                                                           */
/*****************************************************************************************/
	BOOLEAN                 DeQueueRunning[NUM_OF_TX_RING];  // for ensuring RTUSBDeQueuePacket get call once
	NDIS_SPIN_LOCK          DeQueueLock[NUM_OF_TX_RING];
		
#ifdef RT2870
	// Data related context and AC specified, 4 AC supported
	NDIS_SPIN_LOCK			BulkOutLock[6];			// BulkOut spinlock for 4 ACs
	NDIS_SPIN_LOCK			MLMEBulkOutLock;	// MLME BulkOut lock

	HT_TX_CONTEXT			TxContext[NUM_OF_TX_RING];
	NDIS_SPIN_LOCK			TxContextQueueLock[NUM_OF_TX_RING];		// TxContextQueue spinlock

	// 4 sets of Bulk Out index and pending flag
	UCHAR					NextBulkOutIndex[4];	// only used for 4 EDCA bulkout pipe

	BOOLEAN					BulkOutPending[6];	// used for total 6 bulkout pipe
	UCHAR					bulkResetPipeid;
	BOOLEAN					MgmtBulkPending;
	ULONG					bulkResetReq[6];
#endif // RT2870 //

	// resource for software backlog queues
	QUEUE_HEADER            TxSwQueue[NUM_OF_TX_RING];  // 4 AC + 1 HCCA
	NDIS_SPIN_LOCK          TxSwQueueLock[NUM_OF_TX_RING];	// TxSwQueue spinlock
	
	RTMP_DMABUF             MgmtDescRing;               	// Shared memory for MGMT descriptors
	RTMP_MGMT_RING          MgmtRing;
	NDIS_SPIN_LOCK          MgmtRingLock;               	// Prio Ring spinlock


/*****************************************************************************************/
/*      Rx related parameters                                                           */
/*****************************************************************************************/

	
#ifdef RT2870
	RX_CONTEXT				RxContext[RX_RING_SIZE];  // 1 for redundant multiple IRP bulk in.
	NDIS_SPIN_LOCK			BulkInLock;				// BulkIn spinlock for 4 ACs
	UCHAR					PendingRx;				// The Maxima pending Rx value should be 	RX_RING_SIZE.
	UCHAR					NextRxBulkInIndex;		// Indicate the current RxContext Index which hold by Host controller.
	UCHAR					NextRxBulkInReadIndex;	// Indicate the current RxContext Index which driver can read & process it.
	ULONG					NextRxBulkInPosition;   // Want to contatenate 2 URB buffer while 1st is bulkin failed URB. This Position is 1st URB TransferLength.
	ULONG					TransferBufferLength;	// current length of the packet buffer
	ULONG					ReadPosition;			// current read position in a packet buffer
#endif // RT2870 //


/*****************************************************************************************/
/*      ASIC related parameters                                                          */
/*****************************************************************************************/
	UINT32               	MACVersion;      	// MAC version. Record rt2860C(0x28600100) or rt2860D (0x28600101)..

	// ---------------------------
	// E2PROM
	// ---------------------------
	ULONG                   EepromVersion;          // byte 0: version, byte 1: revision, byte 2~3: unused
	UCHAR                   EEPROMAddressNum;       // 93c46=6  93c66=8
	USHORT                  EEPROMDefaultValue[NUM_EEPROM_BBP_PARMS];
	BOOLEAN                 EepromAccess;
	UCHAR                   EFuseTag;
	ULONG                   FirmwareVersion;        // byte 0: Minor version, byte 1: Major version, otherwise unused.

	// ---------------------------
	// BBP Control
	// ---------------------------
	UCHAR                   BbpWriteLatch[140];     // record last BBP register value written via BBP_IO_WRITE/BBP_IO_WRITE_VY_REG_ID
	UCHAR                   BbpRssiToDbmDelta;
	BBP_R66_TUNING          BbpTuning;

	// ----------------------------
	// RFIC control
	// ----------------------------
	UCHAR                   RfIcType;       // RFIC_xxx
	ULONG                   RfFreqOffset;   // Frequency offset for channel switching
	RTMP_RF_REGS            LatchRfRegs;    // latch th latest RF programming value since RF IC doesn't support READ

	EEPROM_ANTENNA_STRUC    Antenna;                            // Since ANtenna definition is different for a & g. We need to save it for future reference.
	EEPROM_NIC_CONFIG2_STRUC    NicConfig2;

	// This soft Rx Antenna Diversity mechanism is used only when user set 
	// RX Antenna = DIVERSITY ON
	SOFT_RX_ANT_DIVERSITY   RxAnt;

	UCHAR                   RFProgSeq;
	CHANNEL_TX_POWER        TxPower[MAX_NUM_OF_CHANNELS];       // Store Tx power value for all channels.
	CHANNEL_TX_POWER        ChannelList[MAX_NUM_OF_CHANNELS];   // list all supported channels for site survey
	CHANNEL_11J_TX_POWER    TxPower11J[MAX_NUM_OF_11JCHANNELS];       // 802.11j channel and bw
	CHANNEL_11J_TX_POWER    ChannelList11J[MAX_NUM_OF_11JCHANNELS];   // list all supported channels for site survey
	
	UCHAR                   ChannelListNum;                     // number of channel in ChannelList[]
	UCHAR					Bbp94;
	BOOLEAN					BbpForCCK;
	ULONG		Tx20MPwrCfgABand[5];
	ULONG		Tx20MPwrCfgGBand[5];
	ULONG		Tx40MPwrCfgABand[5];
	ULONG		Tx40MPwrCfgGBand[5];
	
	BOOLEAN     bAutoTxAgcA;                // Enable driver auto Tx Agc control
	UCHAR	    TssiRefA;					// Store Tssi reference value as 25 temperature.	
	UCHAR	    TssiPlusBoundaryA[5];		// Tssi boundary for increase Tx power to compensate.
	UCHAR	    TssiMinusBoundaryA[5];		// Tssi boundary for decrease Tx power to compensate.
	UCHAR	    TxAgcStepA;					// Store Tx TSSI delta increment / decrement value
	CHAR		TxAgcCompensateA;			// Store the compensation (TxAgcStep * (idx-1))
	
	BOOLEAN     bAutoTxAgcG;                // Enable driver auto Tx Agc control
	UCHAR	    TssiRefG;					// Store Tssi reference value as 25 temperature.	
	UCHAR	    TssiPlusBoundaryG[5];		// Tssi boundary for increase Tx power to compensate.
	UCHAR	    TssiMinusBoundaryG[5];		// Tssi boundary for decrease Tx power to compensate.
	UCHAR	    TxAgcStepG;					// Store Tx TSSI delta increment / decrement value
	CHAR		TxAgcCompensateG;			// Store the compensation (TxAgcStep * (idx-1))

	//+++For RT2870, the parameteres is start from BGRssiOffset1 ~ BGRssiOffset3
	CHAR		BGRssiOffset0;				// Store B/G RSSI#0 Offset value on EEPROM 0x46h
	CHAR		BGRssiOffset1;				// Store B/G RSSI#1 Offset value 
	CHAR		BGRssiOffset2;				// Store B/G RSSI#2 Offset value 
	//---

	//+++For RT2870, the parameteres is start from ARssiOffset1 ~ ARssiOffset3	
	CHAR		ARssiOffset0;				// Store A RSSI#0 Offset value on EEPROM 0x4Ah
	CHAR		ARssiOffset1;				// Store A RSSI#1 Offset value 
	CHAR		ARssiOffset2;				// Store A RSSI#2 Offset value 
	//---
	
	CHAR		BLNAGain;					// Store B/G external LNA#0 value on EEPROM 0x44h
	CHAR		ALNAGain0;					// Store A external LNA#0 value for ch36~64
	CHAR		ALNAGain1;					// Store A external LNA#1 value for ch100~128
	CHAR		ALNAGain2;					// Store A external LNA#2 value for ch132~165
#ifdef RT30xx
	UCHAR		TxMixerGain;				// Tx mixer gain value from EEPROM to improve Tx EVM	
#endif // RT30xx //
	// ----------------------------
	// LED control
	// ----------------------------
	MCU_LEDCS_STRUC		LedCntl;
	USHORT				Led1;	// read from EEPROM 0x3c
	USHORT				Led2;	// EEPROM 0x3e
	USHORT				Led3;	// EEPROM 0x40
	UCHAR				LedIndicatorStregth;
	UCHAR				RssiSingalstrengthOffet;
    BOOLEAN				bLedOnScanning;
	UCHAR				LedStatus;

/*****************************************************************************************/
/*      802.11 related parameters                                                        */
/*****************************************************************************************/
	// outgoing BEACON frame buffer and corresponding TXD 
	TXWI_STRUC              	BeaconTxWI;
	PUCHAR						BeaconBuf;
	USHORT						BeaconOffset[HW_BEACON_MAX_COUNT];

	// pre-build PS-POLL and NULL frame upon link up. for efficiency purpose.
	PSPOLL_FRAME            	PsPollFrame;
	HEADER_802_11           	NullFrame;

#ifdef RT2870
	TX_CONTEXT				BeaconContext[BEACON_RING_SIZE];
	TX_CONTEXT				NullContext;
	TX_CONTEXT				PsPollContext;
	TX_CONTEXT				RTSContext;
#endif // RT2870 //



//=========AP===========
#ifdef CONFIG_AP_SUPPORT
	// -----------------------------------------------
	// AP specific configuration & operation status
	// used only when pAd->OpMode == OPMODE_AP
	// -----------------------------------------------
	AP_ADMIN_CONFIG         ApCfg;             // user configuration when in AP mode
	AP_MLME_AUX             ApMlmeAux;	

#ifdef UAPSD_AP_SUPPORT
    NDIS_SPIN_LOCK          UAPSDEOSPLock; /* EOSP frame access lock use */
	BOOLEAN					bAPSDFlagSPSuspend;	/* 1: SP is suspended; 0: SP is not */
#endif // UAPSD_SUPPORT //

	WDS_TABLE               WdsTab;            // WDS table when working as an AP
	NDIS_SPIN_LOCK          WdsTabLock;
	//RT_802_11_ACL           AccessControlList;

#ifdef MBSS_SUPPORT
	BOOLEAN					flg_mbss_init;
#endif // MBSS_SUPPORT //

#ifdef WDS_SUPPORT
	BOOLEAN					flg_wds_init;
#endif // WDS_SUPPORT //

#ifdef APCLI_SUPPORT
	BOOLEAN					flg_apcli_init;
#endif // APCLI_SUPPORT //

	PBSSINFO				pBssInfoTab;
	PCHANNELINFO			pChannelInfo;


#endif // CONFIG_AP_SUPPORT //


//=======STA===========

//=======Common===========
	// OP mode: either AP or STA
	UCHAR                   OpMode;                     // OPMODE_STA, OPMODE_AP
	
	NDIS_MEDIA_STATE        IndicateMediaState;			// Base on Indication state, default is NdisMediaStateDisConnected

#ifdef WSC_INCLUDED
    pid_t                   write_dat_file_pid;
    int                     time_to_die;
    struct semaphore	    write_dat_file_semaphore;		/* to sleep thread on	*/
    struct completion	    write_dat_file_notify;
	BOOLEAN             	WriteWscCfgToDatFile;

	/* WSC hardware push button function 0811 */
	BOOLEAN					WscHdrPshBtnFlag; /* 1: support, read from EEPROM */
#endif // WSC_INCLUDED //

	// MAT related parameters
#ifdef MAT_SUPPORT
	MAT_STRUCT		MatCfg;
#endif // MAT_SUPPORT //

	// configuration: read from Registry & E2PROM
	BOOLEAN                 bLocalAdminMAC;             // Use user changed MAC
	UCHAR                   PermanentAddress[MAC_ADDR_LEN];    // Factory default MAC address
	UCHAR                   CurrentAddress[MAC_ADDR_LEN];      // User changed MAC address

	// ------------------------------------------------------
	// common configuration to both OPMODE_STA and OPMODE_AP
	// ------------------------------------------------------
	COMMON_CONFIG           CommonCfg;
	MLME_STRUCT             Mlme;

	// AP needs those vaiables for site survey feature.
	MLME_AUX                MlmeAux;           // temporary settings used during MLME state machine
	BSS_TABLE               ScanTab;           // store the latest SCAN result

	//About MacTab, the sta driver will use #0 and #1 for multicast and AP.
	MAC_TABLE                 MacTab;     // ASIC on-chip WCID entry table.  At TX, ASIC always use key according to this on-chip table.
	NDIS_SPIN_LOCK          MacTabLock;

#ifdef DOT11_N_SUPPORT
	BA_TABLE			BATable;
#endif // DOT11_N_SUPPORT //
	NDIS_SPIN_LOCK          BATabLock;
	RALINK_TIMER_STRUCT RECBATimer;
	
	// encryption/decryption KEY tables
	CIPHER_KEY              SharedKey[MAX_MBSSID_NUM][4]; // STA always use SharedKey[BSS0][0..3]

		// RX re-assembly buffer for fragmentation
	FRAGMENT_FRAME          FragFrame;                  // Frame storage for fragment frame

	// various Counters 
	COUNTER_802_3           Counters8023;               // 802.3 counters
	COUNTER_802_11          WlanCounters;               // 802.11 MIB counters
	COUNTER_RALINK          RalinkCounters;             // Ralink propriety counters
	COUNTER_DRS             DrsCounters;                // counters for Dynamic TX Rate Switching
	PRIVATE_STRUC           PrivateInfo;                // Private information & counters

	// flags, see fRTMP_ADAPTER_xxx flags
	ULONG                   Flags;                      // Represent current device status

	// current TX sequence #
	USHORT                  Sequence;

#ifdef UNDER_CE
	NDIS_HANDLE             hGiISR;
#endif


	// Control disconnect / connect event generation
	//+++Didn't used anymore
	ULONG                   LinkDownTime;
	//---
	ULONG                   LastRxRate;
	ULONG                   LastTxRate;
	//+++Used only for Station
	BOOLEAN                 bConfigChanged;         // Config Change flag for the same SSID setting
	//---
	
	ULONG                   ExtraInfo;              // Extra information for displaying status
	ULONG                   SystemErrorBitmap;      // b0: E2PROM version error

	//+++Didn't used anymore
	ULONG                   MacIcVersion;           // MAC/BBP serial interface issue solved after ver.D
	//---

	// ---------------------------
	// System event log
	// ---------------------------
	RT_802_11_EVENT_TABLE   EventTab;


	BOOLEAN		HTCEnable;
	
	/*****************************************************************************************/
	/*      Statistic related parameters                                                     */
	/*****************************************************************************************/
#ifdef RT2870
	ULONG						BulkOutDataOneSecCount;
	ULONG						BulkInDataOneSecCount;
	ULONG						BulkLastOneSecCount; // BulkOutDataOneSecCount + BulkInDataOneSecCount
	ULONG						watchDogRxCnt;
	ULONG						watchDogRxOverFlowCnt;
	ULONG						watchDogTxPendingCnt[NUM_OF_TX_RING];
#endif // RT2870 //

	BOOLEAN						bUpdateBcnCntDone;		
	ULONG						watchDogMacDeadlock;	// prevent MAC/BBP into deadlock condition
	// ----------------------------
	// DEBUG paramerts
	// ----------------------------
	//ULONG		DebugSetting[4];
	BOOLEAN		bBanAllBaSetup;
	BOOLEAN		bPromiscuous;

	// ----------------------------
	// rt2860c emulation-use Parameters 
	// ----------------------------
	ULONG		rtsaccu[30];
	ULONG		ctsaccu[30];
	ULONG		cfendaccu[30];
	ULONG		bacontent[16];
	ULONG		rxint[RX_RING_SIZE+1];
	UCHAR		rcvba[60];
	BOOLEAN		bLinkAdapt;
	BOOLEAN		bForcePrintTX;
	BOOLEAN		bForcePrintRX;
	BOOLEAN		bDisablescanning;		//defined in RT2870 USB
	BOOLEAN		bStaFifoTest;
	BOOLEAN		bProtectionTest;
	BOOLEAN		bHCCATest;
	BOOLEAN		bGenOneHCCA;
	BOOLEAN		bBroadComHT;
	//+++Following add from RT2870 USB.
	ULONG		BulkOutReq;
	ULONG		BulkOutComplete;
	ULONG		BulkOutCompleteOther;
	ULONG		BulkOutCompleteCancel;	// seems not use now?
	ULONG		BulkInReq;
	ULONG		BulkInComplete;
	ULONG		BulkInCompleteFail;
	//---

    struct wificonf			WIFItestbed;

#ifdef RALINK_ATE
	ATE_INFO				ate;
#ifdef RT2870
	BOOLEAN					ContinBulkOut;		//ATE bulk out control
	BOOLEAN					ContinBulkIn;		//ATE bulk in control
	atomic_t				BulkOutRemained;
	atomic_t				BulkInRemained;
#endif // RT2870 //
#endif // RALINK_ATE //

#ifdef DOT11_N_SUPPORT
	struct reordering_mpdu_pool mpdu_blk_pool;
#endif // DOT11_N_SUPPORT //

	ULONG					OneSecondnonBEpackets;		// record non BE packets per second 

#if WIRELESS_EXT >= 12
    struct iw_statistics    iw_stats;
#endif

	struct net_device_stats	stats;

#ifdef BLOCK_NET_IF
	BLOCK_QUEUE_ENTRY		blockQueueTab[NUM_OF_TX_RING];
#endif // BLOCK_NET_IF //

#ifdef CONFIG_AP_SUPPORT
#ifdef IGMP_SNOOP_SUPPORT
	PMULTICAST_FILTER_TABLE	pMulticastFilterTable;
	UCHAR					IgmpGroupTxRate;
#endif // IGMP_SNOOP_SUPPORT //
	UCHAR					StaCount[MAX_MBSSID_NUM];
#endif // CONFIG_AP_SUPPORT //

#ifdef ETH_CONVERT_SUPPORT
	ETH_CONVERT_STRUCT		EthConvert;
#endif // ETH_CONVERT_SUPPORT //

#ifdef MULTIPLE_CARD_SUPPORT
	INT32					MC_RowID;
	UCHAR					MC_FileName[256];
#endif // MULTIPLE_CARD_SUPPORT //

	ULONG					TbttTickCount;
#ifdef PCI_MSI_SUPPORT
	BOOLEAN					HaveMsi;
#endif // PCI_MSI_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
	RALINK_TIMER_STRUCT     PeriodicTimer;

	UINT8					flg_qload_enable; /* 1: any BSS WMM is enabled */
	UINT8					qload_chan_util; /* last QBSS Load, unit: us */
	UINT32					qload_chan_util_total; /* current QBSS Load Total */
	UINT8					qload_chan_util_beacon_cnt; /* 1~100, default: 50 */
	UINT8					qload_chan_util_beacon_int; /* 1~100, default: 50 */
#endif // CONFIG_AP_SUPPORT //

	UCHAR					is_on;

#define TIME_BASE			(1000000/OS_HZ)
#define TIME_ONE_SECOND		(1000000/TIME_BASE)
	UCHAR					flg_be_adjust;
	ULONG					be_adjust_last_time;


#ifdef WSC_INCLUDED
    // for multiple card
    UCHAR	                *pHmacData;
    UCHAR                   Wsc_Uuid_E[UUID_LEN_HEX];
    UCHAR                   Wsc_Uuid_Str[UUID_LEN_STR];
#endif // WSC_INCLUDED //

#ifdef IKANOS_VX_1X0
	struct IKANOS_TX_INFO	IkanosTxInfo;
	struct IKANOS_TX_INFO	IkanosRxInfo[MAX_MBSSID_NUM + MAX_WDS_ENTRY + MAX_APCLI_NUM + MAX_MESH_NUM];
#endif // IKANOS_VX_1X0 //


#ifdef DBG_DIAGNOSE
	RtmpDiagStruct	DiagStruct;
#endif // DBG_DIAGNOSE //


	UINT8					PM_FlgSuspend;

#ifdef RT30xx
//======efuse
	BOOLEAN		bUseEfuse;
	BOOLEAN		bEEPROMFile;
#endif // RT30xx //

} RTMP_ADAPTER, *PRTMP_ADAPTER;

//
// Cisco IAPP format
//
typedef struct  _CISCO_IAPP_CONTENT_
{
	USHORT     Length;        //IAPP Length
	UCHAR      MessageType;      //IAPP type
	UCHAR      FunctionCode;     //IAPP function type
	UCHAR      DestinaionMAC[MAC_ADDR_LEN];   
	UCHAR      SourceMAC[MAC_ADDR_LEN];
	USHORT     Tag;           //Tag(element IE) - Adjacent AP report
	USHORT     TagLength;     //Length of element not including 4 byte header
	UCHAR      OUI[4];           //0x00, 0x40, 0x96, 0x00
	UCHAR      PreviousAP[MAC_ADDR_LEN];       //MAC Address of access point
	USHORT     Channel;       
	USHORT     SsidLen;
	UCHAR      Ssid[MAX_LEN_OF_SSID];
	USHORT     Seconds;          //Seconds that the client has been disassociated.
} CISCO_IAPP_CONTENT, *PCISCO_IAPP_CONTENT;

#define DELAYINTMASK		0x0003fffb
#define INTMASK				0x0003fffb
#define IndMask				0x0003fffc
#define RxINT				0x00000005	// Delayed Rx or indivi rx
#define TxDataInt			0x000000fa	// Delayed Tx or indivi tx
#define TxMgmtInt			0x00000102	// Delayed Tx or indivi tx 
#define TxCoherent			0x00020000	// tx coherent 
#define RxCoherent			0x00010000	// rx coherent
#define McuCommand			0x00000200	// mcu
#define PreTBTTInt			0x00001000	// Pre-TBTT interrupt
#define TBTTInt				0x00000800		// TBTT interrupt
#define GPTimeOutInt			0x00008000		// GPtimeout interrupt
#define AutoWakeupInt		0x00004000		// AutoWakeupInt interrupt
#define FifoStaFullInt			0x00002000	//  fifo statistics full interrupt


typedef struct _RX_BLK_
{
//	RXD_STRUC		RxD; // sample
	RT28XX_RXD_STRUC	RxD;
	PRXWI_STRUC			pRxWI;
	PHEADER_802_11		pHeader;
	PNDIS_PACKET		pRxPacket;
	UCHAR				*pData;
	USHORT				DataSize;
	USHORT				Flags;
	UCHAR				UserPriority;	// for calculate TKIP MIC using
} RX_BLK;


#define RX_BLK_SET_FLAG(_pRxBlk, _flag)		(_pRxBlk->Flags |= _flag)
#define RX_BLK_TEST_FLAG(_pRxBlk, _flag)	(_pRxBlk->Flags & _flag)
#define RX_BLK_CLEAR_FLAG(_pRxBlk, _flag)	(_pRxBlk->Flags &= ~(_flag))


#define fRX_WDS			0x0001
#define fRX_AMSDU       0x0002
#define fRX_ARALINK     0x0004
#define fRX_HTC         0x0008
#define fRX_PAD         0x0010
#define fRX_AMPDU       0x0020
#define fRX_QOS			0x0040
#define fRX_INFRA		0x0080
#define fRX_EAP			0x0100
#define fRX_MESH		0x0200
#define fRX_APCLI		0x0400
#define fRX_DLS			0x0800
#define fRX_WPI			0x1000

#define LENGTH_AMSDU_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_SUBFRAMEHEAD	14
#define LENGTH_ARALINK_HEADER_FIELD	 2

#define TX_UNKOWN_FRAME			0x00
#define TX_MCAST_FRAME			0x01
#define TX_LEGACY_FRAME			0x02
#define TX_AMPDU_FRAME			0x04
#define TX_AMSDU_FRAME			0x08
#define TX_RALINK_FRAME			0x10
#define TX_FRAG_FRAME			0x20


//	Currently the sizeof(TX_BLK) is 148 bytes.
typedef struct _TX_BLK_
{
	UCHAR				QueIdx;
	UCHAR				TxFrameType;				// Indicate the Transmission type of the all frames in one batch
	UCHAR				TotalFrameNum;				// Total frame number want to send-out in one batch
	USHORT				TotalFragNum;				// Total frame fragments required in one batch
	USHORT				TotalFrameLen;				// Total length of all frames want to send-out in one batch

	QUEUE_HEADER		TxPacketList;
	MAC_TABLE_ENTRY		*pMacEntry;					// NULL: packet with 802.11 RA field is multicast/broadcast address
	HTTRANSMIT_SETTING	*pTransmit;
	
	// Following structure used for the characteristics of a specific packet.
	PNDIS_PACKET		pPacket;
	PUCHAR				pSrcBufHeader;				// Reference to the head of sk_buff->data
	PUCHAR				pSrcBufData;				// Reference to the sk_buff->data, will changed depends on hanlding progresss
	UINT				SrcBufLen;					// Length of packet payload which not including Layer 2 header
	PUCHAR				pExtraLlcSnapEncap;			// NULL means no extra LLC/SNAP is required
	UCHAR				HeaderBuf[80];				// TempBuffer for TX_INFO + TX_WI + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP
	UCHAR				MpduHeaderLen;				// 802.11 header length NOT including the padding
	UCHAR				HdrPadLen;					// recording Header Padding Length;
	UCHAR				apidx;						// The interface associated to this packet 
	UCHAR				Wcid;						// The MAC entry associated to this packet
	UCHAR				UserPriority;				// priority class of packet
	UCHAR				FrameGap;					// what kind of IFS this packet use
	UCHAR				MpduReqNum;					// number of fragments of this frame
	UCHAR				TxRate;						// TODO: Obsoleted? Should change to MCS?
	UCHAR				CipherAlg;					// cipher alogrithm
	PCIPHER_KEY			pKey;
	

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	UINT				ApCliIfidx;
	PAPCLI_STRUCT		pApCliEntry;
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //

	USHORT				Flags;						//See following definitions for detail.

	//YOU SHOULD NOT TOUCH IT! Following parameters are used for hardware-depended layer.
	ULONG				Priv;						// Hardware specific value saved in here.
} TX_BLK, *PTX_BLK;


#define fTX_bRtsRequired		0x0001	// Indicate if need send RTS frame for protection. Not used in RT2860/RT2870.
#define fTX_bAckRequired       	0x0002	// the packet need ack response
#define fTX_bPiggyBack     		0x0004	// Legacy device use Piggback or not
#define fTX_bHTRate         	0x0008	// allow to use HT rate
//#define fTX_bForceLowRate       0x0010	// force to use Low Rate 
#define fTX_bForceNonQoS       	0x0010	// force to transmit frame without WMM-QoS in HT mode 
#define fTX_bAllowFrag       	0x0020	// allow to fragment the packet, A-MPDU, A-MSDU, A-Ralink is not allowed to fragment
#define fTX_bMoreData			0x0040	// there are more data packets in PowerSave Queue
#define fTX_bWMM				0x0080	// QOS Data

#define fTX_bClearEAPFrame		0x0100

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#define fTX_bApCliPacket		0x0200
#define	fTX_bClearEAPoLFrame	0x0400	// Used when APCLI_SUPPORT
#endif // APCLI_SUPPORT //

#ifdef UAPSD_AP_SUPPORT
#define	fTX_bWMM_UAPSD_EOSP		0x0800	// Used when UAPSD_SUPPORT
#endif // UAPSD_AP_SUPPORT //

#ifdef WDS_SUPPORT
#define fTX_bWDSEntry			0x1000	// Used when WDS_SUPPORT
#endif // WDS_SUPPORT

#endif // CONFIG_AP_SUPPORT //




#define TX_BLK_ASSIGN_FLAG(_pTxBlk, _flag, value)	\
		do {										\
			if (value) 								\
				(_pTxBlk->Flags |= _flag) 			\
			else 									\
				(_pTxBlk->Flags &= ~(_flag))		\
		}while(0)
		
#define TX_BLK_SET_FLAG(_pTxBlk, _flag)		(_pTxBlk->Flags |= _flag)
#define TX_BLK_TEST_FLAG(_pTxBlk, _flag)	(((_pTxBlk->Flags & _flag) == _flag) ? 1 : 0)
#define TX_BLK_CLEAR_FLAG(_pTxBlk, _flag)	(_pTxBlk->Flags &= ~(_flag))
	




//------------------------------------------------------------------------------------------



#ifdef RT_BIG_ENDIAN
static inline VOID	WriteBackToDescriptor(
	IN  PUCHAR			Dest,
 	IN	PUCHAR			Src,
    IN  BOOLEAN			DoEncrypt,
	IN  ULONG           DescriptorType)
{
	UINT32 *p1, *p2;

	p1 = ((UINT32 *)Dest);
	p2 = ((UINT32 *)Src);
	
	*p1 = *p2;
	*(p1+2) = *(p2+2);
	*(p1+3) = *(p2+3);
	*(p1+1) = *(p2+1); // Word 1; this must be written back last
}

/*
	========================================================================

	Routine Description:
		Endian conversion of Tx/Rx descriptor .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to Tx/Rx descriptor
		DescriptorType	Direction of the frame

	Return Value:
		None

	Note:
		Call this function when read or update descriptor
	========================================================================
*/
static inline VOID	RTMPWIEndianChange(
	IN	PUCHAR			pData,
	IN	ULONG			DescriptorType)
{
	int size;
	int i;
	
	size = ((DescriptorType == TYPE_TXWI) ? TXWI_SIZE : RXWI_SIZE);
	
	if(DescriptorType == TYPE_TXWI)
	{
		*((UINT32 *)(pData)) = SWAP32(*((UINT32 *)(pData)));		// Byte 0~3
		*((UINT32 *)(pData + 4)) = SWAP32(*((UINT32 *)(pData+4)));	// Byte 4~7
	} 
	else
	{
		for(i=0; i < size/4 ; i++)
			*(((UINT32 *)pData) +i) = SWAP32(*(((UINT32 *)pData)+i));
	}
}

/*
	========================================================================

	Routine Description:
		Endian conversion of Tx/Rx descriptor .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to Tx/Rx descriptor
		DescriptorType	Direction of the frame

	Return Value:
		None

	Note:
		Call this function when read or update descriptor
	========================================================================
*/

#ifdef RT2870
static inline VOID	RTMPDescriptorEndianChange(
	IN	PUCHAR			pData,
	IN	ULONG			DescriptorType)
{	
	*((UINT32 *)(pData)) = SWAP32(*((UINT32 *)(pData)));
}
#endif // RT2870 //
/*
	========================================================================

	Routine Description:
		Endian conversion of all kinds of 802.11 frames .

	Arguments:
		pAd 	Pointer to our adapter
		pData			Pointer to the 802.11 frame structure
		Dir 			Direction of the frame
		FromRxDoneInt	Caller is from RxDone interrupt

	Return Value:
		None

	Note:
		Call this function when read or update buffer data
	========================================================================
*/
static inline VOID	RTMPFrameEndianChange(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pData, 
	IN	ULONG			Dir,
	IN	BOOLEAN 		FromRxDoneInt)
{
	PHEADER_802_11 pFrame;
	PUCHAR	pMacHdr;

	// swab 16 bit fields - Frame Control field
	if(Dir == DIR_READ)
	{
		*(USHORT *)pData = SWAP16(*(USHORT *)pData);
	}

	pFrame = (PHEADER_802_11) pData;
	pMacHdr = (PUCHAR) pFrame;

	// swab 16 bit fields - Duration/ID field
	*(USHORT *)(pMacHdr + 2) = SWAP16(*(USHORT *)(pMacHdr + 2));

	// swab 16 bit fields - Sequence Control field
	*(USHORT *)(pMacHdr + 22) = SWAP16(*(USHORT *)(pMacHdr + 22));

	if(pFrame->FC.Type == BTYPE_MGMT)
	{
		switch(pFrame->FC.SubType)
		{
			case SUBTYPE_ASSOC_REQ:
			case SUBTYPE_REASSOC_REQ:
				// swab 16 bit fields - CapabilityInfo field
				pMacHdr += sizeof(HEADER_802_11);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				// swab 16 bit fields - Listen Interval field
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_ASSOC_RSP:
			case SUBTYPE_REASSOC_RSP:
				// swab 16 bit fields - CapabilityInfo field
				pMacHdr += sizeof(HEADER_802_11);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				// swab 16 bit fields - Status Code field
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				// swab 16 bit fields - AID field
				pMacHdr += 2;
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_AUTH:
				// If from APHandleRxDoneInterrupt routine, it is still a encrypt format.
				// The convertion is delayed to RTMPHandleDecryptionDoneInterrupt.
				if(!FromRxDoneInt && pFrame->FC.Wep == 1)
					break;
				else
				{
					// swab 16 bit fields - Auth Alg No. field
					pMacHdr += sizeof(HEADER_802_11);
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

					// swab 16 bit fields - Auth Seq No. field
					pMacHdr += 2;
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

					// swab 16 bit fields - Status Code field
					pMacHdr += 2;
					*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				}
				break;

			case SUBTYPE_BEACON:
			case SUBTYPE_PROBE_RSP:
				// swab 16 bit fields - BeaconInterval field
				pMacHdr += (sizeof(HEADER_802_11) + TIMESTAMP_LEN);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);

				// swab 16 bit fields - CapabilityInfo field
				pMacHdr += sizeof(USHORT);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;

			case SUBTYPE_DEAUTH:
			case SUBTYPE_DISASSOC:
				// swab 16 bit fields - Reason code field
				pMacHdr += sizeof(HEADER_802_11);
				*(USHORT *)pMacHdr = SWAP16(*(USHORT *)pMacHdr);
				break;
		}
	}
	else if( pFrame->FC.Type == BTYPE_DATA )
	{
	}
	else if(pFrame->FC.Type == BTYPE_CNTL)
	{
		switch(pFrame->FC.SubType)
		{
			case SUBTYPE_BLOCK_ACK_REQ:
				{
					PFRAME_BA_REQ pBAReq = (PFRAME_BA_REQ)pFrame;
					*(USHORT *)(&pBAReq->BARControl) = SWAP16(*(USHORT *)(&pBAReq->BARControl));
					pBAReq->BAStartingSeq.word = SWAP16(pBAReq->BAStartingSeq.word);
				}
				break;
			case SUBTYPE_BLOCK_ACK:
				// For Block Ack packet, the HT_CONTROL field is in the same offset with Addr3 
				*(UINT32 *)(&pFrame->Addr3[0]) = SWAP32(*(UINT32 *)(&pFrame->Addr3[0]));
				break;

			case SUBTYPE_ACK:
				//For ACK packet, the HT_CONTROL field is in the same offset with Addr2
				*(UINT32 *)(&pFrame->Addr2[0])=	SWAP32(*(UINT32 *)(&pFrame->Addr2[0]));
				break;
		}
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR,("Invalid Frame Type!!!\n"));
	}

	// swab 16 bit fields - Frame Control
	if(Dir == DIR_WRITE)
	{
		*(USHORT *)pData = SWAP16(*(USHORT *)pData);
	}
}
#endif // RT_BIG_ENDIAN //


static inline VOID ConvertMulticastIP2MAC(
	IN PUCHAR pIpAddr,
	IN PUCHAR *ppMacAddr, 
	IN UINT16 ProtoType)
{
	if (pIpAddr == NULL)
		return;

	if (ppMacAddr == NULL || *ppMacAddr == NULL)
		return;

	switch (ProtoType)
	{
		case ETH_P_IPV6:
//			memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS);
			*(*ppMacAddr) = 0x33;
			*(*ppMacAddr + 1) = 0x33;
			*(*ppMacAddr + 2) = pIpAddr[12];
			*(*ppMacAddr + 3) = pIpAddr[13];
			*(*ppMacAddr + 4) = pIpAddr[14];
			*(*ppMacAddr + 5) = pIpAddr[15];
			break;

		case ETH_P_IP:
		default:
//			memset(*ppMacAddr, 0, ETH_LENGTH_OF_ADDRESS);
			*(*ppMacAddr) = 0x01;
			*(*ppMacAddr + 1) = 0x00;
			*(*ppMacAddr + 2) = 0x5e;
			*(*ppMacAddr + 3) = pIpAddr[1] & 0x7f;
			*(*ppMacAddr + 4) = pIpAddr[2];
			*(*ppMacAddr + 5) = pIpAddr[3];
			break;
	}

	return;
}

BOOLEAN RTMPCheckForHang(
	IN  NDIS_HANDLE MiniportAdapterContext
	);

VOID  RTMPHalt(
	IN  NDIS_HANDLE MiniportAdapterContext
	);

//
//  Private routines in rtmp_init.c
//
NDIS_STATUS RTMPAllocAdapterBlock(
	IN PVOID			handle,
	OUT PRTMP_ADAPTER   *ppAdapter
	);

NDIS_STATUS RTMPAllocTxRxRingMemory(
	IN  PRTMP_ADAPTER   pAd
	);

NDIS_STATUS RTMPFindAdapter(
	IN  PRTMP_ADAPTER   pAd,
	IN  NDIS_HANDLE     WrapperConfigurationContext
	);

NDIS_STATUS	RTMPReadParametersHook(
	IN	PRTMP_ADAPTER pAd
	);

VOID RTMPFreeAdapter(
	IN  PRTMP_ADAPTER   pAd
	);

NDIS_STATUS NICReadRegParameters(
	IN  PRTMP_ADAPTER       pAd,
	IN  NDIS_HANDLE         WrapperConfigurationContext
	);

#ifdef RT30xx
VOID NICInitRT30xxRFRegisters(
	IN PRTMP_ADAPTER pAd);
#endif // RT30xx //
	
VOID NICReadEEPROMParameters(
	IN  PRTMP_ADAPTER       pAd,
	IN	PUCHAR				mac_addr);

VOID NICInitAsicFromEEPROM(
	IN  PRTMP_ADAPTER       pAd);

VOID NICInitTxRxRingAndBacklogQueue(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS NICInitializeAdapter(
	IN  PRTMP_ADAPTER   pAd,
	IN   BOOLEAN    bHardReset);

NDIS_STATUS NICInitializeAsic(
	IN  PRTMP_ADAPTER   pAd,
	IN  BOOLEAN		bHardReset);

VOID NICIssueReset(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPRingCleanUp(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           RingType);

VOID RxTest(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS DbgSendPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    pPacket);

VOID UserCfgInit(
	IN  PRTMP_ADAPTER   pAd);

VOID NICResetFromError(
	IN  PRTMP_ADAPTER   pAd);

VOID NICEraseFirmware(
	IN PRTMP_ADAPTER pAd);

NDIS_STATUS NICLoadFirmware(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS NICLoadRateSwitchingParams(
	IN PRTMP_ADAPTER pAd);

BOOLEAN NICCheckForHang(
	IN  PRTMP_ADAPTER   pAd);

VOID NICUpdateFifoStaCounters(
	IN PRTMP_ADAPTER pAd);

VOID NICUpdateRawCounters(
	IN  PRTMP_ADAPTER   pAd);

#if 0
ULONG RTMPEqualMemory(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	IN  ULONG   Length);
#endif

ULONG	RTMPNotAllZero(
	IN	PVOID	pSrc1,
	IN	ULONG	Length);

VOID RTMPZeroMemory(
	IN  PVOID   pSrc,
	IN  ULONG   Length);

ULONG RTMPCompareMemory(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	IN  ULONG   Length);

VOID RTMPMoveMemory(
	OUT PVOID   pDest,
	IN  PVOID   pSrc,
	IN  ULONG   Length);

VOID AtoH(
	char	*src,
	UCHAR	*dest,
	int		destlen);

UCHAR BtoH(
	char ch);

VOID RTMPPatchMacBbpBug(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPPatchCardBus(
	IN	PRTMP_ADAPTER	pAdapter);

VOID RTMPPatchRalinkCardBus(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	ULONG			Bus);

ULONG RTMPReadCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset);

VOID RTMPWriteCBConfig(
	IN	ULONG	Bus,
	IN	ULONG	Slot,
	IN	ULONG	Func,
	IN	ULONG	Offset,
	IN	ULONG	Value);

VOID RTMPInitTimer(
	IN  PRTMP_ADAPTER           pAd,
	IN  PRALINK_TIMER_STRUCT    pTimer,
	IN  PVOID                   pTimerFunc,
	IN	PVOID					pData,
	IN  BOOLEAN                 Repeat);

VOID RTMPSetTimer(
	IN  PRALINK_TIMER_STRUCT    pTimer,
	IN  ULONG                   Value);


VOID RTMPModTimer(
	IN	PRALINK_TIMER_STRUCT	pTimer,
	IN	ULONG					Value);

VOID RTMPCancelTimer(
	IN  PRALINK_TIMER_STRUCT    pTimer,
	OUT BOOLEAN                 *pCancelled);

VOID RTMPSetLED(
	IN PRTMP_ADAPTER 	pAd, 
	IN UCHAR			Status);

VOID RTMPSetSignalLED(
	IN PRTMP_ADAPTER 	pAd, 
	IN NDIS_802_11_RSSI Dbm);

VOID RTMPEnableRxTx(
	IN PRTMP_ADAPTER	pAd);

//
// prototype in action.c
//
VOID ActionStateMachineInit(
    IN	PRTMP_ADAPTER	pAd, 
    IN  STATE_MACHINE *S, 
    OUT STATE_MACHINE_FUNC Trans[]);

VOID MlmeADDBAAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

VOID MlmeDELBAAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

VOID MlmeDLSAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

VOID MlmeInvalidAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

VOID MlmeQOSAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

#ifdef DOT11_N_SUPPORT
VOID PeerAddBAReqAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerAddBARspAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerDelBAAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerBAAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
#endif // DOT11_N_SUPPORT //

VOID SendPSMPAction(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Wcid,
	IN UCHAR			Psmp);
				   
#ifdef CONFIG_AP_SUPPORT				   
VOID SendBeaconRequest(
	IN PRTMP_ADAPTER		pAd,
	IN UCHAR				Wcid);
#endif // CONFIG_AP_SUPPORT //

#ifdef DOT11N_DRAFT3
VOID SendBSS2040CoexistMgmtAction(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	Wcid,
	IN	UCHAR	apidx,
	IN	UCHAR	InfoReq);

VOID SendNotifyBWActionFrame(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR  Wcid,
	IN UCHAR apidx);
	
BOOLEAN ChannelSwitchSanityCheck(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR  Wcid,
	IN    UCHAR  NewChannel,
	IN    UCHAR  Secondary);

VOID ChannelSwitchAction(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR  Wcid,
	IN    UCHAR  Channel,
	IN    UCHAR  Secondary);

ULONG BuildIntolerantChannelRep(
	IN	PRTMP_ADAPTER	pAd,
	IN    PUCHAR  pDest); 

VOID Update2040CoexistFrameAndNotify(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR  Wcid,
	IN	BOOLEAN	bAddIntolerantCha);
	
VOID Send2040CoexistAction(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR  Wcid,
	IN	BOOLEAN	bAddIntolerantCha);
#endif // DOT11N_DRAFT3 //

VOID PeerRMAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerPublicAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);


#ifdef CONFIG_AP_SUPPORT
VOID ApPublicAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
#endif // CONFIG_AP_SUPPORT //

VOID PeerBSSTranAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

#ifdef DOT11_N_SUPPORT
VOID PeerHTAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);
#endif // DOT11_N_SUPPORT //

VOID PeerQOSAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);

#ifdef QOS_DLS_SUPPORT
VOID PeerDLSAction(
    IN PRTMP_ADAPTER pAd, 
    IN MLME_QUEUE_ELEM *Elem);
#endif // QOS_DLS_SUPPORT //


#ifdef DOT11_N_SUPPORT
VOID RECBATimerTimeout(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3);

VOID ORIBATimerTimeout(
	IN	PRTMP_ADAPTER	pAd);

VOID SendRefreshBAR(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry);
#endif // DOT11_N_SUPPORT //
	
VOID ActHeaderInit(
    IN	PRTMP_ADAPTER	pAd, 
    IN OUT PHEADER_802_11 pHdr80211, 
    IN PUCHAR Addr1, 
    IN PUCHAR Addr2,
    IN PUCHAR Addr3);

VOID BarHeaderInit(
	IN	PRTMP_ADAPTER	pAd, 
	IN OUT PFRAME_BAR pCntlBar, 
	IN PUCHAR pDA,
	IN PUCHAR pSA);

VOID InsertActField(
	IN PRTMP_ADAPTER pAd,
	OUT PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN UINT8 Category,
	IN UINT8 ActCode);

BOOLEAN QosBADataParse(
	IN PRTMP_ADAPTER	pAd, 
	IN BOOLEAN bAMSDU,
	IN PUCHAR p8023Header,
	IN UCHAR	WCID,
	IN UCHAR	TID,
	IN USHORT Sequence,
	IN UCHAR DataOffset, 
	IN USHORT Datasize,
	IN UINT   CurRxIndex);

#ifdef DOT11_N_SUPPORT
BOOLEAN CntlEnqueueForRecv(
    IN	PRTMP_ADAPTER	pAd, 
	IN ULONG Wcid, 
    IN ULONG MsgLen, 
	IN PFRAME_BA_REQ pMsg);

VOID BaAutoManSwitch(
	IN	PRTMP_ADAPTER	pAd);
#endif // DOT11_N_SUPPORT //

VOID HTIOTCheck(
	IN	PRTMP_ADAPTER	pAd,
	IN    UCHAR     BatRecIdx);

//
// Private routines in rtmp_data.c
//
BOOLEAN RTMPHandleRxDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPHandleTxDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd);

BOOLEAN RTMPHandleTxRingDmaDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd,
	IN  INT_SOURCE_CSR_STRUC TxRingBitmap);

VOID RTMPHandleMgmtRingDmaDoneInterrupt(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPHandleTBTTInterrupt(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPHandlePreTBTTInterrupt(
	IN  PRTMP_ADAPTER   pAd);

void RTMPHandleTwakeupInterrupt(
	IN PRTMP_ADAPTER pAd);

VOID	RTMPHandleRxCoherentInterrupt(
	IN	PRTMP_ADAPTER	pAd);

BOOLEAN TxFrameIsAggregatible(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pPrevAddr1,
	IN  PUCHAR          p8023hdr);

BOOLEAN PeerIsAggreOn(
    IN  PRTMP_ADAPTER   pAd,
    IN  ULONG          TxRate,
    IN  PMAC_TABLE_ENTRY pMacEntry);

#if 0	// It's not be used
HTTRANSMIT_SETTING  *GetTxMode(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk);
#endif
	
NDIS_STATUS Sniff2BytesFromNdisBuffer(
	IN  PNDIS_BUFFER    pFirstBuffer,
	IN  UCHAR           DesiredOffset,
	OUT PUCHAR          pByte0,
	OUT PUCHAR          pByte1);

NDIS_STATUS STASendPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    pPacket);

VOID STASendPackets(
	IN  NDIS_HANDLE     MiniportAdapterContext,
	IN  PPNDIS_PACKET   ppPacketArray,
	IN  UINT            NumberOfPackets);

VOID RTMPDeQueuePacket(
	IN  PRTMP_ADAPTER   pAd,
   	IN	BOOLEAN			bIntContext,
	IN  UCHAR			QueIdx,
	IN	UCHAR			Max_Tx_Packets);

NDIS_STATUS	RTMPHardTransmit(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET		pPacket,
	IN  UCHAR			QueIdx,
	OUT	PULONG			pFreeTXDLeft);

NDIS_STATUS	STAHardTransmit(
	IN PRTMP_ADAPTER	pAd,
	IN TX_BLK			*pTxBlk,
	IN  UCHAR			QueIdx);

VOID STARxEAPOLFrameIndicate(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

NDIS_STATUS RTMPFreeTXDRequest(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           RingType,
	IN  UCHAR           NumberRequired,
	IN 	PUCHAR          FreeNumberIs);

NDIS_STATUS MlmeHardTransmit(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR	QueIdx,
	IN  PNDIS_PACKET    pPacket);

NDIS_STATUS MlmeHardTransmitMgmtRing(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR	QueIdx,
	IN  PNDIS_PACKET    pPacket);

NDIS_STATUS MlmeHardTransmitTxRing(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR	QueIdx,
	IN  PNDIS_PACKET    pPacket);

USHORT  RTMPCalcDuration(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           Rate,
	IN  ULONG           Size);

VOID RTMPWriteTxWI(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXWI_STRUC		pTxWI,	
	IN  BOOLEAN    		FRAG,	
	IN  BOOLEAN    		CFACK,
	IN  BOOLEAN    		InsTimestamp,
	IN	BOOLEAN			AMPDU,
	IN	BOOLEAN			Ack,
	IN	BOOLEAN			NSeq,		// HW new a sequence.
	IN	UCHAR			BASize,
	IN	UCHAR			WCID,
	IN	ULONG			Length,
	IN  UCHAR      		PID,
	IN	UCHAR			TID,
	IN	UCHAR			TxRate,
	IN	UCHAR			Txopmode,	
	IN	BOOLEAN			CfAck,	
	IN	HTTRANSMIT_SETTING	*pTransmit);


VOID RTMPWriteTxWI_Data(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);

	
VOID RTMPWriteTxWI_Cache(
	IN	PRTMP_ADAPTER		pAd,
	IN	OUT PTXWI_STRUC		pTxWI,
	IN	TX_BLK				*pTxBlk);
	
VOID RTMPWriteTxDescriptor(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXD_STRUC		pTxD,
	IN	BOOLEAN			bWIV,
	IN	UCHAR			QSEL);

VOID RTMPSuspendMsduTransmission(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPResumeMsduTransmission(
	IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS MiniportMMRequest(
	IN  PRTMP_ADAPTER   pAd,
	IN	UCHAR			QueIdx,
	IN	PUCHAR			pData,
	IN  UINT            Length);

NDIS_STATUS MiniportDataMMRequest(
	 IN  PRTMP_ADAPTER   pAd,
	 IN  UCHAR           QueIdx,
	 IN  PUCHAR          pData,
	 IN  UINT            Length);

VOID RTMPSendNullFrame(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           TxRate,
	IN	BOOLEAN			bQosNull);

VOID RTMPSendDisassociationFrame(
	IN	PRTMP_ADAPTER	pAd);

VOID RTMPSendRTSFrame(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pDA,
	IN	unsigned int	NextMpduSize,
	IN  UCHAR           TxRate,
	IN  UCHAR           RTSRate,
	IN  USHORT          AckDuration,
	IN  UCHAR           QueIdx,
	IN  UCHAR			FrameGap);


NDIS_STATUS RTMPApplyPacketFilter(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PRT28XX_RXD_STRUC      pRxD, 
	IN  PHEADER_802_11  pHeader);

PQUEUE_HEADER   RTMPCheckTxSwQueue(
	IN  PRTMP_ADAPTER   pAd,
	OUT UCHAR           *QueIdx);


NDIS_STATUS RTMPCloneNdisPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN	BOOLEAN    pInsAMSDUHdr,
	IN  PNDIS_PACKET    pInPacket,
	OUT PNDIS_PACKET   *ppOutPacket);

NDIS_STATUS RTMPAllocateNdisPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    *pPacket,
	IN  PUCHAR          pHeader,
	IN  UINT            HeaderLen,
	IN  PUCHAR          pData,
	IN  UINT            DataLen);

VOID RTMPFreeNdisPacket(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    pPacket);

BOOLEAN RTMPFreeTXDUponTxDmaDone(
	IN PRTMP_ADAPTER    pAd, 
	IN UCHAR            QueIdx);

BOOLEAN RTMPCheckDHCPFrame(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket);


BOOLEAN RTMPCheckEtherType(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket);


VOID RTMPCckBbpTuning(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UINT			TxRate);

//
// Private routines in rtmp_wep.c
//
VOID RTMPInitWepEngine(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKey,
	IN  UCHAR           KeyId,
	IN  UCHAR           KeyLen, 
	IN  PUCHAR          pDest);

VOID RTMPEncryptData(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDest,
	IN  UINT            Len);

BOOLEAN	RTMPDecryptData(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	PUCHAR			pSrc,
	IN	UINT			Len,
	IN	UINT			idx);

BOOLEAN	RTMPSoftDecryptWEP(
	IN PRTMP_ADAPTER 	pAd,
	IN PUCHAR			pData,
	IN ULONG			DataByteCnt,
	IN PCIPHER_KEY		pGroupKey);

VOID RTMPSetICV(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pDest);

VOID ARCFOUR_INIT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pKey,
	IN  UINT            KeyLen);

UCHAR   ARCFOUR_BYTE(
	IN  PARCFOURCONTEXT     Ctx);

VOID ARCFOUR_DECRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest, 
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

VOID ARCFOUR_ENCRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest,
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

VOID WPAARCFOUR_ENCRYPT(
	IN  PARCFOURCONTEXT Ctx,
	IN  PUCHAR          pDest,
	IN  PUCHAR          pSrc,
	IN  UINT            Len);

UINT RTMP_CALC_FCS32(
	IN  UINT   Fcs,
	IN  PUCHAR  Cp,
	IN  INT     Len);

//
// MLME routines
//

// Asic/RF/BBP related functions

VOID AsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd);

VOID 	AsicUpdateProtect(
	IN		PRTMP_ADAPTER	pAd,
	IN 		USHORT			OperaionMode,
	IN 		UCHAR			SetMask,
	IN		BOOLEAN			bDisableBGProtect,
	IN		BOOLEAN			bNonGFExist);

VOID AsicSwitchChannel(
	IN  PRTMP_ADAPTER   pAd, 
	IN	UCHAR			Channel,
	IN	BOOLEAN			bScan);

VOID AsicLockChannel(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR Channel) ;

VOID AsicAntennaSelect(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR           Channel);

VOID AsicAntennaSetting(
	IN	PRTMP_ADAPTER	pAd,
	IN	ABGBAND_STATE	BandState);

VOID AsicRfTuningExec(
	IN PVOID SystemSpecific1,
	IN PVOID FunctionContext,
	IN PVOID SystemSpecific2,
	IN PVOID SystemSpecific3);


VOID AsicSetBssid(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pBssid);

VOID AsicSetMcastWC(
	IN PRTMP_ADAPTER pAd);

#if 0	// removed by AlbertY
VOID AsicSetBssidWC(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pBssid);
#endif

VOID AsicDelWcidTab(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR	Wcid);

VOID AsicEnableRDG(
	IN PRTMP_ADAPTER pAd);

VOID AsicDisableRDG(
	IN PRTMP_ADAPTER pAd);

VOID AsicDisableSync(
	IN  PRTMP_ADAPTER   pAd);

VOID AsicEnableBssSync(
	IN  PRTMP_ADAPTER   pAd);

VOID AsicEnableIbssSync(
	IN  PRTMP_ADAPTER   pAd);

VOID AsicSetEdcaParm(
	IN PRTMP_ADAPTER pAd,
	IN PEDCA_PARM    pEdcaParm);

VOID AsicSetSlotTime(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bUseShortSlotTime);

#if 0
VOID AsicAddWcidCipherEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR		 WCID,
	IN UCHAR		 BssIndex,
	IN UCHAR		 KeyTable,
	IN UCHAR		 CipherAlg,
	IN PUCHAR		 pAddr,
	IN CIPHER_KEY		 *pCipherKey);
#endif

VOID AsicAddSharedKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         BssIndex,
	IN UCHAR         KeyIdx,
	IN UCHAR         CipherAlg,
	IN PUCHAR        pKey,
	IN PUCHAR        pTxMic,
	IN PUCHAR        pRxMic);

VOID AsicRemoveSharedKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         BssIndex,
	IN UCHAR         KeyIdx);

VOID AsicUpdateWCIDAttribute(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN UCHAR		BssIndex,
	IN UCHAR        CipherAlg,
	IN BOOLEAN		bUsePairewiseKeyTable);

VOID AsicUpdateWCIDIVEIV(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN ULONG        uIV,
	IN ULONG        uEIV);

VOID AsicUpdateRxWCIDTable(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN PUCHAR        pAddr);

VOID AsicAddKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN USHORT		WCID,
	IN UCHAR		BssIndex,
	IN UCHAR		KeyIdx,
	IN PCIPHER_KEY	pCipherKey,	
	IN BOOLEAN		bUsePairewiseKeyTable,
	IN BOOLEAN		bTxKey);

VOID AsicAddPairwiseKeyEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR        pAddr,
	IN UCHAR		WCID,
	IN CIPHER_KEY		 *pCipherKey);

VOID AsicRemovePairwiseKeyEntry(
	IN PRTMP_ADAPTER  pAd,
	IN UCHAR		 BssIdx,
	IN UCHAR		 Wcid);

BOOLEAN AsicSendCommandToMcu(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         Command,
	IN UCHAR         Token,
	IN UCHAR         Arg0,
	IN UCHAR         Arg1);


VOID MacAddrRandomBssid(
	IN  PRTMP_ADAPTER   pAd, 
	OUT PUCHAR pAddr);

VOID MgtMacHeaderInit(
	IN  PRTMP_ADAPTER     pAd, 
	IN OUT PHEADER_802_11 pHdr80211, 
	IN UCHAR SubType, 
	IN UCHAR ToDs, 
	IN PUCHAR pDA, 
	IN PUCHAR pBssid);

VOID MlmeRadioOff(
	IN PRTMP_ADAPTER pAd);

VOID MlmeRadioOn(
	IN PRTMP_ADAPTER pAd);


VOID BssTableInit(
	IN BSS_TABLE *Tab);

#ifdef DOT11_N_SUPPORT
VOID BATableInit(
	IN PRTMP_ADAPTER pAd,
    IN BA_TABLE *Tab);
#endif // DOT11_N_SUPPORT //

ULONG BssTableSearch(
	IN BSS_TABLE *Tab, 
	IN PUCHAR pBssid,
	IN UCHAR Channel);

ULONG BssSsidTableSearch(
	IN BSS_TABLE *Tab, 
	IN PUCHAR    pBssid,
	IN PUCHAR    pSsid,
	IN UCHAR     SsidLen,
	IN UCHAR     Channel);

ULONG BssTableSearchWithSSID(
	IN BSS_TABLE *Tab, 
	IN PUCHAR    Bssid,
	IN PUCHAR    pSsid,
	IN UCHAR     SsidLen,
	IN UCHAR     Channel);

VOID BssTableDeleteEntry(
	IN OUT  PBSS_TABLE pTab, 
	IN      PUCHAR pBssid,
	IN      UCHAR Channel);

#ifdef DOT11_N_SUPPORT
VOID BATableDeleteORIEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		BA_ORI_ENTRY	*pBAORIEntry);

VOID BATableDeleteRECEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		BA_REC_ENTRY	*pBARECEntry);

VOID BATableTearORIEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR TID, 
	IN		UCHAR Wcid, 
	IN		BOOLEAN bForceDelete, 
	IN		BOOLEAN ALL);

VOID BATableTearRECEntry(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR TID, 
	IN		UCHAR WCID, 
	IN		BOOLEAN ALL);
#endif // DOT11_N_SUPPORT //

VOID  BssEntrySet(
	IN  PRTMP_ADAPTER   pAd, 
	OUT PBSS_ENTRY pBss, 
	IN PUCHAR pBssid, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN USHORT BeaconPeriod,
	IN PCF_PARM CfParm, 
	IN USHORT AtimWin, 
	IN USHORT CapabilityInfo, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[], 
	IN UCHAR ExtRateLen,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN ADD_HT_INFO_IE *pAddHtInfo,	// AP might use this additional ht info IE 
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			AddHtInfoLen,
	IN UCHAR			NewExtChanOffset,
	IN UCHAR Channel,
	IN CHAR Rssi,
	IN LARGE_INTEGER TimeStamp,
	IN UCHAR CkipFlag,
	IN PEDCA_PARM pEdcaParm,
	IN PQOS_CAPABILITY_PARM pQosCapability,
	IN PQBSS_LOAD_PARM pQbssLoad,
	IN USHORT LengthVIE,
	IN PNDIS_802_11_VARIABLE_IEs pVIE);

ULONG  BssTableSetEntry(
	IN  PRTMP_ADAPTER   pAd, 
	OUT PBSS_TABLE pTab, 
	IN PUCHAR pBssid, 
	IN CHAR Ssid[], 
	IN UCHAR SsidLen, 
	IN UCHAR BssType, 
	IN USHORT BeaconPeriod, 
	IN CF_PARM *CfParm, 
	IN USHORT AtimWin, 
	IN USHORT CapabilityInfo, 
	IN UCHAR SupRate[], 
	IN UCHAR SupRateLen,
	IN UCHAR ExtRate[],
	IN UCHAR ExtRateLen,
	IN HT_CAPABILITY_IE *pHtCapability,
	IN ADD_HT_INFO_IE *pAddHtInfo,	// AP might use this additional ht info IE 
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			AddHtInfoLen,
	IN UCHAR			NewExtChanOffset,
	IN UCHAR Channel,
	IN CHAR Rssi,
	IN LARGE_INTEGER TimeStamp,
	IN UCHAR CkipFlag,
	IN PEDCA_PARM pEdcaParm,
	IN PQOS_CAPABILITY_PARM pQosCapability,
	IN PQBSS_LOAD_PARM pQbssLoad,
	IN USHORT LengthVIE,
	IN PNDIS_802_11_VARIABLE_IEs pVIE);

#ifdef DOT11_N_SUPPORT
VOID BATableInsertEntry(
    IN	PRTMP_ADAPTER	pAd, 
	IN USHORT Aid,    
    IN USHORT		TimeOutValue,
	IN USHORT		StartingSeq,
    IN UCHAR TID, 
	IN UCHAR BAWinSize, 
	IN UCHAR OriginatorStatus, 
    IN BOOLEAN IsRecipient);

#ifdef DOT11N_DRAFT3
VOID Bss2040CoexistTimeOut(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);


VOID  TriEventInit(
	IN	PRTMP_ADAPTER	pAd);

ULONG TriEventTableSetEntry(
	IN	PRTMP_ADAPTER	pAd, 
	OUT TRIGGER_EVENT_TAB *Tab, 
	IN PUCHAR pBssid, 
	IN HT_CAPABILITY_IE *pHtCapability,
	IN UCHAR			HtCapabilityLen,
	IN UCHAR			RegClass,
	IN UCHAR ChannelNo);

VOID TriEventCounterMaintenance(
	IN	PRTMP_ADAPTER	pAd);
#endif // DOT11N_DRAFT3 //
#endif // DOT11_N_SUPPORT //

VOID BssTableSsidSort(
	IN  PRTMP_ADAPTER   pAd, 
	OUT BSS_TABLE *OutTab, 
	IN  CHAR Ssid[], 
	IN  UCHAR SsidLen);

VOID  BssTableSortByRssi(
	IN OUT BSS_TABLE *OutTab);

VOID BssCipherParse(
	IN OUT  PBSS_ENTRY  pBss);

NDIS_STATUS  MlmeQueueInit(
	IN MLME_QUEUE *Queue);

VOID  MlmeQueueDestroy(
	IN MLME_QUEUE *Queue);

BOOLEAN MlmeEnqueue(
	IN PRTMP_ADAPTER pAd, 
	IN ULONG Machine, 
	IN ULONG MsgType, 
	IN ULONG MsgLen, 
	IN VOID *Msg);

BOOLEAN MlmeEnqueueForRecv(
	IN  PRTMP_ADAPTER   pAd, 
	IN ULONG Wcid, 
	IN ULONG TimeStampHigh, 
	IN ULONG TimeStampLow, 
	IN UCHAR Rssi0, 
	IN UCHAR Rssi1, 
	IN UCHAR Rssi2, 
	IN ULONG MsgLen, 
	IN PVOID Msg,
	IN UCHAR Signal);

#ifdef WSC_INCLUDED
BOOLEAN MlmeEnqueueForWsc(
	IN PRTMP_ADAPTER	pAd,
	IN ULONG eventID,
	IN LONG senderID,
	IN ULONG Machine,
	IN ULONG MsgType,
	IN ULONG MsgLen,
	IN VOID *Msg);
#endif // WSC_INCLUDED //

BOOLEAN MlmeDequeue(
	IN MLME_QUEUE *Queue, 
	OUT MLME_QUEUE_ELEM **Elem);

VOID    MlmeRestartStateMachine(
	IN  PRTMP_ADAPTER   pAd);

BOOLEAN  MlmeQueueEmpty(
	IN MLME_QUEUE *Queue);

BOOLEAN  MlmeQueueFull(
	IN MLME_QUEUE *Queue);

BOOLEAN  MsgTypeSubst(
	IN PRTMP_ADAPTER pAd, 
	IN PFRAME_802_11 pFrame, 
	OUT INT *Machine, 
	OUT INT *MsgType);

VOID StateMachineInit(
	IN STATE_MACHINE *Sm, 
	IN STATE_MACHINE_FUNC Trans[], 
	IN ULONG StNr, 
	IN ULONG MsgNr, 
	IN STATE_MACHINE_FUNC DefFunc, 
	IN ULONG InitState, 
	IN ULONG Base);

VOID StateMachineSetAction(
	IN STATE_MACHINE *S, 
	IN ULONG St, 
	ULONG Msg, 
	IN STATE_MACHINE_FUNC F);

VOID StateMachinePerformAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN STATE_MACHINE *S, 
	IN MLME_QUEUE_ELEM *Elem);

VOID Drop(
	IN  PRTMP_ADAPTER   pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID AssocStateMachineInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN  STATE_MACHINE *Sm, 
	OUT STATE_MACHINE_FUNC Trans[]);

VOID ReassocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID AssocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID DisassocTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

//----------------------------------------------
VOID MlmeDisassocReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID MlmeAssocReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID MlmeReassocReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID MlmeDisassocReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerAssocRspAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerReassocRspAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerDisassocAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID DisassocTimeoutAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID AssocTimeoutAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID  ReassocTimeoutAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID  Cls3errAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr);

VOID SwitchBetweenWepAndCkip(
	IN PRTMP_ADAPTER pAd);

VOID  InvalidStateWhenAssoc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID  InvalidStateWhenReassoc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID InvalidStateWhenDisassociate(
	IN  PRTMP_ADAPTER pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

#ifdef RT2870
VOID MlmeCntlConfirm(
	IN PRTMP_ADAPTER pAd, 
	IN ULONG MsgType, 
	IN USHORT Msg);
#endif // RT2870 //

VOID  ComposePsPoll(
	IN  PRTMP_ADAPTER   pAd);

VOID  ComposeNullFrame(
	IN  PRTMP_ADAPTER pAd);

VOID  AssocPostProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr2, 
	IN  USHORT CapabilityInfo, 
	IN  USHORT Aid, 
	IN  UCHAR SupRate[], 
	IN  UCHAR SupRateLen,
	IN  UCHAR ExtRate[],
	IN  UCHAR ExtRateLen,
	IN PEDCA_PARM pEdcaParm,
	IN HT_CAPABILITY_IE		*pHtCapability,
	IN  UCHAR HtCapabilityLen,
	IN ADD_HT_INFO_IE		*pAddHtInfo);

VOID AuthStateMachineInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN PSTATE_MACHINE sm, 
	OUT STATE_MACHINE_FUNC Trans[]);

VOID AuthTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID MlmeAuthReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerAuthRspAtSeq2Action(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerAuthRspAtSeq4Action(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID AuthTimeoutAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID Cls2errAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PUCHAR pAddr);

VOID MlmeDeauthReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID InvalidStateWhenAuth(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

//=============================================

VOID AuthRspStateMachineInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PSTATE_MACHINE Sm, 
	IN  STATE_MACHINE_FUNC Trans[]);

VOID PeerDeauthAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerAuthSimpleRspGenAndSend(
	IN  PRTMP_ADAPTER   pAd, 
	IN  PHEADER_802_11  pHdr80211, 
	IN  USHORT Alg, 
	IN  USHORT Seq, 
	IN  USHORT Reason, 
	IN  USHORT Status);

//
// Private routines in dls.c
//
#ifdef CONFIG_AP_SUPPORT
#ifdef QOS_DLS_SUPPORT
VOID APDLSStateMachineInit(
    IN	PRTMP_ADAPTER		pAd, 
    IN  STATE_MACHINE		*S, 
    OUT STATE_MACHINE_FUNC	Trans[]);

VOID APPeerDlsReqAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);

VOID APPeerDlsRspAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);

VOID APPeerDlsTearDownAction(
    IN PRTMP_ADAPTER	pAd, 
    IN MLME_QUEUE_ELEM	*Elem);
#endif // QOS_DLS_SUPPORT //
#endif // CONFIG_AP_SUPPORT //


#ifdef QOS_DLS_SUPPORT
BOOLEAN PeerDlsReqSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pCapabilityInfo, 
    OUT USHORT *pDlsTimeout,
    OUT UCHAR *pRatesLen,
    OUT UCHAR Rates[],
    OUT UCHAR *pHtCapabilityLen,
    OUT HT_CAPABILITY_IE *pHtCapability);

BOOLEAN PeerDlsRspSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pCapabilityInfo, 
    OUT USHORT *pStatus,
    OUT UCHAR *pRatesLen,
    OUT UCHAR Rates[],
    OUT UCHAR *pHtCapabilityLen,
    OUT HT_CAPABILITY_IE *pHtCapability);

BOOLEAN PeerDlsTearDownSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen,
    OUT PUCHAR pDA,
    OUT PUCHAR pSA,
    OUT USHORT *pReason);
#endif // QOS_DLS_SUPPORT //

//========================================

VOID SyncStateMachineInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN  STATE_MACHINE *Sm, 
	OUT STATE_MACHINE_FUNC Trans[]);

VOID BeaconTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID ScanTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID MlmeScanReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID InvalidStateWhenScan(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID InvalidStateWhenJoin(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID InvalidStateWhenStart(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerBeacon(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID EnqueueProbeRequest(
	IN PRTMP_ADAPTER pAd);

BOOLEAN ScanRunning(
		IN PRTMP_ADAPTER pAd);
//=========================================

VOID MlmeCntlInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN  STATE_MACHINE *S, 
	OUT STATE_MACHINE_FUNC Trans[]);

VOID MlmeCntlMachinePerformAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  STATE_MACHINE *S, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlIdleProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlOidScanProc(
	IN  PRTMP_ADAPTER pAd,
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlOidSsidProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM * Elem);

VOID CntlOidRTBssidProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM * Elem);

VOID CntlMlmeRoamingProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM * Elem);

VOID CntlWaitDisassocProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitJoinProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitReassocProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitStartProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitAuthProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitAuthProc2(
	IN  PRTMP_ADAPTER pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID CntlWaitAssocProc(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

#ifdef QOS_DLS_SUPPORT
VOID CntlOidDLSSetupProc(
	IN PRTMP_ADAPTER pAd,
	IN MLME_QUEUE_ELEM *Elem);
#endif // QOS_DLS_SUPPORT //

VOID LinkUp(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR BssType);

VOID LinkDown(
	IN  PRTMP_ADAPTER   pAd,
	IN  BOOLEAN         IsReqFromAP);

VOID IterateOnBssTab(
	IN  PRTMP_ADAPTER   pAd);

VOID IterateOnBssTab2(
	IN  PRTMP_ADAPTER   pAd);;

VOID JoinParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN  OUT MLME_JOIN_REQ_STRUCT *JoinReq, 
	IN  ULONG BssIdx);

VOID AssocParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN OUT MLME_ASSOC_REQ_STRUCT *AssocReq, 
	IN  PUCHAR pAddr, 
	IN  USHORT CapabilityInfo, 
	IN  ULONG Timeout, 
	IN  USHORT ListenIntv);

VOID ScanParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN  OUT MLME_SCAN_REQ_STRUCT *ScanReq, 
	IN  CHAR Ssid[], 
	IN  UCHAR SsidLen, 
	IN  UCHAR BssType, 
	IN  UCHAR ScanType); 

VOID DisassocParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN  OUT MLME_DISASSOC_REQ_STRUCT *DisassocReq, 
	IN  PUCHAR pAddr, 
	IN  USHORT Reason);

VOID StartParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN  OUT MLME_START_REQ_STRUCT *StartReq, 
	IN  CHAR Ssid[], 
	IN  UCHAR SsidLen);

VOID AuthParmFill(
	IN  PRTMP_ADAPTER   pAd, 
	IN  OUT MLME_AUTH_REQ_STRUCT *AuthReq, 
	IN  PUCHAR pAddr, 
	IN  USHORT Alg);

VOID EnqueuePsPoll(
	IN  PRTMP_ADAPTER   pAd);

VOID EnqueueBeaconFrame(
	IN  PRTMP_ADAPTER   pAd); 

VOID MlmeJoinReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID MlmeScanReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID MlmeStartReqAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID ScanTimeoutAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID BeaconTimeoutAtJoinAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerBeaconAtScanAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerBeaconAtJoinAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerBeacon(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID PeerProbeReqAction(
	IN  PRTMP_ADAPTER pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID ScanNextChannel(
	IN  PRTMP_ADAPTER   pAd);

ULONG MakeIbssBeacon(
	IN  PRTMP_ADAPTER   pAd);

VOID CCXAdjacentAPReport(
	IN  PRTMP_ADAPTER   pAd);

BOOLEAN MlmeScanReqSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT UCHAR *BssType, 
	OUT CHAR ssid[], 
	OUT UCHAR *SsidLen, 
	OUT UCHAR *ScanType);

BOOLEAN PeerBeaconAndProbeRspSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	IN  UCHAR MsgChannel,
	OUT PUCHAR pAddr2, 
	OUT PUCHAR pBssid, 
	OUT CHAR Ssid[], 
	OUT UCHAR *pSsidLen, 
	OUT UCHAR *pBssType, 
	OUT USHORT *pBeaconPeriod, 
	OUT UCHAR *pChannel, 
	OUT UCHAR *pNewChannel, 
	OUT LARGE_INTEGER *pTimestamp, 
	OUT CF_PARM *pCfParm, 
	OUT USHORT *pAtimWin, 
	OUT USHORT *pCapabilityInfo, 
	OUT UCHAR *pErp,
	OUT UCHAR *pDtimCount, 
	OUT UCHAR *pDtimPeriod, 
	OUT UCHAR *pBcastFlag, 
	OUT UCHAR *pMessageToMe, 
	OUT UCHAR SupRate[],
	OUT UCHAR *pSupRateLen,
	OUT UCHAR ExtRate[],
	OUT UCHAR *pExtRateLen,
	OUT	UCHAR *pCkipFlag,
	OUT	UCHAR *pAironetCellPowerLimit,
	OUT PEDCA_PARM       pEdcaParm,
	OUT PQBSS_LOAD_PARM  pQbssLoad,
	OUT PQOS_CAPABILITY_PARM pQosCapability,
	OUT ULONG *pRalinkIe,
	OUT UCHAR		 *pHtCapabilityLen,
	OUT HT_CAPABILITY_IE *pHtCapability,
	OUT UCHAR		 *AddHtInfoLen,
	OUT ADD_HT_INFO_IE *AddHtInfo,
	OUT UCHAR *NewExtChannel,
	OUT USHORT *LengthVIE,
	OUT PNDIS_802_11_VARIABLE_IEs pVIE);

BOOLEAN PeerAddBAReqActionSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen,
	OUT PUCHAR pAddr2);

BOOLEAN PeerAddBARspActionSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *pMsg, 
    IN ULONG MsgLen);

BOOLEAN PeerDelBAActionSanity(
    IN PRTMP_ADAPTER pAd, 
    IN UCHAR Wcid, 
    IN VOID *pMsg, 
    IN ULONG MsgLen);

BOOLEAN MlmeAssocReqSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pApAddr, 
	OUT USHORT *CapabilityInfo, 
	OUT ULONG *Timeout, 
	OUT USHORT *ListenIntv);

BOOLEAN MlmeAuthReqSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr, 
	OUT ULONG *Timeout, 
	OUT USHORT *Alg);

BOOLEAN MlmeStartReqSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT CHAR Ssid[], 
	OUT UCHAR *Ssidlen);

BOOLEAN PeerAuthSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr, 
	OUT USHORT *Alg, 
	OUT USHORT *Seq, 
	OUT USHORT *Status, 
	OUT CHAR ChlgText[]);

BOOLEAN PeerAssocRspSanity(
	IN  PRTMP_ADAPTER   pAd, 
    IN VOID *pMsg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr2, 
	OUT USHORT *pCapabilityInfo, 
	OUT USHORT *pStatus, 
	OUT USHORT *pAid, 
	OUT UCHAR SupRate[], 
	OUT UCHAR *pSupRateLen,
	OUT UCHAR ExtRate[],
	OUT UCHAR *pExtRateLen,
    OUT HT_CAPABILITY_IE		*pHtCapability,
    OUT ADD_HT_INFO_IE		*pAddHtInfo,	// AP might use this additional ht info IE 
    OUT UCHAR			*pHtCapabilityLen,
    OUT UCHAR			*pAddHtInfoLen,
    OUT UCHAR			*pNewExtChannelOffset,
	OUT PEDCA_PARM pEdcaParm,
	OUT UCHAR *pCkipFlag);

BOOLEAN PeerDisassocSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr2, 
	OUT USHORT *Reason);

BOOLEAN PeerWpaMessageSanity(
    IN 	PRTMP_ADAPTER 		pAd, 
    IN 	PEAPOL_PACKET 		pMsg, 
    IN 	ULONG 				MsgLen, 
    IN 	UCHAR				MsgType,
    IN 	MAC_TABLE_ENTRY  	*pEntry);

BOOLEAN PeerDeauthSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr2, 
	OUT USHORT *Reason);

BOOLEAN PeerProbeReqSanity(
	IN  PRTMP_ADAPTER   pAd, 
	IN  VOID *Msg, 
	IN  ULONG MsgLen, 
	OUT PUCHAR pAddr2,
	OUT CHAR Ssid[], 
	OUT UCHAR *pSsidLen);

BOOLEAN GetTimBit(
	IN  CHAR *Ptr, 
	IN  USHORT Aid, 
	OUT UCHAR *TimLen, 
	OUT UCHAR *BcastFlag, 
	OUT UCHAR *DtimCount, 
	OUT UCHAR *DtimPeriod, 
	OUT UCHAR *MessageToMe);

UCHAR ChannelSanity(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR channel);

NDIS_802_11_NETWORK_TYPE NetworkTypeInUseSanity(
	IN PBSS_ENTRY pBss);

#if 0	// It's omitted
NDIS_STATUS	RTMPWepKeySanity(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	PVOID			pBuf);
#endif

BOOLEAN MlmeDelBAReqSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen);

BOOLEAN MlmeAddBAReqSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2);

ULONG MakeOutgoingFrame(
	OUT CHAR *Buffer, 
	OUT ULONG *Length, ...);

VOID  LfsrInit(
	IN  PRTMP_ADAPTER   pAd, 
	IN  ULONG Seed);

UCHAR RandomByte(
	IN  PRTMP_ADAPTER   pAd);

VOID AsicUpdateAutoFallBackTable(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			pTxRate);

VOID  MlmePeriodicExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID LinkDownExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID LinkUpExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID STAMlmePeriodicExec(
	PRTMP_ADAPTER pAd);

VOID MlmeAutoScan(
	IN PRTMP_ADAPTER pAd);

VOID MlmeAutoReconnectLastSSID(
	IN PRTMP_ADAPTER pAd);

BOOLEAN MlmeValidateSSID(
	IN PUCHAR pSsid,
	IN UCHAR  SsidLen); 

VOID MlmeCheckForRoaming(
	IN PRTMP_ADAPTER pAd,
	IN ULONG    Now32);

VOID MlmeCheckForFastRoaming(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG           Now);

VOID MlmeDynamicTxRateSwitching(
	IN PRTMP_ADAPTER pAd);

VOID MlmeSetTxRate(
	IN PRTMP_ADAPTER		pAd,
	IN PMAC_TABLE_ENTRY		pEntry,
	IN PRTMP_TX_RATE_SWITCH	pTxRate);

VOID MlmeSelectTxRateTable(
	IN PRTMP_ADAPTER		pAd,
	IN PMAC_TABLE_ENTRY		pEntry,
	IN PUCHAR				*ppTable,
	IN PUCHAR				pTableSize,
	IN PUCHAR				pInitTxRateIdx);

VOID MlmeCalculateChannelQuality(
	IN PRTMP_ADAPTER pAd,
	IN ULONG Now);

VOID MlmeCheckPsmChange(
	IN PRTMP_ADAPTER pAd,
	IN ULONG    Now32);

VOID MlmeSetPsmBit(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT psm);

VOID MlmeSetTxPreamble(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TxPreamble);

VOID UpdateBasicRateBitmap(
	IN	PRTMP_ADAPTER	pAd);

VOID MlmeUpdateTxRates(
	IN PRTMP_ADAPTER 	pAd,
	IN 	BOOLEAN		 	bLinkUp,
	IN	UCHAR			apidx);

#ifdef DOT11_N_SUPPORT
VOID MlmeUpdateHtTxRates(
	IN PRTMP_ADAPTER 		pAd,
	IN	UCHAR				apidx);
#endif // DOT11_N_SUPPORT //

VOID    RTMPCheckRates(
	IN      PRTMP_ADAPTER   pAd,
	IN OUT  UCHAR           SupRate[],
	IN OUT  UCHAR           *SupRateLen);


BOOLEAN 	RTMPCheckHt(
	IN		PRTMP_ADAPTER	pAd,
	IN		UCHAR	Wcid,
	IN OUT	HT_CAPABILITY_IE			*pHtCapability,
	IN OUT	ADD_HT_INFO_IE			*pAddHtInfo);

VOID StaQuickResponeForRateUpExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID AsicBbpTuning1(
	IN PRTMP_ADAPTER pAd);

VOID AsicBbpTuning2(
	IN PRTMP_ADAPTER pAd);

VOID RTMPUpdateMlmeRate(
	IN PRTMP_ADAPTER	pAd);

CHAR RTMPMaxRssi(
	IN PRTMP_ADAPTER	pAd,
	IN CHAR				Rssi0,
	IN CHAR				Rssi1,
	IN CHAR				Rssi2);

VOID AsicSetRxAnt(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Ant);

VOID AsicEvaluateRxAnt(
	IN PRTMP_ADAPTER	pAd);

VOID AsicRxAntEvalTimeout(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID APSDPeriodicExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

BOOLEAN RTMPCheckEntryEnableAutoRateSwitch(
	IN PRTMP_ADAPTER    pAd,
	IN PMAC_TABLE_ENTRY	pEntry);

UCHAR RTMPStaFixedTxMode(
	IN PRTMP_ADAPTER    pAd,
	IN PMAC_TABLE_ENTRY	pEntry);

VOID RTMPUpdateLegacyTxSetting(
		UCHAR				fixed_tx_mode,
		PMAC_TABLE_ENTRY	pEntry);

BOOLEAN RTMPAutoRateSwitchCheck(
	IN PRTMP_ADAPTER    pAd);

NDIS_STATUS MlmeInit(
	IN  PRTMP_ADAPTER   pAd);

VOID MlmeHandler(
	IN  PRTMP_ADAPTER   pAd);

VOID MlmeHalt(
	IN  PRTMP_ADAPTER   pAd);

VOID MlmeResetRalinkCounters(
	IN  PRTMP_ADAPTER   pAd);

VOID BuildChannelList(
	IN PRTMP_ADAPTER pAd);

UCHAR FirstChannel(
	IN  PRTMP_ADAPTER   pAd);

UCHAR NextChannel(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR channel);

VOID ChangeToCellPowerLimit(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR         AironetCellPowerLimit);

VOID RaiseClock(
	IN  PRTMP_ADAPTER   pAd,
	IN  UINT32 *x);

VOID LowerClock(
	IN  PRTMP_ADAPTER   pAd,
	IN  UINT32 *x);

USHORT ShiftInBits(
	IN  PRTMP_ADAPTER   pAd);

VOID ShiftOutBits(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT data,
	IN  USHORT count);

VOID EEpromCleanup(
	IN  PRTMP_ADAPTER   pAd);

VOID EWDS(
	IN  PRTMP_ADAPTER   pAd);

VOID EWEN(
	IN  PRTMP_ADAPTER   pAd);

USHORT RTMP_EEPROM_READ16(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT Offset);

VOID RTMP_EEPROM_WRITE16(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT Offset,
	IN  USHORT Data);

//
// Prototypes of function definition in rtmp_tkip.c
//
VOID    RTMPInitTkipEngine(
	IN  PRTMP_ADAPTER   pAd,    
	IN  PUCHAR          pTKey,
	IN  UCHAR           KeyId,
	IN  PUCHAR          pTA,
	IN  PUCHAR          pMICKey,
	IN  PUCHAR          pTSC,
	OUT PULONG          pIV16,
	OUT PULONG          pIV32);

VOID    RTMPInitMICEngine(
	IN  PRTMP_ADAPTER   pAd,    
	IN  PUCHAR          pKey,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  UCHAR           UserPriority,
	IN  PUCHAR          pMICKey);

BOOLEAN RTMPTkipCompareMICValue(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  PUCHAR          pMICKey,
	IN	UCHAR			UserPriority,
	IN  UINT            Len);

VOID    RTMPCalculateMICValue(
	IN  PRTMP_ADAPTER   pAd,
	IN  PNDIS_PACKET    pPacket,
	IN  PUCHAR          pEncap,
	IN  PCIPHER_KEY     pKey,
	IN	UCHAR			apidx);

BOOLEAN RTMPTkipCompareMICValueWithLLC(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pLLC,
	IN  PUCHAR          pSrc,
	IN  PUCHAR          pDA,
	IN  PUCHAR          pSA,
	IN  PUCHAR          pMICKey,
	IN  UINT            Len);

VOID    RTMPTkipAppendByte( 
	IN  PTKIP_KEY_INFO  pTkip,  
	IN  UCHAR           uChar);

VOID    RTMPTkipAppend( 
	IN  PTKIP_KEY_INFO  pTkip,  
	IN  PUCHAR          pSrc,
	IN  UINT            nBytes);

VOID    RTMPTkipGetMIC( 
	IN  PTKIP_KEY_INFO  pTkip);

BOOLEAN RTMPSoftDecryptTKIP(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN UCHAR    UserPriority,
	IN PCIPHER_KEY	pWpaKey);

BOOLEAN RTMPSoftDecryptAES(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR	pData,
	IN ULONG	DataByteCnt, 
	IN PCIPHER_KEY	pWpaKey);

#if 0	// removed by AlbertY
NDIS_STATUS RTMPWPAAddKeyProc(
	IN  PRTMP_ADAPTER   pAd,
	IN  PVOID           pBuf);
#endif

//
// Prototypes of function definition in cmm_info.c
//
NDIS_STATUS RTMPWPARemoveKeyProc(
	IN  PRTMP_ADAPTER   pAd,
	IN  PVOID           pBuf);

VOID    RTMPWPARemoveAllKeys(
	IN  PRTMP_ADAPTER   pAd);

BOOLEAN RTMPCheckStrPrintAble(
    IN  CHAR *pInPutStr, 
    IN  UCHAR strLen);
    
VOID    RTMPSetPhyMode(
	IN  PRTMP_ADAPTER   pAd,
	IN  ULONG phymode);

VOID	RTMPUpdateHTIE(
	IN	RT_HT_CAPABILITY	*pRtHt,
	IN		UCHAR				*pMcsSet,
	OUT		HT_CAPABILITY_IE *pHtCapability,
	OUT		ADD_HT_INFO_IE		*pAddHtInfo);

VOID	RTMPAddWcidAttributeEntry(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BssIdx,
	IN 	UCHAR		 	KeyIdx,
	IN 	UCHAR		 	CipherAlg,
	IN 	MAC_TABLE_ENTRY *pEntry);

CHAR *GetEncryptType(
	CHAR enc);

CHAR *GetAuthMode(
	CHAR auth);

VOID RTMPIoctlGetSiteSurvey(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq);

VOID RTMPIoctlGetMacTable(
	IN PRTMP_ADAPTER pAd, 
	IN struct iwreq *wrq);

VOID RTMPIndicateWPA2Status(
	IN  PRTMP_ADAPTER  pAdapter);

VOID	RTMPOPModeSwitching(
	IN	PRTMP_ADAPTER	pAd);


#ifdef DOT11_N_SUPPORT
VOID	RTMPSetHT(
	IN	PRTMP_ADAPTER	pAd,
	IN	OID_SET_HT_PHYMODE *pHTPhyMode);

VOID	RTMPSetIndividualHT(
	IN	PRTMP_ADAPTER		pAd,
	IN	UCHAR				apidx);
#endif // DOT11_N_SUPPORT //

VOID RTMPSendWirelessEvent(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Event_flag,
	IN	PUCHAR 			pAddr,
	IN  UCHAR			BssIdx,
	IN	CHAR			Rssi);

VOID	NICUpdateCntlCounters(
	IN	PRTMP_ADAPTER	pAd,
	IN	PHEADER_802_11	pHeader,
	IN    UCHAR			SubType,
	IN	PRXWI_STRUC 	pRxWI);
//
// prototype in wpa.c
//
BOOLEAN WpaMsgTypeSubst(
	IN  UCHAR   EAPType,
	OUT INT		*MsgType);

VOID WpaPskStateMachineInit(
	IN  PRTMP_ADAPTER       pAd, 
	IN  STATE_MACHINE       *S, 
	OUT STATE_MACHINE_FUNC Trans[]);

VOID WpaEAPOLKeyAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    WpaPairMsg1Action(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    WpaPairMsg3Action(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem); 

VOID    WpaGroupMsg1Action(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    WpaMacHeaderInit(
	IN      PRTMP_ADAPTER   pAd, 
	IN OUT  PHEADER_802_11  pHdr80211, 
	IN      UCHAR           wep, 
	IN      PUCHAR          pAddr1); 

VOID    Wpa2PairMsg1Action(
    IN  PRTMP_ADAPTER   pAd, 
    IN  MLME_QUEUE_ELEM *Elem);

VOID    Wpa2PairMsg3Action(
    IN  PRTMP_ADAPTER   pAd, 
    IN  MLME_QUEUE_ELEM *Elem);

BOOLEAN ParseKeyData(
    IN  PRTMP_ADAPTER   pAd,
    IN  PUCHAR          pKeyData,
    IN  UCHAR           KeyDataLen,
	IN	UCHAR			bPairewise);

VOID    RTMPToWirelessSta(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pHeader802_3,
    IN  UINT            HdrLen,
	IN  PUCHAR          pData,
    IN  UINT            DataLen,
    IN	BOOLEAN			is4wayFrame);

/*
VOID    HMAC_SHA1(
	IN  UCHAR   *text,
	IN  UINT    text_len,
	IN  UCHAR   *key,
	IN  UINT    key_len,
	IN  UCHAR   *digest);
*/

VOID    PRF(
	IN  UCHAR   *key,
	IN  INT     key_len,
	IN  UCHAR   *prefix,
	IN  INT     prefix_len,
	IN  UCHAR   *data,
	IN  INT     data_len,
	OUT UCHAR   *output,
	IN  INT     len);

VOID    CCKMPRF(
	IN  UCHAR   *key,
	IN  INT     key_len,
	IN  UCHAR   *data,
	IN  INT     data_len,
	OUT UCHAR   *output,
	IN  INT     len);

VOID WpaCountPTK(
	IN  PRTMP_ADAPTER   pAd,
	IN  UCHAR   *PMK,
	IN  UCHAR   *ANonce,
	IN  UCHAR   *AA,
	IN  UCHAR   *SNonce,
	IN  UCHAR   *SA,
	OUT UCHAR   *output,
	IN  UINT    len);

VOID    GenRandom(
	IN  PRTMP_ADAPTER   pAd, 
	IN	UCHAR			*macAddr,
	OUT	UCHAR			*random);

//
// prototype in aironet.c
//
VOID    AironetStateMachineInit(
	IN  PRTMP_ADAPTER       pAd, 
	IN  STATE_MACHINE       *S, 
	OUT STATE_MACHINE_FUNC  Trans[]);

VOID    AironetMsgAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    AironetRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    ChannelLoadRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    NoiseHistRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    BeaconRequestAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    AironetReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MLME_QUEUE_ELEM *Elem);

VOID    ChannelLoadReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    NoiseHistReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index);

VOID    AironetFinalReportAction(
	IN  PRTMP_ADAPTER   pAd);

VOID    BeaconReportAction(
	IN  PRTMP_ADAPTER   pAd, 
	IN  UCHAR           Index); 

VOID    AironetAddBeaconReport(
	IN  PRTMP_ADAPTER       pAd,
	IN  ULONG               Index,
	IN  PMLME_QUEUE_ELEM    pElem);

VOID    AironetCreateBeaconReportFromBssTable(
	IN  PRTMP_ADAPTER       pAd);

VOID    DBGPRINT_TX_RING(
	IN PRTMP_ADAPTER  pAd,
	IN UCHAR          QueIdx);

VOID DBGPRINT_RX_RING(
	IN PRTMP_ADAPTER  pAd);

CHAR    ConvertToRssi(
	IN PRTMP_ADAPTER  pAd,
	IN CHAR				Rssi,
	IN UCHAR    RssiNumber);


#ifdef DOT11N_DRAFT3
VOID BuildEffectedChannelList(
	IN PRTMP_ADAPTER pAd);
#endif // DOT11N_DRAFT3 //


VOID APAsicEvaluateRxAnt(
	IN PRTMP_ADAPTER	pAd);


VOID APAsicRxAntEvalTimeout(
	IN PRTMP_ADAPTER	pAd);

// 
// function prototype in cmm_wpa.c
//
BOOLEAN RTMPCheckWPAframe(
	IN PRTMP_ADAPTER pAd,
	IN PMAC_TABLE_ENTRY	pEntry,
	IN PUCHAR 			pData,
	IN ULONG 			DataByteCount,	
	IN UCHAR			FromWhichBSSID);

VOID AES_GTK_KEY_UNWRAP( 
	IN  UCHAR   *key,
	OUT UCHAR   *plaintext,
	IN	UCHAR	c_len,
	IN  UCHAR   *ciphertext);

BOOLEAN RTMPCheckRSNIE(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pData,
	IN  UCHAR           DataLen,
	IN  MAC_TABLE_ENTRY *pEntry,
	OUT	UCHAR			*Offset);

BOOLEAN RTMPParseEapolKeyData(
	IN  PRTMP_ADAPTER   pAd,
	IN  PUCHAR          pKeyData,
	IN  UCHAR           KeyDataLen,
	IN	UCHAR			GroupKeyIndex,
	IN	UCHAR			MsgType,
	IN	BOOLEAN			bWPA2,
	IN  MAC_TABLE_ENTRY *pEntry);

VOID	ConstructEapolMsg(
	IN 	PRTMP_ADAPTER    	pAd,
    IN 	UCHAR				PeerAuthMode,
    IN 	UCHAR				PeerWepStatus,
    IN 	UCHAR				MyGroupKeyWepStatus,
    IN 	UCHAR				MsgType,  
    IN	UCHAR				DefaultKeyIdx,
    IN 	UCHAR				*ReplayCounter,
	IN 	UCHAR				*KeyNonce,
	IN	UCHAR				*TxRSC,
	IN	UCHAR				*PTK,
	IN	UCHAR				*GTK,
	IN	UCHAR				*RSNIE,
	IN	UCHAR				RSNIE_Len,
    OUT PEAPOL_PACKET       pMsg);

VOID	CalculateMIC(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			PeerWepStatus,
	IN	UCHAR			*PTK,
	OUT PEAPOL_PACKET   pMsg);

NDIS_STATUS	RTMPSoftDecryptBroadCastData(
	IN	PRTMP_ADAPTER					pAd,
	IN	RX_BLK							*pRxBlk,
	IN  NDIS_802_11_ENCRYPTION_STATUS 	GroupCipher,
	IN  PCIPHER_KEY						pShard_key);

VOID	ConstructEapolKeyData(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			PeerAuthMode,
	IN	UCHAR			PeerWepStatus,
	IN	UCHAR			GroupKeyWepStatus,
	IN 	UCHAR			MsgType,
	IN	UCHAR			DefaultKeyIdx,
	IN	BOOLEAN			bWPA2Capable,
	IN	UCHAR			*PTK,
	IN	UCHAR			*GTK,
	IN	UCHAR			*RSNIE,
	IN	UCHAR			RSNIE_LEN,
	OUT PEAPOL_PACKET   pMsg);

VOID RTMPMakeRSNIE(
	IN  PRTMP_ADAPTER   pAd,
	IN  UINT            AuthMode,
	IN  UINT            WepStatus,
	IN	UCHAR			apidx);

// 
// function prototype in ap_wpa.c
//

BOOLEAN APWpaMsgTypeSubst(
	IN UCHAR    EAPType,
	OUT INT *MsgType) ;

MAC_TABLE_ENTRY *PACInquiry(
	IN  PRTMP_ADAPTER   pAd, 
	IN  ULONG           Wcid);

BOOLEAN RTMPCheckMcast(
	IN PRTMP_ADAPTER pAd,
	IN PEID_STRUCT      eid_ptr,
	IN MAC_TABLE_ENTRY  *pEntry);

BOOLEAN RTMPCheckUcast(
	IN PRTMP_ADAPTER pAd,
	IN PEID_STRUCT      eid_ptr,
	IN MAC_TABLE_ENTRY  *pEntry);

BOOLEAN RTMPCheckAUTH(
	IN PRTMP_ADAPTER pAd,
	IN PEID_STRUCT      eid_ptr,
	IN MAC_TABLE_ENTRY  *pEntry);

VOID WPAStart4WayHS(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MAC_TABLE_ENTRY *pEntry,
	IN	ULONG			TimeInterval);

VOID WPAStart2WayGroupHS(
	IN  PRTMP_ADAPTER   pAd, 
	IN  MAC_TABLE_ENTRY *pEntry);

VOID APWpaEAPPacketAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLStartAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLLogoffAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLKeyAction(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM *Elem);

VOID APWpaEAPOLASFAlertAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  MLME_QUEUE_ELEM  *Elem);

VOID HandleCounterMeasure(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry);

VOID PeerPairMsg2Action(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

VOID PeerPairMsg4Action(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

VOID CMTimerExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID WPARetryExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID EnqueueStartForPSKExec(
    IN PVOID SystemSpecific1, 
    IN PVOID FunctionContext, 
    IN PVOID SystemSpecific2, 
    IN PVOID SystemSpecific3); 

VOID RTMPHandleSTAKey(
    IN PRTMP_ADAPTER    pAdapter, 
    IN MAC_TABLE_ENTRY  *pEntry,
    IN MLME_QUEUE_ELEM  *Elem);

#if 0 // merge into PeerPairMsg4Action
VOID Wpa1PeerPairMsg4Action(
	IN PRTMP_ADAPTER pAd, 
	IN MAC_TABLE_ENTRY  *pEntry,
	IN MLME_QUEUE_ELEM *Elem);

VOID Wpa2PeerPairMsg4Action(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  MLME_QUEUE_ELEM  *Elem);
#endif // 0 //

VOID PeerGroupMsg2Action(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  VOID             *Msg,
	IN  UINT             MsgLen);

#if 0	// replaced by WPAStart2WayGroupHS
NDIS_STATUS APWpaHardTransmit(
	IN  PRTMP_ADAPTER    pAd,
	IN  PMAC_TABLE_ENTRY pEntry);
#endif // 0 //

VOID PairDisAssocAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  USHORT           Reason);

VOID MlmeDeAuthAction(
	IN  PRTMP_ADAPTER    pAd, 
	IN  PMAC_TABLE_ENTRY pEntry,
	IN  USHORT           Reason);

VOID GREKEYPeriodicExec(
	IN  PVOID   SystemSpecific1, 
	IN  PVOID   FunctionContext, 
	IN  PVOID   SystemSpecific2, 
	IN  PVOID   SystemSpecific3);

VOID CountGTK(
	IN  UCHAR   *PMK,
	IN  UCHAR   *GNonce,
	IN  UCHAR   *AA,
	OUT UCHAR   *output,
	IN  UINT    len);

VOID    GetSmall(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	OUT PUCHAR  out,
	IN  ULONG   Length);

VOID    GetLarge(
	IN  PVOID   pSrc1,
	IN  PVOID   pSrc2,
	OUT PUCHAR  out,
	IN  ULONG   Length);

VOID APGenRandom(
	IN PRTMP_ADAPTER pAd, 
	OUT UCHAR       *random);

VOID AES_GTK_KEY_WRAP( 
	IN UCHAR *key,
	IN UCHAR *plaintext,
	IN UCHAR p_len,
	OUT UCHAR *ciphertext);

VOID    WpaSend(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  PUCHAR          pPacket,
    IN  ULONG           Len);

VOID    APToWirelessSta(
	IN  PRTMP_ADAPTER   pAd,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN  PUCHAR          pHeader802_3,
	IN  UINT            HdrLen,
	IN  PUCHAR          pData,
	IN  UINT            DataLen,
    IN	BOOLEAN			bClearFrame);

VOID RTMPAddPMKIDCache(
	IN  PRTMP_ADAPTER   		pAd,
	IN	INT						apidx,
	IN	PUCHAR				pAddr,
	IN	UCHAR					*PMKID,
	IN	UCHAR					*PMK);

INT RTMPSearchPMKIDCache(
	IN  PRTMP_ADAPTER   pAd,
	IN	INT				apidx,
	IN	PUCHAR		pAddr);

VOID RTMPDeletePMKIDCache(
	IN  PRTMP_ADAPTER   pAd,
	IN	INT				apidx,
	IN  INT				idx);

VOID RTMPMaintainPMKIDCache(
	IN  PRTMP_ADAPTER   pAd);

VOID	RTMPSendTriggerFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	PVOID			pBuffer,
	IN	ULONG			Length,
	IN  UCHAR           TxRate,
	IN	BOOLEAN			bQosNull);

#ifdef RT30xx
VOID RTMPFilterCalibration(
	IN PRTMP_ADAPTER pAd);
#endif // RT30xx //


//typedef void (*TIMER_FUNCTION)(unsigned long);


/* timeout -- ms */
VOID RTMP_SetPeriodicTimer(
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	unsigned long timeout);

VOID RTMP_OS_Init_Timer(
	IN	PRTMP_ADAPTER pAd, 
	IN	NDIS_MINIPORT_TIMER *pTimer, 
	IN	TIMER_FUNCTION function,
	IN	PVOID data);

VOID RTMP_OS_Add_Timer(
	IN	NDIS_MINIPORT_TIMER	*pTimer,
	IN	unsigned long timeout);

VOID RTMP_OS_Mod_Timer(
	IN	NDIS_MINIPORT_TIMER	*pTimer,
	IN	unsigned long timeout);


VOID RTMP_OS_Del_Timer(
	IN	NDIS_MINIPORT_TIMER	*pTimer,
	OUT	BOOLEAN				 *pCancelled);


VOID RTMP_OS_Release_Packet(
	IN	PRTMP_ADAPTER pAd,
	IN	PQUEUE_ENTRY  pEntry);

VOID RTMPusecDelay(
	IN	ULONG	usec);

NDIS_STATUS os_alloc_mem(
	IN	PRTMP_ADAPTER pAd,
	OUT	PUCHAR *mem,
	IN	ULONG  size);

NDIS_STATUS os_free_mem(
	IN	PRTMP_ADAPTER pAd,
	IN	PUCHAR mem);


void RTMP_AllocateSharedMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

VOID RTMPFreeTxRxRingMemory(
    IN  PRTMP_ADAPTER   pAd);

NDIS_STATUS AdapterBlockAllocateMemory(
	IN PVOID	handle,
	OUT	PVOID	*ppAd);

void RTMP_AllocateTxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateFirstTxBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	UINT	Index,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateMgmtDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

void RTMP_AllocateRxDescMemory(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS PhysicalAddress);

PNDIS_PACKET RTMP_AllocateTxPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length,
	IN	BOOLEAN	Cached,
	OUT	PVOID	*VirtualAddress);

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(
	IN	PRTMP_ADAPTER pAd,
	IN	ULONG	Length);

void RTMP_QueryPacketInfo(
	IN  PNDIS_PACKET pPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen);

void RTMP_QueryNextPacketInfo(
	IN  PNDIS_PACKET *ppPacket,
	OUT PACKET_INFO  *pPacketInfo,
	OUT PUCHAR		 *pSrcBufVA,
	OUT	UINT		 *pSrcBufLen);


BOOLEAN RTMP_FillTxBlkInfo(
	IN RTMP_ADAPTER *pAd,
	IN TX_BLK *pTxBlk);


PRTMP_SCATTER_GATHER_LIST
rt_get_sg_list_from_packet(PNDIS_PACKET pPacket, RTMP_SCATTER_GATHER_LIST *sg);


 void announce_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket);


UINT BA_Reorder_AMSDU_Annnounce(
	IN	PRTMP_ADAPTER	pAd, 	
	IN	PNDIS_PACKET	pPacket);


UINT Handle_AMSDU_Packet(
	IN	PRTMP_ADAPTER	pAd, 	
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN  UCHAR			FromWhichBSSID);


void convert_802_11_to_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR			p8023hdr,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN  UCHAR			FromWhichBSSID);


PNET_DEV get_netdev_from_bssid(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			FromWhichBSSID);


PNDIS_PACKET duplicate_pkt(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
    IN  UINT            HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			FromWhichBSSID);


PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pOldPkt);
	
PNDIS_PACKET duplicate_pkt_with_VLAN(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
    IN  UINT            HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	IN	UCHAR			FromWhichBSSID);

PNDIS_PACKET duplicate_pkt_with_WPI(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UINT32			ext_head_len,
	IN	UINT32			ext_tail_len);

UCHAR VLAN_8023_Header_Copy(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
	IN	UINT            HdrLen,
	OUT PUCHAR			pData,
	IN	UCHAR			FromWhichBSSID);

#ifdef DOT11_N_SUPPORT
void ba_flush_reordering_timeout_mpdus(
	IN PRTMP_ADAPTER	pAd, 
	IN PBA_REC_ENTRY	pBAEntry,
	IN ULONG			Now32);


VOID BAOriSessionSetUp(
			IN PRTMP_ADAPTER    pAd, 
			IN MAC_TABLE_ENTRY	*pEntry,
			IN UCHAR			TID,
			IN USHORT			TimeOut,
			IN ULONG			DelayTime,
			IN BOOLEAN		isForced);

VOID BASessionTearDownALL(
	IN OUT	PRTMP_ADAPTER pAd, 
	IN		UCHAR Wcid);
#endif // DOT11_N_SUPPORT //

BOOLEAN OS_Need_Clone_Packet(void);


VOID build_tx_packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	PUCHAR	pFrame,
	IN	ULONG	FrameLen);


VOID BAOriSessionTearDown(
	IN OUT	PRTMP_ADAPTER	pAd, 
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive,
	IN		BOOLEAN			bForceSend);

VOID BARecSessionTearDown(
	IN OUT	PRTMP_ADAPTER	pAd, 
	IN		UCHAR			Wcid,
	IN		UCHAR			TID,
	IN		BOOLEAN			bPassive);

BOOLEAN ba_reordering_resource_init(PRTMP_ADAPTER pAd, int num);
void ba_reordering_resource_release(PRTMP_ADAPTER pAd);

ULONG AutoChBssInsertEntry(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pBssid,
	IN CHAR Ssid[],
	IN UCHAR SsidLen, 
	IN UCHAR ChannelNo,
	IN CHAR Rssi);

void AutoChBssTableInit(
	IN PRTMP_ADAPTER pAd);

void ChannelInfoInit(
	IN PRTMP_ADAPTER pAd);

void AutoChBssTableDestroy(
	IN PRTMP_ADAPTER pAd);

void ChannelInfoDestroy(
	IN PRTMP_ADAPTER pAd);

UCHAR New_ApAutoSelectChannel(
	IN PRTMP_ADAPTER pAd);

#ifdef WSC_INCLUDED
INT	Set_WscGenPinCode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT Set_WscVendorPinCode_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PUCHAR          arg);

#ifdef WSC_STA_SUPPORT
VOID CntlWscSsidProc(
	IN PRTMP_ADAPTER pAd, 
	IN MLME_QUEUE_ELEM * Elem);

VOID CntlWscIterate(
	IN PRTMP_ADAPTER pAd);

USHORT WscGetAuthTypeFromStr(
    IN  PUCHAR          arg);

USHORT WscGetEncrypTypeFromStr(
    IN  PUCHAR          arg);
#endif // WSC_STA_SUPPORT //
//
// prototype in wsc.c
//
BOOLEAN	WscMsgTypeSubst(
	IN	UCHAR	EAPType,
	IN	UCHAR	EAPCode,
	OUT	INT	    *MsgType);

VOID    WscStateMachineInit(
	IN	PRTMP_ADAPTER		pAd, 
	IN	STATE_MACHINE		*S, 
	OUT STATE_MACHINE_FUNC Trans[]);

#ifdef CONFIG_AP_SUPPORT
VOID    WscEAPOLStartAction(
    IN  PRTMP_ADAPTER    pAd, 
    IN  MLME_QUEUE_ELEM  *Elem);
#endif // CONFIG_AP_SUPPORT //

VOID    WscEAPAction(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	MLME_QUEUE_ELEM *Elem);

VOID    WscEapEnrolleeAction(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	MLME_QUEUE_ELEM	*Elem,
	IN  UCHAR	        MsgType,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN  PWSC_CTRL       pWscControl);

#ifdef CONFIG_AP_SUPPORT
VOID    WscEapApProxyAction(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	MLME_QUEUE_ELEM	*Elem,
	IN  UCHAR	        MsgType,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN  PWSC_CTRL       pWscControl);
#endif // CONFIG_AP_SUPPORT //

VOID    WscEapRegistrarAction(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	MLME_QUEUE_ELEM	*Elem,
	IN  UCHAR	        MsgType,
	IN  MAC_TABLE_ENTRY *pEntry,
	IN  PWSC_CTRL       pWscControl);
	
#ifdef CONFIG_AP_SUPPORT	
VOID    WscEnqueueEapolStart(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);
#endif // CONFIG_AP_SUPPORT //
    
VOID    WscEAPOLTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

VOID    Wsc2MinsTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

UCHAR	WscRxMsgType(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	PMLME_QUEUE_ELEM	pElem);

VOID	WscInitRegistrarPair(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl);

#ifdef CONFIG_AP_SUPPORT
VOID    WscInitEntryFunc(
    IN  PMAC_TABLE_ENTRY pEntry);
#endif // CONFIG_AP_SUPPORT //

BOOLEAN WscCheckWSCHeader(
    IN	PRTMP_ADAPTER		pAdapter,
    IN  PUCHAR              pData);

#ifdef CONFIG_AP_SUPPORT
VOID	WscSendEapReqId(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN	PMAC_TABLE_ENTRY	pEntry);
#endif // CONFIG_AP_SUPPORT //

VOID    WscSendEapolStart(
	IN	PRTMP_ADAPTER	pAdapter,
	IN  PUCHAR          pBssid);

VOID	WscSendEapRspId(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PMAC_TABLE_ENTRY    pEntry,
	IN  PWSC_CTRL           pWscControl);

VOID	WscMacHeaderInit(
	IN	PRTMP_ADAPTER	pAd, 
	IN OUT	PHEADER_802_11	Hdr, 
	IN	PUCHAR 			pAddr1,
	IN  PUCHAR          pBSSID,
	IN  BOOLEAN         bFromApCli);

VOID	WscSendMessage(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN  UCHAR               OpCode,
	IN  PUCHAR				pData,
	IN  INT					Len,
	IN  PWSC_CTRL           pWscControl,
    IN  UCHAR               OpMode); // 0: AP Mode, 1: AP Client Mode, 2: STA Mode

VOID	WscSendEapReqAck(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN	PMAC_TABLE_ENTRY	pEntry);
	
VOID	WscSendEapReqDone(
	IN	PRTMP_ADAPTER		pAdapter, 
	IN	PMLME_QUEUE_ELEM	pElem);

#ifdef CONFIG_AP_SUPPORT
VOID	WscSendEapFail(
	IN	PRTMP_ADAPTER		pAdapter);
#endif // CONFIG_AP_SUPPORT //

VOID WscUPnPM2DTimeOutAction(
    IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

VOID WscUPnPMsgTimeOutAction(
	IN  PVOID SystemSpecific1, 
    IN  PVOID FunctionContext, 
    IN  PVOID SystemSpecific2, 
    IN  PVOID SystemSpecific3);

int WscSendUPnPConfReqMsg(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR ssidStr,
	IN PUCHAR macAddr,
	IN INT	  Status,
	IN UINT   eventID);

	
int WscSendUPnPMessage(
	IN	PRTMP_ADAPTER		pAd,
	IN  USHORT				msgType,
	IN  USHORT				msgSubType,
	IN  PUCHAR				pData,
	IN  INT					dataLen,
	IN	UINT				eventID,
	IN	UINT				toIPAddr,
	IN  PUCHAR				pMACAddr);

VOID WscUPnPErrHandle(
	IN PRTMP_ADAPTER 	pAd,
	IN PWSC_CTRL		pWscControl,
	IN UINT 			eventID);

#ifdef CONFIG_AP_SUPPORT
VOID    WscBuildBeaconIE(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	UCHAR b_configured,
	IN	BOOLEAN b_selRegistrar,
	IN	USHORT devPwdId,
	IN	USHORT selRegCfgMethods);

VOID    WscBuildProbeRespIE(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	UCHAR respType,
	IN	UCHAR scState,
	IN	BOOLEAN b_selRegistrar,
	IN	USHORT devPwdId,
	IN	USHORT selRegCfgMethods);

VOID	WscSelectedRegistrar(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR	RegInfo,
	IN	UINT	length);

VOID    WscInformFromWPA(
    IN  PMAC_TABLE_ENTRY    pEntry);
#endif // CONFIG_AP_SUPPORT //


INT 
ComputeChecksum(
	IN UINT PIN);


INT WscGenerateUUID(
	RTMP_ADAPTER *pAd,
	UCHAR *uuidHexStr,
	UCHAR *uuidAscStr,
	int apIdx);
	
VOID WscStop(
	IN	PRTMP_ADAPTER	pAd,
#ifdef CONFIG_AP_SUPPORT
    IN  BOOLEAN         bFromApcli,
#endif // CONFIG_AP_SUPPORT //
	IN  PWSC_CTRL       pWscControl);

VOID WscInit(
	IN	PRTMP_ADAPTER	pAd,
#ifdef CONFIG_AP_SUPPORT
    IN  BOOLEAN         bFromApcli,	
#endif // CONFIG_AP_SUPPORT //	
	IN  PWSC_CTRL       pWscControl);

BOOLEAN
ValidateChecksum(
	IN UINT PIN);

UINT
WscGeneratePinCode(
	IN	PRTMP_ADAPTER	pAd,
#ifdef CONFIG_AP_SUPPORT
    IN  BOOLEAN         bFromApcli,
#endif // CONFIG_AP_SUPPORT //
	IN	UCHAR	apidx);

UINT WscRandomGeneratePinCode(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR	apidx);

int BuildMessageM1(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM2(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM2D(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM3(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM4(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM5(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM6(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM7(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageM8(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageDONE(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageACK(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int BuildMessageNACK(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	OUT	VOID *pbuf);

int ProcessMessageM1(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM2(
	IN	PRTMP_ADAPTER		pAdapter,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM2D(
	IN	PRTMP_ADAPTER		pAdapter,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM3(
	IN	PRTMP_ADAPTER		pAdapter,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM4(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM5(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM6(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM7(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  PWSC_CTRL           pWscControl,
	IN	VOID *precv,
	IN	INT Length,
	OUT	PWSC_REG_DATA pReg);

int ProcessMessageM8(
	IN	PRTMP_ADAPTER		pAdapter,
	IN	VOID *precv,
	IN	INT Length,
	IN  PWSC_CTRL       pWscControl);

USHORT  WscGetAuthType(
    IN  NDIS_802_11_AUTHENTICATION_MODE authType);

USHORT  WscGetEncryType(
    IN  NDIS_802_11_WEP_STATUS encryType);

void    WscWriteConfToDatFile(
    IN  PRTMP_ADAPTER pAd);

void    start_write_dat_file_thread(
    IN  PRTMP_ADAPTER pAd);

int     AppendWSCTLV(
    IN  USHORT index, 
    OUT UCHAR * obuf, 
    IN  UCHAR * ibuf, 
    IN  u16 varlen);

VOID    WscGetRegDataPIN(
    IN  PRTMP_ADAPTER   pAdapter,
    IN  UINT            PinCode,
    IN  PWSC_CTRL       pWscControl);


VOID	WscCreateProfileFromCfg(
	IN	PRTMP_ADAPTER		pAdapter,
	IN  UCHAR               OpMode,         // 0: AP Mode, 1: AP Client Mode, 2: STA Mode
	IN  PWSC_CTRL           pWscControl,
	OUT PWSC_PROFILE        pWscProfile); 

void    WscWriteConfToPortCfg(
    IN  PRTMP_ADAPTER   pAd,
    IN  MAC_TABLE_ENTRY *pEntry,
    IN  BOOLEAN         bEnrollee);

#ifdef CONFIG_AP_SUPPORT
VOID   WpsApSmProcess(
    IN PRTMP_ADAPTER        pAd,
    IN MLME_QUEUE_ELEM 	   *Elem);

INT	WscGetConfWithoutTrigger(
	IN	PRTMP_ADAPTER	pAd,
	IN  PWSC_CTRL       pWscControl,
	IN  BOOLEAN         bFromUPnP);

VOID WscPBCSessionOverlapCheck(
	IN  PWSC_CTRL           pWscControl);
#endif // CONFIG_AP_SUPPORT //


/* WSC hardware push button function 0811 */
VOID WSC_HDR_BTN_Init(
	IN	PRTMP_ADAPTER	pAd);

VOID WSC_HDR_BTN_Stop(
	IN	PRTMP_ADAPTER	pAd);
 
VOID WSC_HDR_BTN_CheckHandler(
	IN	PRTMP_ADAPTER	pAd);
#endif // WSC_INCLUDED //


BOOLEAN rtstrmactohex(
	IN char *s1,
	IN char *s2);

BOOLEAN rtstrcasecmp(
	IN char *s1,
	IN char *s2);

char *rtstrstruncasecmp(
	IN char *s1,
	IN char *s2);

char    *rtstrstr(
	IN	const char * s1,
	IN	const char * s2);

char *rstrtok(
	IN char * s,
	IN const char * ct);
	
int rtinet_aton(
	const char *cp, 
	unsigned int *addr);
	
////////// common ioctl functions //////////
INT Set_DriverVersion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT Set_CountryRegion_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT Set_CountryRegionABand_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT Set_WirelessMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT Set_Channel_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_ShortSlot_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_TxPower_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT Set_BGProtection_Proc(
	IN  PRTMP_ADAPTER		pAd, 
	IN  PUCHAR			arg);

INT Set_TxPreamble_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR			arg);

INT Set_RTSThreshold_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR			arg);

INT Set_FragThreshold_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR			arg);

INT Set_TxBurst_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR			arg);

#ifdef AGGREGATION_SUPPORT
INT	Set_PktAggregate_Proc(
	IN  PRTMP_ADAPTER		pAd,
	IN  PUCHAR			arg);
#endif

INT	Set_IEEE80211H_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef DBG
INT	Set_Debug_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif

INT	Show_DescInfo_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_ResetStatCounter_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef DOT11_N_SUPPORT
INT	Set_BASetup_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BADecline_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BAOriTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_BARecTearDown_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtBw_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMcs_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtGi_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtOpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtStbc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtHtc_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtExtcha_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMpduDensity_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtBaWinSize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtRdg_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtLinkAdapt_Proc(																																																																																																																																																																																																																																									
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtAmsdu_Proc(																																																																																																																																																																																																																																																																																																																			
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
	
INT	Set_HtAutoBa_Proc(																																																																																																																																																																																																																																																																																																																			
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
					
INT	Set_HtProtect_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_HtMimoPs_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

#ifdef CONFIG_AP_SUPPORT
INT	Set_HtTxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtRxStream_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // CONFIG_AP_SUPPORT //

INT	Set_ForceShortGI_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	Set_ForceGF_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT	SetCommonHT(
	IN	PRTMP_ADAPTER	pAd);

INT	Set_SendPSMPAction_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

INT	Set_HtMIMOPSmode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);


INT	Set_HtTxBASize_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // DOT11_N_SUPPORT //

#ifdef ETH_CONVERT_SUPPORT
INT Set_EthConvertMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);


INT Set_EthCloneMac_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // ETH_CONVERT_SUPPORT //



#ifdef CONFIG_AP_SUPPORT
VOID detect_wmm_traffic(
	IN	PRTMP_ADAPTER	pAd, 
	IN	UCHAR			UserPriority);

VOID dynamic_tune_be_tx_op(
	IN	PRTMP_ADAPTER	pAd,
	IN	ULONG			nonBEpackets);
#endif // CONFIG_AP_SUPPORT //


#ifdef DOT11_N_SUPPORT
VOID Handle_BSS_Width_Trigger_Events(
	IN PRTMP_ADAPTER pAd);

void build_ext_channel_switch_ie(
	IN PRTMP_ADAPTER pAd,
	IN HT_EXT_CHANNEL_SWITCH_ANNOUNCEMENT_IE *pIE);
#endif // DOT11_N_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
void SendSingalToDaemon(
	IN INT              sig,
	ULONG               pid);

BOOLEAN IEEE8021X_L2_Disconnect_Frame_Send(
    IN  PRTMP_ADAPTER	pAd,
    IN  MAC_TABLE_ENTRY *pEntry);

BOOLEAN IEEE8021X_L2_Trigger_Frame_Send(
    IN  PRTMP_ADAPTER	pAd,
    IN  MAC_TABLE_ENTRY *pEntry);

#endif // CONFIG_AP_SUPPORT //

BOOLEAN APRxDoneInterruptHandle(
	IN	PRTMP_ADAPTER	pAd);

BOOLEAN STARxDoneInterruptHandle(
	IN	PRTMP_ADAPTER	pAd,
	IN	BOOLEAN			argc);

#ifdef DOT11_N_SUPPORT
// AMPDU packet indication
VOID Indicate_AMPDU_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

// AMSDU packet indication
VOID Indicate_AMSDU_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);
#endif // DOT11_N_SUPPORT //

// Normal legacy Rx packet indication
VOID Indicate_Legacy_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

VOID Indicate_EAPOL_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

void  update_os_packet_info(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

void wlan_802_11_to_802_3_packet(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RX_BLK			*pRxBlk,
	IN	PUCHAR			pHeader802_3,
	IN  UCHAR			FromWhichBSSID);

UINT deaggregate_AMSDU_announce(
	IN	PRTMP_ADAPTER	pAd,
	PNDIS_PACKET		pPacket,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize);

#ifdef CONFIG_AP_SUPPORT
// remove LLC and get 802_3 Header
#define  RTMP_AP_802_11_REMOVE_LLC_AND_CONVERT_TO_802_3(_pRxBlk, _pHeader802_3)	\
{																						\
	PUCHAR _pRemovedLLCSNAP = NULL, _pDA, _pSA;                                 						\
																						\
																						\
	if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_WDS) || RX_BLK_TEST_FLAG(_pRxBlk, fRX_MESH)) 		\
	{                                                                           											\
		_pDA = _pRxBlk->pHeader->Addr3;                                         							\
		_pSA = (PUCHAR)_pRxBlk->pHeader + sizeof(HEADER_802_11);                					\
	}                                                                           											\
	else if (RX_BLK_TEST_FLAG(_pRxBlk, fRX_APCLI))										\
	{																					\
		_pDA = _pRxBlk->pHeader->Addr1; 													\
		_pSA = _pRxBlk->pHeader->Addr3;													\
	}																					\
	else                                                                        										\
	{                                                                           											\
		_pDA = _pRxBlk->pHeader->Addr3;                                         							\
		_pSA = _pRxBlk->pHeader->Addr2;                                         							\
	}                                                                           											\
																						\
	CONVERT_TO_802_3(_pHeader802_3, _pDA, _pSA, _pRxBlk->pData, 						\
		_pRxBlk->DataSize, _pRemovedLLCSNAP);                                   						\
}
#endif // CONFIG_AP_SUPPORT //



BOOLEAN APFowardWirelessStaToWirelessSta(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	ULONG			FromWhichBSSID);

VOID Announce_or_Forward_802_3_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);

VOID Sta_Announce_or_Forward_802_3_Packet(
	IN	PRTMP_ADAPTER	pAd,
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);

#ifdef CONFIG_AP_SUPPORT
#define AP_ANNOUNCE_OR_FORWARD_802_3_PACKET(_pAd, _pPacket, _FromWhichBSS)\
			Announce_or_Forward_802_3_Packet(_pAd, _pPacket, _FromWhichBSS);
#endif // CONFIG_AP_SUPPORT //



PNDIS_PACKET DuplicatePacket(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,
	IN	UCHAR			FromWhichBSSID);


PNDIS_PACKET ClonePacket(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PNDIS_PACKET	pPacket,	
	IN	PUCHAR			pData,
	IN	ULONG			DataSize);


// Normal, AMPDU or AMSDU
VOID CmmRxnonRalinkFrameIndicate(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

VOID CmmRxRalinkFrameIndicate(
	IN	PRTMP_ADAPTER	pAd,
	IN	MAC_TABLE_ENTRY	*pEntry,
	IN	RX_BLK			*pRxBlk,
	IN	UCHAR			FromWhichBSSID);

VOID Update_Rssi_Sample(
	IN PRTMP_ADAPTER	pAd,
	IN RSSI_SAMPLE		*pRssi,
	IN PRXWI_STRUC		pRxWI);

PNDIS_PACKET GetPacketFromRxRing(
	IN		PRTMP_ADAPTER	pAd,
	OUT		PRT28XX_RXD_STRUC		pSaveRxD,
	OUT		BOOLEAN			*pbReschedule,
	IN OUT	UINT32			*pRxPending);

PNDIS_PACKET RTMPDeFragmentDataFrame(
	IN	PRTMP_ADAPTER	pAd,
	IN	RX_BLK			*pRxBlk);

////////////////////////////////////////

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
INT Set_ApCli_AuthMode_Proc(
    IN PRTMP_ADAPTER pAd, 
    IN PUCHAR arg);

INT Set_ApCli_EncrypType_Proc(
    IN PRTMP_ADAPTER pAd, 
    IN PUCHAR arg);
#endif // APCLI_SUPPORT //
#endif // CONFIG_AP_SUPPORT //


#ifdef MAT_SUPPORT

NDIS_STATUS MATEngineInit(
	IN RTMP_ADAPTER *pAd);

NDIS_STATUS MATEngineExit(
	IN RTMP_ADAPTER *pAd);

PUCHAR MATEngineRxHandle(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET		pPkt,
	IN UINT				infIdx);


PUCHAR MATEngineTxHandle(
	IN PRTMP_ADAPTER	pAd,
	IN PNDIS_PACKET     pPkt,
	IN UINT				infIdx);

BOOLEAN MATPktRxNeedConvert(
	IN PRTMP_ADAPTER	pAd, 
	IN PNET_DEV			net_dev);

#endif // MAT_SUPPORT //

#ifdef CONFIG_AP_SUPPORT
typedef struct CountryCodeToCountryRegion {
	USHORT		CountryNum;
    UCHAR   	IsoName[3];
    UCHAR   	CountryName[40];
	BOOLEAN		SupportABand;
    ULONG		RegDomainNum11A;
    BOOLEAN  	SupportGBand;
    ULONG		RegDomainNum11G;
} COUNTRY_CODE_TO_COUNTRY_REGION;
#endif // CONFIG_AP_SUPPORT //

#ifdef SNMP_SUPPORT
//for snmp , kathy
typedef struct _DefaultKeyIdxValue
{
	UCHAR	KeyIdx;
	UCHAR	Value[16];
} DefaultKeyIdxValue, *PDefaultKeyIdxValue;
#endif



INT	Set_FixedTxMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);

#ifdef CONFIG_APSTA_MIXED_SUPPORT
INT	Set_OpMode_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			arg);
#endif // CONFIG_APSTA_MIXED_SUPPORT //

static inline char* GetPhyMode(
	int Mode)
{
	switch(Mode)
	{
		case MODE_CCK:
			return "CCK";

		case MODE_OFDM:
			return "OFDM";
#ifdef DOT11_N_SUPPORT
		case MODE_HTMIX:
			return "HTMIX";

		case MODE_HTGREENFIELD:
			return "GREEN";
#endif // DOT11_N_SUPPORT //
		default:
			return "N/A";
	}
}


static inline char* GetBW(
	int BW)
{
	switch(BW)
	{
		case BW_10:
			return "10M";

		case BW_20:
			return "20M";
#ifdef DOT11_N_SUPPORT
		case BW_40:
			return "40M";
#endif // DOT11_N_SUPPORT //
		default:
			return "N/A";
	}
}


VOID RT28xxThreadTerminate(
	IN RTMP_ADAPTER *pAd);

BOOLEAN RT28XXChipsetCheck(
	IN void *_dev_p);

BOOLEAN RT28XXNetDevInit(
	IN void 				*_dev_p,
	IN struct  net_device	*net_dev,
	IN RTMP_ADAPTER 		*pAd);

BOOLEAN RT28XXProbePostConfig(
	IN void 				*_dev_p,
	IN RTMP_ADAPTER 		*pAd,
	IN INT32				argc);

VOID RT28XXDMADisable(
	IN RTMP_ADAPTER 		*pAd);

VOID RT28XXDMAEnable(
	IN RTMP_ADAPTER 		*pAd);
	
VOID RT28xx_UpdateBeaconToAsic(
	IN RTMP_ADAPTER * pAd, 
	IN INT apidx,
	IN ULONG BeaconLen,
	IN ULONG UpdatePos);

INT rt28xx_ioctl(
	IN	struct net_device	*net_dev, 
	IN	OUT	struct ifreq	*rq, 
	IN	INT			cmd);

#ifdef CONFIG_AP_SUPPORT
INT rt28xx_ap_ioctl(
	IN	struct net_device	*net_dev, 
	IN	OUT	struct ifreq	*rq, 
	IN	INT			cmd);
#endif // CONFIG_AP_SUPPORT //


BOOLEAN RT28XXSecurityKeyAdd(
	IN		PRTMP_ADAPTER		pAd,
	IN		ULONG				apidx,
	IN		ULONG				KeyIdx,
	IN		MAC_TABLE_ENTRY 	*pEntry);

////////////////////////////////////////
PNDIS_PACKET GetPacketFromRxRing(
	IN		PRTMP_ADAPTER	pAd,
	OUT		PRT28XX_RXD_STRUC	pSaveRxD,
	OUT		BOOLEAN			*pbReschedule,
	IN OUT	UINT32			*pRxPending);


void kill_thread_task(PRTMP_ADAPTER pAd);

void tbtt_tasklet(unsigned long data);


VOID AsicTurnOffRFClk(
	IN PRTMP_ADAPTER    pAd, 
	IN	UCHAR           Channel);

VOID AsicTurnOnRFClk(
	IN PRTMP_ADAPTER 	pAd, 
	IN	UCHAR			Channel);

#ifdef RT30xx
NTSTATUS RT30xxWriteRFRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			RegID,
	IN	UCHAR			Value);

NTSTATUS RT30xxReadRFRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			RegID,
	IN	PUCHAR			pValue);

//2008/09/11:KH add to support efuse<--
UCHAR eFuseReadRegisters(
	IN	PRTMP_ADAPTER	pAd, 
	IN	USHORT Offset, 
	IN	USHORT Length, 
	OUT	USHORT* pData);

VOID eFuseReadPhysical( 
	IN	PRTMP_ADAPTER	pAd, 
  	IN	PUSHORT lpInBuffer,
  	IN	ULONG nInBufferSize,
  	OUT	PUSHORT lpOutBuffer,
  	IN	ULONG nOutBufferSize  
);

NTSTATUS eFuseRead(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	OUT	PUCHAR			pData,
	IN	USHORT			Length);

VOID eFusePhysicalWriteRegisters(
	IN	PRTMP_ADAPTER	pAd,	
	IN	USHORT Offset, 
	IN	USHORT Length, 
	OUT	USHORT* pData);

NTSTATUS eFuseWriteRegisters(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT Offset, 
	IN	USHORT Length, 
	IN	USHORT* pData);

VOID eFuseWritePhysical( 
	IN	PRTMP_ADAPTER	pAd,	
  	PUSHORT lpInBuffer,
	ULONG nInBufferSize,
  	PUCHAR lpOutBuffer,
  	ULONG nOutBufferSize  
);

NTSTATUS eFuseWrite(  
   	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData,
	IN	USHORT			length);

INT set_eFuseGetFreeBlockCount_Proc(  
   	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT set_eFusedump_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

INT set_eFuseLoadFromBin_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PUCHAR			arg);

NTSTATUS eFuseWriteRegistersFromBin(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT Offset, 
	IN	USHORT Length, 
	IN	USHORT* pData);

VOID eFusePhysicalReadRegisters( 
	IN	PRTMP_ADAPTER	pAd, 
	IN	USHORT Offset, 
	IN	USHORT Length, 
	OUT	USHORT* pData);

NDIS_STATUS NICLoadEEPROM(
	IN PRTMP_ADAPTER pAd);

BOOLEAN bNeedLoadEEPROM(
	IN	PRTMP_ADAPTER	pAd);
//2008/09/11:KH add to support efuse-->
#endif // RT30xx //

#ifdef RT30xx
// add by johnli, RF power sequence setup
VOID RT30xxLoadRFNormalModeSetup(
	IN PRTMP_ADAPTER 	pAd);

VOID RT30xxLoadRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd);

VOID RT30xxReverseRFSleepModeSetup(
	IN PRTMP_ADAPTER 	pAd);
// end johnli
#endif // RT30xx //

#ifdef RT2870
//
// Function Prototype in rtusb_bulk.c
//
VOID	RTUSBInitTxDesc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTX_CONTEXT		pTxContext,
	IN	UCHAR			BulkOutPipeId,
	IN	usb_complete_t	Func);

VOID	RTUSBInitHTTxDesc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PHT_TX_CONTEXT	pTxContext,
	IN	UCHAR			BulkOutPipeId,
	IN	ULONG			BulkOutSize,
	IN	usb_complete_t	Func);

VOID	RTUSBInitRxDesc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PRX_CONTEXT		pRxContext);

VOID RTUSBCleanUpDataBulkOutQueue(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBCancelPendingBulkOutIRP(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBBulkOutDataPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BulkOutPipeId,
	IN	UCHAR			Index);

VOID RTUSBBulkOutNullFrame(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBBulkOutRTSFrame(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBCancelPendingBulkInIRP(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBCancelPendingIRPs(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBBulkOutMLMEPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Index);

VOID RTUSBBulkOutPsPoll(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBCleanUpMLMEBulkOutQueue(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBKickBulkOut(
	IN	PRTMP_ADAPTER pAd);

VOID	RTUSBBulkReceive(
	IN	PRTMP_ADAPTER	pAd);

VOID DoBulkIn(
	IN RTMP_ADAPTER *pAd);
	
VOID RTUSBInitRxDesc(
	IN	PRTMP_ADAPTER	pAd,
	IN  PRX_CONTEXT		pRxContext);

VOID RTUSBBulkRxHandle(
	IN unsigned long data);	

//
// Function Prototype in rtusb_io.c
//
NTSTATUS RTUSBMultiRead(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	OUT	PUCHAR			pData,
	IN	USHORT			length);

NTSTATUS RTUSBMultiWrite(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData,
	IN	USHORT			length);

NTSTATUS RTUSBMultiWrite_OneByte(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData);

NTSTATUS RTUSBReadBBPRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Id,
	IN	PUCHAR			pValue);

NTSTATUS RTUSBWriteBBPRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Id,
	IN	UCHAR			Value);

NTSTATUS RTUSBWriteRFRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	UINT32			Value);
	
NTSTATUS RTUSB_VendorRequest(
	IN	PRTMP_ADAPTER	pAd,
	IN	UINT32			TransferFlags,
	IN	UCHAR			ReservedBits,
	IN	UCHAR			Request,
	IN	USHORT			Value,
	IN	USHORT			Index,
	IN	PVOID			TransferBuffer,
	IN	UINT32			TransferBufferLength);

NTSTATUS RTUSBReadEEPROM(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	OUT	PUCHAR			pData,
	IN	USHORT			length);

NTSTATUS RTUSBWriteEEPROM(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	IN	PUCHAR			pData,
	IN	USHORT			length);

VOID RTUSBPutToSleep(
	IN	PRTMP_ADAPTER	pAd);

NTSTATUS RTUSBWakeUp(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBInitializeCmdQ(
	IN	PCmdQ	cmdq);

NDIS_STATUS	RTUSBEnqueueCmdFromNdis(
	IN	PRTMP_ADAPTER	pAd,
	IN	NDIS_OID		Oid,
	IN	BOOLEAN			SetInformation,
	IN	PVOID			pInformationBuffer,
	IN	UINT32			InformationBufferLength);

NDIS_STATUS RTUSBEnqueueInternalCmd(
	IN	PRTMP_ADAPTER	pAd,
	IN NDIS_OID			Oid,
	IN PVOID			pInformationBuffer,
	IN UINT32			InformationBufferLength);

VOID RTUSBDequeueCmd(
	IN	PCmdQ		cmdq,
	OUT	PCmdQElmt	*pcmdqelmt);

INT RTUSBCmdThread(
	IN OUT PVOID Context);

INT TimerQThread(
	IN OUT PVOID Context);

RT2870_TIMER_ENTRY *RT2870_TimerQ_Insert(
	IN RTMP_ADAPTER *pAd, 
	IN RALINK_TIMER_STRUCT *pTimer);

BOOLEAN RT2870_TimerQ_Remove(
	IN RTMP_ADAPTER *pAd, 
	IN RALINK_TIMER_STRUCT *pTimer);

void RT2870_TimerQ_Exit(
	IN RTMP_ADAPTER *pAd);

void RT2870_TimerQ_Init(
	IN RTMP_ADAPTER *pAd);

VOID RT2870_BssBeaconExit(
	IN RTMP_ADAPTER *pAd);

VOID RT2870_BssBeaconStop(
	IN RTMP_ADAPTER *pAd);

VOID RT2870_BssBeaconStart(
	IN RTMP_ADAPTER * pAd);

VOID RT2870_BssBeaconInit(
	IN RTMP_ADAPTER *pAd);

VOID RT2870_WatchDog(
	IN RTMP_ADAPTER *pAd);
	
NTSTATUS RTUSBWriteMACRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	IN	UINT32			Value);

NTSTATUS RTUSBReadMACRegister(
	IN	PRTMP_ADAPTER	pAd,
	IN	USHORT			Offset,
	OUT	PUINT32			pValue);

NTSTATUS RTUSBSingleWrite(
	IN 	RTMP_ADAPTER 	*pAd,
	IN	USHORT			Offset,
	IN	USHORT			Value);
	
NTSTATUS RTUSBFirmwareRun(
	IN	PRTMP_ADAPTER	pAd);

NTSTATUS RTUSBFirmwareWrite(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR		pFwImage,
	IN ULONG		FwLen);
	
NTSTATUS	RTUSBFirmwareOpmode(
	IN	PRTMP_ADAPTER	pAd,
	OUT	PUINT32			pValue);

NTSTATUS	RTUSBVenderReset(
	IN	PRTMP_ADAPTER	pAd);

NDIS_STATUS RTUSBSetHardWareRegister(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	PVOID			pBuf);

NDIS_STATUS RTUSBQueryHardWareRegister(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	PVOID			pBuf);

VOID CMDHandler(                                                                                                                                                
    IN PRTMP_ADAPTER pAd);


NDIS_STATUS	 CreateThreads( 
	IN	struct net_device *net_dev );


VOID MacTableInitialize(
	IN  PRTMP_ADAPTER   pAd);

VOID MlmeSetPsm(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT psm);

NDIS_STATUS RTMPWPAAddKeyProc(
	IN  PRTMP_ADAPTER   pAd,
	IN  PVOID           pBuf);

VOID AsicRxAntEvalAction(
	IN PRTMP_ADAPTER pAd);

#if 0 // Mark because not used in RT28xx.
NTSTATUS RTUSBRxPacket(
	IN	PRTMP_ADAPTER  pAd,
	IN    BOOLEAN          bBulkReceive);

VOID RTUSBDequeueMLMEPacket(
	IN	PRTMP_ADAPTER	pAd);

VOID RTUSBCleanUpMLMEWaitQueue(
	IN	PRTMP_ADAPTER	pAd);
#endif

void append_pkt(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pHeader802_3,
    IN  UINT            HdrLen,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize,
	OUT  PNDIS_PACKET	*ppPacket);

UINT deaggregate_AMSDU_announce(
	IN	PRTMP_ADAPTER	pAd,
	PNDIS_PACKET		pPacket,
	IN	PUCHAR			pData,
	IN	ULONG			DataSize);

NDIS_STATUS	RTMPCheckRxError(
	IN	PRTMP_ADAPTER	pAd,
	IN	PHEADER_802_11	pHeader,	
	IN	PRXWI_STRUC	pRxWI,	
	IN	PRT28XX_RXD_STRUC	pRxINFO);


VOID RTUSBMlmeHardTransmit(
	IN	PRTMP_ADAPTER	pAd,
	IN	PMGMT_STRUC		pMgmt);

INT MlmeThread(
	IN PVOID Context);

#if 0
VOID    RTUSBResumeMsduTransmission(
	IN	PRTMP_ADAPTER	pAd);

VOID    RTUSBSuspendMsduTransmission(
	IN	PRTMP_ADAPTER	pAd);
#endif

//
// Function Prototype in rtusb_data.c
//
NDIS_STATUS	RTUSBFreeDescriptorRequest(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BulkOutPipeId,
	IN	UINT32			NumberRequired);


BOOLEAN	RTUSBNeedQueueBackForAgg(
	IN RTMP_ADAPTER *pAd, 
	IN UCHAR		BulkOutPipeId);


VOID RTMPWriteTxInfo(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXINFO_STRUC 	pTxInfo,
	IN	  USHORT		USBDMApktLen,
	IN	  BOOLEAN		bWiv,
	IN	  UCHAR			QueueSel,
	IN	  UCHAR			NextValid,
	IN	  UCHAR			TxBurst);

//
// Function Prototype in cmm_data_2870.c
//
USHORT RtmpUSB_WriteSubTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber);

USHORT RtmpUSB_WriteSingleTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	BOOLEAN			bIsLast,
	OUT	USHORT			*FreeNumber);

USHORT	RtmpUSB_WriteFragTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			fragNum,
	OUT	USHORT			*FreeNumber);
	
USHORT RtmpUSB_WriteMultiTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			frameNum,
	OUT	USHORT			*FreeNumber);

VOID RtmpUSB_FinalWriteTxResource(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	USHORT			totalMPDUSize,
	IN	USHORT			TxIdx);

VOID RtmpUSBDataLastTxIdx(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			QueIdx,
	IN	USHORT			TxIdx);

VOID RtmpUSBDataKickOut(
	IN	PRTMP_ADAPTER	pAd,
	IN	TX_BLK			*pTxBlk,
	IN	UCHAR			QueIdx);


int RtmpUSBMgmtKickOut(
	IN RTMP_ADAPTER 	*pAd, 
	IN UCHAR 			QueIdx,
	IN PNDIS_PACKET		pPacket,
	IN PUCHAR			pSrcBufVA,
	IN UINT 			SrcBufLen);

VOID RtmpUSBNullFrameKickOut(
	IN RTMP_ADAPTER *pAd,
	IN UCHAR		QueIdx,
	IN UCHAR		*pNullFrame,
	IN UINT32		frameLen);
	
VOID RT28xxUsbStaAsicForceWakeup(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN       bFromTx);

VOID RT28xxUsbStaAsicSleepThenAutoWakeup(
	IN PRTMP_ADAPTER pAd, 
	IN USHORT TbttNumToNextWakeUp);

VOID RT28xxUsbMlmeRadioOn(
	IN PRTMP_ADAPTER pAd);

VOID RT28xxUsbMlmeRadioOFF(
	IN PRTMP_ADAPTER pAd);
#endif // RT2870 //

////////////////////////////////////////

VOID QBSS_LoadInit(
 	IN		RTMP_ADAPTER	*pAd);

UINT32 QBSS_LoadElementAppend(
 	IN		RTMP_ADAPTER	*pAd,
	OUT		UINT8			*buf_p);

VOID QBSS_LoadUpdate(
 	IN		RTMP_ADAPTER	*pAd);

///////////////////////////////////////
INT RTMPShowCfgValue(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PUCHAR			pName,
	IN	PUCHAR			pBuf);

PCHAR   RTMPGetRalinkAuthModeStr(
    IN  NDIS_802_11_AUTHENTICATION_MODE authMode);

PCHAR   RTMPGetRalinkEncryModeStr(
    IN  USHORT encryMode);
//////////////////////////////////////


void RTMP_IndicateMediaState(	
	IN	PRTMP_ADAPTER	pAd);

VOID ReSyncBeaconTime(
	IN  PRTMP_ADAPTER   pAd);

VOID RTMPSetAGCInitValue(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			BandWidth);

int rt28xx_close(IN PNET_DEV dev);
int rt28xx_open(IN PNET_DEV dev);

__inline INT VIRTUAL_IF_UP(PRTMP_ADAPTER pAd)
{
#ifdef CONFIG_AP_SUPPORT
extern VOID APMakeAllBssBeacon(IN PRTMP_ADAPTER pAd);
extern VOID  APUpdateAllBeaconFrame(IN PRTMP_ADAPTER pAd);
#endif // CONFIG_AP_SUPPORT //
extern VOID MeshMakeBeacon(IN PRTMP_ADAPTER pAd, IN UCHAR idx);
extern VOID MeshUpdateBeaconFrame(IN PRTMP_ADAPTER pAd, IN UCHAR idx);

	if (VIRTUAL_IF_NUM(pAd) == 0)
	{
		if (rt28xx_open(pAd->net_dev) != 0)
			return -1;
	}
	else
	{
#ifdef CONFIG_AP_SUPPORT
		APMakeAllBssBeacon(pAd);
		APUpdateAllBeaconFrame(pAd);
#endif // CONFIG_AP_SUPPORT //
	}
	VIRTUAL_IF_INC(pAd);
	return 0;
}

__inline VOID VIRTUAL_IF_DOWN(PRTMP_ADAPTER pAd)
{
	VIRTUAL_IF_DEC(pAd);
	if (VIRTUAL_IF_NUM(pAd) == 0)
		rt28xx_close(pAd->net_dev);
	return;
}


#endif  // __RTMP_H__

