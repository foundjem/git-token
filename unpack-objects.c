begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_decl_stmt
DECL|variable|dry_run
specifier|static
name|int
name|dry_run
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|nr_entries
specifier|static
name|int
name|nr_entries
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|base_name
specifier|static
specifier|const
name|char
modifier|*
name|base_name
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|unpack_usage
specifier|static
specifier|const
name|char
name|unpack_usage
index|[]
init|=
literal|"git-unpack-objects basename"
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|pack_entry
struct|struct
name|pack_entry
block|{
DECL|member|offset
name|unsigned
name|int
name|offset
decl_stmt|;
comment|/* network byte order */
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|pack_base
specifier|static
name|void
modifier|*
name|pack_base
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_size
specifier|static
name|unsigned
name|long
name|pack_size
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|index_base
specifier|static
name|void
modifier|*
name|index_base
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|index_size
specifier|static
name|unsigned
name|long
name|index_size
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_list
specifier|static
name|struct
name|pack_entry
modifier|*
modifier|*
name|pack_list
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|map_file
specifier|static
name|void
modifier|*
name|map_file
parameter_list|(
specifier|const
name|char
modifier|*
name|suffix
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|)
block|{
specifier|static
name|char
name|pathname
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|unsigned
name|long
name|len
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|void
modifier|*
name|map
decl_stmt|;
name|len
operator|=
name|snprintf
argument_list|(
name|pathname
argument_list|,
name|PATH_MAX
argument_list|,
literal|"%s.%s"
argument_list|,
name|base_name
argument_list|,
name|suffix
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|PATH_MAX
condition|)
name|die
argument_list|(
literal|"bad pack base-name"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|pathname
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
operator|||
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to open '%s'"
argument_list|,
name|pathname
argument_list|)
expr_stmt|;
name|len
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
name|die
argument_list|(
literal|"bad pack file '%s'"
argument_list|,
name|pathname
argument_list|)
expr_stmt|;
name|map
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|len
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
name|die
argument_list|(
literal|"unable to mmap '%s'"
argument_list|,
name|pathname
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
operator|*
name|sizep
operator|=
name|len
expr_stmt|;
return|return
name|map
return|;
block|}
end_function
begin_function
DECL|function|sort_by_offset
specifier|static
name|int
name|sort_by_offset
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
name|pack_entry
modifier|*
name|a
init|=
operator|*
operator|(
expr|struct
name|pack_entry
operator|*
operator|*
operator|)
name|_a
decl_stmt|;
name|struct
name|pack_entry
modifier|*
name|b
init|=
operator|*
operator|(
expr|struct
name|pack_entry
operator|*
operator|*
operator|)
name|_b
decl_stmt|;
name|unsigned
name|int
name|o1
decl_stmt|,
name|o2
decl_stmt|;
name|o1
operator|=
name|ntohl
argument_list|(
name|a
operator|->
name|offset
argument_list|)
expr_stmt|;
name|o2
operator|=
name|ntohl
argument_list|(
name|b
operator|->
name|offset
argument_list|)
expr_stmt|;
return|return
name|o1
operator|<
name|o2
condition|?
operator|-
literal|1
else|:
literal|1
return|;
block|}
end_function
begin_function
DECL|function|check_index
specifier|static
name|int
name|check_index
parameter_list|(
name|void
parameter_list|)
block|{
name|unsigned
name|int
modifier|*
name|array
init|=
name|index_base
decl_stmt|;
name|unsigned
name|int
name|nr
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|index_size
operator|<
literal|4
operator|*
literal|256
operator|+
literal|20
condition|)
return|return
name|error
argument_list|(
literal|"index file too small"
argument_list|)
return|;
name|nr
operator|=
literal|0
expr_stmt|;
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
name|unsigned
name|int
name|n
init|=
name|ntohl
argument_list|(
name|array
index|[
name|i
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
name|n
operator|<
name|nr
condition|)
return|return
name|error
argument_list|(
literal|"non-monotonic index"
argument_list|)
return|;
name|nr
operator|=
name|n
expr_stmt|;
block|}
comment|/* 	 * Total size: 	 *  - 256 index entries 4 bytes each 	 *  - 24-byte entries * nr (20-byte sha1 + 4-byte offset) 	 *  - 20-byte SHA1 of the packfile 	 *  - 20-byte SHA1 file checksum 	 */
if|if
condition|(
name|index_size
operator|!=
literal|4
operator|*
literal|256
operator|+
name|nr
operator|*
literal|24
operator|+
literal|20
operator|+
literal|20
condition|)
return|return
name|error
argument_list|(
literal|"wrong index file size"
argument_list|)
return|;
name|nr_entries
operator|=
name|nr
expr_stmt|;
name|pack_list
operator|=
name|xmalloc
argument_list|(
name|nr
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|pack_entry
operator|*
argument_list|)
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
name|nr
condition|;
name|i
operator|++
control|)
name|pack_list
index|[
name|i
index|]
operator|=
name|index_base
operator|+
literal|4
operator|*
literal|256
operator|+
name|i
operator|*
literal|24
expr_stmt|;
name|qsort
argument_list|(
name|pack_list
argument_list|,
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|pack_list
argument_list|)
argument_list|,
name|sort_by_offset
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%d entries\n"
argument_list|,
name|nr
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|unpack_non_delta_entry
specifier|static
name|int
name|unpack_non_delta_entry
parameter_list|(
name|struct
name|pack_entry
modifier|*
name|entry
parameter_list|,
name|int
name|kind
parameter_list|,
name|unsigned
name|char
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|unsigned
name|long
name|left
parameter_list|)
block|{
name|int
name|st
decl_stmt|;
name|z_stream
name|stream
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|type_s
decl_stmt|;
name|printf
argument_list|(
literal|"%s %c %lu\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|->
name|sha1
argument_list|)
argument_list|,
name|kind
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
name|dry_run
condition|)
return|return
literal|0
return|;
name|buffer
operator|=
name|xmalloc
argument_list|(
name|size
operator|+
literal|1
argument_list|)
expr_stmt|;
name|buffer
index|[
name|size
index|]
operator|=
literal|0
expr_stmt|;
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
name|data
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|left
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
name|size
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|st
operator|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_FINISH
argument_list|)
expr_stmt|;
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|st
operator|!=
name|Z_STREAM_END
operator|)
operator|||
name|stream
operator|.
name|total_out
operator|!=
name|size
condition|)
goto|goto
name|err_finish
goto|;
switch|switch
condition|(
name|kind
condition|)
block|{
case|case
literal|'C'
case|:
name|type_s
operator|=
literal|"commit"
expr_stmt|;
break|break;
case|case
literal|'T'
case|:
name|type_s
operator|=
literal|"tree"
expr_stmt|;
break|break;
case|case
literal|'B'
case|:
name|type_s
operator|=
literal|"blob"
expr_stmt|;
break|break;
default|default:
goto|goto
name|err_finish
goto|;
block|}
if|if
condition|(
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
name|type_s
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to write %s (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|->
name|sha1
argument_list|)
argument_list|,
name|type_s
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|type_s
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|entry
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
name|die
argument_list|(
literal|"resulting %s have wrong SHA1"
argument_list|,
name|type_s
argument_list|)
expr_stmt|;
name|finish
label|:
name|st
operator|=
literal|0
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|st
return|;
name|err_finish
label|:
name|st
operator|=
operator|-
literal|1
expr_stmt|;
goto|goto
name|finish
goto|;
block|}
end_function
begin_function
DECL|function|find_pack_entry
specifier|static
name|int
name|find_pack_entry
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|pack_entry
modifier|*
modifier|*
name|ent
parameter_list|)
block|{
name|int
modifier|*
name|level1_ofs
init|=
name|index_base
decl_stmt|;
name|int
name|hi
init|=
name|ntohl
argument_list|(
name|level1_ofs
index|[
operator|*
name|sha1
index|]
argument_list|)
decl_stmt|;
name|int
name|lo
init|=
operator|(
operator|(
operator|*
name|sha1
operator|==
literal|0x0
operator|)
condition|?
literal|0
else|:
name|ntohl
argument_list|(
name|level1_ofs
index|[
operator|*
name|sha1
operator|-
literal|1
index|]
argument_list|)
operator|)
decl_stmt|;
name|void
modifier|*
name|index
init|=
name|index_base
operator|+
literal|4
operator|*
literal|256
decl_stmt|;
do|do
block|{
name|int
name|mi
init|=
operator|(
name|lo
operator|+
name|hi
operator|)
operator|/
literal|2
decl_stmt|;
name|int
name|cmp
init|=
name|memcmp
argument_list|(
name|index
operator|+
literal|24
operator|*
name|mi
operator|+
literal|4
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
block|{
operator|*
name|ent
operator|=
name|index
operator|+
literal|24
operator|*
name|mi
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
name|hi
operator|=
name|mi
expr_stmt|;
else|else
name|lo
operator|=
name|mi
operator|+
literal|1
expr_stmt|;
block|}
do|while
condition|(
name|lo
operator|<
name|hi
condition|)
do|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* forward declaration for a mutually recursive function */
end_comment
begin_function_decl
specifier|static
name|void
name|unpack_entry
parameter_list|(
name|struct
name|pack_entry
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|unpack_delta_entry
specifier|static
name|int
name|unpack_delta_entry
parameter_list|(
name|struct
name|pack_entry
modifier|*
name|entry
parameter_list|,
name|unsigned
name|char
modifier|*
name|base_sha1
parameter_list|,
name|unsigned
name|long
name|delta_size
parameter_list|,
name|unsigned
name|long
name|left
parameter_list|)
block|{
name|void
modifier|*
name|data
decl_stmt|,
modifier|*
name|delta_data
decl_stmt|,
modifier|*
name|result
decl_stmt|,
modifier|*
name|base
decl_stmt|;
name|unsigned
name|long
name|data_size
decl_stmt|,
name|result_size
decl_stmt|,
name|base_size
decl_stmt|;
name|z_stream
name|stream
decl_stmt|;
name|int
name|st
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
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
name|left
operator|<
literal|20
condition|)
name|die
argument_list|(
literal|"truncated pack file"
argument_list|)
expr_stmt|;
name|data
operator|=
name|base_sha1
operator|+
literal|20
expr_stmt|;
name|data_size
operator|=
name|left
operator|-
literal|20
expr_stmt|;
name|printf
argument_list|(
literal|"%s D %lu"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|->
name|sha1
argument_list|)
argument_list|,
name|delta_size
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|" %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|dry_run
condition|)
return|return
literal|0
return|;
comment|/* pack+5 is the base sha1, unless we have it, we need to 	 * unpack it first. 	 */
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|base_sha1
argument_list|)
condition|)
block|{
name|struct
name|pack_entry
modifier|*
name|base
decl_stmt|;
if|if
condition|(
operator|!
name|find_pack_entry
argument_list|(
name|base_sha1
argument_list|,
operator|&
name|base
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot find delta-pack base object"
argument_list|)
expr_stmt|;
name|unpack_entry
argument_list|(
name|base
argument_list|)
expr_stmt|;
block|}
name|delta_data
operator|=
name|xmalloc
argument_list|(
name|delta_size
argument_list|)
expr_stmt|;
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
name|data
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|data_size
expr_stmt|;
name|stream
operator|.
name|next_out
operator|=
name|delta_data
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
name|delta_size
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
name|st
operator|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
name|Z_FINISH
argument_list|)
expr_stmt|;
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|st
operator|!=
name|Z_STREAM_END
operator|)
operator|||
name|stream
operator|.
name|total_out
operator|!=
name|delta_size
condition|)
name|die
argument_list|(
literal|"delta data unpack failed"
argument_list|)
expr_stmt|;
name|base
operator|=
name|read_sha1_file
argument_list|(
name|base_sha1
argument_list|,
name|type
argument_list|,
operator|&
name|base_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
name|die
argument_list|(
literal|"failed to read delta-pack base object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|result
operator|=
name|patch_delta
argument_list|(
name|base
argument_list|,
name|base_size
argument_list|,
name|delta_data
argument_list|,
name|delta_size
argument_list|,
operator|&
name|result_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|result
condition|)
name|die
argument_list|(
literal|"failed to apply delta"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|delta_data
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_sha1_file
argument_list|(
name|result
argument_list|,
name|result_size
argument_list|,
name|type
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to write %s (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|->
name|sha1
argument_list|)
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|result
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|type
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|entry
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
name|die
argument_list|(
literal|"resulting %s have wrong SHA1"
argument_list|,
name|type
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|unpack_entry
specifier|static
name|void
name|unpack_entry
parameter_list|(
name|struct
name|pack_entry
modifier|*
name|entry
parameter_list|)
block|{
name|unsigned
name|long
name|offset
decl_stmt|,
name|size
decl_stmt|,
name|left
decl_stmt|;
name|unsigned
name|char
modifier|*
name|pack
decl_stmt|;
comment|/* Have we done this one already due to deltas based on it? */
if|if
condition|(
name|lookup_object
argument_list|(
name|entry
operator|->
name|sha1
argument_list|)
condition|)
return|return;
name|offset
operator|=
name|ntohl
argument_list|(
name|entry
operator|->
name|offset
argument_list|)
expr_stmt|;
if|if
condition|(
name|offset
operator|>
name|pack_size
operator|-
literal|5
condition|)
name|die
argument_list|(
literal|"object offset outside of pack file"
argument_list|)
expr_stmt|;
name|pack
operator|=
name|pack_base
operator|+
name|offset
expr_stmt|;
name|size
operator|=
operator|(
name|pack
index|[
literal|1
index|]
operator|<<
literal|24
operator|)
operator|+
operator|(
name|pack
index|[
literal|2
index|]
operator|<<
literal|16
operator|)
operator|+
operator|(
name|pack
index|[
literal|3
index|]
operator|<<
literal|8
operator|)
operator|+
name|pack
index|[
literal|4
index|]
expr_stmt|;
name|left
operator|=
name|pack_size
operator|-
name|offset
operator|-
literal|5
expr_stmt|;
switch|switch
condition|(
operator|*
name|pack
condition|)
block|{
case|case
literal|'C'
case|:
case|case
literal|'T'
case|:
case|case
literal|'B'
case|:
name|unpack_non_delta_entry
argument_list|(
name|entry
argument_list|,
operator|*
name|pack
argument_list|,
name|pack
operator|+
literal|5
argument_list|,
name|size
argument_list|,
name|left
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'D'
case|:
name|unpack_delta_entry
argument_list|(
name|entry
argument_list|,
name|pack
operator|+
literal|5
argument_list|,
name|size
argument_list|,
name|left
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"corrupted pack file"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * We unpack from the end, older files first. Now, usually  * there are deltas etc, so we'll not actually write the  * objects in that order, but we might as well try..  */
end_comment
begin_function
DECL|function|unpack_all
specifier|static
name|void
name|unpack_all
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
init|=
name|nr_entries
decl_stmt|;
while|while
condition|(
operator|--
name|i
operator|>=
literal|0
condition|)
block|{
name|struct
name|pack_entry
modifier|*
name|entry
init|=
name|pack_list
index|[
name|i
index|]
decl_stmt|;
name|unpack_entry
argument_list|(
name|entry
argument_list|)
expr_stmt|;
block|}
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
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-n"
argument_list|)
condition|)
block|{
name|dry_run
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|unpack_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|base_name
condition|)
name|usage
argument_list|(
name|unpack_usage
argument_list|)
expr_stmt|;
name|base_name
operator|=
name|arg
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|base_name
condition|)
name|usage
argument_list|(
name|unpack_usage
argument_list|)
expr_stmt|;
name|index_base
operator|=
name|map_file
argument_list|(
literal|"idx"
argument_list|,
operator|&
name|index_size
argument_list|)
expr_stmt|;
name|pack_base
operator|=
name|map_file
argument_list|(
literal|"pack"
argument_list|,
operator|&
name|pack_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|check_index
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"bad index file"
argument_list|)
expr_stmt|;
name|unpack_all
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
