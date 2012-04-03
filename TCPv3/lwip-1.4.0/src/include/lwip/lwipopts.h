#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

//#include "stdio.h"

//*****************************************************************************
//
// ---------- Platform specific locking ----------
//
//*****************************************************************************
#define SYS_LIGHTWEIGHT_PROT            1           // default is 0
#define NO_SYS                          0           // default is 0
//#define MEMCPY(dst,src,len)             memcpy(dst,src,len)
//#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)

//*****************************************************************************
//
// ---------- Memory options ----------
//
//*****************************************************************************
//#define MEM_LIBC_MALLOC                 0
#define MEM_ALIGNMENT                    4           // default is 1

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#define MEM_SIZE                        (2 * 1024)   // default is 1600, was 16K	było 2 *1024	//stos

/**
 * MEMP_OVERFLOW_CHECK: memp overflow protection reserves a configurable
 * amount of bytes before and after each memp element in every pool and fills
 * it with a prominent default value.
 *    MEMP_OVERFLOW_CHECK == 0 no checking
 *    MEMP_OVERFLOW_CHECK == 1 checks each element when it is freed
 *    MEMP_OVERFLOW_CHECK >= 2 checks each element in every pool every time
 *      memp_malloc() or memp_free() is called (useful but slow!)
 */
#define MEMP_OVERFLOW_CHECK             0

/**
 * MEMP_SANITY_CHECK==1: run a sanity check after each memp_free() to make
 * sure that there are no cycles in the linked lists.
 */
#define MEMP_SANITY_CHECK               1

//#define MEM_USE_POOLS                   0
//#define MEMP_USE_CUSTOM_POOLS           0

//*****************************************************************************
//
// ---------- Internal Memory Pool Sizes ----------
//
//*****************************************************************************
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                     5    // Default 16, było 16

//#define MEMP_NUM_RAW_PCB                4
//#define MEMP_NUM_UDP_PCB                4

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                  3    // Default 5, was 12 było 16

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         1

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
//#define MEMP_NUM_TCP_SEG                16

/**
 * MEMP_NUM_REASSDATA: the number of simultaneously IP packets queued for
 * reassembly (whole packets, not fragments!)
 */
//#define MEMP_NUM_REASSDATA              5
//#define MEMP_NUM_ARP_QUEUE              30
//#define MEMP_NUM_IGMP_GROUP             8
#define MEMP_NUM_SYS_TIMEOUT              10
//#define MEMP_NUM_NETBUF                 2
//#define MEMP_NUM_NETCONN                4
//#define MEMP_NUM_TCPIP_MSG_API          8
//#define MEMP_NUM_TCPIP_MSG_INPKT        8

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#define PBUF_POOL_SIZE                    22    // Default 16, was 36

//*****************************************************************************
//
// ---------- ARP options ----------
//
//*****************************************************************************
#define LWIP_ARP                        1
#define ARP_TABLE_SIZE                  10
#define ARP_QUEUEING                    1
#define ETHARP_TRUST_IP_MAC             1

//*****************************************************************************
//
// ---------- IP options ----------
//
//*****************************************************************************
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   0           // default is 1
#define IP_FRAG                         0           // default is 1
//#define IP_REASS_MAXAGE                 3
//#define IP_REASS_MAX_PBUFS              10
//#define IP_FRAG_USES_STATIC_BUF         1
//#define IP_FRAG_MAX_MTU                 1500
#define IP_DEFAULT_TTL                  255

//*****************************************************************************
//
// ---------- ICMP options ----------
//
//*****************************************************************************
#define LWIP_ICMP                       1		// PING
#define ICMP_TTL                       (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- RAW options ----------
//
//*****************************************************************************
//#define LWIP_RAW                        1
//#define RAW_TTL                        (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- DHCP options ----------
//
//*****************************************************************************
#define LWIP_DHCP                       1           // default is 0
//#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))

//*****************************************************************************
//
// ---------- UPNP options ----------
//
//*****************************************************************************
//#define LWIP_UPNP                       0

//*****************************************************************************
//
// ---------- PTPD options ----------
//
//*****************************************************************************
//#define LWIP_PTPD                       0

//*****************************************************************************
//
// ---------- AUTOIP options ----------
//
//*****************************************************************************
#define LWIP_AUTOIP                     0           // default is 0
#define LWIP_DHCP_AUTOIP_COOP           ((LWIP_DHCP) && (LWIP_AUTOIP))
                                                    // default is 0

//*****************************************************************************
//
// ---------- SNMP options ----------
//
//*****************************************************************************
//#define LWIP_SNMP                       0
//#define SNMP_CONCURRENT_REQUESTS        1
//#define SNMP_TRAP_DESTINATIONS          1
//#define SNMP_PRIVATE_MIB                0
//#define SNMP_SAFE_REQUESTS              1

//*****************************************************************************
//
// ---------- IGMP options ----------
//
//*****************************************************************************
//#define LWIP_IGMP                       0

//*****************************************************************************
//
// ---------- DNS options -----------
//
//*****************************************************************************
//#define LWIP_DNS                        0
//#define DNS_TABLE_SIZE                  4
//#define DNS_MAX_NAME_LENGTH             256
//#define DNS_MAX_SERVERS                 2
//#define DNS_DOES_NAME_CHECK             1
//#define DNS_USES_STATIC_BUF             1
//#define DNS_MSG_SIZE                    512

//*****************************************************************************
//
// ---------- UDP options ----------
//
//*****************************************************************************
#define LWIP_UDP                        1	// DHCP
//#define LWIP_UDPLITE                    0
#define UDP_TTL                         (IP_DEFAULT_TTL)

//*****************************************************************************
//
// ---------- TCP options ----------
//
//*****************************************************************************
//#define LWIP_TCP                        1
#define TCP_TTL                         (IP_DEFAULT_TTL)
#define TCP_WND                         4096   // default is 2048, okno miało wartosc 4096
//#define TCP_MAXRTX                      12
//#define TCP_SYNMAXRTX                   6
//#define TCP_QUEUE_OOSEQ                 1
#define TCP_MSS                        1460        // default is 128
//#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (4 * TCP_MSS)
                                                    // default is 256, was 6 *

/**
 * TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
 * as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work.
 */
#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
//#define TCP_SNDLOWAT                    (TCP_SND_BUF/2)
//#define TCP_LISTEN_BACKLOG              0
//#define TCP_DEFAULT_LISTEN_BACKLOG      0xff

//*****************************************************************************
//
// ---------- API options ----------
//
//*****************************************************************************
//#define LWIP_EVENT_API                  0
//#define LWIP_CALLBACK_API               1

//*****************************************************************************
//
// ---------- Pbuf options ----------
//
//*****************************************************************************
#define PBUF_LINK_HLEN                  16          // default is 14
#define PBUF_POOL_BUFSIZE               256
                                                    // default is LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)
#define ETH_PAD_SIZE                    2           // default is 0

//*****************************************************************************
//
// ---------- Network Interfaces options ----------
//
//*****************************************************************************
//#define LWIP_NETIF_HOSTNAME             0
//#define LWIP_NETIF_API                  0
//#define LWIP_NETIF_STATUS_CALLBACK      0
//#define LWIP_NETIF_LINK_CALLBACK        0
//#define LWIP_NETIF_HWADDRHINT           0

//*****************************************************************************
//
// ---------- LOOPIF options ----------
//
//*****************************************************************************
//#define LWIP_HAVE_LOOPIF                0
//#define LWIP_LOOPIF_MULTITHREADING      1

//*****************************************************************************
//
// ---------- Thread options ----------
//
//*****************************************************************************
#define TCPIP_THREAD_NAME              "tcpip_thread"
#define TCPIP_THREAD_STACKSIZE          300
#define TCPIP_THREAD_PRIO               7

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE                 10
//#define SLIPIF_THREAD_NAME             "slipif_loop"
//#define SLIPIF_THREAD_STACKSIZE         0
//#define SLIPIF_THREAD_PRIO              1
//#define PPP_THREAD_NAME                "pppMain"
//#define PPP_THREAD_STACKSIZE            0
//#define PPP_THREAD_PRIO                 1
#define DEFAULT_THREAD_NAME            "lwIP"
#define DEFAULT_THREAD_STACKSIZE        128
#define DEFAULT_THREAD_PRIO             4

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE       12

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE       12

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE       12

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE         12

//*****************************************************************************
//
// ---------- Sequential layer options ----------
//
//*****************************************************************************
//#define LWIP_TCPIP_CORE_LOCKING         0
#define LWIP_NETCONN                    1           // default is 1

//*****************************************************************************
//
// ---------- Socket Options ----------
//
//*****************************************************************************
#define LWIP_SOCKET                     0           // default is 1
//#define LWIP_COMPAT_SOCKETS             1
//#define LWIP_POSIX_SOCKETS_IO_NAMES     1

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#define LWIP_TCP_KEEPALIVE              1

/**
 * LWIP_SO_RCVTIMEO==1: Enable SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1				// time OUT <--------------------------------------------------
//#define LWIP_SO_RCVBUF                  0
//#define SO_REUSE                        0

//*****************************************************************************
//
// ---------- Statistics options ----------
//
//*****************************************************************************
#define LWIP_STATS                      1
#define LWIP_STATS_DISPLAY              1
#define LINK_STATS                      0
#define ETHARP_STATS                    0 //(LWIP_ARP)
#define IP_STATS                        0
#define IPFRAG_STATS                    0//(IP_REASSEMBLY || IP_FRAG)
#define ICMP_STATS                      0
#define IGMP_STATS                      0//(LWIP_IGMP)
#define UDP_STATS                       0//(LWIP_UDP)
#define TCP_STATS                       0//(LWIP_TCP)
#define MEM_STATS                       0
#define MEMP_STATS                      1
#define SYS_STATS                       0

//*****************************************************************************
//
// ---------- PPP options ----------
//
//*****************************************************************************
#define PPP_SUPPORT                     0
#define PPPOE_SUPPORT                   0
//#define PPPOS_SUPPORT                   PPP_SUPPORT

#if PPP_SUPPORT
//#define NUM_PPP                         1
//#define PAP_SUPPORT                     0
//#define CHAP_SUPPORT                    0
//#define MSCHAP_SUPPORT                  0
//#define CBCP_SUPPORT                    0
//#define CCP_SUPPORT                     0
//#define VJ_SUPPORT                      0
//#define MD5_SUPPORT                     0
//#define FSM_DEFTIMEOUT                  6
//#define FSM_DEFMAXTERMREQS              2
//#define FSM_DEFMAXCONFREQS              10
//#define FSM_DEFMAXNAKLOOPS              5
//#define UPAP_DEFTIMEOUT                 6
//#define UPAP_DEFREQTIME                 30
//#define CHAP_DEFTIMEOUT                 6
//#define CHAP_DEFTRANSMITS               10
//#define LCP_ECHOINTERVAL                0
//#define LCP_MAXECHOFAILS                3
//#define PPP_MAXIDLEFLAG                 100

//#define PPP_MAXMTU                      1500
//#define PPP_DEFMRU                      296
#endif

//*****************************************************************************
//
// ---------- checksum options ----------
//
//*****************************************************************************
//#define CHECKSUM_GEN_IP                 1
//#define CHECKSUM_GEN_UDP                1
//#define CHECKSUM_GEN_TCP                1
//#define CHECKSUM_CHECK_IP               1
//#define CHECKSUM_CHECK_UDP              1
//#define CHECKSUM_CHECK_TCP              1

//*****************************************************************************
//
// ---------- Debugging options ----------
//
//*****************************************************************************
#define U8_F "c"
#define S8_F "c"
#define X8_F "x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

//extern signed int printfuart(const char *pFormat, ...);
//#define LWIP_PLATFORM_DIAG(x) {printfuart x;}

//extern void LWIPDebug(const char *pcString, ...);
//#define LWIP_PLATFORM_DIAG(x) {LWIPDebug x;}
#define LWIP_PLATFORM_DIAG(x)


#define LWIP_DEBUG						0


//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
//#define LWIP_DBG_MIN_LEVEL             LWIP_DBG_LEVEL_OFF
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

#define LWIP_DBG_TYPES_ON              LWIP_DBG_ON
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH)

#define ETHARP_DEBUG                   LWIP_DBG_OFF     // default is OFF
#define NETIF_DEBUG                    LWIP_DBG_OFF     // default is OFF
#define PBUF_DEBUG                     LWIP_DBG_OFF
#define API_LIB_DEBUG                  LWIP_DBG_OFF
#define API_MSG_DEBUG                  LWIP_DBG_OFF
#define SOCKETS_DEBUG                  LWIP_DBG_OFF
#define ICMP_DEBUG                     LWIP_DBG_OFF
#define IGMP_DEBUG                     LWIP_DBG_OFF
#define INET_DEBUG                     LWIP_DBG_OFF
#define IP_DEBUG                       LWIP_DBG_OFF     // default is OFF
#define IP_REASS_DEBUG                 LWIP_DBG_OFF
#define RAW_DEBUG                      LWIP_DBG_OFF
#define MEM_DEBUG                      LWIP_DBG_ON
#define MEMP_DEBUG                     LWIP_DBG_ON
#define SYS_DEBUG                      LWIP_DBG_OFF
#define TCP_DEBUG                      LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                LWIP_DBG_OFF
#define TCP_FR_DEBUG                   LWIP_DBG_ON
#define TCP_RTO_DEBUG                  LWIP_DBG_ON
#define TCP_CWND_DEBUG                 LWIP_DBG_ON
#define TCP_WND_DEBUG                  LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG               LWIP_DBG_ON
#define TCP_RST_DEBUG                  LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                 LWIP_DBG_ON
#define UDP_DEBUG                      LWIP_DBG_OFF     // default is OFF
#define TCPIP_DEBUG                    LWIP_DBG_OFF
#define PPP_DEBUG                      LWIP_DBG_OFF
#define SLIP_DEBUG                     LWIP_DBG_OFF
#define DHCP_DEBUG                     LWIP_DBG_OFF     // default is OFF
#define AUTOIP_DEBUG                   LWIP_DBG_OFF
#define SNMP_MSG_DEBUG                 LWIP_DBG_OFF
#define SNMP_MIB_DEBUG                 LWIP_DBG_OFF
#define DNS_DEBUG                      LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */
