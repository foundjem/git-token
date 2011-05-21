begin_unit
begin_comment
comment|/*  * Copyright (c) 2011, Google Inc.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"streaming.h"
end_include
begin_enum
DECL|enum|input_source
enum|enum
name|input_source
block|{
DECL|enumerator|stream_error
name|stream_error
init|=
operator|-
literal|1
block|,
DECL|enumerator|incore
name|incore
init|=
literal|0
block|,
DECL|enumerator|loose
name|loose
init|=
literal|1
block|,
DECL|enumerator|pack_non_delta
name|pack_non_delta
init|=
literal|2
block|}
enum|;
end_enum
begin_typedef
DECL|typedef|open_istream_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|open_istream_fn
function_decl|)
parameter_list|(
name|struct
name|git_istream
modifier|*
parameter_list|,
name|struct
name|object_info
modifier|*
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
parameter_list|,
name|enum
name|object_type
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|close_istream_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|close_istream_fn
function_decl|)
parameter_list|(
name|struct
name|git_istream
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|read_istream_fn
typedef|typedef
name|ssize_t
function_decl|(
modifier|*
name|read_istream_fn
function_decl|)
parameter_list|(
name|struct
name|git_istream
modifier|*
parameter_list|,
name|char
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|stream_vtbl
struct|struct
name|stream_vtbl
block|{
DECL|member|close
name|close_istream_fn
name|close
decl_stmt|;
DECL|member|read
name|read_istream_fn
name|read
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|open_method_decl
define|#
directive|define
name|open_method_decl
parameter_list|(
name|name
parameter_list|)
define|\
value|int open_istream_ ##name \ 	(struct git_istream *st, struct object_info *oi, \ 	 const unsigned char *sha1, \ 	 enum object_type *type)
end_define
begin_define
DECL|macro|close_method_decl
define|#
directive|define
name|close_method_decl
parameter_list|(
name|name
parameter_list|)
define|\
value|int close_istream_ ##name \ 	(struct git_istream *st)
end_define
begin_define
DECL|macro|read_method_decl
define|#
directive|define
name|read_method_decl
parameter_list|(
name|name
parameter_list|)
define|\
value|ssize_t read_istream_ ##name \ 	(struct git_istream *st, char *buf, size_t sz)
end_define
begin_comment
comment|/* forward declaration */
end_comment
begin_expr_stmt
specifier|static
name|open_method_decl
argument_list|(
name|incore
argument_list|)
expr_stmt|;
end_expr_stmt
begin_expr_stmt
specifier|static
name|open_method_decl
argument_list|(
name|loose
argument_list|)
expr_stmt|;
end_expr_stmt
begin_expr_stmt
specifier|static
name|open_method_decl
argument_list|(
name|pack_non_delta
argument_list|)
expr_stmt|;
end_expr_stmt
begin_function_decl
specifier|static
name|struct
name|git_istream
modifier|*
name|attach_stream_filter
parameter_list|(
name|struct
name|git_istream
modifier|*
name|st
parameter_list|,
name|struct
name|stream_filter
modifier|*
name|filter
parameter_list|)
function_decl|;
end_function_decl
begin_decl_stmt
DECL|variable|open_istream_tbl
specifier|static
name|open_istream_fn
name|open_istream_tbl
index|[]
init|=
block|{
name|open_istream_incore
block|,
name|open_istream_loose
block|,
name|open_istream_pack_non_delta
block|, }
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|FILTER_BUFFER
define|#
directive|define
name|FILTER_BUFFER
value|(1024*16)
end_define
begin_struct
DECL|struct|filtered_istream
struct|struct
name|filtered_istream
block|{
DECL|member|upstream
name|struct
name|git_istream
modifier|*
name|upstream
decl_stmt|;
DECL|member|filter
name|struct
name|stream_filter
modifier|*
name|filter
decl_stmt|;
DECL|member|ibuf
name|char
name|ibuf
index|[
name|FILTER_BUFFER
index|]
decl_stmt|;
DECL|member|obuf
name|char
name|obuf
index|[
name|FILTER_BUFFER
index|]
decl_stmt|;
DECL|member|i_end
DECL|member|i_ptr
name|int
name|i_end
decl_stmt|,
name|i_ptr
decl_stmt|;
DECL|member|o_end
DECL|member|o_ptr
name|int
name|o_end
decl_stmt|,
name|o_ptr
decl_stmt|;
DECL|member|input_finished
name|int
name|input_finished
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|git_istream
struct|struct
name|git_istream
block|{
DECL|member|vtbl
specifier|const
name|struct
name|stream_vtbl
modifier|*
name|vtbl
decl_stmt|;
DECL|member|size
name|unsigned
name|long
name|size
decl_stmt|;
comment|/* inflated size of full object */
DECL|member|z
name|z_stream
name|z
decl_stmt|;
DECL|enumerator|z_unused
DECL|enumerator|z_used
DECL|enumerator|z_done
DECL|enumerator|z_error
DECL|member|z_state
enum|enum
block|{
name|z_unused
block|,
name|z_used
block|,
name|z_done
block|,
name|z_error
block|}
name|z_state
enum|;
union|union
block|{
struct|struct
block|{
DECL|member|buf
name|char
modifier|*
name|buf
decl_stmt|;
comment|/* from read_object() */
DECL|member|read_ptr
name|unsigned
name|long
name|read_ptr
decl_stmt|;
block|}
DECL|member|incore
name|incore
struct|;
struct|struct
block|{
DECL|member|mapped
name|void
modifier|*
name|mapped
decl_stmt|;
DECL|member|mapsize
name|unsigned
name|long
name|mapsize
decl_stmt|;
DECL|member|hdr
name|char
name|hdr
index|[
literal|32
index|]
decl_stmt|;
DECL|member|hdr_avail
name|int
name|hdr_avail
decl_stmt|;
DECL|member|hdr_used
name|int
name|hdr_used
decl_stmt|;
block|}
DECL|member|loose
name|loose
struct|;
struct|struct
block|{
DECL|member|pack
name|struct
name|packed_git
modifier|*
name|pack
decl_stmt|;
DECL|member|pos
name|off_t
name|pos
decl_stmt|;
block|}
DECL|member|in_pack
name|in_pack
struct|;
DECL|member|filtered
name|struct
name|filtered_istream
name|filtered
decl_stmt|;
block|}
DECL|member|u
name|u
union|;
block|}
struct|;
end_struct
begin_function
DECL|function|close_istream
name|int
name|close_istream
parameter_list|(
name|struct
name|git_istream
modifier|*
name|st
parameter_list|)
block|{
return|return
name|st
operator|->
name|vtbl
operator|->
name|close
argument_list|(
name|st
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_istream
name|ssize_t
name|read_istream
parameter_list|(
name|struct
name|git_istream
modifier|*
name|st
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|sz
parameter_list|)
block|{
return|return
name|st
operator|->
name|vtbl
operator|->
name|read
argument_list|(
name|st
argument_list|,
name|buf
argument_list|,
name|sz
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|istream_source
specifier|static
name|enum
name|input_source
name|istream_source
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|enum
name|object_type
modifier|*
name|type
parameter_list|,
name|struct
name|object_info
modifier|*
name|oi
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|status
decl_stmt|;
name|oi
operator|->
name|sizep
operator|=
operator|&
name|size
expr_stmt|;
name|status
operator|=
name|sha1_object_info_extended
argument_list|(
name|sha1
argument_list|,
name|oi
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
operator|<
literal|0
condition|)
return|return
name|stream_error
return|;
operator|*
name|type
operator|=
name|status
expr_stmt|;
switch|switch
condition|(
name|oi
operator|->
name|whence
condition|)
block|{
case|case
name|OI_LOOSE
case|:
return|return
name|loose
return|;
case|case
name|OI_PACKED
case|:
if|if
condition|(
operator|!
name|oi
operator|->
name|u
operator|.
name|packed
operator|.
name|is_delta
operator|&&
name|big_file_threshold
operator|<=
name|size
condition|)
return|return
name|pack_non_delta
return|;
comment|/* fallthru */
default|default:
return|return
name|incore
return|;
block|}
block|}
end_function
begin_function
DECL|function|open_istream
name|struct
name|git_istream
modifier|*
name|open_istream
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|enum
name|object_type
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|struct
name|stream_filter
modifier|*
name|filter
parameter_list|)
block|{
name|struct
name|git_istream
modifier|*
name|st
decl_stmt|;
name|struct
name|object_info
name|oi
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|real
init|=
name|lookup_replace_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|enum
name|input_source
name|src
init|=
name|istream_source
argument_list|(
name|real
argument_list|,
name|type
argument_list|,
operator|&
name|oi
argument_list|)
decl_stmt|;
if|if
condition|(
name|src
operator|<
literal|0
condition|)
return|return
name|NULL
return|;
name|st
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|st
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|open_istream_tbl
index|[
name|src
index|]
operator|(
name|st
operator|,
operator|&
name|oi
operator|,
name|real
operator|,
name|type
operator|)
condition|)
block|{
if|if
condition|(
name|open_istream_incore
argument_list|(
name|st
argument_list|,
operator|&
name|oi
argument_list|,
name|real
argument_list|,
name|type
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|st
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
if|if
condition|(
name|st
operator|&&
name|filter
condition|)
block|{
comment|/* Add "&& !is_null_stream_filter(filter)" for performance */
name|struct
name|git_istream
modifier|*
name|nst
init|=
name|attach_stream_filter
argument_list|(
name|st
argument_list|,
name|filter
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|nst
condition|)
name|close_istream
argument_list|(
name|st
argument_list|)
expr_stmt|;
name|st
operator|=
name|nst
expr_stmt|;
block|}
operator|*
name|size
operator|=
name|st
operator|->
name|size
expr_stmt|;
return|return
name|st
return|;
block|}
end_function
begin_comment
comment|/*****************************************************************  *  * Common helpers  *  *****************************************************************/
end_comment
begin_function
DECL|function|close_deflated_stream
specifier|static
name|void
name|close_deflated_stream
parameter_list|(
name|struct
name|git_istream
modifier|*
name|st
parameter_list|)
block|{
if|if
condition|(
name|st
operator|->
name|z_state
operator|==
name|z_used
condition|)
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*****************************************************************  *  * Filtered stream  *  *****************************************************************/
end_comment
begin_expr_stmt
DECL|function|close_method_decl
specifier|static
name|close_method_decl
argument_list|(
argument|filtered
argument_list|)
block|{
name|free_stream_filter
argument_list|(
name|st
operator|->
name|u
operator|.
name|filtered
operator|.
name|filter
argument_list|)
block|;
return|return
name|close_istream
argument_list|(
name|st
operator|->
name|u
operator|.
name|filtered
operator|.
name|upstream
argument_list|)
return|;
block|}
end_expr_stmt
begin_expr_stmt
DECL|function|read_method_decl
specifier|static
name|read_method_decl
argument_list|(
argument|filtered
argument_list|)
block|{ 	struct
name|filtered_istream
operator|*
name|fs
operator|=
operator|&
operator|(
name|st
operator|->
name|u
operator|.
name|filtered
operator|)
block|;
name|size_t
name|filled
operator|=
literal|0
block|;
while|while
condition|(
name|sz
condition|)
block|{
comment|/* do we already have filtered output? */
if|if
condition|(
name|fs
operator|->
name|o_ptr
operator|<
name|fs
operator|->
name|o_end
condition|)
block|{
name|size_t
name|to_move
init|=
name|fs
operator|->
name|o_end
operator|-
name|fs
operator|->
name|o_ptr
decl_stmt|;
if|if
condition|(
name|sz
operator|<
name|to_move
condition|)
name|to_move
operator|=
name|sz
expr_stmt|;
name|memcpy
argument_list|(
name|buf
operator|+
name|filled
argument_list|,
name|fs
operator|->
name|obuf
operator|+
name|fs
operator|->
name|o_ptr
argument_list|,
name|to_move
argument_list|)
expr_stmt|;
name|fs
operator|->
name|o_ptr
operator|+=
name|to_move
expr_stmt|;
name|sz
operator|-=
name|to_move
expr_stmt|;
name|filled
operator|+=
name|to_move
expr_stmt|;
continue|continue;
block|}
name|fs
operator|->
name|o_end
operator|=
name|fs
operator|->
name|o_ptr
operator|=
literal|0
expr_stmt|;
end_expr_stmt
begin_comment
comment|/* do we have anything to feed the filter with? */
end_comment
begin_if
if|if
condition|(
name|fs
operator|->
name|i_ptr
operator|<
name|fs
operator|->
name|i_end
condition|)
block|{
name|size_t
name|to_feed
init|=
name|fs
operator|->
name|i_end
operator|-
name|fs
operator|->
name|i_ptr
decl_stmt|;
name|size_t
name|to_receive
init|=
name|FILTER_BUFFER
decl_stmt|;
if|if
condition|(
name|stream_filter
argument_list|(
name|fs
operator|->
name|filter
argument_list|,
name|fs
operator|->
name|ibuf
operator|+
name|fs
operator|->
name|i_ptr
argument_list|,
operator|&
name|to_feed
argument_list|,
name|fs
operator|->
name|obuf
argument_list|,
operator|&
name|to_receive
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|fs
operator|->
name|i_ptr
operator|=
name|fs
operator|->
name|i_end
operator|-
name|to_feed
expr_stmt|;
name|fs
operator|->
name|o_end
operator|=
name|FILTER_BUFFER
operator|-
name|to_receive
expr_stmt|;
continue|continue;
block|}
end_if
begin_comment
comment|/* tell the filter to drain upon no more input */
end_comment
begin_if
if|if
condition|(
name|fs
operator|->
name|input_finished
condition|)
block|{
name|size_t
name|to_receive
init|=
name|FILTER_BUFFER
decl_stmt|;
if|if
condition|(
name|stream_filter
argument_list|(
name|fs
operator|->
name|filter
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|fs
operator|->
name|obuf
argument_list|,
operator|&
name|to_receive
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|fs
operator|->
name|o_end
operator|=
name|FILTER_BUFFER
operator|-
name|to_receive
expr_stmt|;
if|if
condition|(
operator|!
name|fs
operator|->
name|o_end
condition|)
break|break;
continue|continue;
block|}
end_if
begin_expr_stmt
name|fs
operator|->
name|i_end
operator|=
name|fs
operator|->
name|i_ptr
operator|=
literal|0
expr_stmt|;
end_expr_stmt
begin_comment
comment|/* refill the input from the upstream */
end_comment
begin_if
if|if
condition|(
operator|!
name|fs
operator|->
name|input_finished
condition|)
block|{
name|fs
operator|->
name|i_end
operator|=
name|read_istream
argument_list|(
name|fs
operator|->
name|upstream
argument_list|,
name|fs
operator|->
name|ibuf
argument_list|,
name|FILTER_BUFFER
argument_list|)
expr_stmt|;
if|if
condition|(
name|fs
operator|->
name|i_end
operator|<
literal|0
condition|)
break|break;
if|if
condition|(
name|fs
operator|->
name|i_end
condition|)
continue|continue;
block|}
end_if
begin_expr_stmt
name|fs
operator|->
name|input_finished
operator|=
literal|1
expr_stmt|;
end_expr_stmt
begin_expr_stmt
unit|} 	return
name|filled
expr_stmt|;
end_expr_stmt
begin_decl_stmt
unit|}  static
DECL|variable|filtered_vtbl
name|struct
name|stream_vtbl
name|filtered_vtbl
init|=
block|{
name|close_istream_filtered
block|,
name|read_istream_filtered
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|attach_stream_filter
specifier|static
name|struct
name|git_istream
modifier|*
name|attach_stream_filter
parameter_list|(
name|struct
name|git_istream
modifier|*
name|st
parameter_list|,
name|struct
name|stream_filter
modifier|*
name|filter
parameter_list|)
block|{
name|struct
name|git_istream
modifier|*
name|ifs
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|ifs
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|filtered_istream
modifier|*
name|fs
init|=
operator|&
operator|(
name|ifs
operator|->
name|u
operator|.
name|filtered
operator|)
decl_stmt|;
name|ifs
operator|->
name|vtbl
operator|=
operator|&
name|filtered_vtbl
expr_stmt|;
name|fs
operator|->
name|upstream
operator|=
name|st
expr_stmt|;
name|fs
operator|->
name|filter
operator|=
name|filter
expr_stmt|;
name|fs
operator|->
name|i_end
operator|=
name|fs
operator|->
name|i_ptr
operator|=
literal|0
expr_stmt|;
name|fs
operator|->
name|o_end
operator|=
name|fs
operator|->
name|o_ptr
operator|=
literal|0
expr_stmt|;
name|fs
operator|->
name|input_finished
operator|=
literal|0
expr_stmt|;
name|ifs
operator|->
name|size
operator|=
operator|-
literal|1
expr_stmt|;
comment|/* unknown */
return|return
name|ifs
return|;
block|}
end_function
begin_comment
comment|/*****************************************************************  *  * Loose object stream  *  *****************************************************************/
end_comment
begin_expr_stmt
DECL|function|read_method_decl
specifier|static
name|read_method_decl
argument_list|(
argument|loose
argument_list|)
block|{
name|size_t
name|total_read
operator|=
literal|0
block|;
switch|switch
condition|(
name|st
operator|->
name|z_state
condition|)
block|{
case|case
name|z_done
case|:
return|return
literal|0
return|;
case|case
name|z_error
case|:
return|return
operator|-
literal|1
return|;
default|default:
break|break;
block|}
end_expr_stmt
begin_if
if|if
condition|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_used
operator|<
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_avail
condition|)
block|{
name|size_t
name|to_copy
init|=
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_avail
operator|-
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_used
decl_stmt|;
if|if
condition|(
name|sz
operator|<
name|to_copy
condition|)
name|to_copy
operator|=
name|sz
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr
operator|+
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_used
argument_list|,
name|to_copy
argument_list|)
expr_stmt|;
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_used
operator|+=
name|to_copy
expr_stmt|;
name|total_read
operator|+=
name|to_copy
expr_stmt|;
block|}
end_if
begin_while
while|while
condition|(
name|total_read
operator|<
name|sz
condition|)
block|{
name|int
name|status
decl_stmt|;
name|st
operator|->
name|z
operator|.
name|next_out
operator|=
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buf
operator|+
name|total_read
expr_stmt|;
name|st
operator|->
name|z
operator|.
name|avail_out
operator|=
name|sz
operator|-
name|total_read
expr_stmt|;
name|status
operator|=
name|git_inflate
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|,
name|Z_FINISH
argument_list|)
expr_stmt|;
name|total_read
operator|=
name|st
operator|->
name|z
operator|.
name|next_out
operator|-
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buf
expr_stmt|;
if|if
condition|(
name|status
operator|==
name|Z_STREAM_END
condition|)
block|{
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|st
operator|->
name|z_state
operator|=
name|z_done
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|status
operator|!=
name|Z_OK
operator|&&
name|status
operator|!=
name|Z_BUF_ERROR
condition|)
block|{
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|st
operator|->
name|z_state
operator|=
name|z_error
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
end_while
begin_return
return|return
name|total_read
return|;
end_return
begin_macro
unit|}  static
DECL|function|close_method_decl
name|close_method_decl
argument_list|(
argument|loose
argument_list|)
end_macro
begin_block
block|{
name|close_deflated_stream
argument_list|(
name|st
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapped
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapsize
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_block
begin_decl_stmt
DECL|variable|loose_vtbl
specifier|static
name|struct
name|stream_vtbl
name|loose_vtbl
init|=
block|{
name|close_istream_loose
block|,
name|read_istream_loose
block|, }
decl_stmt|;
end_decl_stmt
begin_expr_stmt
DECL|function|open_method_decl
specifier|static
name|open_method_decl
argument_list|(
argument|loose
argument_list|)
block|{
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapped
operator|=
name|map_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapsize
argument_list|)
block|;
if|if
condition|(
operator|!
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapped
condition|)
return|return
operator|-
literal|1
return|;
end_expr_stmt
begin_if
if|if
condition|(
name|unpack_sha1_header
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapped
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapsize
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
block|{
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapped
argument_list|,
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|mapsize
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_if
begin_expr_stmt
name|parse_sha1_header
argument_list|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr
argument_list|,
operator|&
name|st
operator|->
name|size
argument_list|)
expr_stmt|;
end_expr_stmt
begin_expr_stmt
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_used
operator|=
name|strlen
argument_list|(
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr
argument_list|)
operator|+
literal|1
expr_stmt|;
end_expr_stmt
begin_expr_stmt
name|st
operator|->
name|u
operator|.
name|loose
operator|.
name|hdr_avail
operator|=
name|st
operator|->
name|z
operator|.
name|total_out
expr_stmt|;
end_expr_stmt
begin_expr_stmt
name|st
operator|->
name|z_state
operator|=
name|z_used
expr_stmt|;
end_expr_stmt
begin_expr_stmt
name|st
operator|->
name|vtbl
operator|=
operator|&
name|loose_vtbl
expr_stmt|;
end_expr_stmt
begin_return
return|return
literal|0
return|;
end_return
begin_comment
unit|}
comment|/*****************************************************************  *  * Non-delta packed object stream  *  *****************************************************************/
end_comment
begin_macro
unit|static
DECL|function|read_method_decl
name|read_method_decl
argument_list|(
argument|pack_non_delta
argument_list|)
end_macro
begin_block
block|{
name|size_t
name|total_read
init|=
literal|0
decl_stmt|;
switch|switch
condition|(
name|st
operator|->
name|z_state
condition|)
block|{
case|case
name|z_unused
case|:
name|memset
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|st
operator|->
name|z
argument_list|)
argument_list|)
expr_stmt|;
name|git_inflate_init
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|st
operator|->
name|z_state
operator|=
name|z_used
expr_stmt|;
break|break;
case|case
name|z_done
case|:
return|return
literal|0
return|;
case|case
name|z_error
case|:
return|return
operator|-
literal|1
return|;
case|case
name|z_used
case|:
break|break;
block|}
while|while
condition|(
name|total_read
operator|<
name|sz
condition|)
block|{
name|int
name|status
decl_stmt|;
name|struct
name|pack_window
modifier|*
name|window
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
modifier|*
name|mapped
decl_stmt|;
name|mapped
operator|=
name|use_pack
argument_list|(
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pack
argument_list|,
operator|&
name|window
argument_list|,
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pos
argument_list|,
operator|&
name|st
operator|->
name|z
operator|.
name|avail_in
argument_list|)
expr_stmt|;
name|st
operator|->
name|z
operator|.
name|next_out
operator|=
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buf
operator|+
name|total_read
expr_stmt|;
name|st
operator|->
name|z
operator|.
name|avail_out
operator|=
name|sz
operator|-
name|total_read
expr_stmt|;
name|st
operator|->
name|z
operator|.
name|next_in
operator|=
name|mapped
expr_stmt|;
name|status
operator|=
name|git_inflate
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|,
name|Z_FINISH
argument_list|)
expr_stmt|;
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pos
operator|+=
name|st
operator|->
name|z
operator|.
name|next_in
operator|-
name|mapped
expr_stmt|;
name|total_read
operator|=
name|st
operator|->
name|z
operator|.
name|next_out
operator|-
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buf
expr_stmt|;
name|unuse_pack
argument_list|(
operator|&
name|window
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
operator|==
name|Z_STREAM_END
condition|)
block|{
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|st
operator|->
name|z_state
operator|=
name|z_done
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|status
operator|!=
name|Z_OK
operator|&&
name|status
operator|!=
name|Z_BUF_ERROR
condition|)
block|{
name|git_inflate_end
argument_list|(
operator|&
name|st
operator|->
name|z
argument_list|)
expr_stmt|;
name|st
operator|->
name|z_state
operator|=
name|z_error
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
return|return
name|total_read
return|;
block|}
end_block
begin_expr_stmt
DECL|function|close_method_decl
specifier|static
name|close_method_decl
argument_list|(
argument|pack_non_delta
argument_list|)
block|{
name|close_deflated_stream
argument_list|(
name|st
argument_list|)
block|;
return|return
literal|0
return|;
block|}
end_expr_stmt
begin_decl_stmt
DECL|variable|pack_non_delta_vtbl
specifier|static
name|struct
name|stream_vtbl
name|pack_non_delta_vtbl
init|=
block|{
name|close_istream_pack_non_delta
block|,
name|read_istream_pack_non_delta
block|, }
decl_stmt|;
end_decl_stmt
begin_expr_stmt
DECL|function|open_method_decl
specifier|static
name|open_method_decl
argument_list|(
argument|pack_non_delta
argument_list|)
block|{ 	struct
name|pack_window
operator|*
name|window
block|; 	enum
name|object_type
name|in_pack_type
block|;
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pack
operator|=
name|oi
operator|->
name|u
operator|.
name|packed
operator|.
name|pack
block|;
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pos
operator|=
name|oi
operator|->
name|u
operator|.
name|packed
operator|.
name|offset
block|;
name|window
operator|=
name|NULL
block|;
name|in_pack_type
operator|=
name|unpack_object_header
argument_list|(
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pack
argument_list|,
operator|&
name|window
argument_list|,
operator|&
name|st
operator|->
name|u
operator|.
name|in_pack
operator|.
name|pos
argument_list|,
operator|&
name|st
operator|->
name|size
argument_list|)
block|;
name|unuse_pack
argument_list|(
operator|&
name|window
argument_list|)
block|;
switch|switch
condition|(
name|in_pack_type
condition|)
block|{
default|default:
return|return
operator|-
literal|1
return|;
comment|/* we do not do deltas for now */
case|case
name|OBJ_COMMIT
case|:
case|case
name|OBJ_TREE
case|:
case|case
name|OBJ_BLOB
case|:
case|case
name|OBJ_TAG
case|:
break|break;
block|}
name|st
operator|->
name|z_state
operator|=
name|z_unused
expr_stmt|;
end_expr_stmt
begin_expr_stmt
name|st
operator|->
name|vtbl
operator|=
operator|&
name|pack_non_delta_vtbl
expr_stmt|;
end_expr_stmt
begin_return
return|return
literal|0
return|;
end_return
begin_comment
unit|}
comment|/*****************************************************************  *  * In-core stream  *  *****************************************************************/
end_comment
begin_macro
unit|static
DECL|function|close_method_decl
name|close_method_decl
argument_list|(
argument|incore
argument_list|)
end_macro
begin_block
block|{
name|free
argument_list|(
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_block
begin_expr_stmt
DECL|function|read_method_decl
specifier|static
name|read_method_decl
argument_list|(
argument|incore
argument_list|)
block|{
name|size_t
name|read_size
operator|=
name|sz
block|;
name|size_t
name|remainder
operator|=
name|st
operator|->
name|size
operator|-
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|read_ptr
block|;
if|if
condition|(
name|remainder
operator|<=
name|read_size
condition|)
name|read_size
operator|=
name|remainder
expr_stmt|;
end_expr_stmt
begin_if
if|if
condition|(
name|read_size
condition|)
block|{
name|memcpy
argument_list|(
name|buf
argument_list|,
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|buf
operator|+
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|read_ptr
argument_list|,
name|read_size
argument_list|)
expr_stmt|;
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|read_ptr
operator|+=
name|read_size
expr_stmt|;
block|}
end_if
begin_return
return|return
name|read_size
return|;
end_return
begin_decl_stmt
unit|}  static
DECL|variable|incore_vtbl
name|struct
name|stream_vtbl
name|incore_vtbl
init|=
block|{
name|close_istream_incore
block|,
name|read_istream_incore
block|, }
decl_stmt|;
end_decl_stmt
begin_expr_stmt
DECL|function|open_method_decl
specifier|static
name|open_method_decl
argument_list|(
argument|incore
argument_list|)
block|{
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|buf
operator|=
name|read_sha1_file_extended
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|st
operator|->
name|size
argument_list|,
literal|0
argument_list|)
block|;
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|read_ptr
operator|=
literal|0
block|;
name|st
operator|->
name|vtbl
operator|=
operator|&
name|incore_vtbl
block|;
return|return
name|st
operator|->
name|u
operator|.
name|incore
operator|.
name|buf
condition|?
literal|0
else|:
operator|-
literal|1
return|;
block|}
end_expr_stmt
end_unit
