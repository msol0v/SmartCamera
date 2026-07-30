cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)
# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	MBEDTLS_CONFIG_FILE=<mbedtls_config.h> 
	USE_HAL_DRIVER 
	STM32F746xx
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../LWIP/App
    ${CMAKE_CURRENT_SOURCE_DIR}/../../LWIP/Target
    ${CMAKE_CURRENT_SOURCE_DIR}/../../MBEDTLS/App
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/system
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/BSP/Components/lan8742
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/netif/ppp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/apps/http
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/include/mbedtls
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/lwip
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/lwip/apps
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/lwip/priv
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/lwip/prot
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/netif
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/compat/posix
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/compat/posix/net
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/compat/posix/sys
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/include/compat/stdc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/system/arch
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../MBEDTLS/Target/hardware_rng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../MBEDTLS/App/mbedtls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../MBEDTLS/App/net_sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../LWIP/Target/ethernetif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../LWIP/App/lwip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/freertos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f7xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f7xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f7xx_hal_timebase_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f746xx.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/system_stm32f7xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_eth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_qspi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart_ex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/BSP/Components/lan8742/lan8742.c
)

# Drivers Midllewares


set(FreeRTOS_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c
)
set(LwIP_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/ccp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/chap_ms.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/chap-md5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/chap-new.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/demand.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/eap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/eui64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/fsm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/ipcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/ipv6cp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/lcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/magic.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/mppe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/multilink.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/ppp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/pppapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/pppcrypt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/pppoe.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/pppol2tp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/pppos.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/upap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ppp/vj.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/bridgeif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/bridgeif_fdb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/ethernet.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/lowpan6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/lowpan6_ble.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/lowpan6_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/slipif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/netif/zepif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/apps/http/httpd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/apps/http/fs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/netbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/api_lib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/api_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/netifapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/if_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/netdb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/tcpip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/sockets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/api/err.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/dns.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/tcp_out.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/sys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/altcp_tcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/udp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/timeouts.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/pbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/netif.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/altcp_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/inet_chksum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/tcp_in.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/memp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/def.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/altcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/tcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/raw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/icmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/dhcp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/igmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/etharp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/autoip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_frag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_frag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/mld6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/ethip6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6_addr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/ip6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/icmp6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/inet6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/dhcp6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/core/ipv6/nd6.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/system/OS/sys_arch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/LwIP/src/apps/mqtt/mqtt.c
)
set(mbedTLS_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/aesni.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/arc4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/asn1parse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/asn1write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/bignum.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/blowfish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/camellia.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ccm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/certs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/chacha20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/chachapoly.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/cipher.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/cipher_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ctr_drbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/des.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/dhm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ecdh.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ecdsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ecp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ecp_curves.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/entropy.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/entropy_poll.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/error.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/gcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/hkdf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/hmac_drbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/md.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/md5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/md_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/memory_buffer_alloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/oid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/padlock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pkcs12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pkcs5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pkparse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pkwrite.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/pk_wrap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/platform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/platform_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/poly1305.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ripemd160.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/rsa.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/rsa_internal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/sha256.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/sha512.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_cache.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_ciphersuites.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_cookie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_srv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_ticket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/ssl_tls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/version.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/version_features.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509write_crt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509write_csr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509_crl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509_crt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/x509_csr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/Third_Party/mbedTLS/library/xtea.c
)

# Link directories setup
set(MX_LINK_DIRS

)
# Project static libraries
set(MX_LINK_LIBS 
    STM32_Drivers
    ${TOOLCHAIN_LINK_LIBRARIES}
    FreeRTOS	LwIP	mbedTLS	
)
# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)


# Create FreeRTOS static library
add_library(FreeRTOS OBJECT)
target_sources(FreeRTOS PRIVATE ${FreeRTOS_Src})
target_link_libraries(FreeRTOS PUBLIC stm32cubemx)

# Create LwIP static library
add_library(LwIP OBJECT)
target_sources(LwIP PRIVATE ${LwIP_Src})
target_link_libraries(LwIP PUBLIC stm32cubemx)

# Create mbedTLS static library
add_library(mbedTLS OBJECT)
target_sources(mbedTLS PRIVATE ${mbedTLS_Src})
target_link_libraries(mbedTLS PUBLIC stm32cubemx)

# Add STM32CubeMX generated application sources to the project
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${MX_Application_Src})

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE ${MX_LINK_DIRS})

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} ${MX_LINK_LIBS})

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)

# Validate that STM32CubeMX code is compatible with C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
