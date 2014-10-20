begin_unit
begin_comment
comment|/*  * Copyright (c) 2005, 2006 Rene Scharfe  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tar.h"
end_include
begin_include
include|#
directive|include
file|"archive.h"
end_include
begin_include
include|#
directive|include
file|"streaming.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
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
DECL|variable|tar_umask
specifier|static
name|int
name|tar_umask
init|=
literal|002
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|static
name|int
name|write_tar_filter_archive
parameter_list|(
specifier|const
name|struct
name|archiver
modifier|*
name|ar
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|)
function_decl|;
end_function_decl
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
name|write_or_die
argument_list|(
literal|1
argument_list|,
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
DECL|function|do_write_blocked
specifier|static
name|void
name|do_write_blocked
parameter_list|(
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|buf
init|=
name|data
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
name|write_or_die
argument_list|(
literal|1
argument_list|,
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
name|offset
operator|+=
name|size
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|finish_record
specifier|static
name|void
name|finish_record
parameter_list|(
name|void
parameter_list|)
block|{
name|unsigned
name|long
name|tail
decl_stmt|;
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
DECL|function|write_blocked
specifier|static
name|void
name|write_blocked
parameter_list|(
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|do_write_blocked
argument_list|(
name|data
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|finish_record
argument_list|()
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * The end of tar archives is marked by 2*512 nul bytes and after that  * follows the rest of the block (if any).  */
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
name|int
name|tail
init|=
name|BLOCKSIZE
operator|-
name|offset
decl_stmt|;
name|memset
argument_list|(
name|block
operator|+
name|offset
argument_list|,
literal|0
argument_list|,
name|tail
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
literal|1
argument_list|,
name|block
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
if|if
condition|(
name|tail
operator|<
literal|2
operator|*
name|RECORDSIZE
condition|)
block|{
name|memset
argument_list|(
name|block
argument_list|,
literal|0
argument_list|,
name|offset
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
literal|1
argument_list|,
name|block
argument_list|,
name|BLOCKSIZE
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * queues up writes, so that all our write(2) calls write exactly one  * full block; pads writes to RECORDSIZE  */
end_comment
begin_function
DECL|function|stream_blocked
specifier|static
name|int
name|stream_blocked
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|git_istream
modifier|*
name|st
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|unsigned
name|long
name|sz
decl_stmt|;
name|char
name|buf
index|[
name|BLOCKSIZE
index|]
decl_stmt|;
name|ssize_t
name|readlen
decl_stmt|;
name|st
operator|=
name|open_istream
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|sz
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|st
condition|)
return|return
name|error
argument_list|(
literal|"cannot stream blob %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|readlen
operator|=
name|read_istream
argument_list|(
name|st
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|readlen
operator|<=
literal|0
condition|)
break|break;
name|do_write_blocked
argument_list|(
name|buf
argument_list|,
name|readlen
argument_list|)
expr_stmt|;
block|}
name|close_istream
argument_list|(
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|readlen
condition|)
name|finish_record
argument_list|()
expr_stmt|;
return|return
name|readlen
return|;
block|}
end_function
begin_comment
comment|/*  * pax extended header records have the format "%u %s=%s\n".  %u contains  * the size of the whole string (including the %u), the first %s is the  * keyword, the second one is the value.  This function constructs such a  * string and appends it to a struct strbuf.  */
end_comment
begin_function
DECL|function|strbuf_append_ext_header
specifier|static
name|void
name|strbuf_append_ext_header
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
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
name|valuelen
parameter_list|)
block|{
name|int
name|len
decl_stmt|,
name|tmp
decl_stmt|;
comment|/* "%u %s=%s\n" */
name|len
operator|=
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
expr_stmt|;
for|for
control|(
name|tmp
operator|=
name|len
init|;
name|tmp
operator|>
literal|9
condition|;
name|tmp
operator|/=
literal|10
control|)
name|len
operator|++
expr_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
name|sb
argument_list|,
literal|"%u %s="
argument_list|,
name|len
argument_list|,
name|keyword
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|value
argument_list|,
name|valuelen
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ustar_header_chksum
specifier|static
name|unsigned
name|int
name|ustar_header_chksum
parameter_list|(
specifier|const
name|struct
name|ustar_header
modifier|*
name|header
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|p
init|=
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|header
decl_stmt|;
name|unsigned
name|int
name|chksum
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|p
operator|<
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|header
operator|->
name|chksum
condition|)
name|chksum
operator|+=
operator|*
name|p
operator|++
expr_stmt|;
name|chksum
operator|+=
sizeof|sizeof
argument_list|(
name|header
operator|->
name|chksum
argument_list|)
operator|*
literal|' '
expr_stmt|;
name|p
operator|+=
sizeof|sizeof
argument_list|(
name|header
operator|->
name|chksum
argument_list|)
expr_stmt|;
while|while
condition|(
name|p
operator|<
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|header
operator|+
sizeof|sizeof
argument_list|(
expr|struct
name|ustar_header
argument_list|)
condition|)
name|chksum
operator|+=
operator|*
name|p
operator|++
expr_stmt|;
return|return
name|chksum
return|;
block|}
end_function
begin_function
DECL|function|get_path_prefix
specifier|static
name|size_t
name|get_path_prefix
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|pathlen
parameter_list|,
name|size_t
name|maxlen
parameter_list|)
block|{
name|size_t
name|i
init|=
name|pathlen
decl_stmt|;
if|if
condition|(
name|i
operator|>
literal|1
operator|&&
name|path
index|[
name|i
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|i
operator|--
expr_stmt|;
if|if
condition|(
name|i
operator|>
name|maxlen
condition|)
name|i
operator|=
name|maxlen
expr_stmt|;
do|do
block|{
name|i
operator|--
expr_stmt|;
block|}
do|while
condition|(
name|i
operator|>
literal|0
operator|&&
name|path
index|[
name|i
index|]
operator|!=
literal|'/'
condition|)
do|;
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|prepare_header
specifier|static
name|void
name|prepare_header
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
name|struct
name|ustar_header
modifier|*
name|header
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|sprintf
argument_list|(
name|header
operator|->
name|mode
argument_list|,
literal|"%07o"
argument_list|,
name|mode
operator|&
literal|07777
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|size
argument_list|,
literal|"%011lo"
argument_list|,
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|?
name|size
else|:
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|mtime
argument_list|,
literal|"%011lo"
argument_list|,
operator|(
name|unsigned
name|long
operator|)
name|args
operator|->
name|time
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|uid
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|gid
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strlcpy
argument_list|(
name|header
operator|->
name|uname
argument_list|,
literal|"root"
argument_list|,
sizeof|sizeof
argument_list|(
name|header
operator|->
name|uname
argument_list|)
argument_list|)
expr_stmt|;
name|strlcpy
argument_list|(
name|header
operator|->
name|gname
argument_list|,
literal|"root"
argument_list|,
sizeof|sizeof
argument_list|(
name|header
operator|->
name|gname
argument_list|)
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|devmajor
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|devminor
argument_list|,
literal|"%07o"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|->
name|magic
argument_list|,
literal|"ustar"
argument_list|,
literal|6
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|->
name|version
argument_list|,
literal|"00"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|->
name|chksum
argument_list|,
literal|"%07o"
argument_list|,
name|ustar_header_chksum
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_extended_header
specifier|static
name|int
name|write_extended_header
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|struct
name|ustar_header
name|header
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|memset
argument_list|(
operator|&
name|header
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_EXT_HEADER
expr_stmt|;
name|mode
operator|=
literal|0100666
expr_stmt|;
name|sprintf
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"%s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|prepare_header
argument_list|(
name|args
argument_list|,
operator|&
name|header
argument_list|,
name|mode
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
operator|&
name|header
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_tar_entry
specifier|static
name|int
name|write_tar_entry
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|pathlen
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|)
block|{
name|struct
name|ustar_header
name|header
decl_stmt|;
name|struct
name|strbuf
name|ext_header
init|=
name|STRBUF_INIT
decl_stmt|;
name|unsigned
name|int
name|old_mode
init|=
name|mode
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|int
name|err
init|=
literal|0
decl_stmt|;
name|memset
argument_list|(
operator|&
name|header
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
operator|||
name|S_ISGITLINK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_DIR
expr_stmt|;
name|mode
operator|=
operator|(
name|mode
operator||
literal|0777
operator|)
operator|&
operator|~
name|tar_umask
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_LNK
expr_stmt|;
name|mode
operator||=
literal|0777
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
block|{
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_REG
expr_stmt|;
name|mode
operator|=
operator|(
name|mode
operator||
operator|(
operator|(
name|mode
operator|&
literal|0100
operator|)
condition|?
literal|0777
else|:
literal|0666
operator|)
operator|)
operator|&
operator|~
name|tar_umask
expr_stmt|;
block|}
else|else
block|{
return|return
name|error
argument_list|(
literal|"unsupported file mode: 0%o (SHA1: %s)"
argument_list|,
name|mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
block|}
if|if
condition|(
name|pathlen
operator|>
sizeof|sizeof
argument_list|(
name|header
operator|.
name|name
argument_list|)
condition|)
block|{
name|size_t
name|plen
init|=
name|get_path_prefix
argument_list|(
name|path
argument_list|,
name|pathlen
argument_list|,
sizeof|sizeof
argument_list|(
name|header
operator|.
name|prefix
argument_list|)
argument_list|)
decl_stmt|;
name|size_t
name|rest
init|=
name|pathlen
operator|-
name|plen
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|plen
operator|>
literal|0
operator|&&
name|rest
operator|<=
sizeof|sizeof
argument_list|(
name|header
operator|.
name|name
argument_list|)
condition|)
block|{
name|memcpy
argument_list|(
name|header
operator|.
name|prefix
argument_list|,
name|path
argument_list|,
name|plen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
name|path
operator|+
name|plen
operator|+
literal|1
argument_list|,
name|rest
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|sprintf
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"%s.data"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
argument_list|,
literal|"path"
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
block|}
block|}
else|else
name|memcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISREG
argument_list|(
name|mode
argument_list|)
operator|&&
operator|!
name|args
operator|->
name|convert
operator|&&
name|sha1_object_info
argument_list|(
name|sha1
argument_list|,
operator|&
name|size
argument_list|)
operator|==
name|OBJ_BLOB
operator|&&
name|size
operator|>
name|big_file_threshold
condition|)
name|buffer
operator|=
name|NULL
expr_stmt|;
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
operator|||
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|buffer
operator|=
name|sha1_file_to_archive
argument_list|(
name|args
argument_list|,
name|path
argument_list|,
name|sha1
argument_list|,
name|old_mode
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
name|error
argument_list|(
literal|"cannot read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
block|}
else|else
block|{
name|buffer
operator|=
name|NULL
expr_stmt|;
name|size
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
if|if
condition|(
name|size
operator|>
sizeof|sizeof
argument_list|(
name|header
operator|.
name|linkname
argument_list|)
condition|)
block|{
name|sprintf
argument_list|(
name|header
operator|.
name|linkname
argument_list|,
literal|"see %s.paxheader"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
argument_list|,
literal|"linkpath"
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
else|else
name|memcpy
argument_list|(
name|header
operator|.
name|linkname
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|prepare_header
argument_list|(
name|args
argument_list|,
operator|&
name|header
argument_list|,
name|mode
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|ext_header
operator|.
name|len
operator|>
literal|0
condition|)
block|{
name|err
operator|=
name|write_extended_header
argument_list|(
name|args
argument_list|,
name|sha1
argument_list|,
name|ext_header
operator|.
name|buf
argument_list|,
name|ext_header
operator|.
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|err
return|;
block|}
block|}
name|strbuf_release
argument_list|(
operator|&
name|ext_header
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
operator|&
name|header
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|S_ISREG
argument_list|(
name|mode
argument_list|)
operator|&&
name|size
operator|>
literal|0
condition|)
block|{
if|if
condition|(
name|buffer
condition|)
name|write_blocked
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
else|else
name|err
operator|=
name|stream_blocked
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|write_global_extended_header
specifier|static
name|int
name|write_global_extended_header
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|args
operator|->
name|commit_sha1
decl_stmt|;
name|struct
name|strbuf
name|ext_header
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|ustar_header
name|header
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
name|int
name|err
init|=
literal|0
decl_stmt|;
name|strbuf_append_ext_header
argument_list|(
operator|&
name|ext_header
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
name|memset
argument_list|(
operator|&
name|header
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
operator|*
name|header
operator|.
name|typeflag
operator|=
name|TYPEFLAG_GLOBAL_HEADER
expr_stmt|;
name|mode
operator|=
literal|0100666
expr_stmt|;
name|strcpy
argument_list|(
name|header
operator|.
name|name
argument_list|,
literal|"pax_global_header"
argument_list|)
expr_stmt|;
name|prepare_header
argument_list|(
name|args
argument_list|,
operator|&
name|header
argument_list|,
name|mode
argument_list|,
name|ext_header
operator|.
name|len
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
operator|&
name|header
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
name|write_blocked
argument_list|(
name|ext_header
operator|.
name|buf
argument_list|,
name|ext_header
operator|.
name|len
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|ext_header
argument_list|)
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|tar_filters
specifier|static
name|struct
name|archiver
modifier|*
modifier|*
name|tar_filters
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_tar_filters
specifier|static
name|int
name|nr_tar_filters
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|alloc_tar_filters
specifier|static
name|int
name|alloc_tar_filters
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|find_tar_filter
specifier|static
name|struct
name|archiver
modifier|*
name|find_tar_filter
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_tar_filters
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|archiver
modifier|*
name|ar
init|=
name|tar_filters
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ar
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
operator|&&
operator|!
name|ar
operator|->
name|name
index|[
name|len
index|]
condition|)
return|return
name|ar
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|tar_filter_config
specifier|static
name|int
name|tar_filter_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|archiver
modifier|*
name|ar
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
specifier|const
name|char
modifier|*
name|type
decl_stmt|;
name|int
name|namelen
decl_stmt|;
if|if
condition|(
name|parse_config_key
argument_list|(
name|var
argument_list|,
literal|"tar"
argument_list|,
operator|&
name|name
argument_list|,
operator|&
name|namelen
argument_list|,
operator|&
name|type
argument_list|)
operator|<
literal|0
operator|||
operator|!
name|name
condition|)
return|return
literal|0
return|;
name|ar
operator|=
name|find_tar_filter
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ar
condition|)
block|{
name|ar
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|ar
argument_list|)
argument_list|)
expr_stmt|;
name|ar
operator|->
name|name
operator|=
name|xmemdupz
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
name|ar
operator|->
name|write_archive
operator|=
name|write_tar_filter_archive
expr_stmt|;
name|ar
operator|->
name|flags
operator|=
name|ARCHIVER_WANT_COMPRESSION_LEVELS
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|tar_filters
argument_list|,
name|nr_tar_filters
operator|+
literal|1
argument_list|,
name|alloc_tar_filters
argument_list|)
expr_stmt|;
name|tar_filters
index|[
name|nr_tar_filters
operator|++
index|]
operator|=
name|ar
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"command"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|free
argument_list|(
name|ar
operator|->
name|data
argument_list|)
expr_stmt|;
name|ar
operator|->
name|data
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"remote"
argument_list|)
condition|)
block|{
if|if
condition|(
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
condition|)
name|ar
operator|->
name|flags
operator||=
name|ARCHIVER_REMOTE
expr_stmt|;
else|else
name|ar
operator|->
name|flags
operator|&=
operator|~
name|ARCHIVER_REMOTE
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|git_tar_config
specifier|static
name|int
name|git_tar_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"tar.umask"
argument_list|)
condition|)
block|{
if|if
condition|(
name|value
operator|&&
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"user"
argument_list|)
condition|)
block|{
name|tar_umask
operator|=
name|umask
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|umask
argument_list|(
name|tar_umask
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|tar_umask
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
return|return
name|tar_filter_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_tar_archive
specifier|static
name|int
name|write_tar_archive
parameter_list|(
specifier|const
name|struct
name|archiver
modifier|*
name|ar
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|)
block|{
name|int
name|err
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|args
operator|->
name|commit_sha1
condition|)
name|err
operator|=
name|write_global_extended_header
argument_list|(
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|err
condition|)
name|err
operator|=
name|write_archive_entries
argument_list|(
name|args
argument_list|,
name|write_tar_entry
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|err
condition|)
name|write_trailer
argument_list|()
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|write_tar_filter_archive
specifier|static
name|int
name|write_tar_filter_archive
parameter_list|(
specifier|const
name|struct
name|archiver
modifier|*
name|ar
parameter_list|,
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|)
block|{
name|struct
name|strbuf
name|cmd
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|child_process
name|filter
init|=
name|CHILD_PROCESS_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[
literal|2
index|]
decl_stmt|;
name|int
name|r
decl_stmt|;
if|if
condition|(
operator|!
name|ar
operator|->
name|data
condition|)
name|die
argument_list|(
literal|"BUG: tar-filter archiver called with no filter defined"
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|cmd
argument_list|,
name|ar
operator|->
name|data
argument_list|)
expr_stmt|;
if|if
condition|(
name|args
operator|->
name|compression_level
operator|>=
literal|0
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|cmd
argument_list|,
literal|" -%d"
argument_list|,
name|args
operator|->
name|compression_level
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|cmd
operator|.
name|buf
expr_stmt|;
name|argv
index|[
literal|1
index|]
operator|=
name|NULL
expr_stmt|;
name|filter
operator|.
name|argv
operator|=
name|argv
expr_stmt|;
name|filter
operator|.
name|use_shell
operator|=
literal|1
expr_stmt|;
name|filter
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|filter
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to start '%s' filter"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|dup2
argument_list|(
name|filter
operator|.
name|in
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to redirect descriptor"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|filter
operator|.
name|in
argument_list|)
expr_stmt|;
name|r
operator|=
name|write_tar_archive
argument_list|(
name|ar
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|close
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|filter
argument_list|)
operator|!=
literal|0
condition|)
name|die
argument_list|(
literal|"'%s' filter reported error"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
return|return
name|r
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|tar_archiver
specifier|static
name|struct
name|archiver
name|tar_archiver
init|=
block|{
literal|"tar"
block|,
name|write_tar_archive
block|,
name|ARCHIVER_REMOTE
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|init_tar_archiver
name|void
name|init_tar_archiver
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|register_archiver
argument_list|(
operator|&
name|tar_archiver
argument_list|)
expr_stmt|;
name|tar_filter_config
argument_list|(
literal|"tar.tgz.command"
argument_list|,
literal|"gzip -cn"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|tar_filter_config
argument_list|(
literal|"tar.tgz.remote"
argument_list|,
literal|"true"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|tar_filter_config
argument_list|(
literal|"tar.tar.gz.command"
argument_list|,
literal|"gzip -cn"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|tar_filter_config
argument_list|(
literal|"tar.tar.gz.remote"
argument_list|,
literal|"true"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_tar_config
argument_list|,
name|NULL
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
name|nr_tar_filters
condition|;
name|i
operator|++
control|)
block|{
comment|/* omit any filters that never had a command configured */
if|if
condition|(
name|tar_filters
index|[
name|i
index|]
operator|->
name|data
condition|)
name|register_archiver
argument_list|(
name|tar_filters
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
block|}
end_function
end_unit
