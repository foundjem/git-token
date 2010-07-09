begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_function
DECL|function|copy_fd
name|int
name|copy_fd
parameter_list|(
name|int
name|ifd
parameter_list|,
name|int
name|ofd
parameter_list|)
block|{
while|while
condition|(
literal|1
condition|)
block|{
name|char
name|buffer
index|[
literal|8192
index|]
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|buffer
decl_stmt|;
name|ssize_t
name|len
init|=
name|xread
argument_list|(
name|ifd
argument_list|,
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
break|break;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
block|{
name|int
name|read_error
init|=
name|errno
decl_stmt|;
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"copy-fd: read returned %s"
argument_list|,
name|strerror
argument_list|(
name|read_error
argument_list|)
argument_list|)
return|;
block|}
while|while
condition|(
name|len
condition|)
block|{
name|int
name|written
init|=
name|xwrite
argument_list|(
name|ofd
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
name|written
operator|>
literal|0
condition|)
block|{
name|buf
operator|+=
name|written
expr_stmt|;
name|len
operator|-=
name|written
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|written
condition|)
block|{
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"copy-fd: write returned 0"
argument_list|)
return|;
block|}
else|else
block|{
name|int
name|write_error
init|=
name|errno
decl_stmt|;
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"copy-fd: write returned %s"
argument_list|,
name|strerror
argument_list|(
name|write_error
argument_list|)
argument_list|)
return|;
block|}
block|}
block|}
name|close
argument_list|(
name|ifd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|copy_times
specifier|static
name|int
name|copy_times
parameter_list|(
specifier|const
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
name|struct
name|stat
name|st
decl_stmt|;
name|struct
name|utimbuf
name|times
decl_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|src
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|times
operator|.
name|actime
operator|=
name|st
operator|.
name|st_atime
expr_stmt|;
name|times
operator|.
name|modtime
operator|=
name|st
operator|.
name|st_mtime
expr_stmt|;
if|if
condition|(
name|utime
argument_list|(
name|dst
argument_list|,
operator|&
name|times
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|copy_file
name|int
name|copy_file
parameter_list|(
specifier|const
name|char
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|int
name|fdi
decl_stmt|,
name|fdo
decl_stmt|,
name|status
decl_stmt|;
name|mode
operator|=
operator|(
name|mode
operator|&
literal|0111
operator|)
condition|?
literal|0777
else|:
literal|0666
expr_stmt|;
if|if
condition|(
operator|(
name|fdi
operator|=
name|open
argument_list|(
name|src
argument_list|,
name|O_RDONLY
argument_list|)
operator|)
operator|<
literal|0
condition|)
return|return
name|fdi
return|;
if|if
condition|(
operator|(
name|fdo
operator|=
name|open
argument_list|(
name|dst
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
name|mode
argument_list|)
operator|)
operator|<
literal|0
condition|)
block|{
name|close
argument_list|(
name|fdi
argument_list|)
expr_stmt|;
return|return
name|fdo
return|;
block|}
name|status
operator|=
name|copy_fd
argument_list|(
name|fdi
argument_list|,
name|fdo
argument_list|)
expr_stmt|;
if|if
condition|(
name|close
argument_list|(
name|fdo
argument_list|)
operator|!=
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"%s: close error: %s"
argument_list|,
name|dst
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
if|if
condition|(
operator|!
name|status
operator|&&
name|adjust_shared_perm
argument_list|(
name|dst
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|status
return|;
block|}
end_function
begin_function
DECL|function|copy_file_with_time
name|int
name|copy_file_with_time
parameter_list|(
specifier|const
name|char
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|int
name|status
init|=
name|copy_file
argument_list|(
name|dst
argument_list|,
name|src
argument_list|,
name|mode
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|status
condition|)
return|return
name|copy_times
argument_list|(
name|dst
argument_list|,
name|src
argument_list|)
return|;
return|return
name|status
return|;
block|}
end_function
end_unit
