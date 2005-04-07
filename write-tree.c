begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_function
DECL|function|check_valid_sha1
specifier|static
name|int
name|check_valid_sha1
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
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
name|ret
decl_stmt|;
comment|/* If we were anal, we'd check that the sha1 of the contents actually matches */
name|ret
operator|=
name|access
argument_list|(
name|filename
argument_list|,
name|R_OK
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
name|perror
argument_list|(
name|filename
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|prepend_integer
specifier|static
name|int
name|prepend_integer
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|val
parameter_list|,
name|int
name|i
parameter_list|)
block|{
name|buffer
index|[
operator|--
name|i
index|]
operator|=
literal|'\0'
expr_stmt|;
do|do
block|{
name|buffer
index|[
operator|--
name|i
index|]
operator|=
literal|'0'
operator|+
operator|(
name|val
operator|%
literal|10
operator|)
expr_stmt|;
name|val
operator|/=
literal|10
expr_stmt|;
block|}
do|while
condition|(
name|val
condition|)
do|;
return|return
name|i
return|;
block|}
end_function
begin_define
DECL|macro|ORIG_OFFSET
define|#
directive|define
name|ORIG_OFFSET
value|(40)
end_define
begin_comment
DECL|macro|ORIG_OFFSET
comment|/* Enough space to add the header of "tree<size>\0" */
end_comment
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
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|,
name|val
decl_stmt|;
name|int
name|i
decl_stmt|,
name|entries
init|=
name|read_cache
argument_list|()
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|;
if|if
condition|(
name|entries
operator|<=
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"No file-cache to create a tree of\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* Guess at an initial size */
name|size
operator|=
name|entries
operator|*
literal|40
operator|+
literal|400
expr_stmt|;
name|buffer
operator|=
name|malloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|offset
operator|=
name|ORIG_OFFSET
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
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
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|check_valid_sha1
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|offset
operator|+
name|ce
operator|->
name|namelen
operator|+
literal|60
operator|>
name|size
condition|)
block|{
name|size
operator|=
name|alloc_nr
argument_list|(
name|offset
operator|+
name|ce
operator|->
name|namelen
operator|+
literal|60
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|realloc
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|offset
operator|+=
name|sprintf
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
literal|"%o %s"
argument_list|,
name|ce
operator|->
name|st_mode
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|buffer
index|[
name|offset
operator|++
index|]
operator|=
literal|0
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|offset
operator|+=
literal|20
expr_stmt|;
block|}
name|i
operator|=
name|prepend_integer
argument_list|(
name|buffer
argument_list|,
name|offset
operator|-
name|ORIG_OFFSET
argument_list|,
name|ORIG_OFFSET
argument_list|)
expr_stmt|;
name|i
operator|-=
literal|5
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
operator|+
name|i
argument_list|,
literal|"tree "
argument_list|,
literal|5
argument_list|)
expr_stmt|;
name|buffer
operator|+=
name|i
expr_stmt|;
name|offset
operator|-=
name|i
expr_stmt|;
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|offset
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
