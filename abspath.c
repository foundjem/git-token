begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_comment
comment|/*  * Do not use this for inspecting *tracked* content.  When path is a  * symlink to a directory, we do not want to say it is a directory when  * dealing with tracked content in the working tree.  */
end_comment
begin_function
DECL|function|is_directory
name|int
name|is_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
return|return
operator|(
operator|!
name|stat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|)
return|;
block|}
end_function
begin_comment
comment|/* We allow "recursive" symbolic links. Only within reason, though. */
end_comment
begin_define
DECL|macro|MAXDEPTH
define|#
directive|define
name|MAXDEPTH
value|5
end_define
begin_function
DECL|function|make_absolute_path
specifier|const
name|char
modifier|*
name|make_absolute_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|static
name|char
name|bufs
index|[
literal|2
index|]
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|,
modifier|*
name|buf
init|=
name|bufs
index|[
literal|0
index|]
decl_stmt|,
modifier|*
name|next_buf
init|=
name|bufs
index|[
literal|1
index|]
decl_stmt|;
name|char
name|cwd
index|[
literal|1024
index|]
init|=
literal|""
decl_stmt|;
name|int
name|buf_index
init|=
literal|1
decl_stmt|,
name|len
decl_stmt|;
name|int
name|depth
init|=
name|MAXDEPTH
decl_stmt|;
name|char
modifier|*
name|last_elem
init|=
name|NULL
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|strlcpy
argument_list|(
name|buf
argument_list|,
name|path
argument_list|,
name|PATH_MAX
argument_list|)
operator|>=
name|PATH_MAX
condition|)
name|die
argument_list|(
literal|"Too long path: %.*s"
argument_list|,
literal|60
argument_list|,
name|path
argument_list|)
expr_stmt|;
while|while
condition|(
name|depth
operator|--
condition|)
block|{
if|if
condition|(
operator|!
name|is_directory
argument_list|(
name|buf
argument_list|)
condition|)
block|{
name|char
modifier|*
name|last_slash
init|=
name|strrchr
argument_list|(
name|buf
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
if|if
condition|(
name|last_slash
condition|)
block|{
operator|*
name|last_slash
operator|=
literal|'\0'
expr_stmt|;
name|last_elem
operator|=
name|xstrdup
argument_list|(
name|last_slash
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|last_elem
operator|=
name|xstrdup
argument_list|(
name|buf
argument_list|)
expr_stmt|;
operator|*
name|buf
operator|=
literal|'\0'
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|*
name|buf
condition|)
block|{
if|if
condition|(
operator|!
operator|*
name|cwd
operator|&&
operator|!
name|getcwd
argument_list|(
name|cwd
argument_list|,
sizeof|sizeof
argument_list|(
name|cwd
argument_list|)
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not get current working directory"
argument_list|)
expr_stmt|;
if|if
condition|(
name|chdir
argument_list|(
name|buf
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not switch to '%s'"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|buf
argument_list|,
name|PATH_MAX
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not get current working directory"
argument_list|)
expr_stmt|;
if|if
condition|(
name|last_elem
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|buf
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|+
name|strlen
argument_list|(
name|last_elem
argument_list|)
operator|+
literal|2
operator|>
name|PATH_MAX
condition|)
name|die
argument_list|(
literal|"Too long path name: '%s/%s'"
argument_list|,
name|buf
argument_list|,
name|last_elem
argument_list|)
expr_stmt|;
name|buf
index|[
name|len
index|]
operator|=
literal|'/'
expr_stmt|;
name|strcpy
argument_list|(
name|buf
operator|+
name|len
operator|+
literal|1
argument_list|,
name|last_elem
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|last_elem
argument_list|)
expr_stmt|;
name|last_elem
operator|=
name|NULL
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|buf
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|len
operator|=
name|readlink
argument_list|(
name|buf
argument_list|,
name|next_buf
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Invalid symlink: %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
name|next_buf
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
name|buf
operator|=
name|next_buf
expr_stmt|;
name|buf_index
operator|=
literal|1
operator|-
name|buf_index
expr_stmt|;
name|next_buf
operator|=
name|bufs
index|[
name|buf_index
index|]
expr_stmt|;
block|}
else|else
break|break;
block|}
if|if
condition|(
operator|*
name|cwd
operator|&&
name|chdir
argument_list|(
name|cwd
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not change back to '%s'"
argument_list|,
name|cwd
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|get_pwd_cwd
specifier|static
specifier|const
name|char
modifier|*
name|get_pwd_cwd
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
name|cwd
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
name|char
modifier|*
name|pwd
decl_stmt|;
name|struct
name|stat
name|cwd_stat
decl_stmt|,
name|pwd_stat
decl_stmt|;
if|if
condition|(
name|getcwd
argument_list|(
name|cwd
argument_list|,
name|PATH_MAX
argument_list|)
operator|==
name|NULL
condition|)
return|return
name|NULL
return|;
name|pwd
operator|=
name|getenv
argument_list|(
literal|"PWD"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pwd
operator|&&
name|strcmp
argument_list|(
name|pwd
argument_list|,
name|cwd
argument_list|)
condition|)
block|{
name|stat
argument_list|(
name|cwd
argument_list|,
operator|&
name|cwd_stat
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|pwd
argument_list|,
operator|&
name|pwd_stat
argument_list|)
operator|&&
name|pwd_stat
operator|.
name|st_dev
operator|==
name|cwd_stat
operator|.
name|st_dev
operator|&&
name|pwd_stat
operator|.
name|st_ino
operator|==
name|cwd_stat
operator|.
name|st_ino
condition|)
block|{
name|strlcpy
argument_list|(
name|cwd
argument_list|,
name|pwd
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|cwd
return|;
block|}
end_function
begin_function
DECL|function|make_nonrelative_path
specifier|const
name|char
modifier|*
name|make_nonrelative_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|static
name|char
name|buf
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
if|if
condition|(
name|is_absolute_path
argument_list|(
name|path
argument_list|)
condition|)
block|{
if|if
condition|(
name|strlcpy
argument_list|(
name|buf
argument_list|,
name|path
argument_list|,
name|PATH_MAX
argument_list|)
operator|>=
name|PATH_MAX
condition|)
name|die
argument_list|(
literal|"Too long path: %.*s"
argument_list|,
literal|60
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
else|else
block|{
specifier|const
name|char
modifier|*
name|cwd
init|=
name|get_pwd_cwd
argument_list|()
decl_stmt|;
if|if
condition|(
operator|!
name|cwd
condition|)
name|die
argument_list|(
literal|"Cannot determine the current working directory"
argument_list|)
expr_stmt|;
if|if
condition|(
name|snprintf
argument_list|(
name|buf
argument_list|,
name|PATH_MAX
argument_list|,
literal|"%s/%s"
argument_list|,
name|cwd
argument_list|,
name|path
argument_list|)
operator|>=
name|PATH_MAX
condition|)
name|die
argument_list|(
literal|"Too long path: %.*s"
argument_list|,
literal|60
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
return|return
name|buf
return|;
block|}
end_function
end_unit
