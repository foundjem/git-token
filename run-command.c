begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_function
DECL|function|run_command_v_opt
name|int
name|run_command_v_opt
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|pid_t
name|pid
init|=
name|fork
argument_list|()
decl_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_FORK
return|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
if|if
condition|(
name|flags
operator|&
name|RUN_COMMAND_NO_STDIO
condition|)
block|{
name|int
name|fd
init|=
name|open
argument_list|(
literal|"/dev/null"
argument_list|,
name|O_RDWR
argument_list|)
decl_stmt|;
name|dup2
argument_list|(
name|fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|fd
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|flags
operator|&
name|RUN_GIT_CMD
condition|)
block|{
name|execv_git_cmd
argument_list|(
name|argv
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|execvp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
operator|(
name|char
operator|*
specifier|const
operator|*
operator|)
name|argv
argument_list|)
expr_stmt|;
block|}
name|die
argument_list|(
literal|"exec %s failed."
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
block|}
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|status
decl_stmt|,
name|code
decl_stmt|;
name|int
name|retval
init|=
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|retval
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EINTR
condition|)
continue|continue;
name|error
argument_list|(
literal|"waitpid failed (%s)"
argument_list|,
name|strerror
argument_list|(
name|retval
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID
return|;
block|}
if|if
condition|(
name|retval
operator|!=
name|pid
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_WRONG_PID
return|;
if|if
condition|(
name|WIFSIGNALED
argument_list|(
name|status
argument_list|)
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_SIGNAL
return|;
if|if
condition|(
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
return|return
operator|-
name|ERR_RUN_COMMAND_WAITPID_NOEXIT
return|;
name|code
operator|=
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
expr_stmt|;
if|if
condition|(
name|code
condition|)
return|return
operator|-
name|code
return|;
return|return
literal|0
return|;
block|}
block|}
end_function
begin_function
DECL|function|run_command_v
name|int
name|run_command_v
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
return|return
name|run_command_v_opt
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|run_command
name|int
name|run_command
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
modifier|...
parameter_list|)
block|{
name|int
name|argc
decl_stmt|;
name|char
modifier|*
name|argv
index|[
name|MAX_RUN_COMMAND_ARGS
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|arg
decl_stmt|;
name|va_list
name|param
decl_stmt|;
name|va_start
argument_list|(
name|param
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
operator|(
name|char
operator|*
operator|)
name|cmd
expr_stmt|;
name|argc
operator|=
literal|1
expr_stmt|;
while|while
condition|(
name|argc
operator|<
name|MAX_RUN_COMMAND_ARGS
condition|)
block|{
name|arg
operator|=
name|argv
index|[
name|argc
operator|++
index|]
operator|=
name|va_arg
argument_list|(
name|param
argument_list|,
name|char
operator|*
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
break|break;
block|}
name|va_end
argument_list|(
name|param
argument_list|)
expr_stmt|;
if|if
condition|(
name|MAX_RUN_COMMAND_ARGS
operator|<=
name|argc
condition|)
return|return
name|error
argument_list|(
literal|"too many args to run %s"
argument_list|,
name|cmd
argument_list|)
return|;
return|return
name|run_command_v_opt
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
end_unit
