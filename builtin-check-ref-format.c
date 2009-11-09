begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_comment
comment|/*  * Replace each run of adjacent slashes in src with a single slash,  * and write the result to dst.  *  * This function is similar to normalize_path_copy(), but stripped down  * to meet check_ref_format's simpler needs.  */
end_comment
begin_function
DECL|function|collapse_slashes
specifier|static
name|void
name|collapse_slashes
parameter_list|(
name|char
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|char
name|ch
decl_stmt|;
name|char
name|prev
init|=
literal|'\0'
decl_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|src
operator|++
operator|)
operator|!=
literal|'\0'
condition|)
block|{
if|if
condition|(
name|prev
operator|==
literal|'/'
operator|&&
name|ch
operator|==
name|prev
condition|)
continue|continue;
operator|*
name|dst
operator|++
operator|=
name|ch
expr_stmt|;
name|prev
operator|=
name|ch
expr_stmt|;
block|}
operator|*
name|dst
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_check_ref_format
name|int
name|cmd_check_ref_format
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
if|if
condition|(
name|argc
operator|==
literal|2
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage
argument_list|(
name|builtin_check_ref_format_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|3
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--branch"
argument_list|)
condition|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|strbuf_check_branch_ref
argument_list|(
operator|&
name|sb
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
condition|)
name|die
argument_list|(
literal|"'%s' is not a valid branch name"
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sb
operator|.
name|buf
operator|+
literal|11
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|==
literal|3
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--print"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|refname
init|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
operator|+
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|check_ref_format
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|collapse_slashes
argument_list|(
name|refname
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|refname
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|!=
literal|2
condition|)
name|usage
argument_list|(
literal|"git check-ref-format refname"
argument_list|)
expr_stmt|;
return|return
operator|!
operator|!
name|check_ref_format
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
block|}
end_function
end_unit
