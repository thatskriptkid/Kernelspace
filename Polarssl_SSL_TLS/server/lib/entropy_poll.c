/*
 *  Platform-specific and custom entropy polling functions
 *
 *  Copyright (C) 2006-2014, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#if !defined(POLARSSL_CONFIG_FILE)
#include "config.h"
#else
#include POLARSSL_CONFIG_FILE
#endif

#if defined(POLARSSL_LINUX_KERNEL)
#include "polarssl_kernel_support.h"
#endif

#if defined(POLARSSL_ENTROPY_C)

#include "entropy.h"
#include "entropy_poll.h"
/*
#if defined(POLARSSL_TIMING_C)
#include "timing.h"
tsk | 10.09
*/
#if defined(POLARSSL_HAVEGE_C)
#include "havege.h"
#endif

#if !defined(POLARSSL_NO_PLATFORM_ENTROPY)
#if defined(_WIN32) && !defined(EFIX64) && !defined(EFI32)

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0400
#endif

#if !defined(POLARSSL_LINUX_KERNEL)
#include <windows.h>
#include <wincrypt.h>
#endif

int platform_entropy_poll( void *data, unsigned char *output, size_t len,
                           size_t *olen )
{
    HCRYPTPROV provider;
    ((void) data);
    *olen = 0;

    if( CryptAcquireContext( &provider, NULL, NULL,
                              PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) == FALSE )
    {
        return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );
    }

    if( CryptGenRandom( provider, (DWORD) len, output ) == FALSE )
        return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );

    CryptReleaseContext( provider, 0 );
    *olen = len;

    return( 0 );
}
 
#else /* _WIN32 && !EFIX64 && !EFI32 */

#if !defined(POLARSSL_LINUX_KERNEL)
#include <stdio.h>
#endif


int platform_entropy_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    //FILE *file;
    
    struct  file *filp;
    loff_t  pos;
    size_t  ret;
    
    ((void) data);
    
    mm_segment_t old_fs;
    
    old_fs = get_fs();
	
	set_fs(get_ds());

    *olen = 0;
    
	pos = 0;
	
	/*
    file = fopen( "/dev/urandom", "rb" );
    
    if( file == NULL )
        return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );
    */
    
    filp = filp_open("/dev/urandom",O_RDONLY,0);
    
    if(IS_ERR(filp)) 
		return POLARSSL_ERR_ENTROPY_SOURCE_FAILED;
	
	/*
    ret = fread( output, 1, len, file );
    if( ret != len )
    {
        fclose( file );
        return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );
    }

    fclose( file );
    */
    
    ret = vfs_read(filp, output, len, &pos); 
	
	if (ret!=len) {
		filp_close(filp,NULL);
		return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );
	}
	
	*olen = len;
	
	filp_close(filp,NULL);
	
	set_fs(old_fs);
	
    return( 0 );
}

#endif /* _WIN32 && !EFIX64 && !EFI32 */

#endif /* !POLARSSL_NO_PLATFORM_ENTROPY */
/*
#if defined(POLARSSL_TIMING_C)
int hardclock_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    unsigned long timer = hardclock();
    ((void) data);
    *olen = 0;

    if( len < sizeof(unsigned long) )
        return( 0 );

    memcpy( output, &timer, sizeof(unsigned long) );
    *olen = sizeof(unsigned long);

    return( 0 );
}
*/
//#endif /* POLARSSL_TIMING_C */

#if defined(POLARSSL_HAVEGE_C)
int havege_poll( void *data,
                 unsigned char *output, size_t len, size_t *olen )
{
    havege_state *hs = (havege_state *) data;
    *olen = 0;

    if( havege_random( hs, output, len ) != 0 )
        return( POLARSSL_ERR_ENTROPY_SOURCE_FAILED );

    *olen = len;

    return( 0 );
}

#endif /* POLARSSL_HAVEGE_C */

#endif /* POLARSSL_ENTROPY_C */
