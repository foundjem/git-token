begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
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
file|"pack-revindex.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_define
DECL|macro|MAX_CHAIN
define|#
directive|define
name|MAX_CHAIN
value|50
end_define
begin_define
DECL|macro|VERIFY_PACK_VERBOSE
define|#
directive|define
name|VERIFY_PACK_VERBOSE
value|01
end_define
begin_define
DECL|macro|VERIFY_PACK_STAT_ONLY
define|#
directive|define
name|VERIFY_PACK_STAT_ONLY
value|02
end_define
begin_function
DECL|function|show_pack_info
specifier|static
name|void
name|show_pack_info
parameter_list|(
name|struct
name|packed_git
modifier|*
name|p
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|)
block|{
name|uint32_t
name|nr_objects
decl_stmt|,
name|i
decl_stmt|;
name|int
name|cnt
decl_stmt|;
name|int
name|stat_only
init|=
name|flags
operator|&
name|VERIFY_PACK_STAT_ONLY
decl_stmt|;
name|unsigned
name|long
name|chain_histogram
index|[
name|MAX_CHAIN
operator|+
literal|1
index|]
decl_stmt|,
name|baseobjects
decl_stmt|;
name|nr_objects
operator|=
name|p
operator|->
name|num_objects
expr_stmt|;
name|memset
argument_list|(
name|chain_histogram
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|chain_histogram
argument_list|)
argument_list|)
expr_stmt|;
name|baseobjects
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
name|nr_objects
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|char
name|base_sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|type
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|unsigned
name|long
name|store_size
decl_stmt|;
name|off_t
name|offset
decl_stmt|;
name|unsigned
name|int
name|delta_chain_length
decl_stmt|;
name|sha1
operator|=
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sha1
condition|)
name|die
argument_list|(
literal|"internal error pack-check nth-packed-object"
argument_list|)
expr_stmt|;
name|offset
operator|=
name|nth_packed_object_offset
argument_list|(
name|p
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|type
operator|=
name|typename
argument_list|(
name|packed_object_info_detail
argument_list|(
name|p
argument_list|,
name|offset
argument_list|,
operator|&
name|size
argument_list|,
operator|&
name|store_size
argument_list|,
operator|&
name|delta_chain_length
argument_list|,
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat_only
condition|)
name|printf
argument_list|(
literal|"%s "
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
name|delta_chain_length
condition|)
block|{
if|if
condition|(
operator|!
name|stat_only
condition|)
name|printf
argument_list|(
literal|"%-6s %lu %lu %"
name|PRIuMAX
literal|"\n"
argument_list|,
name|type
argument_list|,
name|size
argument_list|,
name|store_size
argument_list|,
operator|(
name|uintmax_t
operator|)
name|offset
argument_list|)
expr_stmt|;
name|baseobjects
operator|++
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
operator|!
name|stat_only
condition|)
name|printf
argument_list|(
literal|"%-6s %lu %lu %"
name|PRIuMAX
literal|" %u %s\n"
argument_list|,
name|type
argument_list|,
name|size
argument_list|,
name|store_size
argument_list|,
operator|(
name|uintmax_t
operator|)
name|offset
argument_list|,
name|delta_chain_length
argument_list|,
name|sha1_to_hex
argument_list|(
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta_chain_length
operator|<=
name|MAX_CHAIN
condition|)
name|chain_histogram
index|[
name|delta_chain_length
index|]
operator|++
expr_stmt|;
else|else
name|chain_histogram
index|[
literal|0
index|]
operator|++
expr_stmt|;
block|}
block|}
if|if
condition|(
name|baseobjects
condition|)
name|printf
argument_list|(
literal|"non delta: %lu object%s\n"
argument_list|,
name|baseobjects
argument_list|,
name|baseobjects
operator|>
literal|1
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
for|for
control|(
name|cnt
operator|=
literal|1
init|;
name|cnt
operator|<=
name|MAX_CHAIN
condition|;
name|cnt
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|chain_histogram
index|[
name|cnt
index|]
condition|)
continue|continue;
name|printf
argument_list|(
literal|"chain length = %d: %lu object%s\n"
argument_list|,
name|cnt
argument_list|,
name|chain_histogram
index|[
name|cnt
index|]
argument_list|,
name|chain_histogram
index|[
name|cnt
index|]
operator|>
literal|1
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|chain_histogram
index|[
literal|0
index|]
condition|)
name|printf
argument_list|(
literal|"chain length> %d: %lu object%s\n"
argument_list|,
name|MAX_CHAIN
argument_list|,
name|chain_histogram
index|[
literal|0
index|]
argument_list|,
name|chain_histogram
index|[
literal|0
index|]
operator|>
literal|1
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|verify_one_pack
specifier|static
name|int
name|verify_one_pack
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|int
name|flags
parameter_list|)
block|{
name|char
name|arg
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|len
decl_stmt|;
name|int
name|verbose
init|=
name|flags
operator|&
name|VERIFY_PACK_VERBOSE
decl_stmt|;
name|int
name|stat_only
init|=
name|flags
operator|&
name|VERIFY_PACK_STAT_ONLY
decl_stmt|;
name|struct
name|packed_git
modifier|*
name|pack
decl_stmt|;
name|int
name|err
decl_stmt|;
name|len
operator|=
name|strlcpy
argument_list|(
name|arg
argument_list|,
name|path
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|PATH_MAX
condition|)
return|return
name|error
argument_list|(
literal|"name too long: %s"
argument_list|,
name|path
argument_list|)
return|;
comment|/* 	 * In addition to "foo.idx" we accept "foo.pack" and "foo"; 	 * normalize these forms to "foo.idx" for add_packed_git(). 	 */
if|if
condition|(
name|has_extension
argument_list|(
name|arg
argument_list|,
literal|".pack"
argument_list|)
condition|)
block|{
name|strcpy
argument_list|(
name|arg
operator|+
name|len
operator|-
literal|5
argument_list|,
literal|".idx"
argument_list|)
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|has_extension
argument_list|(
name|arg
argument_list|,
literal|".idx"
argument_list|)
condition|)
block|{
if|if
condition|(
name|len
operator|+
literal|4
operator|>=
name|PATH_MAX
condition|)
return|return
name|error
argument_list|(
literal|"name too long: %s.idx"
argument_list|,
name|arg
argument_list|)
return|;
name|strcpy
argument_list|(
name|arg
operator|+
name|len
argument_list|,
literal|".idx"
argument_list|)
expr_stmt|;
name|len
operator|+=
literal|4
expr_stmt|;
block|}
comment|/* 	 * add_packed_git() uses our buffer (containing "foo.idx") to 	 * build the pack filename ("foo.pack").  Make sure it fits. 	 */
if|if
condition|(
name|len
operator|+
literal|1
operator|>=
name|PATH_MAX
condition|)
block|{
name|arg
index|[
name|len
operator|-
literal|4
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
name|error
argument_list|(
literal|"name too long: %s.pack"
argument_list|,
name|arg
argument_list|)
return|;
block|}
name|pack
operator|=
name|add_packed_git
argument_list|(
name|arg
argument_list|,
name|len
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pack
condition|)
return|return
name|error
argument_list|(
literal|"packfile %s not found."
argument_list|,
name|arg
argument_list|)
return|;
name|install_packed_git
argument_list|(
name|pack
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat_only
condition|)
name|err
operator|=
name|verify_pack
argument_list|(
name|pack
argument_list|)
expr_stmt|;
else|else
name|err
operator|=
name|open_pack_index
argument_list|(
name|pack
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
operator|||
name|stat_only
condition|)
block|{
if|if
condition|(
name|err
condition|)
name|printf
argument_list|(
literal|"%s: bad\n"
argument_list|,
name|pack
operator|->
name|pack_name
argument_list|)
expr_stmt|;
else|else
block|{
name|show_pack_info
argument_list|(
name|pack
argument_list|,
name|flags
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat_only
condition|)
name|printf
argument_list|(
literal|"%s: ok\n"
argument_list|,
name|pack
operator|->
name|pack_name
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|err
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|verify_pack_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|verify_pack_usage
index|[]
init|=
block|{
literal|"git verify-pack [-v|--verbose] [-s|--stat-only]<pack>..."
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_verify_pack
name|int
name|cmd_verify_pack
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|err
init|=
literal|0
decl_stmt|;
name|unsigned
name|int
name|flags
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
specifier|const
name|struct
name|option
name|verify_pack_options
index|[]
init|=
block|{
name|OPT_BIT
argument_list|(
literal|'v'
argument_list|,
literal|"verbose"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"verbose"
argument_list|,
name|VERIFY_PACK_VERBOSE
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'s'
argument_list|,
literal|"stat-only"
argument_list|,
operator|&
name|flags
argument_list|,
literal|"show statistics only"
argument_list|,
name|VERIFY_PACK_STAT_ONLY
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|verify_pack_options
argument_list|,
name|verify_pack_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|1
condition|)
name|usage_with_options
argument_list|(
name|verify_pack_usage
argument_list|,
name|verify_pack_options
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
name|argc
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|verify_one_pack
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
name|flags
argument_list|)
condition|)
name|err
operator|=
literal|1
expr_stmt|;
name|discard_revindex
argument_list|()
expr_stmt|;
block|}
return|return
name|err
return|;
block|}
end_function
end_unit
