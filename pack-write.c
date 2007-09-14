begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_include
include|#
directive|include
file|"csum-file.h"
end_include
begin_decl_stmt
DECL|variable|pack_idx_default_version
name|uint32_t
name|pack_idx_default_version
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_idx_off32_limit
name|uint32_t
name|pack_idx_off32_limit
init|=
literal|0x7fffffff
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|sha1_compare
specifier|static
name|int
name|sha1_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|_a
parameter_list|,
specifier|const
name|void
modifier|*
name|_b
parameter_list|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|a
init|=
operator|*
operator|(
expr|struct
name|pack_idx_entry
operator|*
operator|*
operator|)
name|_a
decl_stmt|;
name|struct
name|pack_idx_entry
modifier|*
name|b
init|=
operator|*
operator|(
expr|struct
name|pack_idx_entry
operator|*
operator|*
operator|)
name|_b
decl_stmt|;
return|return
name|hashcmp
argument_list|(
name|a
operator|->
name|sha1
argument_list|,
name|b
operator|->
name|sha1
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * On entry *sha1 contains the pack content SHA1 hash, on exit it is  * the SHA1 hash of sorted object names. The objects array passed in  * will be sorted by SHA1 on exit.  */
end_comment
begin_function
DECL|function|write_idx_file
specifier|const
name|char
modifier|*
name|write_idx_file
parameter_list|(
specifier|const
name|char
modifier|*
name|index_name
parameter_list|,
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|objects
parameter_list|,
name|int
name|nr_objects
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|sha1file
modifier|*
name|f
decl_stmt|;
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|sorted_by_sha
decl_stmt|,
modifier|*
modifier|*
name|list
decl_stmt|,
modifier|*
modifier|*
name|last
decl_stmt|;
name|off_t
name|last_obj_offset
init|=
literal|0
decl_stmt|;
name|uint32_t
name|array
index|[
literal|256
index|]
decl_stmt|;
name|int
name|i
decl_stmt|,
name|fd
decl_stmt|;
name|SHA_CTX
name|ctx
decl_stmt|;
name|uint32_t
name|index_version
decl_stmt|;
if|if
condition|(
name|nr_objects
condition|)
block|{
name|sorted_by_sha
operator|=
name|objects
expr_stmt|;
name|list
operator|=
name|sorted_by_sha
expr_stmt|;
name|last
operator|=
name|sorted_by_sha
operator|+
name|nr_objects
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objects
condition|;
operator|++
name|i
control|)
block|{
if|if
condition|(
name|objects
index|[
name|i
index|]
operator|->
name|offset
operator|>
name|last_obj_offset
condition|)
name|last_obj_offset
operator|=
name|objects
index|[
name|i
index|]
operator|->
name|offset
expr_stmt|;
block|}
name|qsort
argument_list|(
name|sorted_by_sha
argument_list|,
name|nr_objects
argument_list|,
sizeof|sizeof
argument_list|(
name|sorted_by_sha
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|sha1_compare
argument_list|)
expr_stmt|;
block|}
else|else
name|sorted_by_sha
operator|=
name|list
operator|=
name|last
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|index_name
condition|)
block|{
specifier|static
name|char
name|tmpfile
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|snprintf
argument_list|(
name|tmpfile
argument_list|,
sizeof|sizeof
argument_list|(
name|tmpfile
argument_list|)
argument_list|,
literal|"%s/tmp_idx_XXXXXX"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|fd
operator|=
name|xmkstemp
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
name|index_name
operator|=
name|xstrdup
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|unlink
argument_list|(
name|index_name
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|index_name
argument_list|,
name|O_CREAT
operator||
name|O_EXCL
operator||
name|O_WRONLY
argument_list|,
literal|0600
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create %s: %s"
argument_list|,
name|index_name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|f
operator|=
name|sha1fd
argument_list|(
name|fd
argument_list|,
name|index_name
argument_list|)
expr_stmt|;
comment|/* if last object's offset is>= 2^31 we should use index V2 */
name|index_version
operator|=
operator|(
name|last_obj_offset
operator|>>
literal|31
operator|)
condition|?
literal|2
else|:
name|pack_idx_default_version
expr_stmt|;
comment|/* index versions 2 and above need a header */
if|if
condition|(
name|index_version
operator|>=
literal|2
condition|)
block|{
name|struct
name|pack_idx_header
name|hdr
decl_stmt|;
name|hdr
operator|.
name|idx_signature
operator|=
name|htonl
argument_list|(
name|PACK_IDX_SIGNATURE
argument_list|)
expr_stmt|;
name|hdr
operator|.
name|idx_version
operator|=
name|htonl
argument_list|(
name|index_version
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * Write the first-level table (the list is sorted, 	 * but we use a 256-entry lookup to be able to avoid 	 * having to do eight extra binary search iterations). 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|256
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|next
init|=
name|list
decl_stmt|;
while|while
condition|(
name|next
operator|<
name|last
condition|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|obj
init|=
operator|*
name|next
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|sha1
index|[
literal|0
index|]
operator|!=
name|i
condition|)
break|break;
name|next
operator|++
expr_stmt|;
block|}
name|array
index|[
name|i
index|]
operator|=
name|htonl
argument_list|(
name|next
operator|-
name|sorted_by_sha
argument_list|)
expr_stmt|;
name|list
operator|=
name|next
expr_stmt|;
block|}
name|sha1write
argument_list|(
name|f
argument_list|,
name|array
argument_list|,
literal|256
operator|*
literal|4
argument_list|)
expr_stmt|;
comment|/* compute the SHA1 hash of sorted object names. */
name|SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
comment|/* 	 * Write the actual SHA1 entries.. 	 */
name|list
operator|=
name|sorted_by_sha
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objects
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|obj
init|=
operator|*
name|list
operator|++
decl_stmt|;
if|if
condition|(
name|index_version
operator|<
literal|2
condition|)
block|{
name|uint32_t
name|offset
init|=
name|htonl
argument_list|(
name|obj
operator|->
name|offset
argument_list|)
decl_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|offset
argument_list|,
literal|4
argument_list|)
expr_stmt|;
block|}
name|sha1write
argument_list|(
name|f
argument_list|,
name|obj
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|obj
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|index_version
operator|>=
literal|2
condition|)
block|{
name|unsigned
name|int
name|nr_large_offset
init|=
literal|0
decl_stmt|;
comment|/* write the crc32 table */
name|list
operator|=
name|sorted_by_sha
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objects
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|obj
init|=
operator|*
name|list
operator|++
decl_stmt|;
name|uint32_t
name|crc32_val
init|=
name|htonl
argument_list|(
name|obj
operator|->
name|crc32
argument_list|)
decl_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|crc32_val
argument_list|,
literal|4
argument_list|)
expr_stmt|;
block|}
comment|/* write the 32-bit offset table */
name|list
operator|=
name|sorted_by_sha
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_objects
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|obj
init|=
operator|*
name|list
operator|++
decl_stmt|;
name|uint32_t
name|offset
init|=
operator|(
name|obj
operator|->
name|offset
operator|<=
name|pack_idx_off32_limit
operator|)
condition|?
name|obj
operator|->
name|offset
else|:
operator|(
literal|0x80000000
operator||
name|nr_large_offset
operator|++
operator|)
decl_stmt|;
name|offset
operator|=
name|htonl
argument_list|(
name|offset
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|offset
argument_list|,
literal|4
argument_list|)
expr_stmt|;
block|}
comment|/* write the large offset table */
name|list
operator|=
name|sorted_by_sha
expr_stmt|;
while|while
condition|(
name|nr_large_offset
condition|)
block|{
name|struct
name|pack_idx_entry
modifier|*
name|obj
init|=
operator|*
name|list
operator|++
decl_stmt|;
name|uint64_t
name|offset
init|=
name|obj
operator|->
name|offset
decl_stmt|;
if|if
condition|(
name|offset
operator|>
name|pack_idx_off32_limit
condition|)
block|{
name|uint32_t
name|split
index|[
literal|2
index|]
decl_stmt|;
name|split
index|[
literal|0
index|]
operator|=
name|htonl
argument_list|(
name|offset
operator|>>
literal|32
argument_list|)
expr_stmt|;
name|split
index|[
literal|1
index|]
operator|=
name|htonl
argument_list|(
name|offset
operator|&
literal|0xffffffff
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
name|split
argument_list|,
literal|8
argument_list|)
expr_stmt|;
name|nr_large_offset
operator|--
expr_stmt|;
block|}
block|}
block|}
name|sha1write
argument_list|(
name|f
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|sha1close
argument_list|(
name|f
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
return|return
name|index_name
return|;
block|}
end_function
begin_function
DECL|function|fixup_pack_header_footer
name|void
name|fixup_pack_header_footer
parameter_list|(
name|int
name|pack_fd
parameter_list|,
name|unsigned
name|char
modifier|*
name|pack_file_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|pack_name
parameter_list|,
name|uint32_t
name|object_count
parameter_list|)
block|{
specifier|static
specifier|const
name|int
name|buf_sz
init|=
literal|128
operator|*
literal|1024
decl_stmt|;
name|SHA_CTX
name|c
decl_stmt|;
name|struct
name|pack_header
name|hdr
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
if|if
condition|(
name|lseek
argument_list|(
name|pack_fd
argument_list|,
literal|0
argument_list|,
name|SEEK_SET
argument_list|)
operator|!=
literal|0
condition|)
name|die
argument_list|(
literal|"Failed seeking to start: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_in_full
argument_list|(
name|pack_fd
argument_list|,
operator|&
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
condition|)
name|die
argument_list|(
literal|"Unable to reread header of %s: %s"
argument_list|,
name|pack_name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|lseek
argument_list|(
name|pack_fd
argument_list|,
literal|0
argument_list|,
name|SEEK_SET
argument_list|)
operator|!=
literal|0
condition|)
name|die
argument_list|(
literal|"Failed seeking to start: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|hdr
operator|.
name|hdr_entries
operator|=
name|htonl
argument_list|(
name|object_count
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|pack_fd
argument_list|,
operator|&
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|)
expr_stmt|;
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
operator|&
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|)
expr_stmt|;
name|buf
operator|=
name|xmalloc
argument_list|(
name|buf_sz
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|ssize_t
name|n
init|=
name|xread
argument_list|(
name|pack_fd
argument_list|,
name|buf
argument_list|,
name|buf_sz
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|n
condition|)
break|break;
if|if
condition|(
name|n
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Failed to checksum %s: %s"
argument_list|,
name|pack_name
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|c
argument_list|,
name|buf
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|pack_file_sha1
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|pack_fd
argument_list|,
name|pack_file_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|index_pack_lockfile
name|char
modifier|*
name|index_pack_lockfile
parameter_list|(
name|int
name|ip_out
parameter_list|)
block|{
name|int
name|len
decl_stmt|,
name|s
decl_stmt|;
name|char
name|packname
index|[
literal|46
index|]
decl_stmt|;
comment|/* 	 * The first thing we expects from index-pack's output 	 * is "pack\t%40s\n" or "keep\t%40s\n" (46 bytes) where 	 * %40s is the newly created pack SHA1 name.  In the "keep" 	 * case, we need it to remove the corresponding .keep file 	 * later on.  If we don't get that then tough luck with it. 	 */
for|for
control|(
name|len
operator|=
literal|0
init|;
name|len
operator|<
literal|46
operator|&&
operator|(
name|s
operator|=
name|xread
argument_list|(
name|ip_out
argument_list|,
name|packname
operator|+
name|len
argument_list|,
literal|46
operator|-
name|len
argument_list|)
operator|)
operator|>
literal|0
condition|;
name|len
operator|+=
name|s
control|)
empty_stmt|;
if|if
condition|(
name|len
operator|==
literal|46
operator|&&
name|packname
index|[
literal|45
index|]
operator|==
literal|'\n'
operator|&&
name|memcmp
argument_list|(
name|packname
argument_list|,
literal|"keep\t"
argument_list|,
literal|5
argument_list|)
operator|==
literal|0
condition|)
block|{
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|packname
index|[
literal|45
index|]
operator|=
literal|0
expr_stmt|;
name|snprintf
argument_list|(
name|path
argument_list|,
sizeof|sizeof
argument_list|(
name|path
argument_list|)
argument_list|,
literal|"%s/pack/pack-%s.keep"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|,
name|packname
operator|+
literal|5
argument_list|)
expr_stmt|;
return|return
name|xstrdup
argument_list|(
name|path
argument_list|)
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
end_unit
