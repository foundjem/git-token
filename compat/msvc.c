begin_unit
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"win32.h"
end_include
begin_include
include|#
directive|include
file|<conio.h>
end_include
begin_include
include|#
directive|include
file|"../strbuf.h"
end_include
begin_function
DECL|function|opendir
name|DIR
modifier|*
name|opendir
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|DWORD
name|attrs
init|=
name|GetFileAttributes
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|int
name|len
decl_stmt|;
name|DIR
modifier|*
name|p
decl_stmt|;
comment|/* check for valid path */
if|if
condition|(
name|attrs
operator|==
name|INVALID_FILE_ATTRIBUTES
condition|)
block|{
name|errno
operator|=
name|ENOENT
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* check if it's a directory */
if|if
condition|(
operator|!
operator|(
name|attrs
operator|&
name|FILE_ATTRIBUTE_DIRECTORY
operator|)
condition|)
block|{
name|errno
operator|=
name|ENOTDIR
expr_stmt|;
return|return
name|NULL
return|;
block|}
comment|/* check that the pattern won't be too long for FindFirstFileA */
name|len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_dir_sep
argument_list|(
name|name
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|len
operator|--
expr_stmt|;
if|if
condition|(
name|len
operator|+
literal|2
operator|>=
name|MAX_PATH
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|p
operator|=
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
name|DIR
argument_list|)
operator|+
name|len
operator|+
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
return|return
name|NULL
return|;
name|memset
argument_list|(
name|p
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|DIR
argument_list|)
operator|+
name|len
operator|+
literal|2
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|p
operator|->
name|dd_name
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|p
operator|->
name|dd_name
index|[
name|len
index|]
operator|=
literal|'/'
expr_stmt|;
name|p
operator|->
name|dd_name
index|[
name|len
operator|+
literal|1
index|]
operator|=
literal|'*'
expr_stmt|;
name|p
operator|->
name|dd_handle
operator|=
operator|(
name|long
operator|)
name|INVALID_HANDLE_VALUE
expr_stmt|;
return|return
name|p
return|;
block|}
end_function
begin_function
DECL|function|closedir
name|int
name|closedir
parameter_list|(
name|DIR
modifier|*
name|dir
parameter_list|)
block|{
if|if
condition|(
operator|!
name|dir
condition|)
block|{
name|errno
operator|=
name|EBADF
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|dir
operator|->
name|dd_handle
operator|!=
operator|(
name|long
operator|)
name|INVALID_HANDLE_VALUE
condition|)
name|FindClose
argument_list|(
operator|(
name|HANDLE
operator|)
name|dir
operator|->
name|dd_handle
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_include
include|#
directive|include
file|"mingw.c"
end_include
end_unit
