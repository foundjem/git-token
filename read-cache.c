begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|sha1_file_directory
specifier|const
name|char
modifier|*
name|sha1_file_directory
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|active_cache
name|struct
name|cache_entry
modifier|*
modifier|*
name|active_cache
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|active_nr
DECL|variable|active_alloc
name|unsigned
name|int
name|active_nr
init|=
literal|0
decl_stmt|,
name|active_alloc
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|usage
name|void
name|usage
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"read-tree: %s\n"
argument_list|,
name|err
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hexval
specifier|static
name|unsigned
name|hexval
parameter_list|(
name|char
name|c
parameter_list|)
block|{
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
return|return
name|c
operator|-
literal|'0'
return|;
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
return|return
name|c
operator|-
literal|'a'
operator|+
literal|10
return|;
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
return|return
name|c
operator|-
literal|'A'
operator|+
literal|10
return|;
return|return
operator|~
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_sha1_hex
name|int
name|get_sha1_hex
parameter_list|(
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
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
literal|20
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
init|=
operator|(
name|hexval
argument_list|(
name|hex
index|[
literal|0
index|]
argument_list|)
operator|<<
literal|4
operator|)
operator||
name|hexval
argument_list|(
name|hex
index|[
literal|1
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
name|val
operator|&
operator|~
literal|0xff
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|sha1
operator|++
operator|=
name|val
expr_stmt|;
name|hex
operator|+=
literal|2
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|sha1_to_hex
name|char
modifier|*
name|sha1_to_hex
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|50
index|]
decl_stmt|;
specifier|static
specifier|const
name|char
name|hex
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|buffer
decl_stmt|;
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
literal|20
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
init|=
operator|*
name|sha1
operator|++
decl_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|>>
literal|4
index|]
expr_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|&
literal|0xf
index|]
expr_stmt|;
block|}
return|return
name|buffer
return|;
block|}
end_function
begin_comment
comment|/*  * NOTE! This returns a statically allocated buffer, so you have to be  * careful about using it. Do a "strdup()" if you need to save the  * filename.  */
end_comment
begin_function
DECL|function|sha1_file_name
name|char
modifier|*
name|sha1_file_name
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
specifier|static
name|char
modifier|*
name|name
decl_stmt|,
modifier|*
name|base
decl_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
block|{
name|char
modifier|*
name|sha1_file_directory
init|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
condition|?
else|:
name|DEFAULT_DB_ENVIRONMENT
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|sha1_file_directory
argument_list|)
decl_stmt|;
name|base
operator|=
name|malloc
argument_list|(
name|len
operator|+
literal|60
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|base
argument_list|,
name|sha1_file_directory
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|base
operator|+
name|len
argument_list|,
literal|0
argument_list|,
literal|60
argument_list|)
expr_stmt|;
name|base
index|[
name|len
index|]
operator|=
literal|'/'
expr_stmt|;
name|base
index|[
name|len
operator|+
literal|3
index|]
operator|=
literal|'/'
expr_stmt|;
name|name
operator|=
name|base
operator|+
name|len
operator|+
literal|1
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|20
condition|;
name|i
operator|++
control|)
block|{
specifier|static
name|char
name|hex
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|unsigned
name|int
name|val
init|=
name|sha1
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|pos
init|=
name|name
operator|+
name|i
operator|*
literal|2
operator|+
operator|(
name|i
operator|>
literal|0
operator|)
decl_stmt|;
operator|*
name|pos
operator|++
operator|=
name|hex
index|[
name|val
operator|>>
literal|4
index|]
expr_stmt|;
operator|*
name|pos
operator|=
name|hex
index|[
name|val
operator|&
literal|0xf
index|]
expr_stmt|;
block|}
return|return
name|base
return|;
block|}
end_function
begin_function
DECL|function|read_sha1_file
name|void
modifier|*
name|read_sha1_file
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|z_stream
name|stream
decl_stmt|;
name|char
name|buffer
index|[
literal|8192
index|]
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|i
decl_stmt|,
name|fd
decl_stmt|,
name|ret
decl_stmt|,
name|bytes
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|,
modifier|*
name|buf
decl_stmt|;
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
name|filename
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|-
literal|1
operator|==
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|map
condition|)
return|return
name|NULL
return|;
comment|/* Get the data stream */
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|stream
operator|.
name|next_in
operator|=
name|map
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|stream
operator|.
name|next_out
operator|=
name|buffer
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|ret
operator|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|sscanf
argument_list|(
name|buffer
argument_list|,
literal|"%10s %lu"
argument_list|,
name|type
argument_list|,
name|size
argument_list|)
operator|!=
literal|2
condition|)
return|return
name|NULL
return|;
name|bytes
operator|=
name|strlen
argument_list|(
name|buffer
argument_list|)
operator|+
literal|1
expr_stmt|;
name|buf
operator|=
name|malloc
argument_list|(
operator|*
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
return|return
name|NULL
return|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|buffer
operator|+
name|bytes
argument_list|,
name|stream
operator|.
name|total_out
operator|-
name|bytes
argument_list|)
expr_stmt|;
name|bytes
operator|=
name|stream
operator|.
name|total_out
operator|-
name|bytes
expr_stmt|;
if|if
condition|(
name|bytes
operator|<
operator|*
name|size
operator|&&
name|ret
operator|==
name|Z_OK
condition|)
block|{
name|stream
operator|.
name|next_out
operator|=
name|buf
operator|+
name|bytes
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
operator|*
name|size
operator|-
name|bytes
expr_stmt|;
while|while
condition|(
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_FINISH
argument_list|)
operator|==
name|Z_OK
condition|)
comment|/* nothing */
empty_stmt|;
block|}
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|write_sha1_file
name|int
name|write_sha1_file
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|len
parameter_list|)
block|{
name|int
name|size
decl_stmt|;
name|char
modifier|*
name|compressed
decl_stmt|;
name|z_stream
name|stream
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|SHA_CTX
name|c
decl_stmt|;
comment|/* Set it up */
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|deflateInit
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_BEST_COMPRESSION
argument_list|)
expr_stmt|;
name|size
operator|=
name|deflateBound
argument_list|(
operator|&
name|stream
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|compressed
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
comment|/* Compress it */
name|stream
operator|.
name|next_in
operator|=
name|buf
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|len
expr_stmt|;
name|stream
operator|.
name|next_out
operator|=
name|compressed
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
name|size
expr_stmt|;
while|while
condition|(
name|deflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_FINISH
argument_list|)
operator|==
name|Z_OK
condition|)
comment|/* nothing */
empty_stmt|;
name|deflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|size
operator|=
name|stream
operator|.
name|total_out
expr_stmt|;
comment|/* Sha1.. */
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|compressed
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_sha1_buffer
argument_list|(
name|sha1
argument_list|,
name|compressed
argument_list|,
name|size
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_sha1_buffer
name|int
name|write_sha1_buffer
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|char
modifier|*
name|filename
init|=
name|sha1_file_name
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|int
name|i
decl_stmt|,
name|fd
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|filename
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|(
name|errno
operator|==
name|EEXIST
operator|)
condition|?
literal|0
else|:
operator|-
literal|1
return|;
name|write
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|error
specifier|static
name|int
name|error
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"error: %s\n"
argument_list|,
name|string
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|verify_hdr
specifier|static
name|int
name|verify_hdr
parameter_list|(
name|struct
name|cache_header
modifier|*
name|hdr
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|SHA_CTX
name|c
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|hdr
operator|->
name|signature
operator|!=
name|CACHE_SIGNATURE
condition|)
return|return
name|error
argument_list|(
literal|"bad signature"
argument_list|)
return|;
if|if
condition|(
name|hdr
operator|->
name|version
operator|!=
literal|1
condition|)
return|return
name|error
argument_list|(
literal|"bad version"
argument_list|)
return|;
name|SHA1_Init
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|hdr
argument_list|,
name|offsetof
argument_list|(
expr|struct
name|cache_header
argument_list|,
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|hdr
operator|+
literal|1
argument_list|,
name|size
operator|-
sizeof|sizeof
argument_list|(
operator|*
name|hdr
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|hdr
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"bad header sha1"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_cache
name|int
name|read_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|i
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
name|struct
name|cache_header
modifier|*
name|hdr
decl_stmt|;
name|errno
operator|=
name|EBUSY
expr_stmt|;
if|if
condition|(
name|active_cache
condition|)
return|return
name|error
argument_list|(
literal|"more than one cachefile"
argument_list|)
return|;
name|errno
operator|=
name|ENOENT
expr_stmt|;
name|sha1_file_directory
operator|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sha1_file_directory
condition|)
name|sha1_file_directory
operator|=
name|DEFAULT_DB_ENVIRONMENT
expr_stmt|;
if|if
condition|(
name|access
argument_list|(
name|sha1_file_directory
argument_list|,
name|X_OK
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"no access to SHA1 file directory"
argument_list|)
return|;
name|fd
operator|=
name|open
argument_list|(
literal|".dircache/index"
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|(
name|errno
operator|==
name|ENOENT
operator|)
condition|?
literal|0
else|:
name|error
argument_list|(
literal|"open failed"
argument_list|)
return|;
name|map
operator|=
operator|(
name|void
operator|*
operator|)
operator|-
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|map
operator|=
name|NULL
expr_stmt|;
name|size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
if|if
condition|(
name|size
operator|>
sizeof|sizeof
argument_list|(
expr|struct
name|cache_header
argument_list|)
condition|)
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|-
literal|1
operator|==
operator|(
name|int
operator|)
operator|(
name|long
operator|)
name|map
condition|)
return|return
name|error
argument_list|(
literal|"mmap failed"
argument_list|)
return|;
name|hdr
operator|=
name|map
expr_stmt|;
if|if
condition|(
name|verify_hdr
argument_list|(
name|hdr
argument_list|,
name|size
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|unmap
goto|;
name|active_nr
operator|=
name|hdr
operator|->
name|entries
expr_stmt|;
name|active_alloc
operator|=
name|alloc_nr
argument_list|(
name|active_nr
argument_list|)
expr_stmt|;
name|active_cache
operator|=
name|calloc
argument_list|(
name|active_alloc
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|cache_entry
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|offset
operator|=
sizeof|sizeof
argument_list|(
operator|*
name|hdr
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
name|hdr
operator|->
name|entries
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|map
operator|+
name|offset
decl_stmt|;
name|offset
operator|=
name|offset
operator|+
name|ce_size
argument_list|(
name|ce
argument_list|)
expr_stmt|;
name|active_cache
index|[
name|i
index|]
operator|=
name|ce
expr_stmt|;
block|}
return|return
name|active_nr
return|;
name|unmap
label|:
name|munmap
argument_list|(
name|map
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|errno
operator|=
name|EINVAL
expr_stmt|;
return|return
name|error
argument_list|(
literal|"verify header failed"
argument_list|)
return|;
block|}
end_function
end_unit
