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
DECL|member|fd
name|int
name|fd
decl_stmt|;
comment|/* open for reading */
comment|/* NEEDSWORK: what else? */
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
comment|/*****************************************************************  *  * Loose object stream  *  *****************************************************************/
end_comment
begin_expr_stmt
DECL|function|open_method_decl
specifier|static
name|open_method_decl
argument_list|(
argument|loose
argument_list|)
block|{
return|return
operator|-
literal|1
return|;
comment|/* for now */
block|}
end_expr_stmt
begin_comment
comment|/*****************************************************************  *  * Non-delta packed object stream  *  *****************************************************************/
end_comment
begin_expr_stmt
DECL|function|read_method_decl
specifier|static
name|read_method_decl
argument_list|(
argument|pack_non_delta
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
end_expr_stmt
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
argument|pack_non_delta
argument_list|)
end_macro
begin_block
block|{
name|close_deflated_stream
argument_list|(
name|st
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_block
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
