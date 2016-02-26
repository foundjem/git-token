begin_unit
begin_comment
comment|/**  * Copyright 2013, GitHub, Inc  * Copyright 2009-2013, Daniel Lemire, Cliff Moon,  *	David McIntosh, Robert Becho, Google Inc. and Veronika Zenz  *  * This program is free software; you can redistribute it and/or  * modify it under the terms of the GNU General Public License  * as published by the Free Software Foundation; either version 2  * of the License, or (at your option) any later version.  *  * This program is distributed in the hope that it will be useful,  * but WITHOUT ANY WARRANTY; without even the implied warranty of  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  * GNU General Public License for more details.  *  * You should have received a copy of the GNU General Public License  * along with this program; if not, write to the Free Software  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"ewok.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_function
DECL|function|ewah_serialize_native
name|int
name|ewah_serialize_native
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
name|uint32_t
name|write32
decl_stmt|;
name|size_t
name|to_write
init|=
name|self
operator|->
name|buffer_size
operator|*
literal|8
decl_stmt|;
comment|/* 32 bit -- bit size for the map */
name|write32
operator|=
operator|(
name|uint32_t
operator|)
name|self
operator|->
name|bit_size
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
operator|&
name|write32
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
comment|/** 32 bit -- number of compressed 64-bit words */
name|write32
operator|=
operator|(
name|uint32_t
operator|)
name|self
operator|->
name|buffer_size
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
operator|&
name|write32
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|self
operator|->
name|buffer
argument_list|,
name|to_write
argument_list|)
operator|!=
name|to_write
condition|)
return|return
operator|-
literal|1
return|;
comment|/** 32 bit -- position for the RLW */
name|write32
operator|=
name|self
operator|->
name|rlw
operator|-
name|self
operator|->
name|buffer
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
operator|&
name|write32
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
return|return
operator|(
literal|3
operator|*
literal|4
operator|)
operator|+
name|to_write
return|;
block|}
end_function
begin_function
DECL|function|ewah_serialize_to
name|int
name|ewah_serialize_to
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
name|int
function_decl|(
modifier|*
name|write_fun
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|,
name|size_t
parameter_list|)
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|size_t
name|i
decl_stmt|;
name|eword_t
name|dump
index|[
literal|2048
index|]
decl_stmt|;
specifier|const
name|size_t
name|words_per_dump
init|=
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
decl_stmt|;
name|uint32_t
name|bitsize
decl_stmt|,
name|word_count
decl_stmt|,
name|rlw_pos
decl_stmt|;
specifier|const
name|eword_t
modifier|*
name|buffer
decl_stmt|;
name|size_t
name|words_left
decl_stmt|;
comment|/* 32 bit -- bit size for the map */
name|bitsize
operator|=
name|htonl
argument_list|(
operator|(
name|uint32_t
operator|)
name|self
operator|->
name|bit_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_fun
argument_list|(
name|data
argument_list|,
operator|&
name|bitsize
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
comment|/** 32 bit -- number of compressed 64-bit words */
name|word_count
operator|=
name|htonl
argument_list|(
operator|(
name|uint32_t
operator|)
name|self
operator|->
name|buffer_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_fun
argument_list|(
name|data
argument_list|,
operator|&
name|word_count
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
comment|/** 64 bit x N -- compressed words */
name|buffer
operator|=
name|self
operator|->
name|buffer
expr_stmt|;
name|words_left
operator|=
name|self
operator|->
name|buffer_size
expr_stmt|;
while|while
condition|(
name|words_left
operator|>=
name|words_per_dump
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|words_per_dump
condition|;
operator|++
name|i
operator|,
operator|++
name|buffer
control|)
name|dump
index|[
name|i
index|]
operator|=
name|htonll
argument_list|(
operator|*
name|buffer
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_fun
argument_list|(
name|data
argument_list|,
name|dump
argument_list|,
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|words_left
operator|-=
name|words_per_dump
expr_stmt|;
block|}
if|if
condition|(
name|words_left
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|words_left
condition|;
operator|++
name|i
operator|,
operator|++
name|buffer
control|)
name|dump
index|[
name|i
index|]
operator|=
name|htonll
argument_list|(
operator|*
name|buffer
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_fun
argument_list|(
name|data
argument_list|,
name|dump
argument_list|,
name|words_left
operator|*
literal|8
argument_list|)
operator|!=
name|words_left
operator|*
literal|8
condition|)
return|return
operator|-
literal|1
return|;
block|}
comment|/** 32 bit -- position for the RLW */
name|rlw_pos
operator|=
operator|(
name|uint8_t
operator|*
operator|)
name|self
operator|->
name|rlw
operator|-
operator|(
name|uint8_t
operator|*
operator|)
name|self
operator|->
name|buffer
expr_stmt|;
name|rlw_pos
operator|=
name|htonl
argument_list|(
name|rlw_pos
operator|/
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_fun
argument_list|(
name|data
argument_list|,
operator|&
name|rlw_pos
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
return|return
operator|(
literal|3
operator|*
literal|4
operator|)
operator|+
operator|(
name|self
operator|->
name|buffer_size
operator|*
literal|8
operator|)
return|;
block|}
end_function
begin_function
DECL|function|write_helper
specifier|static
name|int
name|write_helper
parameter_list|(
name|void
modifier|*
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
return|return
name|write
argument_list|(
operator|(
name|intptr_t
operator|)
name|fd
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ewah_serialize
name|int
name|ewah_serialize
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
return|return
name|ewah_serialize_to
argument_list|(
name|self
argument_list|,
name|write_helper
argument_list|,
operator|(
name|void
operator|*
operator|)
operator|(
name|intptr_t
operator|)
name|fd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_strbuf
specifier|static
name|int
name|write_strbuf
parameter_list|(
name|void
modifier|*
name|user_data
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|struct
name|strbuf
modifier|*
name|sb
init|=
name|user_data
decl_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|data
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|ewah_serialize_strbuf
name|int
name|ewah_serialize_strbuf
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
return|return
name|ewah_serialize_to
argument_list|(
name|self
argument_list|,
name|write_strbuf
argument_list|,
name|sb
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ewah_read_mmap
name|int
name|ewah_read_mmap
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
specifier|const
name|void
modifier|*
name|map
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
specifier|const
name|uint8_t
modifier|*
name|ptr
init|=
name|map
decl_stmt|;
name|size_t
name|i
decl_stmt|;
name|self
operator|->
name|bit_size
operator|=
name|get_be32
argument_list|(
name|ptr
argument_list|)
expr_stmt|;
name|ptr
operator|+=
sizeof|sizeof
argument_list|(
name|uint32_t
argument_list|)
expr_stmt|;
name|self
operator|->
name|buffer_size
operator|=
name|self
operator|->
name|alloc_size
operator|=
name|get_be32
argument_list|(
name|ptr
argument_list|)
expr_stmt|;
name|ptr
operator|+=
sizeof|sizeof
argument_list|(
name|uint32_t
argument_list|)
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|self
operator|->
name|buffer
argument_list|,
name|self
operator|->
name|alloc_size
argument_list|)
expr_stmt|;
comment|/* 	 * Copy the raw data for the bitmap as a whole chunk; 	 * if we're in a little-endian platform, we'll perform 	 * the endianness conversion in a separate pass to ensure 	 * we're loading 8-byte aligned words. 	 */
name|memcpy
argument_list|(
name|self
operator|->
name|buffer
argument_list|,
name|ptr
argument_list|,
name|self
operator|->
name|buffer_size
operator|*
sizeof|sizeof
argument_list|(
name|uint64_t
argument_list|)
argument_list|)
expr_stmt|;
name|ptr
operator|+=
name|self
operator|->
name|buffer_size
operator|*
sizeof|sizeof
argument_list|(
name|uint64_t
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|self
operator|->
name|buffer_size
condition|;
operator|++
name|i
control|)
name|self
operator|->
name|buffer
index|[
name|i
index|]
operator|=
name|ntohll
argument_list|(
name|self
operator|->
name|buffer
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|self
operator|->
name|rlw
operator|=
name|self
operator|->
name|buffer
operator|+
name|get_be32
argument_list|(
name|ptr
argument_list|)
expr_stmt|;
return|return
operator|(
literal|3
operator|*
literal|4
operator|)
operator|+
operator|(
name|self
operator|->
name|buffer_size
operator|*
literal|8
operator|)
return|;
block|}
end_function
begin_function
DECL|function|ewah_deserialize
name|int
name|ewah_deserialize
parameter_list|(
name|struct
name|ewah_bitmap
modifier|*
name|self
parameter_list|,
name|int
name|fd
parameter_list|)
block|{
name|size_t
name|i
decl_stmt|;
name|eword_t
name|dump
index|[
literal|2048
index|]
decl_stmt|;
specifier|const
name|size_t
name|words_per_dump
init|=
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
operator|/
sizeof|sizeof
argument_list|(
name|eword_t
argument_list|)
decl_stmt|;
name|uint32_t
name|bitsize
decl_stmt|,
name|word_count
decl_stmt|,
name|rlw_pos
decl_stmt|;
name|eword_t
modifier|*
name|buffer
init|=
name|NULL
decl_stmt|;
name|size_t
name|words_left
decl_stmt|;
name|ewah_clear
argument_list|(
name|self
argument_list|)
expr_stmt|;
comment|/* 32 bit -- bit size for the map */
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
operator|&
name|bitsize
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
name|self
operator|->
name|bit_size
operator|=
operator|(
name|size_t
operator|)
name|ntohl
argument_list|(
name|bitsize
argument_list|)
expr_stmt|;
comment|/** 32 bit -- number of compressed 64-bit words */
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
operator|&
name|word_count
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
name|self
operator|->
name|buffer_size
operator|=
name|self
operator|->
name|alloc_size
operator|=
operator|(
name|size_t
operator|)
name|ntohl
argument_list|(
name|word_count
argument_list|)
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|self
operator|->
name|buffer
argument_list|,
name|self
operator|->
name|alloc_size
argument_list|)
expr_stmt|;
comment|/** 64 bit x N -- compressed words */
name|buffer
operator|=
name|self
operator|->
name|buffer
expr_stmt|;
name|words_left
operator|=
name|self
operator|->
name|buffer_size
expr_stmt|;
while|while
condition|(
name|words_left
operator|>=
name|words_per_dump
condition|)
block|{
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
name|dump
argument_list|,
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|dump
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|words_per_dump
condition|;
operator|++
name|i
operator|,
operator|++
name|buffer
control|)
operator|*
name|buffer
operator|=
name|ntohll
argument_list|(
name|dump
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|words_left
operator|-=
name|words_per_dump
expr_stmt|;
block|}
if|if
condition|(
name|words_left
condition|)
block|{
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
name|dump
argument_list|,
name|words_left
operator|*
literal|8
argument_list|)
operator|!=
name|words_left
operator|*
literal|8
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|words_left
condition|;
operator|++
name|i
operator|,
operator|++
name|buffer
control|)
operator|*
name|buffer
operator|=
name|ntohll
argument_list|(
name|dump
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
comment|/** 32 bit -- position for the RLW */
if|if
condition|(
name|read
argument_list|(
name|fd
argument_list|,
operator|&
name|rlw_pos
argument_list|,
literal|4
argument_list|)
operator|!=
literal|4
condition|)
return|return
operator|-
literal|1
return|;
name|self
operator|->
name|rlw
operator|=
name|self
operator|->
name|buffer
operator|+
name|ntohl
argument_list|(
name|rlw_pos
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
