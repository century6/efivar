/*
 * libefivar - library for the manipulation of EFI variables
 * Copyright 2012-2014 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EFIVAR_H
#define EFIVAR_H 1

#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <byteswap.h>

typedef struct {
	uint32_t	a;
	uint16_t	b;
	uint16_t	c;
	uint16_t	d;
	uint8_t		e[6];
} efi_guid_t;

#ifndef EFIVAR_BUILD_ENVIRONMENT
#include "efivar-guids.h"
#endif

#define EFI_GUID(a,b,c,d,e0,e1,e2,e3,e4,e5) \
((efi_guid_t) {(a), (b), (c), bswap_16(d), { (e0), (e1), (e2), (e3), (e4), (e5) }})

#define EFI_GLOBAL_GUID EFI_GUID(0x8be4df61,0x93ca,0x11d2,0xaa0d,0x00,0xe0,0x98,0x03,0x2b,0x8c)

#define EFI_VARIABLE_NON_VOLATILE	0x0000000000000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS	0x0000000000000002
#define EFI_VARIABLE_RUNTIME_ACCESS	0x0000000000000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD	0x0000000000000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS	0x0000000000000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x0000000000000020
#define EFI_VARIABLE_APPEND_WRITE	0x0000000000000040

extern int efi_variables_supported(void);
extern int efi_get_variable_size(efi_guid_t guid, const char *name,
				 size_t *size)
				__attribute__ ((__nonnull__ (2, 3)));
extern int efi_get_variable_attributes(efi_guid_t, const char *name,
				       uint32_t *attributes)
				__attribute__ ((__nonnull__ (2, 3)));
extern int efi_get_variable(efi_guid_t guid, const char *name, uint8_t **data,
			    size_t *data_size, uint32_t *attributes)
				__attribute__ ((__nonnull__ (2, 3, 4, 5)));
extern int efi_del_variable(efi_guid_t guid, const char *name)
				__attribute__ ((__nonnull__ (2)));
extern int _efi_set_variable(efi_guid_t guid, const char *name,
			     uint8_t *data, size_t data_size,
			     uint32_t attributes, mode_t mode)
			    __attribute__ ((__nonnull__ (2, 3)));
extern int _efi_set_variable_variadic(efi_guid_t guid, const char *name,
				      uint8_t *data, size_t data_size,
				      uint32_t attributes, ...);
				     __attribute__ ((__nonnull__ (2, 3)))
extern inline int
__attribute__ ((__gnu_inline__))
__attribute__ ((__artificial__))
efi_set_variable(efi_guid_t guid, const char *name,
			       uint8_t *data, size_t data_size,
			       uint32_t attributes, ...)
{
	if (__builtin_va_arg_pack_len() != 0 &&
			__builtin_va_arg_pack_len() != 1) {
		errno = EINVAL;
		return -1;
	}

	if (__va_arg_pack_len() == 0)
		return _efi_set_variable(guid, name, data, data_size,
					 attributes, 0644);

	return _efi_set_variable_variadic(guid, name, data, data_size,
					  attributes, __builtin_va_arg_pack());
}

extern int efi_append_variable(efi_guid_t guid, const char *name,
			       uint8_t *data, size_t data_size,
			       uint32_t attributes)
			      __attribute__ ((__nonnull__ (2, 3)));
extern int efi_get_next_variable_name(efi_guid_t **guid, char **name)
			      __attribute__ ((__nonnull__ (1, 2)));
extern int efi_chmod_variable(efi_guid_t guid, const char *name, mode_t mode)
			      __attribute__ ((__nonnull__ (2)));

extern int efi_str_to_guid(const char *s, efi_guid_t *guid)
			  __attribute__ ((__nonnull__ (1, 2)));
extern int efi_guid_to_str(const efi_guid_t *guid, char **sp)
			  __attribute__ ((__nonnull__ (1)));

extern int efi_guid_to_symbol(efi_guid_t *guid, char **symbol)
			     __attribute__ ((__nonnull__ (1, 2)));
extern int efi_guid_to_name(efi_guid_t *guid, char **name)
			   __attribute__ ((__nonnull__ (1, 2)));
extern int efi_name_to_guid(const char *name, efi_guid_t *guid)
			   __attribute__ ((__nonnull__ (1, 2)));
extern int efi_symbol_to_guid(const char *symbol, efi_guid_t *guid)
			     __attribute__ ((__nonnull__ (1, 2)));

static inline int
__attribute__ ((unused))
__attribute__ ((__nonnull__ (1, 2)))
efi_guid_cmp(const efi_guid_t *a, const efi_guid_t *b)
{
	return memcmp(a, b, sizeof (efi_guid_t));
}

extern efi_guid_t efi_guid_zero;

static inline int
__attribute__ ((unused))
__attribute__ ((__nonnull__ (1)))
efi_guid_is_zero(const efi_guid_t *guid)
{
	return !efi_guid_cmp(guid,&efi_guid_zero);
}
extern int
efi_guid_is_empty(const efi_guid_t *guid)
__attribute__ ((unused))
__attribute__ ((__nonnull__ (1)))
__attribute__ ((weak, alias ("efi_guid_is_zero")));

#endif /* EFIVAR_H */
