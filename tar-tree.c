begin_unit
begin_include
include|#
directive|include
file|<time.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_define
DECL|macro|RECORDSIZE
define|#
directive|define
name|RECORDSIZE
value|(512)
end_define
begin_define
DECL|macro|BLOCKSIZE
define|#
directive|define
name|BLOCKSIZE
value|(RECORDSIZE * 20)
end_define
begin_define
DECL|macro|TYPEFLAG_AUTO
define|#
directive|define
name|TYPEFLAG_AUTO
value|'\0'
end_define
begin_define
DECL|macro|TYPEFLAG_REG
define|#
directive|define
name|TYPEFLAG_REG
value|'0'
end_define
begin_define
DECL|macro|TYPEFLAG_LNK
define|#
directive|define
name|TYPEFLAG_LNK
value|'2'
end_define
begin_define
DECL|macro|TYPEFLAG_DIR
define|#
directive|define
name|TYPEFLAG_DIR
value|'5'
end_define
begin_define
DECL|macro|TYPEFLAG_GLOBAL_HEADER
define|#
directive|define
name|TYPEFLAG_GLOBAL_HEADER
value|'g'
end_define
begin_define
DECL|macro|TYPEFLAG_EXT_HEADER
define|#
directive|define
name|TYPEFLAG_EXT_HEADER
value|'x'
end_define
begin_define
DECL|macro|EXT_HEADER_PATH
define|#
directive|define
name|EXT_HEADER_PATH
value|1
end_define
begin_define
DECL|macro|EXT_HEADER_LINKPATH
define|#
directive|define
name|EXT_HEADER_LINKPATH
value|2
end_define
begin_decl_stmt
DECL|variable|tar_tree_usage
specifier|static
specifier|const
name|char
modifier|*
name|tar_tree_usage
init|=
literal|"tar-tree<key> [basedir]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|block
specifier|static
name|char
name|block
index|[
name|BLOCKSIZE
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|offset
specifier|static
name|unsigned
name|long
name|offset
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|basedir
specifier|static
specifier|const
name|char
modifier|*
name|basedir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|archive_time
specifier|static
name|time_t
name|archive_time
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|path_prefix
struct|struct
name|path_prefix
block|{
DECL|member|prev
name|struct
name|path_prefix
modifier|*
name|prev
decl_stmt|;
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/* tries hard to write, either succeeds or dies in the attempt */
end_comment
begin_function
DECL|function|reliable_write
specifier|static
name|void
name|reliable_write
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
while|while
condition|(
name|size
operator|>
literal|0
condition|)
block|{
name|long
name|ret
init|=
name|write
argument_list|(
literal|1
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EAGAIN
condition|)
continue|continue;
if|if
condition|(
name|errno
operator|==
name|EPIPE
condition|)
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"tar-tree: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|die
argument_list|(
literal|"tar-tree: disk full?"
argument_list|)
expr_stmt|;
block|}
name|size
operator|-=
name|ret
expr_stmt|;
name|buf
operator|+=
name|ret
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* writes out the whole block, but only if it is full */
end_comment
begin_function
DECL|function|write_if_needed
specifier|static
name|void
name|write_if_needed
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|offset
operator|==
name|BLOCKSIZE
condition|)
block|{
name|reliable_write
argument_list|(
name|block
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
name|offset
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* acquire the next record from the buffer; user must call write_if_needed() */
end_comment
begin_function
DECL|function|get_record
specifier|static
name|char
modifier|*
name|get_record
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
name|block
operator|+
name|offset
decl_stmt|;
name|memset
argument_list|(
name|p
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
expr_stmt|;
return|return
name|p
return|;
block|}
end_function
begin_comment
comment|/*  * The end of tar archives is marked by 1024 nul bytes and after that  * follows the rest of the block (if any).  */
end_comment
begin_function
DECL|function|write_trailer
specifier|static
name|void
name|write_trailer
parameter_list|(
name|void
parameter_list|)
block|{
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
if|if
condition|(
name|offset
condition|)
block|{
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|BLOCKSIZE
operator|-
name|offset
argument_list|)
expr_stmt|;
name|reliable_write
argument_list|(
name|block
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
name|offset
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * queues up writes, so that all our write(2) calls write exactly one  * full block; pads writes to RECORDSIZE  */
end_comment
begin_function
DECL|function|write_blocked
specifier|static
name|void
name|write_blocked
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|long
name|tail
decl_stmt|;
if|if
condition|(
name|offset
condition|)
block|{
name|unsigned
name|long
name|chunk
init|=
name|BLOCKSIZE
operator|-
name|offset
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|chunk
condition|)
name|chunk
operator|=
name|size
expr_stmt|;
name|memcpy
argument_list|(
name|block
operator|+
name|offset
argument_list|,
name|buf
argument_list|,
name|chunk
argument_list|)
expr_stmt|;
name|size
operator|-=
name|chunk
expr_stmt|;
name|offset
operator|+=
name|chunk
expr_stmt|;
name|buf
operator|+=
name|chunk
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
while|while
condition|(
name|size
operator|>=
name|BLOCKSIZE
condition|)
block|{
name|reliable_write
argument_list|(
name|buf
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
name|size
operator|-=
name|BLOCKSIZE
expr_stmt|;
name|buf
operator|+=
name|BLOCKSIZE
expr_stmt|;
block|}
if|if
condition|(
name|size
condition|)
block|{
name|memcpy
argument_list|(
name|block
operator|+
name|offset
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buf
operator|+=
name|size
expr_stmt|;
name|offset
operator|+=
name|size
expr_stmt|;
block|}
name|tail
operator|=
name|offset
operator|%
name|RECORDSIZE
expr_stmt|;
if|if
condition|(
name|tail
condition|)
block|{
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|RECORDSIZE
operator|-
name|tail
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|RECORDSIZE
operator|-
name|tail
expr_stmt|;
block|}
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|append_string
specifier|static
name|void
name|append_string
parameter_list|(
name|char
modifier|*
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|unsigned
name|int
name|len
init|=
name|strlen
argument_list|(
name|s
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
operator|*
name|p
argument_list|,
name|s
argument_list|,
name|len
argument_list|)
expr_stmt|;
operator|*
name|p
operator|+=
name|len
expr_stmt|;
block|}
end_function
begin_function
DECL|function|append_char
specifier|static
name|void
name|append_char
parameter_list|(
name|char
modifier|*
modifier|*
name|p
parameter_list|,
name|char
name|c
parameter_list|)
block|{
operator|*
operator|*
name|p
operator|=
name|c
expr_stmt|;
operator|*
name|p
operator|+=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|append_path_prefix
specifier|static
name|void
name|append_path_prefix
parameter_list|(
name|char
modifier|*
modifier|*
name|buffer
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|)
block|{
if|if
condition|(
operator|!
name|prefix
condition|)
return|return;
name|append_path_prefix
argument_list|(
name|buffer
argument_list|,
name|prefix
operator|->
name|prev
argument_list|)
expr_stmt|;
name|append_string
argument_list|(
name|buffer
argument_list|,
name|prefix
operator|->
name|name
argument_list|)
expr_stmt|;
name|append_char
argument_list|(
name|buffer
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|path_prefix_len
specifier|static
name|unsigned
name|int
name|path_prefix_len
parameter_list|(
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|)
block|{
if|if
condition|(
operator|!
name|prefix
condition|)
return|return
literal|0
return|;
return|return
name|path_prefix_len
argument_list|(
name|prefix
operator|->
name|prev
argument_list|)
operator|+
name|strlen
argument_list|(
name|prefix
operator|->
name|name
argument_list|)
operator|+
literal|1
return|;
block|}
end_function
begin_function
DECL|function|append_path
specifier|static
name|void
name|append_path
parameter_list|(
name|char
modifier|*
modifier|*
name|p
parameter_list|,
name|int
name|is_dir
parameter_list|,
specifier|const
name|char
modifier|*
name|basepath
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
if|if
condition|(
name|basepath
condition|)
block|{
name|append_string
argument_list|(
name|p
argument_list|,
name|basepath
argument_list|)
expr_stmt|;
name|append_char
argument_list|(
name|p
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
name|append_path_prefix
argument_list|(
name|p
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|append_string
argument_list|(
name|p
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_dir
condition|)
name|append_char
argument_list|(
name|p
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|path_len
specifier|static
name|unsigned
name|int
name|path_len
parameter_list|(
name|int
name|is_dir
parameter_list|,
specifier|const
name|char
modifier|*
name|basepath
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|unsigned
name|int
name|len
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|basepath
condition|)
name|len
operator|+=
name|strlen
argument_list|(
name|basepath
argument_list|)
operator|+
literal|1
expr_stmt|;
name|len
operator|+=
name|path_prefix_len
argument_list|(
name|prefix
argument_list|)
operator|+
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_dir
condition|)
name|len
operator|++
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|append_extended_header_prefix
specifier|static
name|void
name|append_extended_header_prefix
parameter_list|(
name|char
modifier|*
modifier|*
name|p
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|keyword
parameter_list|)
block|{
name|int
name|len
init|=
name|sprintf
argument_list|(
operator|*
name|p
argument_list|,
literal|"%u %s="
argument_list|,
name|size
argument_list|,
name|keyword
argument_list|)
decl_stmt|;
operator|*
name|p
operator|+=
name|len
expr_stmt|;
block|}
end_function
begin_function
DECL|function|extended_header_len
specifier|static
name|unsigned
name|int
name|extended_header_len
parameter_list|(
specifier|const
name|char
modifier|*
name|keyword
parameter_list|,
name|unsigned
name|int
name|valuelen
parameter_list|)
block|{
comment|/* "%u %s=%s\n" */
name|unsigned
name|int
name|len
init|=
literal|1
operator|+
literal|1
operator|+
name|strlen
argument_list|(
name|keyword
argument_list|)
operator|+
literal|1
operator|+
name|valuelen
operator|+
literal|1
decl_stmt|;
if|if
condition|(
name|len
operator|>
literal|9
condition|)
name|len
operator|++
expr_stmt|;
if|if
condition|(
name|len
operator|>
literal|99
condition|)
name|len
operator|++
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|append_extended_header
specifier|static
name|void
name|append_extended_header
parameter_list|(
name|char
modifier|*
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|keyword
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|unsigned
name|int
name|len
parameter_list|)
block|{
name|unsigned
name|int
name|size
init|=
name|extended_header_len
argument_list|(
name|keyword
argument_list|,
name|len
argument_list|)
decl_stmt|;
name|append_extended_header_prefix
argument_list|(
name|p
argument_list|,
name|size
argument_list|,
name|keyword
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
operator|*
name|p
argument_list|,
name|value
argument_list|,
name|len
argument_list|)
expr_stmt|;
operator|*
name|p
operator|+=
name|len
expr_stmt|;
name|append_char
argument_list|(
name|p
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|write_header
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
name|char
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|struct
name|path_prefix
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|unsigned
name|int
parameter_list|,
name|void
modifier|*
parameter_list|,
name|unsigned
name|long
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* stores a pax extended header directly in the block buffer */
end_comment
begin_function
DECL|function|write_extended_header
specifier|static
name|void
name|write_extended_header
parameter_list|(
specifier|const
name|char
modifier|*
name|headerfilename
parameter_list|,
name|int
name|is_dir
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|,
specifier|const
name|char
modifier|*
name|basepath
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|namelen
parameter_list|,
name|void
modifier|*
name|content
parameter_list|,
name|unsigned
name|int
name|contentsize
parameter_list|)
block|{
name|char
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|unsigned
name|int
name|pathlen
decl_stmt|,
name|size
decl_stmt|,
name|linkpathlen
init|=
literal|0
decl_stmt|;
name|size
operator|=
name|pathlen
operator|=
name|extended_header_len
argument_list|(
literal|"path"
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
name|flags
operator|&
name|EXT_HEADER_LINKPATH
condition|)
block|{
name|linkpathlen
operator|=
name|extended_header_len
argument_list|(
literal|"linkpath"
argument_list|,
name|contentsize
argument_list|)
expr_stmt|;
name|size
operator|+=
name|linkpathlen
expr_stmt|;
block|}
name|write_header
argument_list|(
name|NULL
argument_list|,
name|TYPEFLAG_EXT_HEADER
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|headerfilename
argument_list|,
literal|0100600
argument_list|,
name|NULL
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|p
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
name|die
argument_list|(
literal|"git-tar-tree: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|append_extended_header_prefix
argument_list|(
operator|&
name|p
argument_list|,
name|pathlen
argument_list|,
literal|"path"
argument_list|)
expr_stmt|;
name|append_path
argument_list|(
operator|&
name|p
argument_list|,
name|is_dir
argument_list|,
name|basepath
argument_list|,
name|prefix
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|append_char
argument_list|(
operator|&
name|p
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|flags
operator|&
name|EXT_HEADER_LINKPATH
condition|)
name|append_extended_header
argument_list|(
operator|&
name|p
argument_list|,
literal|"linkpath"
argument_list|,
name|content
argument_list|,
name|contentsize
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_global_extended_header
specifier|static
name|void
name|write_global_extended_header
parameter_list|(
specifier|const
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|char
modifier|*
name|p
decl_stmt|;
name|unsigned
name|int
name|size
decl_stmt|;
name|size
operator|=
name|extended_header_len
argument_list|(
literal|"comment"
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_header
argument_list|(
name|NULL
argument_list|,
name|TYPEFLAG_GLOBAL_HEADER
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|"pax_global_header"
argument_list|,
literal|0100600
argument_list|,
name|NULL
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|p
operator|=
name|get_record
argument_list|()
expr_stmt|;
name|append_extended_header
argument_list|(
operator|&
name|p
argument_list|,
literal|"comment"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
end_function
begin_comment
comment|/* stores a ustar header directly in the block buffer */
end_comment
begin_function
DECL|function|write_header
specifier|static
name|void
name|write_header
parameter_list|(
specifier|const
name|char
modifier|*
name|sha1
parameter_list|,
name|char
name|typeflag
parameter_list|,
specifier|const
name|char
modifier|*
name|basepath
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|unsigned
name|int
name|namelen
decl_stmt|;
name|char
modifier|*
name|header
init|=
name|NULL
decl_stmt|;
name|unsigned
name|int
name|checksum
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
name|unsigned
name|int
name|ext_header
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|typeflag
operator|==
name|TYPEFLAG_AUTO
condition|)
block|{
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|typeflag
operator|=
name|TYPEFLAG_DIR
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
name|typeflag
operator|=
name|TYPEFLAG_LNK
expr_stmt|;
else|else
name|typeflag
operator|=
name|TYPEFLAG_REG
expr_stmt|;
block|}
name|namelen
operator|=
name|path_len
argument_list|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
argument_list|,
name|basepath
argument_list|,
name|prefix
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|namelen
operator|>
literal|100
condition|)
name|ext_header
operator||=
name|EXT_HEADER_PATH
expr_stmt|;
if|if
condition|(
name|typeflag
operator|==
name|TYPEFLAG_LNK
operator|&&
name|size
operator|>
literal|100
condition|)
name|ext_header
operator||=
name|EXT_HEADER_LINKPATH
expr_stmt|;
comment|/* the extended header must be written before the normal one */
if|if
condition|(
name|ext_header
condition|)
block|{
name|char
name|headerfilename
index|[
literal|51
index|]
decl_stmt|;
name|sprintf
argument_list|(
name|headerfilename
argument_list|,
literal|"%s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|write_extended_header
argument_list|(
name|headerfilename
argument_list|,
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
argument_list|,
name|ext_header
argument_list|,
name|basepath
argument_list|,
name|prefix
argument_list|,
name|path
argument_list|,
name|namelen
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|header
operator|=
name|get_record
argument_list|()
expr_stmt|;
if|if
condition|(
name|ext_header
condition|)
block|{
name|sprintf
argument_list|(
name|header
argument_list|,
literal|"%s.data"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|char
modifier|*
name|p
init|=
name|header
decl_stmt|;
name|append_path
argument_list|(
operator|&
name|p
argument_list|,
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
argument_list|,
name|basepath
argument_list|,
name|prefix
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|typeflag
operator|==
name|TYPEFLAG_LNK
condition|)
block|{
if|if
condition|(
name|ext_header
operator|&
name|EXT_HEADER_LINKPATH
condition|)
block|{
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|157
index|]
argument_list|,
literal|"see %s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|buffer
condition|)
name|strncpy
argument_list|(
operator|&
name|header
index|[
literal|157
index|]
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
name|mode
operator||=
literal|0755
expr_stmt|;
comment|/* GIT doesn't store permissions of dirs */
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
name|mode
operator||=
literal|0777
expr_stmt|;
comment|/* ... nor of symlinks */
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|100
index|]
argument_list|,
literal|"%07o"
argument_list|,
name|mode
operator|&
literal|07777
argument_list|)
expr_stmt|;
comment|/* XXX: should we provide more meaningful info here? */
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|108
index|]
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
comment|/* uid */
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|116
index|]
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
comment|/* gid */
name|strncpy
argument_list|(
operator|&
name|header
index|[
literal|265
index|]
argument_list|,
literal|"git"
argument_list|,
literal|31
argument_list|)
expr_stmt|;
comment|/* uname */
name|strncpy
argument_list|(
operator|&
name|header
index|[
literal|297
index|]
argument_list|,
literal|"git"
argument_list|,
literal|31
argument_list|)
expr_stmt|;
comment|/* gname */
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
operator|||
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
name|size
operator|=
literal|0
expr_stmt|;
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|124
index|]
argument_list|,
literal|"%011lo"
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|136
index|]
argument_list|,
literal|"%011lo"
argument_list|,
name|archive_time
argument_list|)
expr_stmt|;
name|header
index|[
literal|156
index|]
operator|=
name|typeflag
expr_stmt|;
name|memcpy
argument_list|(
operator|&
name|header
index|[
literal|257
index|]
argument_list|,
literal|"ustar"
argument_list|,
literal|6
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
operator|&
name|header
index|[
literal|263
index|]
argument_list|,
literal|"00"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|printf
argument_list|(
operator|&
name|header
index|[
literal|329
index|]
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
comment|/* devmajor */
name|printf
argument_list|(
operator|&
name|header
index|[
literal|337
index|]
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
comment|/* devminor */
name|memset
argument_list|(
operator|&
name|header
index|[
literal|148
index|]
argument_list|,
literal|' '
argument_list|,
literal|8
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
name|RECORDSIZE
condition|;
name|i
operator|++
control|)
name|checksum
operator|+=
name|header
index|[
name|i
index|]
expr_stmt|;
name|sprintf
argument_list|(
operator|&
name|header
index|[
literal|148
index|]
argument_list|,
literal|"%07o"
argument_list|,
name|checksum
operator|&
literal|0x1fffff
argument_list|)
expr_stmt|;
name|write_if_needed
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|traverse_tree
specifier|static
name|void
name|traverse_tree
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|struct
name|path_prefix
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|path_prefix
name|this_prefix
decl_stmt|;
name|this_prefix
operator|.
name|prev
operator|=
name|prefix
expr_stmt|;
while|while
condition|(
name|size
condition|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|buffer
argument_list|)
operator|+
literal|1
decl_stmt|;
name|void
modifier|*
name|eltbuf
decl_stmt|;
name|char
name|elttype
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|eltsize
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|buffer
operator|+
name|namelen
decl_stmt|;
name|char
modifier|*
name|path
init|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|' '
argument_list|)
operator|+
literal|1
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|namelen
operator|+
literal|20
operator|||
name|sscanf
argument_list|(
name|buffer
argument_list|,
literal|"%o"
argument_list|,
operator|&
name|mode
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"corrupt 'tree' file"
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|sha1
operator|+
literal|20
expr_stmt|;
name|size
operator|-=
name|namelen
operator|+
literal|20
expr_stmt|;
name|eltbuf
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|elttype
argument_list|,
operator|&
name|eltsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eltbuf
condition|)
name|die
argument_list|(
literal|"cannot read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|write_header
argument_list|(
name|sha1
argument_list|,
name|TYPEFLAG_AUTO
argument_list|,
name|basedir
argument_list|,
name|prefix
argument_list|,
name|path
argument_list|,
name|mode
argument_list|,
name|eltbuf
argument_list|,
name|eltsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|elttype
argument_list|,
literal|"tree"
argument_list|)
condition|)
block|{
name|this_prefix
operator|.
name|name
operator|=
name|path
expr_stmt|;
name|traverse_tree
argument_list|(
name|eltbuf
argument_list|,
name|eltsize
argument_list|,
operator|&
name|this_prefix
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|elttype
argument_list|,
literal|"blob"
argument_list|)
operator|&&
operator|!
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|write_blocked
argument_list|(
name|eltbuf
argument_list|,
name|eltsize
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|eltbuf
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* get commit time from committer line of commit object */
end_comment
begin_function
DECL|function|commit_time
name|time_t
name|commit_time
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|time_t
name|result
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|p
init|=
name|buffer
decl_stmt|;
while|while
condition|(
name|size
operator|>
literal|0
condition|)
block|{
name|char
modifier|*
name|endp
init|=
name|memchr
argument_list|(
name|p
argument_list|,
literal|'\n'
argument_list|,
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|endp
operator|||
name|endp
operator|==
name|p
condition|)
break|break;
operator|*
name|endp
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|endp
operator|-
name|p
operator|>
literal|10
operator|&&
operator|!
name|memcmp
argument_list|(
name|p
argument_list|,
literal|"committer "
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|char
modifier|*
name|nump
init|=
name|strrchr
argument_list|(
name|p
argument_list|,
literal|'>'
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|nump
condition|)
break|break;
name|nump
operator|++
expr_stmt|;
name|result
operator|=
name|strtoul
argument_list|(
name|nump
argument_list|,
operator|&
name|endp
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|endp
operator|!=
literal|' '
condition|)
name|result
operator|=
literal|0
expr_stmt|;
break|break;
block|}
name|size
operator|-=
name|endp
operator|-
name|p
operator|-
literal|1
expr_stmt|;
name|p
operator|=
name|endp
operator|+
literal|1
expr_stmt|;
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|commit_sha1
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
switch|switch
condition|(
name|argc
condition|)
block|{
case|case
literal|3
case|:
name|basedir
operator|=
name|argv
index|[
literal|2
index|]
expr_stmt|;
comment|/* FALLTHROUGH */
case|case
literal|2
case|:
if|if
condition|(
name|get_sha1
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|usage
argument_list|(
name|tar_tree_usage
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|tar_tree_usage
argument_list|)
expr_stmt|;
block|}
name|buffer
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
literal|"commit"
argument_list|,
operator|&
name|size
argument_list|,
name|commit_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
condition|)
block|{
name|write_global_extended_header
argument_list|(
name|commit_sha1
argument_list|)
expr_stmt|;
name|archive_time
operator|=
name|commit_time
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
name|buffer
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
literal|"tree"
argument_list|,
operator|&
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
name|die
argument_list|(
literal|"not a reference to a tag, commit or tree object: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|archive_time
condition|)
name|archive_time
operator|=
name|time
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|basedir
condition|)
name|write_header
argument_list|(
literal|"0"
argument_list|,
name|TYPEFLAG_DIR
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|basedir
argument_list|,
literal|040755
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|traverse_tree
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|write_trailer
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
