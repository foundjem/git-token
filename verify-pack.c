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
name|int
name|verbose
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
name|struct
name|packed_git
modifier|*
name|g
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
while|while
condition|(
literal|1
condition|)
block|{
comment|/* Should name foo.idx, but foo.pack may be named; 		 * convert it to foo.idx 		 */
if|if
condition|(
name|has_extension
argument_list|(
name|arg
argument_list|,
name|len
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
name|len
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
if|if
condition|(
operator|(
name|g
operator|=
name|add_packed_git
argument_list|(
name|arg
argument_list|,
name|len
argument_list|,
literal|1
argument_list|)
operator|)
condition|)
break|break;
return|return
name|error
argument_list|(
literal|"packfile %s not found."
argument_list|,
name|arg
argument_list|)
return|;
block|}
return|return
name|verify_pack
argument_list|(
name|g
argument_list|,
name|verbose
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|verify_pack_usage
specifier|static
specifier|const
name|char
name|verify_pack_usage
index|[]
init|=
literal|"git-verify-pack [-v]<pack>..."
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|ac
parameter_list|,
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|int
name|errs
init|=
literal|0
decl_stmt|;
name|int
name|verbose
init|=
literal|0
decl_stmt|;
name|int
name|no_more_options
init|=
literal|0
decl_stmt|;
name|int
name|nothing_done
init|=
literal|1
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|ac
condition|)
block|{
if|if
condition|(
operator|!
name|no_more_options
operator|&&
name|av
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"-v"
argument_list|,
name|av
index|[
literal|1
index|]
argument_list|)
condition|)
name|verbose
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"--"
argument_list|,
name|av
index|[
literal|1
index|]
argument_list|)
condition|)
name|no_more_options
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|verify_pack_usage
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|verify_one_pack
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|,
name|verbose
argument_list|)
condition|)
name|errs
operator|++
expr_stmt|;
name|nothing_done
operator|=
literal|0
expr_stmt|;
block|}
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|nothing_done
condition|)
name|usage
argument_list|(
name|verify_pack_usage
argument_list|)
expr_stmt|;
return|return
operator|!
operator|!
name|errs
return|;
block|}
end_function
end_unit
