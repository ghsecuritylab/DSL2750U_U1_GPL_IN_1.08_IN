#ifndef __VOIP_MGR_DO_MISC_H__
#define __VOIP_MGR_DO_MISC_H__

#define M_EXTERN_MISC( x )	extern int do_mgr_ ## x ( int cmd, void *user, unsigned int len, unsigned short seq_no );


// Miscellanous  
//! @addtogroup VOIP_MISC
//! @ingroup VOIP_CONTROL
M_EXTERN_MISC( VOIP_MGR_SIP_REGISTER ); 
M_EXTERN_MISC( VOIP_MGR_GET_FEATURE );
M_EXTERN_MISC( VOIP_MGR_VOIP_RESOURCE_CHECK );
M_EXTERN_MISC( VOIP_MGR_SET_FW_UPDATE );
M_EXTERN_MISC( VOIP_MGR_SET_DSP_ID_TO_DSP );
M_EXTERN_MISC( VOIP_MGR_SET_DSP_PHY_ID );
M_EXTERN_MISC( VOIP_MGR_CHECK_DSP_ALL_SW_READY );

#undef M_EXTERN_MISC

#endif /* __VOIP_MGR_DO_MISC_H__ */

