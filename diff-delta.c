begin_unit
begin_comment
comment|/*  * diff-delta.c: generate a delta between two buffers  *  *  Many parts of this file have been lifted from LibXDiff version 0.10.  *  http://www.xmailserver.org/xdiff-lib.html  *  *  LibXDiff was written by Davide Libenzi<davidel@xmailserver.org>  *  Copyright (C) 2003	Davide Libenzi  *  *  Many mods for GIT usage by Nicolas Pitre<nico@cam.org>, (C) 2005.  *  *  This file is free software; you can redistribute it and/or  *  modify it under the terms of the GNU Lesser General Public  *  License as published by the Free Software Foundation; either  *  version 2.1 of the License, or (at your option) any later version.  *  *  Use of this within git automatically means that the LGPL  *  licensing gets turned into GPLv2 within this project.  */
end_comment
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_struct
DECL|struct|index
struct|struct
name|index
block|{
DECL|member|ptr
specifier|const
name|unsigned
name|char
modifier|*
name|ptr
decl_stmt|;
DECL|member|next
name|struct
name|index
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|delta_index
specifier|static
name|struct
name|index
modifier|*
modifier|*
name|delta_index
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|bufsize
parameter_list|,
name|unsigned
name|long
name|trg_bufsize
parameter_list|)
block|{
name|unsigned
name|long
name|hsize
decl_stmt|;
name|unsigned
name|int
name|i
decl_stmt|,
name|hshift
decl_stmt|,
name|hlimit
decl_stmt|,
modifier|*
name|hash_count
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|data
decl_stmt|;
name|struct
name|index
modifier|*
name|entry
decl_stmt|,
modifier|*
modifier|*
name|hash
decl_stmt|;
name|void
modifier|*
name|mem
decl_stmt|;
comment|/* determine index hash size */
name|hsize
operator|=
name|bufsize
operator|/
literal|4
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|8
init|;
operator|(
literal|1
operator|<<
name|i
operator|)
operator|<
name|hsize
operator|&&
name|i
operator|<
literal|24
condition|;
name|i
operator|+=
literal|2
control|)
empty_stmt|;
name|hsize
operator|=
literal|1
operator|<<
name|i
expr_stmt|;
name|hshift
operator|=
operator|(
name|i
operator|-
literal|8
operator|)
operator|/
literal|2
expr_stmt|;
comment|/* 	 * Allocate lookup index.  Note the first hash pointer 	 * is used to store the hash shift value. 	 */
name|mem
operator|=
name|malloc
argument_list|(
operator|(
literal|1
operator|+
name|hsize
operator|)
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
operator|+
name|bufsize
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mem
condition|)
return|return
name|NULL
return|;
name|hash
operator|=
name|mem
expr_stmt|;
operator|*
name|hash
operator|++
operator|=
operator|(
name|void
operator|*
operator|)
name|hshift
expr_stmt|;
name|entry
operator|=
name|mem
operator|+
operator|(
literal|1
operator|+
name|hsize
operator|)
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|hash
argument_list|,
literal|0
argument_list|,
name|hsize
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
argument_list|)
expr_stmt|;
comment|/* allocate an array to count hash entries */
name|hash_count
operator|=
name|calloc
argument_list|(
name|hsize
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|hash_count
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|hash_count
condition|)
block|{
name|free
argument_list|(
name|hash
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* then populate the index */
name|data
operator|=
name|buf
operator|+
name|bufsize
operator|-
literal|2
expr_stmt|;
while|while
condition|(
name|data
operator|>
name|buf
condition|)
block|{
name|entry
operator|->
name|ptr
operator|=
operator|--
name|data
expr_stmt|;
name|i
operator|=
name|data
index|[
literal|0
index|]
operator|^
operator|(
operator|(
name|data
index|[
literal|1
index|]
operator|^
operator|(
name|data
index|[
literal|2
index|]
operator|<<
name|hshift
operator|)
operator|)
operator|<<
name|hshift
operator|)
expr_stmt|;
name|entry
operator|->
name|next
operator|=
name|hash
index|[
name|i
index|]
expr_stmt|;
name|hash
index|[
name|i
index|]
operator|=
name|entry
operator|++
expr_stmt|;
name|hash_count
index|[
name|i
index|]
operator|++
expr_stmt|;
block|}
comment|/* 	 * Determine a limit on the number of entries in the same hash 	 * bucket.  This guard us against patological data sets causing 	 * really bad hash distribution with most entries in the same hash 	 * bucket that would bring us to O(m*n) computing costs (m and n 	 * corresponding to reference and target buffer sizes). 	 * 	 * The more the target buffer is large, the more it is important to 	 * have small entry lists for each hash buckets.  With such a limit 	 * the cost is bounded to something more like O(m+n). 	 */
name|hlimit
operator|=
operator|(
literal|1
operator|<<
literal|26
operator|)
operator|/
name|trg_bufsize
expr_stmt|;
if|if
condition|(
name|hlimit
operator|<
literal|16
condition|)
name|hlimit
operator|=
literal|16
expr_stmt|;
comment|/* 	 * Now make sure none of the hash buckets has more entries than 	 * we're willing to test.  Otherwise we short-circuit the entry 	 * list uniformly to still preserve a good repartition across 	 * the reference buffer. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|hsize
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|hash_count
index|[
name|i
index|]
operator|<
name|hlimit
condition|)
continue|continue;
name|entry
operator|=
name|hash
index|[
name|i
index|]
expr_stmt|;
do|do
block|{
name|struct
name|index
modifier|*
name|keep
init|=
name|entry
decl_stmt|;
name|int
name|skip
init|=
name|hash_count
index|[
name|i
index|]
operator|/
name|hlimit
operator|/
literal|2
decl_stmt|;
do|do
block|{
name|entry
operator|=
name|entry
operator|->
name|next
expr_stmt|;
block|}
do|while
condition|(
operator|--
name|skip
operator|&&
name|entry
condition|)
do|;
name|keep
operator|->
name|next
operator|=
name|entry
expr_stmt|;
block|}
do|while
condition|(
name|entry
condition|)
do|;
block|}
name|free
argument_list|(
name|hash_count
argument_list|)
expr_stmt|;
return|return
name|hash
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/* provide the size of the copy opcode given the block offset and size */
end_comment
begin_define
DECL|macro|COPYOP_SIZE
define|#
directive|define
name|COPYOP_SIZE
parameter_list|(
name|o
parameter_list|,
name|s
parameter_list|)
define|\
value|(!!(o& 0xff) + !!(o& 0xff00) + !!(o& 0xff0000) + !!(o& 0xff000000) + \      !!(s& 0xff) + !!(s& 0xff00) + 1)
end_define
begin_comment
comment|/* the maximum size for any opcode */
end_comment
begin_define
DECL|macro|MAX_OP_SIZE
define|#
directive|define
name|MAX_OP_SIZE
value|COPYOP_SIZE(0xffffffff, 0xffffffff)
end_define
begin_function
DECL|function|diff_delta
name|void
modifier|*
name|diff_delta
parameter_list|(
name|void
modifier|*
name|from_buf
parameter_list|,
name|unsigned
name|long
name|from_size
parameter_list|,
name|void
modifier|*
name|to_buf
parameter_list|,
name|unsigned
name|long
name|to_size
parameter_list|,
name|unsigned
name|long
modifier|*
name|delta_size
parameter_list|,
name|unsigned
name|long
name|max_size
parameter_list|,
name|void
modifier|*
modifier|*
name|from_index
parameter_list|)
block|{
name|unsigned
name|int
name|i
decl_stmt|,
name|outpos
decl_stmt|,
name|outsize
decl_stmt|,
name|inscnt
decl_stmt|,
name|hash_shift
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|ref_data
decl_stmt|,
modifier|*
name|ref_top
decl_stmt|,
modifier|*
name|data
decl_stmt|,
modifier|*
name|top
decl_stmt|;
name|unsigned
name|char
modifier|*
name|out
decl_stmt|;
name|struct
name|index
modifier|*
name|entry
decl_stmt|,
modifier|*
modifier|*
name|hash
decl_stmt|;
if|if
condition|(
operator|!
name|from_size
operator|||
operator|!
name|to_size
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|from_index
operator|&&
operator|*
name|from_index
condition|)
block|{
name|hash
operator|=
operator|*
name|from_index
expr_stmt|;
block|}
else|else
block|{
name|hash
operator|=
name|delta_index
argument_list|(
name|from_buf
argument_list|,
name|from_size
argument_list|,
name|to_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|hash
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|from_index
condition|)
operator|*
name|from_index
operator|=
name|hash
expr_stmt|;
block|}
name|hash_shift
operator|=
call|(
name|unsigned
name|int
call|)
argument_list|(
operator|*
name|hash
operator|++
argument_list|)
expr_stmt|;
name|outpos
operator|=
literal|0
expr_stmt|;
name|outsize
operator|=
literal|8192
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outsize
operator|>=
name|max_size
condition|)
name|outsize
operator|=
name|max_size
operator|+
name|MAX_OP_SIZE
operator|+
literal|1
expr_stmt|;
name|out
operator|=
name|malloc
argument_list|(
name|outsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out
condition|)
block|{
if|if
condition|(
operator|!
name|from_index
condition|)
name|free
argument_list|(
name|hash
operator|-
literal|1
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|ref_data
operator|=
name|from_buf
expr_stmt|;
name|ref_top
operator|=
name|from_buf
operator|+
name|from_size
expr_stmt|;
name|data
operator|=
name|to_buf
expr_stmt|;
name|top
operator|=
name|to_buf
operator|+
name|to_size
expr_stmt|;
comment|/* store reference buffer size */
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|from_size
expr_stmt|;
name|from_size
operator|>>=
literal|7
expr_stmt|;
while|while
condition|(
name|from_size
condition|)
block|{
name|out
index|[
name|outpos
operator|-
literal|1
index|]
operator||=
literal|0x80
expr_stmt|;
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|from_size
expr_stmt|;
name|from_size
operator|>>=
literal|7
expr_stmt|;
block|}
comment|/* store target buffer size */
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|to_size
expr_stmt|;
name|to_size
operator|>>=
literal|7
expr_stmt|;
while|while
condition|(
name|to_size
condition|)
block|{
name|out
index|[
name|outpos
operator|-
literal|1
index|]
operator||=
literal|0x80
expr_stmt|;
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|to_size
expr_stmt|;
name|to_size
operator|>>=
literal|7
expr_stmt|;
block|}
name|inscnt
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|data
operator|<
name|top
condition|)
block|{
name|unsigned
name|int
name|moff
init|=
literal|0
decl_stmt|,
name|msize
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|data
operator|+
literal|3
operator|<=
name|top
condition|)
block|{
name|i
operator|=
name|data
index|[
literal|0
index|]
operator|^
operator|(
operator|(
name|data
index|[
literal|1
index|]
operator|^
operator|(
name|data
index|[
literal|2
index|]
operator|<<
name|hash_shift
operator|)
operator|)
operator|<<
name|hash_shift
operator|)
expr_stmt|;
for|for
control|(
name|entry
operator|=
name|hash
index|[
name|i
index|]
init|;
name|entry
condition|;
name|entry
operator|=
name|entry
operator|->
name|next
control|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|ref
init|=
name|entry
operator|->
name|ptr
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|src
init|=
name|data
decl_stmt|;
name|unsigned
name|int
name|ref_size
init|=
name|ref_top
operator|-
name|ref
decl_stmt|;
if|if
condition|(
name|ref_size
operator|>
name|top
operator|-
name|src
condition|)
name|ref_size
operator|=
name|top
operator|-
name|src
expr_stmt|;
if|if
condition|(
name|ref_size
operator|>
literal|0x10000
condition|)
name|ref_size
operator|=
literal|0x10000
expr_stmt|;
if|if
condition|(
name|ref_size
operator|<=
name|msize
condition|)
break|break;
if|if
condition|(
operator|*
name|ref
operator|!=
operator|*
name|src
condition|)
continue|continue;
while|while
condition|(
name|ref_size
operator|--
operator|&&
operator|*
operator|++
name|src
operator|==
operator|*
operator|++
name|ref
condition|)
empty_stmt|;
if|if
condition|(
name|msize
operator|<
name|ref
operator|-
name|entry
operator|->
name|ptr
condition|)
block|{
comment|/* this is our best match so far */
name|msize
operator|=
name|ref
operator|-
name|entry
operator|->
name|ptr
expr_stmt|;
name|moff
operator|=
name|entry
operator|->
name|ptr
operator|-
name|ref_data
expr_stmt|;
block|}
block|}
block|}
if|if
condition|(
operator|!
name|msize
operator|||
name|msize
operator|<
name|COPYOP_SIZE
argument_list|(
name|moff
argument_list|,
name|msize
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|inscnt
condition|)
name|outpos
operator|++
expr_stmt|;
name|out
index|[
name|outpos
operator|++
index|]
operator|=
operator|*
name|data
operator|++
expr_stmt|;
name|inscnt
operator|++
expr_stmt|;
if|if
condition|(
name|inscnt
operator|==
literal|0x7f
condition|)
block|{
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
name|inscnt
operator|=
literal|0
expr_stmt|;
block|}
block|}
else|else
block|{
name|unsigned
name|char
modifier|*
name|op
decl_stmt|;
if|if
condition|(
name|inscnt
condition|)
block|{
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
name|inscnt
operator|=
literal|0
expr_stmt|;
block|}
name|data
operator|+=
name|msize
expr_stmt|;
name|op
operator|=
name|out
operator|+
name|outpos
operator|++
expr_stmt|;
name|i
operator|=
literal|0x80
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
expr_stmt|;
name|i
operator||=
literal|0x01
expr_stmt|;
block|}
name|moff
operator|>>=
literal|8
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
expr_stmt|;
name|i
operator||=
literal|0x02
expr_stmt|;
block|}
name|moff
operator|>>=
literal|8
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
expr_stmt|;
name|i
operator||=
literal|0x04
expr_stmt|;
block|}
name|moff
operator|>>=
literal|8
expr_stmt|;
if|if
condition|(
name|moff
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|moff
expr_stmt|;
name|i
operator||=
literal|0x08
expr_stmt|;
block|}
if|if
condition|(
name|msize
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|msize
expr_stmt|;
name|i
operator||=
literal|0x10
expr_stmt|;
block|}
name|msize
operator|>>=
literal|8
expr_stmt|;
if|if
condition|(
name|msize
operator|&
literal|0xff
condition|)
block|{
name|out
index|[
name|outpos
operator|++
index|]
operator|=
name|msize
expr_stmt|;
name|i
operator||=
literal|0x20
expr_stmt|;
block|}
operator|*
name|op
operator|=
name|i
expr_stmt|;
block|}
if|if
condition|(
name|outpos
operator|>=
name|outsize
operator|-
name|MAX_OP_SIZE
condition|)
block|{
name|void
modifier|*
name|tmp
init|=
name|out
decl_stmt|;
name|outsize
operator|=
name|outsize
operator|*
literal|3
operator|/
literal|2
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outsize
operator|>=
name|max_size
condition|)
name|outsize
operator|=
name|max_size
operator|+
name|MAX_OP_SIZE
operator|+
literal|1
expr_stmt|;
if|if
condition|(
name|max_size
operator|&&
name|outpos
operator|>
name|max_size
condition|)
name|out
operator|=
name|NULL
expr_stmt|;
else|else
name|out
operator|=
name|realloc
argument_list|(
name|out
argument_list|,
name|outsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out
condition|)
block|{
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|from_index
condition|)
name|free
argument_list|(
name|hash
operator|-
literal|1
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
block|}
if|if
condition|(
name|inscnt
condition|)
name|out
index|[
name|outpos
operator|-
name|inscnt
operator|-
literal|1
index|]
operator|=
name|inscnt
expr_stmt|;
if|if
condition|(
operator|!
name|from_index
condition|)
name|free
argument_list|(
name|hash
operator|-
literal|1
argument_list|)
expr_stmt|;
operator|*
name|delta_size
operator|=
name|outpos
expr_stmt|;
return|return
name|out
return|;
block|}
end_function
end_unit
