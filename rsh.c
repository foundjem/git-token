begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"rsh.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_define
DECL|macro|COMMAND_SIZE
define|#
directive|define
name|COMMAND_SIZE
value|4096
end_define
begin_function
DECL|function|setup_connection
name|int
name|setup_connection
parameter_list|(
name|int
modifier|*
name|fd_in
parameter_list|,
name|int
modifier|*
name|fd_out
parameter_list|,
specifier|const
name|char
modifier|*
name|remote_prog
parameter_list|,
name|char
modifier|*
name|url
parameter_list|,
name|int
name|rmt_argc
parameter_list|,
name|char
modifier|*
modifier|*
name|rmt_argv
parameter_list|)
block|{
name|char
modifier|*
name|host
decl_stmt|;
name|char
modifier|*
name|path
decl_stmt|;
name|int
name|sv
index|[
literal|2
index|]
decl_stmt|;
name|char
name|command
index|[
name|COMMAND_SIZE
index|]
decl_stmt|;
name|char
modifier|*
name|posn
decl_stmt|;
name|int
name|sizen
decl_stmt|;
name|int
name|of
decl_stmt|;
name|int
name|i
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|url
argument_list|,
literal|"-"
argument_list|)
condition|)
block|{
operator|*
name|fd_in
operator|=
literal|0
expr_stmt|;
operator|*
name|fd_out
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
name|host
operator|=
name|strstr
argument_list|(
name|url
argument_list|,
literal|"//"
argument_list|)
expr_stmt|;
if|if
condition|(
name|host
condition|)
block|{
name|host
operator|+=
literal|2
expr_stmt|;
name|path
operator|=
name|strchr
argument_list|(
name|host
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|host
operator|=
name|url
expr_stmt|;
name|path
operator|=
name|strchr
argument_list|(
name|host
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|path
condition|)
operator|*
operator|(
name|path
operator|++
operator|)
operator|=
literal|'\0'
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|path
condition|)
block|{
return|return
name|error
argument_list|(
literal|"Bad URL: %s"
argument_list|,
name|url
argument_list|)
return|;
block|}
comment|/* $GIT_RSH<host> "env GIT_DIR=<path><remote_prog><args...>" */
name|sizen
operator|=
name|COMMAND_SIZE
expr_stmt|;
name|posn
operator|=
name|command
expr_stmt|;
name|of
operator|=
literal|0
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|"env "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|GIT_DIR_ENVIRONMENT
literal|"="
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|path
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|remote_prog
argument_list|,
literal|1
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
name|rmt_argc
condition|;
name|i
operator|++
control|)
block|{
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|rmt_argv
index|[
name|i
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" -"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|of
condition|)
return|return
name|error
argument_list|(
literal|"Command line too long"
argument_list|)
return|;
if|if
condition|(
name|socketpair
argument_list|(
name|AF_UNIX
argument_list|,
name|SOCK_STREAM
argument_list|,
literal|0
argument_list|,
name|sv
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't create socket"
argument_list|)
return|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't fork"
argument_list|)
return|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
specifier|const
name|char
modifier|*
name|ssh
decl_stmt|,
modifier|*
name|ssh_basename
decl_stmt|;
name|ssh
operator|=
name|getenv
argument_list|(
literal|"GIT_SSH"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ssh
condition|)
name|ssh
operator|=
literal|"ssh"
expr_stmt|;
name|ssh_basename
operator|=
name|strrchr
argument_list|(
name|ssh
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ssh_basename
condition|)
name|ssh_basename
operator|=
name|ssh
expr_stmt|;
else|else
name|ssh_basename
operator|++
expr_stmt|;
name|close
argument_list|(
name|sv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
name|ssh
argument_list|,
name|ssh_basename
argument_list|,
name|host
argument_list|,
name|command
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
operator|*
name|fd_in
operator|=
name|sv
index|[
literal|1
index|]
expr_stmt|;
operator|*
name|fd_out
operator|=
name|sv
index|[
literal|1
index|]
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
