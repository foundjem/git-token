begin_unit
begin_comment
comment|/* obstack.c - subroutines used implicitly by object stack macros    Copyright (C) 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1996, 1997, 1998,    1999, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.    This file is part of the GNU C Library.     The GNU C Library is free software; you can redistribute it and/or    modify it under the terms of the GNU Lesser General Public    License as published by the Free Software Foundation; either    version 2.1 of the License, or (at your option) any later version.     The GNU C Library is distributed in the hope that it will be useful,    but WITHOUT ANY WARRANTY; without even the implied warranty of    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    Lesser General Public License for more details.     You should have received a copy of the GNU Lesser General Public    License along with the GNU C Library; if not, write to the Free    Software Foundation, Inc., 51 Franklin Street, Fifth Floor,    Boston, MA 02110-1301, USA.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|<gettext.h>
end_include
begin_include
include|#
directive|include
file|"obstack.h"
end_include
begin_comment
comment|/* NOTE BEFORE MODIFYING THIS FILE: This version number must be    incremented whenever callers compiled using an old obstack.h can no    longer properly call the functions in this obstack.c.  */
end_comment
begin_define
DECL|macro|OBSTACK_INTERFACE_VERSION
define|#
directive|define
name|OBSTACK_INTERFACE_VERSION
value|1
end_define
begin_comment
comment|/* Comment out all this code if we are using the GNU C Library, and are not    actually compiling the library itself, and the installed library    supports the same library interface we do.  This code is part of the GNU    C Library, but also included in many other GNU distributions.  Compiling    and linking in this code is a waste when using the GNU C library    (especially if it is a shared library).  Rather than having every GNU    program understand `configure --with-gnu-libc' and omit the object    files, it is simpler to just do this in the source for each such file.  */
end_comment
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_comment
comment|/* Random thing to get __GNU_LIBRARY__.  */
end_comment
begin_if
if|#
directive|if
operator|!
name|defined
name|_LIBC
operator|&&
name|defined
name|__GNU_LIBRARY__
operator|&&
name|__GNU_LIBRARY__
operator|>
literal|1
end_if
begin_include
include|#
directive|include
file|<gnu-versions.h>
end_include
begin_if
if|#
directive|if
name|_GNU_OBSTACK_INTERFACE_VERSION
operator|==
name|OBSTACK_INTERFACE_VERSION
end_if
begin_define
DECL|macro|ELIDE_CODE
define|#
directive|define
name|ELIDE_CODE
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_include
include|#
directive|include
file|<stddef.h>
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|ELIDE_CODE
end_ifndef
begin_if
if|#
directive|if
name|HAVE_INTTYPES_H
end_if
begin_include
include|#
directive|include
file|<inttypes.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_if
if|#
directive|if
name|HAVE_STDINT_H
operator|||
name|defined
name|_LIBC
end_if
begin_include
include|#
directive|include
file|<stdint.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* Determine default alignment.  */
end_comment
begin_union
DECL|union|fooround
union|union
name|fooround
block|{
DECL|member|i
name|uintmax_t
name|i
decl_stmt|;
DECL|member|d
name|long
name|double
name|d
decl_stmt|;
DECL|member|p
name|void
modifier|*
name|p
decl_stmt|;
block|}
union|;
end_union
begin_struct
DECL|struct|fooalign
struct|struct
name|fooalign
block|{
DECL|member|c
name|char
name|c
decl_stmt|;
DECL|member|u
name|union
name|fooround
name|u
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/* If malloc were really smart, it would round addresses to DEFAULT_ALIGNMENT.    But in fact it might be less smart and round addresses to as much as    DEFAULT_ROUNDING.  So we prepare for it to do that.  */
end_comment
begin_enum
enum|enum
block|{
DECL|enumerator|DEFAULT_ALIGNMENT
name|DEFAULT_ALIGNMENT
init|=
name|offsetof
argument_list|(
expr|struct
name|fooalign
argument_list|,
name|u
argument_list|)
block|,
DECL|enumerator|DEFAULT_ROUNDING
name|DEFAULT_ROUNDING
init|=
expr|sizeof
operator|(
expr|union
name|fooround
operator|)
block|}
enum|;
end_enum
begin_comment
comment|/* When we copy a long block of data, this is the unit to do it with.    On some machines, copying successive ints does not work;    in such a case, redefine COPYING_UNIT to `long' (if that works)    or `char' as a last resort.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|COPYING_UNIT
end_ifndef
begin_define
DECL|macro|COPYING_UNIT
define|#
directive|define
name|COPYING_UNIT
value|int
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* The functions allocating more room by calling `obstack_chunk_alloc'    jump to the handler pointed to by `obstack_alloc_failed_handler'.    This can be set to a user defined function which should either    abort gracefully or use longjump - but shouldn't return.  This    variable by default points to the internal function    `print_and_abort'.  */
end_comment
begin_function_decl
specifier|static
name|void
name|print_and_abort
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
DECL|variable|obstack_alloc_failed_handler
name|void
function_decl|(
modifier|*
name|obstack_alloc_failed_handler
function_decl|)
parameter_list|(
name|void
parameter_list|)
init|=
name|print_and_abort
function_decl|;
end_function_decl
begin_ifdef
ifdef|#
directive|ifdef
name|_LIBC
end_ifdef
begin_if
if|#
directive|if
name|SHLIB_COMPAT
argument_list|(
name|libc
operator|,
name|GLIBC_2_0
operator|,
name|GLIBC_2_3_4
argument_list|)
end_if
begin_comment
comment|/* A looong time ago (before 1994, anyway; we're not sure) this global variable    was used by non-GNU-C macros to avoid multiple evaluation.  The GNU C    library still exports it because somebody might use it.  */
end_comment
begin_decl_stmt
DECL|variable|_obstack_compat
name|struct
name|obstack
modifier|*
name|_obstack_compat
decl_stmt|;
end_decl_stmt
begin_expr_stmt
name|compat_symbol
argument_list|(
name|libc
argument_list|,
name|_obstack_compat
argument_list|,
name|_obstack
argument_list|,
name|GLIBC_2_0
argument_list|)
expr_stmt|;
end_expr_stmt
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* Define a macro that either calls functions with the traditional malloc/free    calling interface, or calls functions with the mmalloc/mfree interface    (that adds an extra first argument), based on the state of use_extra_arg.    For free, do not use ?:, since some compilers, like the MIPS compilers,    do not allow (expr) ? void : void.  */
end_comment
begin_define
DECL|macro|CALL_CHUNKFUN
define|#
directive|define
name|CALL_CHUNKFUN
parameter_list|(
name|h
parameter_list|,
name|size
parameter_list|)
define|\
value|(((h) -> use_extra_arg) \    ? (*(h)->chunkfun) ((h)->extra_arg, (size)) \    : (*(struct _obstack_chunk *(*) (long)) (h)->chunkfun) ((size)))
end_define
begin_define
DECL|macro|CALL_FREEFUN
define|#
directive|define
name|CALL_FREEFUN
parameter_list|(
name|h
parameter_list|,
name|old_chunk
parameter_list|)
define|\
value|do { \     if ((h) -> use_extra_arg) \       (*(h)->freefun) ((h)->extra_arg, (old_chunk)); \     else \       (*(void (*) (void *)) (h)->freefun) ((old_chunk)); \   } while (0)
end_define
begin_escape
end_escape
begin_comment
comment|/* Initialize an obstack H for use.  Specify chunk size SIZE (0 means default).    Objects start on multiples of ALIGNMENT (0 means use default).    CHUNKFUN is the function to use to allocate chunks,    and FREEFUN the function to free them.     Return nonzero if successful, calls obstack_alloc_failed_handler if    allocation fails.  */
end_comment
begin_function
name|int
DECL|function|_obstack_begin
name|_obstack_begin
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|int
name|size
parameter_list|,
name|int
name|alignment
parameter_list|,
name|void
modifier|*
function_decl|(
modifier|*
name|chunkfun
function_decl|)
parameter_list|(
name|long
parameter_list|)
parameter_list|,
name|void
function_decl|(
modifier|*
name|freefun
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|)
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|chunk
decl_stmt|;
comment|/* points to new chunk */
if|if
condition|(
name|alignment
operator|==
literal|0
condition|)
name|alignment
operator|=
name|DEFAULT_ALIGNMENT
expr_stmt|;
if|if
condition|(
name|size
operator|==
literal|0
condition|)
comment|/* Default size is what GNU malloc can fit in a 4096-byte block.  */
block|{
comment|/* 12 is sizeof (mhead) and 4 is EXTRA from GNU malloc. 	 Use the values for range checking, because if range checking is off, 	 the extra bytes won't be missed terribly, but if range checking is on 	 and we used a larger request, a whole extra 4096 bytes would be 	 allocated.  	 These number are irrelevant to the new GNU malloc.  I suspect it is 	 less sensitive to the size of the request.  */
name|int
name|extra
init|=
operator|(
operator|(
operator|(
operator|(
literal|12
operator|+
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|&
operator|~
operator|(
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|)
operator|+
literal|4
operator|+
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|&
operator|~
operator|(
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|)
decl_stmt|;
name|size
operator|=
literal|4096
operator|-
name|extra
expr_stmt|;
block|}
name|h
operator|->
name|chunkfun
operator|=
operator|(
expr|struct
name|_obstack_chunk
operator|*
call|(
modifier|*
call|)
argument_list|(
name|void
operator|*
argument_list|,
name|long
argument_list|)
operator|)
name|chunkfun
expr_stmt|;
name|h
operator|->
name|freefun
operator|=
operator|(
name|void
argument_list|(
operator|*
argument_list|)
argument_list|(
name|void
operator|*
argument_list|,
expr|struct
name|_obstack_chunk
operator|*
argument_list|)
operator|)
name|freefun
expr_stmt|;
name|h
operator|->
name|chunk_size
operator|=
name|size
expr_stmt|;
name|h
operator|->
name|alignment_mask
operator|=
name|alignment
operator|-
literal|1
expr_stmt|;
name|h
operator|->
name|use_extra_arg
operator|=
literal|0
expr_stmt|;
name|chunk
operator|=
name|h
operator|->
name|chunk
operator|=
name|CALL_CHUNKFUN
argument_list|(
name|h
argument_list|,
name|h
operator|->
name|chunk_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|chunk
condition|)
call|(
modifier|*
name|obstack_alloc_failed_handler
call|)
argument_list|()
expr_stmt|;
name|h
operator|->
name|next_free
operator|=
name|h
operator|->
name|object_base
operator|=
name|__PTR_ALIGN
argument_list|(
operator|(
name|char
operator|*
operator|)
name|chunk
argument_list|,
name|chunk
operator|->
name|contents
argument_list|,
name|alignment
operator|-
literal|1
argument_list|)
expr_stmt|;
name|h
operator|->
name|chunk_limit
operator|=
name|chunk
operator|->
name|limit
operator|=
operator|(
name|char
operator|*
operator|)
name|chunk
operator|+
name|h
operator|->
name|chunk_size
expr_stmt|;
name|chunk
operator|->
name|prev
operator|=
name|NULL
expr_stmt|;
comment|/* The initial chunk now contains no empty object.  */
name|h
operator|->
name|maybe_empty_object
operator|=
literal|0
expr_stmt|;
name|h
operator|->
name|alloc_failed
operator|=
literal|0
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
name|int
DECL|function|_obstack_begin_1
name|_obstack_begin_1
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|int
name|size
parameter_list|,
name|int
name|alignment
parameter_list|,
name|void
modifier|*
function_decl|(
modifier|*
name|chunkfun
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|,
name|long
parameter_list|)
parameter_list|,
name|void
function_decl|(
modifier|*
name|freefun
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|)
parameter_list|,
name|void
modifier|*
name|arg
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|chunk
decl_stmt|;
comment|/* points to new chunk */
if|if
condition|(
name|alignment
operator|==
literal|0
condition|)
name|alignment
operator|=
name|DEFAULT_ALIGNMENT
expr_stmt|;
if|if
condition|(
name|size
operator|==
literal|0
condition|)
comment|/* Default size is what GNU malloc can fit in a 4096-byte block.  */
block|{
comment|/* 12 is sizeof (mhead) and 4 is EXTRA from GNU malloc. 	 Use the values for range checking, because if range checking is off, 	 the extra bytes won't be missed terribly, but if range checking is on 	 and we used a larger request, a whole extra 4096 bytes would be 	 allocated.  	 These number are irrelevant to the new GNU malloc.  I suspect it is 	 less sensitive to the size of the request.  */
name|int
name|extra
init|=
operator|(
operator|(
operator|(
operator|(
literal|12
operator|+
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|&
operator|~
operator|(
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|)
operator|+
literal|4
operator|+
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|&
operator|~
operator|(
name|DEFAULT_ROUNDING
operator|-
literal|1
operator|)
operator|)
decl_stmt|;
name|size
operator|=
literal|4096
operator|-
name|extra
expr_stmt|;
block|}
name|h
operator|->
name|chunkfun
operator|=
operator|(
expr|struct
name|_obstack_chunk
operator|*
call|(
modifier|*
call|)
argument_list|(
name|void
operator|*
argument_list|,
name|long
argument_list|)
operator|)
name|chunkfun
expr_stmt|;
name|h
operator|->
name|freefun
operator|=
operator|(
name|void
argument_list|(
operator|*
argument_list|)
argument_list|(
name|void
operator|*
argument_list|,
expr|struct
name|_obstack_chunk
operator|*
argument_list|)
operator|)
name|freefun
expr_stmt|;
name|h
operator|->
name|chunk_size
operator|=
name|size
expr_stmt|;
name|h
operator|->
name|alignment_mask
operator|=
name|alignment
operator|-
literal|1
expr_stmt|;
name|h
operator|->
name|extra_arg
operator|=
name|arg
expr_stmt|;
name|h
operator|->
name|use_extra_arg
operator|=
literal|1
expr_stmt|;
name|chunk
operator|=
name|h
operator|->
name|chunk
operator|=
name|CALL_CHUNKFUN
argument_list|(
name|h
argument_list|,
name|h
operator|->
name|chunk_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|chunk
condition|)
call|(
modifier|*
name|obstack_alloc_failed_handler
call|)
argument_list|()
expr_stmt|;
name|h
operator|->
name|next_free
operator|=
name|h
operator|->
name|object_base
operator|=
name|__PTR_ALIGN
argument_list|(
operator|(
name|char
operator|*
operator|)
name|chunk
argument_list|,
name|chunk
operator|->
name|contents
argument_list|,
name|alignment
operator|-
literal|1
argument_list|)
expr_stmt|;
name|h
operator|->
name|chunk_limit
operator|=
name|chunk
operator|->
name|limit
operator|=
operator|(
name|char
operator|*
operator|)
name|chunk
operator|+
name|h
operator|->
name|chunk_size
expr_stmt|;
name|chunk
operator|->
name|prev
operator|=
name|NULL
expr_stmt|;
comment|/* The initial chunk now contains no empty object.  */
name|h
operator|->
name|maybe_empty_object
operator|=
literal|0
expr_stmt|;
name|h
operator|->
name|alloc_failed
operator|=
literal|0
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/* Allocate a new current chunk for the obstack *H    on the assumption that LENGTH bytes need to be added    to the current object, or a new object of length LENGTH allocated.    Copies any partial object from the end of the old chunk    to the beginning of the new one.  */
end_comment
begin_function
name|void
DECL|function|_obstack_newchunk
name|_obstack_newchunk
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|int
name|length
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|old_chunk
init|=
name|h
operator|->
name|chunk
decl_stmt|;
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|new_chunk
decl_stmt|;
specifier|register
name|long
name|new_size
decl_stmt|;
specifier|register
name|long
name|obj_size
init|=
name|h
operator|->
name|next_free
operator|-
name|h
operator|->
name|object_base
decl_stmt|;
specifier|register
name|long
name|i
decl_stmt|;
name|long
name|already
decl_stmt|;
name|char
modifier|*
name|object_base
decl_stmt|;
comment|/* Compute size for new chunk.  */
name|new_size
operator|=
operator|(
name|obj_size
operator|+
name|length
operator|)
operator|+
operator|(
name|obj_size
operator|>>
literal|3
operator|)
operator|+
name|h
operator|->
name|alignment_mask
operator|+
literal|100
expr_stmt|;
if|if
condition|(
name|new_size
operator|<
name|h
operator|->
name|chunk_size
condition|)
name|new_size
operator|=
name|h
operator|->
name|chunk_size
expr_stmt|;
comment|/* Allocate and initialize the new chunk.  */
name|new_chunk
operator|=
name|CALL_CHUNKFUN
argument_list|(
name|h
argument_list|,
name|new_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|new_chunk
condition|)
call|(
modifier|*
name|obstack_alloc_failed_handler
call|)
argument_list|()
expr_stmt|;
name|h
operator|->
name|chunk
operator|=
name|new_chunk
expr_stmt|;
name|new_chunk
operator|->
name|prev
operator|=
name|old_chunk
expr_stmt|;
name|new_chunk
operator|->
name|limit
operator|=
name|h
operator|->
name|chunk_limit
operator|=
operator|(
name|char
operator|*
operator|)
name|new_chunk
operator|+
name|new_size
expr_stmt|;
comment|/* Compute an aligned object_base in the new chunk */
name|object_base
operator|=
name|__PTR_ALIGN
argument_list|(
operator|(
name|char
operator|*
operator|)
name|new_chunk
argument_list|,
name|new_chunk
operator|->
name|contents
argument_list|,
name|h
operator|->
name|alignment_mask
argument_list|)
expr_stmt|;
comment|/* Move the existing object to the new chunk.      Word at a time is fast and is safe if the object      is sufficiently aligned.  */
if|if
condition|(
name|h
operator|->
name|alignment_mask
operator|+
literal|1
operator|>=
name|DEFAULT_ALIGNMENT
condition|)
block|{
for|for
control|(
name|i
operator|=
name|obj_size
operator|/
sizeof|sizeof
argument_list|(
name|COPYING_UNIT
argument_list|)
operator|-
literal|1
init|;
name|i
operator|>=
literal|0
condition|;
name|i
operator|--
control|)
operator|(
operator|(
name|COPYING_UNIT
operator|*
operator|)
name|object_base
operator|)
index|[
name|i
index|]
operator|=
operator|(
operator|(
name|COPYING_UNIT
operator|*
operator|)
name|h
operator|->
name|object_base
operator|)
index|[
name|i
index|]
expr_stmt|;
comment|/* We used to copy the odd few remaining bytes as one extra COPYING_UNIT, 	 but that can cross a page boundary on a machine 	 which does not do strict alignment for COPYING_UNITS.  */
name|already
operator|=
name|obj_size
operator|/
sizeof|sizeof
argument_list|(
name|COPYING_UNIT
argument_list|)
operator|*
sizeof|sizeof
argument_list|(
name|COPYING_UNIT
argument_list|)
expr_stmt|;
block|}
else|else
name|already
operator|=
literal|0
expr_stmt|;
comment|/* Copy remaining bytes one by one.  */
for|for
control|(
name|i
operator|=
name|already
init|;
name|i
operator|<
name|obj_size
condition|;
name|i
operator|++
control|)
name|object_base
index|[
name|i
index|]
operator|=
name|h
operator|->
name|object_base
index|[
name|i
index|]
expr_stmt|;
comment|/* If the object just copied was the only data in OLD_CHUNK,      free that chunk and remove it from the chain.      But not if that chunk might contain an empty object.  */
if|if
condition|(
operator|!
name|h
operator|->
name|maybe_empty_object
operator|&&
operator|(
name|h
operator|->
name|object_base
operator|==
name|__PTR_ALIGN
argument_list|(
operator|(
name|char
operator|*
operator|)
name|old_chunk
argument_list|,
name|old_chunk
operator|->
name|contents
argument_list|,
name|h
operator|->
name|alignment_mask
argument_list|)
operator|)
condition|)
block|{
name|new_chunk
operator|->
name|prev
operator|=
name|old_chunk
operator|->
name|prev
expr_stmt|;
name|CALL_FREEFUN
argument_list|(
name|h
argument_list|,
name|old_chunk
argument_list|)
expr_stmt|;
block|}
name|h
operator|->
name|object_base
operator|=
name|object_base
expr_stmt|;
name|h
operator|->
name|next_free
operator|=
name|h
operator|->
name|object_base
operator|+
name|obj_size
expr_stmt|;
comment|/* The new chunk certainly contains no empty object yet.  */
name|h
operator|->
name|maybe_empty_object
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|_LIBC
end_ifdef
begin_macro
name|libc_hidden_def
argument_list|(
argument|_obstack_newchunk
argument_list|)
end_macro
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* Return nonzero if object OBJ has been allocated from obstack H.    This is here for debugging.    If you use it in a program, you are probably losing.  */
end_comment
begin_comment
comment|/* Suppress -Wmissing-prototypes warning.  We don't want to declare this in    obstack.h because it is just for debugging.  */
end_comment
begin_function_decl
name|int
name|_obstack_allocated_p
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|void
modifier|*
name|obj
parameter_list|)
function_decl|;
end_function_decl
begin_function
name|int
name|_obstack_allocated_p
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|void
modifier|*
name|obj
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|lp
decl_stmt|;
comment|/* below addr of any objects in this chunk */
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|plp
decl_stmt|;
comment|/* point to previous chunk if any */
name|lp
operator|=
operator|(
name|h
operator|)
operator|->
name|chunk
expr_stmt|;
comment|/* We use>= rather than> since the object cannot be exactly at      the beginning of the chunk but might be an empty object exactly      at the end of an adjacent chunk.  */
while|while
condition|(
name|lp
operator|!=
name|NULL
operator|&&
operator|(
operator|(
name|void
operator|*
operator|)
name|lp
operator|>=
name|obj
operator|||
operator|(
name|void
operator|*
operator|)
operator|(
name|lp
operator|)
operator|->
name|limit
operator|<
name|obj
operator|)
condition|)
block|{
name|plp
operator|=
name|lp
operator|->
name|prev
expr_stmt|;
name|lp
operator|=
name|plp
expr_stmt|;
block|}
return|return
name|lp
operator|!=
name|NULL
return|;
block|}
end_function
begin_escape
end_escape
begin_comment
comment|/* Free objects in obstack H, including OBJ and everything allocate    more recently than OBJ.  If OBJ is zero, free everything in H.  */
end_comment
begin_undef
DECL|macro|obstack_free
undef|#
directive|undef
name|obstack_free
end_undef
begin_function
name|void
DECL|function|obstack_free
name|obstack_free
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|,
name|void
modifier|*
name|obj
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|lp
decl_stmt|;
comment|/* below addr of any objects in this chunk */
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|plp
decl_stmt|;
comment|/* point to previous chunk if any */
name|lp
operator|=
name|h
operator|->
name|chunk
expr_stmt|;
comment|/* We use>= because there cannot be an object at the beginning of a chunk.      But there can be an empty object at that address      at the end of another chunk.  */
while|while
condition|(
name|lp
operator|!=
name|NULL
operator|&&
operator|(
operator|(
name|void
operator|*
operator|)
name|lp
operator|>=
name|obj
operator|||
operator|(
name|void
operator|*
operator|)
operator|(
name|lp
operator|)
operator|->
name|limit
operator|<
name|obj
operator|)
condition|)
block|{
name|plp
operator|=
name|lp
operator|->
name|prev
expr_stmt|;
name|CALL_FREEFUN
argument_list|(
name|h
argument_list|,
name|lp
argument_list|)
expr_stmt|;
name|lp
operator|=
name|plp
expr_stmt|;
comment|/* If we switch chunks, we can't tell whether the new current 	 chunk contains an empty object, so assume that it may.  */
name|h
operator|->
name|maybe_empty_object
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|lp
condition|)
block|{
name|h
operator|->
name|object_base
operator|=
name|h
operator|->
name|next_free
operator|=
operator|(
name|char
operator|*
operator|)
operator|(
name|obj
operator|)
expr_stmt|;
name|h
operator|->
name|chunk_limit
operator|=
name|lp
operator|->
name|limit
expr_stmt|;
name|h
operator|->
name|chunk
operator|=
name|lp
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|obj
operator|!=
name|NULL
condition|)
comment|/* obj is not in any of the chunks! */
name|abort
argument_list|()
expr_stmt|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|_LIBC
end_ifdef
begin_comment
comment|/* Older versions of libc used a function _obstack_free intended to be    called by non-GCC compilers.  */
end_comment
begin_macro
DECL|function|strong_alias
name|strong_alias
argument_list|(
argument|obstack_free
argument_list|,
argument|_obstack_free
argument_list|)
end_macro
begin_endif
endif|#
directive|endif
end_endif
begin_escape
end_escape
begin_function
name|int
name|_obstack_memory_used
parameter_list|(
name|struct
name|obstack
modifier|*
name|h
parameter_list|)
block|{
specifier|register
name|struct
name|_obstack_chunk
modifier|*
name|lp
decl_stmt|;
specifier|register
name|int
name|nbytes
init|=
literal|0
decl_stmt|;
for|for
control|(
name|lp
operator|=
name|h
operator|->
name|chunk
init|;
name|lp
operator|!=
name|NULL
condition|;
name|lp
operator|=
name|lp
operator|->
name|prev
control|)
block|{
name|nbytes
operator|+=
name|lp
operator|->
name|limit
operator|-
operator|(
name|char
operator|*
operator|)
name|lp
expr_stmt|;
block|}
return|return
name|nbytes
return|;
block|}
end_function
begin_escape
end_escape
begin_ifdef
ifdef|#
directive|ifdef
name|_LIBC
end_ifdef
begin_include
include|#
directive|include
file|<libio/iolibio.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_ifndef
ifndef|#
directive|ifndef
name|__attribute__
end_ifndef
begin_comment
comment|/* This feature is available in gcc versions 2.5 and later.  */
end_comment
begin_if
if|#
directive|if
name|__GNUC__
operator|<
literal|2
operator|||
operator|(
name|__GNUC__
operator|==
literal|2
operator|&&
name|__GNUC_MINOR__
operator|<
literal|5
operator|)
end_if
begin_define
DECL|macro|__attribute__
define|#
directive|define
name|__attribute__
parameter_list|(
name|Spec
parameter_list|)
end_define
begin_comment
DECL|macro|__attribute__
comment|/* empty */
end_comment
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_function
specifier|static
name|void
DECL|function|print_and_abort
name|print_and_abort
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* Don't change any of these strings.  Yes, it would be possible to add      the newline to the string and use fputs or so.  But this must not      happen because the "memory exhausted" message appears in other places      like this and the translation should be reused instead of creating      a very similar string which requires a separate translation.  */
ifdef|#
directive|ifdef
name|_LIBC
operator|(
name|void
operator|)
name|__fxprintf
argument_list|(
name|NULL
argument_list|,
literal|"%s\n"
argument_list|,
name|_
argument_list|(
literal|"memory exhausted"
argument_list|)
argument_list|)
expr_stmt|;
else|#
directive|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|_
argument_list|(
literal|"memory exhausted"
argument_list|)
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* !ELIDE_CODE */
end_comment
end_unit
