begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"unix-socket.h"
end_include
begin_function
DECL|function|unix_stream_socket
specifier|static
name|int
name|unix_stream_socket
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|fd
init|=
name|socket
argument_list|(
name|AF_UNIX
argument_list|,
name|SOCK_STREAM
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to create socket"
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|chdir_len
specifier|static
name|int
name|chdir_len
parameter_list|(
specifier|const
name|char
modifier|*
name|orig
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|xmemdupz
argument_list|(
name|orig
argument_list|,
name|len
argument_list|)
decl_stmt|;
name|int
name|r
init|=
name|chdir
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|r
return|;
block|}
end_function
begin_struct
DECL|struct|unix_sockaddr_context
struct|struct
name|unix_sockaddr_context
block|{
DECL|member|orig_dir
name|char
name|orig_dir
index|[
name|PATH_MAX
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|unix_sockaddr_cleanup
specifier|static
name|void
name|unix_sockaddr_cleanup
parameter_list|(
name|struct
name|unix_sockaddr_context
modifier|*
name|ctx
parameter_list|)
block|{
if|if
condition|(
operator|!
name|ctx
operator|->
name|orig_dir
index|[
literal|0
index|]
condition|)
return|return;
comment|/* 	 * If we fail, we can't just return an error, since we have 	 * moved the cwd of the whole process, which could confuse calling 	 * code.  We are better off to just die. 	 */
if|if
condition|(
name|chdir
argument_list|(
name|ctx
operator|->
name|orig_dir
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to restore original working directory"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|unix_sockaddr_init
specifier|static
name|int
name|unix_sockaddr_init
parameter_list|(
name|struct
name|sockaddr_un
modifier|*
name|sa
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|unix_sockaddr_context
modifier|*
name|ctx
parameter_list|)
block|{
name|int
name|size
init|=
name|strlen
argument_list|(
name|path
argument_list|)
operator|+
literal|1
decl_stmt|;
name|ctx
operator|->
name|orig_dir
index|[
literal|0
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|size
operator|>
sizeof|sizeof
argument_list|(
name|sa
operator|->
name|sun_path
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|slash
init|=
name|find_last_dir_sep
argument_list|(
name|path
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|dir
decl_stmt|;
if|if
condition|(
operator|!
name|slash
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|dir
operator|=
name|path
expr_stmt|;
name|path
operator|=
name|slash
operator|+
literal|1
expr_stmt|;
name|size
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
operator|+
literal|1
expr_stmt|;
if|if
condition|(
name|size
operator|>
sizeof|sizeof
argument_list|(
name|sa
operator|->
name|sun_path
argument_list|)
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|getcwd
argument_list|(
name|ctx
operator|->
name|orig_dir
argument_list|,
sizeof|sizeof
argument_list|(
name|ctx
operator|->
name|orig_dir
argument_list|)
argument_list|)
condition|)
block|{
name|errno
operator|=
name|ENAMETOOLONG
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|chdir_len
argument_list|(
name|dir
argument_list|,
name|slash
operator|-
name|dir
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
name|memset
argument_list|(
name|sa
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|sa
argument_list|)
argument_list|)
expr_stmt|;
name|sa
operator|->
name|sun_family
operator|=
name|AF_UNIX
expr_stmt|;
name|memcpy
argument_list|(
name|sa
operator|->
name|sun_path
argument_list|,
name|path
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|unix_stream_connect
name|int
name|unix_stream_connect
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|saved_errno
decl_stmt|;
name|struct
name|sockaddr_un
name|sa
decl_stmt|;
name|struct
name|unix_sockaddr_context
name|ctx
decl_stmt|;
if|if
condition|(
name|unix_sockaddr_init
argument_list|(
operator|&
name|sa
argument_list|,
name|path
argument_list|,
operator|&
name|ctx
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|fd
operator|=
name|unix_stream_socket
argument_list|()
expr_stmt|;
if|if
condition|(
name|connect
argument_list|(
name|fd
argument_list|,
operator|(
expr|struct
name|sockaddr
operator|*
operator|)
operator|&
name|sa
argument_list|,
sizeof|sizeof
argument_list|(
name|sa
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|fail
goto|;
name|unix_sockaddr_cleanup
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
name|fail
label|:
name|saved_errno
operator|=
name|errno
expr_stmt|;
name|unix_sockaddr_cleanup
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|errno
operator|=
name|saved_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|unix_stream_listen
name|int
name|unix_stream_listen
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|saved_errno
decl_stmt|;
name|struct
name|sockaddr_un
name|sa
decl_stmt|;
name|struct
name|unix_sockaddr_context
name|ctx
decl_stmt|;
name|unlink
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|unix_sockaddr_init
argument_list|(
operator|&
name|sa
argument_list|,
name|path
argument_list|,
operator|&
name|ctx
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|fd
operator|=
name|unix_stream_socket
argument_list|()
expr_stmt|;
if|if
condition|(
name|bind
argument_list|(
name|fd
argument_list|,
operator|(
expr|struct
name|sockaddr
operator|*
operator|)
operator|&
name|sa
argument_list|,
sizeof|sizeof
argument_list|(
name|sa
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|fail
goto|;
if|if
condition|(
name|listen
argument_list|(
name|fd
argument_list|,
literal|5
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|fail
goto|;
name|unix_sockaddr_cleanup
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
name|fail
label|:
name|saved_errno
operator|=
name|errno
expr_stmt|;
name|unix_sockaddr_cleanup
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|errno
operator|=
name|saved_errno
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
end_unit
