/* PipeWire
 * Copyright (C) 2018 Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __PIPEWIRE_VERSION_H__
#define __PIPEWIRE_VERSION_H__

/* WARNING: Make sure to edit the real source file version.h.in! */

#ifdef __cplusplus
extern "C" {
#endif

/** Return the version of the header files. Keep in mind that this is
a macro and not a function, so it is impossible to get the pointer of
it. */
#define pw_get_headers_version() ("0.2.7")

/** Return the version of the library the current application is
 * linked to. */
const char* pw_get_library_version(void);

/** The current API version. Versions prior to 0.2.0 have
 * PW_API_VERSION undefined. Please note that this is only ever
 * increased on incompatible API changes!  */
#define PW_API_VERSION "0.2"

/** The major version of PipeWire. \since 0.2.0 */
#define PW_MAJOR 0

/** The minor version of PipeWire. \since 0.2.0 */
#define PW_MINOR 2

/** The micro version of PipeWire. \since 0.2.0 */
#define PW_MICRO 7

/** Evaluates to TRUE if the PipeWire library version is equal or
 * newer than the specified. \since 0.2.0 */
#define PW_CHECK_VERSION(major,minor,micro)                             \
    ((PW_MAJOR > (major)) ||                                            \
     (PW_MAJOR == (major) && PW_MINOR > (minor)) ||                     \
     (PW_MAJOR == (major) && PW_MINOR == (minor) && PW_MICRO >= (micro)))

#ifdef __cplusplus
}
#endif

#endif /* __PIPEWIRE_VERION_H__ */
