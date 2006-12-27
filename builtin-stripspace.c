begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_comment
comment|/*  * Remove empty lines from the beginning and end.  *  * Turn multiple consecutive empty lines into just one  * empty line.  Return true if it is an incomplete line.  */
end_comment
begin_function
DECL|function|cleanup
specifier|static
name|int
name|cleanup
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
block|{
if|if
condition|(
name|len
operator|==
literal|1
condition|)
return|return
literal|0
return|;
do|do
block|{
name|unsigned
name|char
name|c
init|=
name|line
index|[
name|len
operator|-
literal|2
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|line
index|[
name|len
operator|-
literal|2
index|]
operator|=
literal|'\n'
expr_stmt|;
name|len
operator|--
expr_stmt|;
name|line
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
block|}
do|while
condition|(
name|len
operator|>
literal|1
condition|)
do|;
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|stripspace
name|void
name|stripspace
parameter_list|(
name|FILE
modifier|*
name|in
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|)
block|{
name|int
name|empties
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|incomplete
init|=
literal|0
decl_stmt|;
name|char
name|line
index|[
literal|1024
index|]
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|in
argument_list|)
condition|)
block|{
name|incomplete
operator|=
name|cleanup
argument_list|(
name|line
argument_list|)
expr_stmt|;
comment|/* Not just an empty line? */
if|if
condition|(
name|line
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
block|{
if|if
condition|(
name|empties
operator|>
literal|0
condition|)
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|out
argument_list|)
expr_stmt|;
name|empties
operator|=
literal|0
expr_stmt|;
name|fputs
argument_list|(
name|line
argument_list|,
name|out
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|empties
operator|<
literal|0
condition|)
continue|continue;
name|empties
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|incomplete
condition|)
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|out
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_stripspace
name|int
name|cmd_stripspace
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
name|stripspace
argument_list|(
name|stdin
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
