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
file|"exec_cmd.h"
end_include
begin_function
DECL|function|close_pair
specifier|static
specifier|inline
name|void
name|close_pair
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|)
block|{
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|dup_devnull
specifier|static
specifier|inline
name|void
name|dup_devnull
parameter_list|(
name|int
name|to
parameter_list|)
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
name|to
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|start_command
name|int
name|start_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
name|int
name|need_in
decl_stmt|,
name|need_out
decl_stmt|,
name|need_err
decl_stmt|;
name|int
name|fdin
index|[
literal|2
index|]
decl_stmt|,
name|fdout
index|[
literal|2
index|]
decl_stmt|,
name|fderr
index|[
literal|2
index|]
decl_stmt|;
comment|/* 	 * In case of errors we must keep the promise to close FDs 	 * that have been passed in via ->in and ->out. 	 */
name|need_in
operator|=
operator|!
name|cmd
operator|->
name|no_stdin
operator|&&
name|cmd
operator|->
name|in
operator|<
literal|0
expr_stmt|;
if|if
condition|(
name|need_in
condition|)
block|{
if|if
condition|(
name|pipe
argument_list|(
name|fdin
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|cmd
operator|->
name|out
operator|>
literal|0
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_PIPE
return|;
block|}
name|cmd
operator|->
name|in
operator|=
name|fdin
index|[
literal|1
index|]
expr_stmt|;
block|}
name|need_out
operator|=
operator|!
name|cmd
operator|->
name|no_stdout
operator|&&
operator|!
name|cmd
operator|->
name|stdout_to_stderr
operator|&&
name|cmd
operator|->
name|out
operator|<
literal|0
expr_stmt|;
if|if
condition|(
name|need_out
condition|)
block|{
if|if
condition|(
name|pipe
argument_list|(
name|fdout
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|need_in
condition|)
name|close_pair
argument_list|(
name|fdin
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_PIPE
return|;
block|}
name|cmd
operator|->
name|out
operator|=
name|fdout
index|[
literal|0
index|]
expr_stmt|;
block|}
name|need_err
operator|=
operator|!
name|cmd
operator|->
name|no_stderr
operator|&&
name|cmd
operator|->
name|err
operator|<
literal|0
expr_stmt|;
if|if
condition|(
name|need_err
condition|)
block|{
if|if
condition|(
name|pipe
argument_list|(
name|fderr
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|need_in
condition|)
name|close_pair
argument_list|(
name|fdin
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_out
condition|)
name|close_pair
argument_list|(
name|fdout
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|out
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_PIPE
return|;
block|}
name|cmd
operator|->
name|err
operator|=
name|fderr
index|[
literal|0
index|]
expr_stmt|;
block|}
ifndef|#
directive|ifndef
name|__MINGW32__
name|cmd
operator|->
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|cmd
operator|->
name|pid
condition|)
block|{
if|if
condition|(
name|cmd
operator|->
name|no_stdin
condition|)
name|dup_devnull
argument_list|(
literal|0
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_in
condition|)
block|{
name|dup2
argument_list|(
name|fdin
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close_pair
argument_list|(
name|fdin
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
block|{
name|dup2
argument_list|(
name|cmd
operator|->
name|in
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|no_stderr
condition|)
name|dup_devnull
argument_list|(
literal|2
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_err
condition|)
block|{
name|dup2
argument_list|(
name|fderr
index|[
literal|1
index|]
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|close_pair
argument_list|(
name|fderr
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|no_stdout
condition|)
name|dup_devnull
argument_list|(
literal|1
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|stdout_to_stderr
condition|)
name|dup2
argument_list|(
literal|2
argument_list|,
literal|1
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_out
condition|)
block|{
name|dup2
argument_list|(
name|fdout
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close_pair
argument_list|(
name|fdout
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|out
operator|>
literal|1
condition|)
block|{
name|dup2
argument_list|(
name|cmd
operator|->
name|out
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|dir
operator|&&
name|chdir
argument_list|(
name|cmd
operator|->
name|dir
argument_list|)
condition|)
name|die
argument_list|(
literal|"exec %s: cd to %s failed (%s)"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
name|cmd
operator|->
name|dir
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|env
condition|)
block|{
for|for
control|(
init|;
operator|*
name|cmd
operator|->
name|env
condition|;
name|cmd
operator|->
name|env
operator|++
control|)
block|{
if|if
condition|(
name|strchr
argument_list|(
operator|*
name|cmd
operator|->
name|env
argument_list|,
literal|'='
argument_list|)
condition|)
name|putenv
argument_list|(
operator|(
name|char
operator|*
operator|)
operator|*
name|cmd
operator|->
name|env
argument_list|)
expr_stmt|;
else|else
name|unsetenv
argument_list|(
operator|*
name|cmd
operator|->
name|env
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|cmd
operator|->
name|git_cmd
condition|)
block|{
name|execv_git_cmd
argument_list|(
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|execvp
argument_list|(
name|cmd
operator|->
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
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
name|die
argument_list|(
literal|"exec %s failed."
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
block|}
else|#
directive|else
name|int
name|s0
init|=
operator|-
literal|1
decl_stmt|,
name|s1
init|=
operator|-
literal|1
decl_stmt|,
name|s2
init|=
operator|-
literal|1
decl_stmt|;
comment|/* backups of stdin, stdout, stderr */
specifier|const
name|char
modifier|*
name|sargv0
init|=
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
decl_stmt|;
name|char
modifier|*
modifier|*
name|env
init|=
name|environ
decl_stmt|;
name|struct
name|strbuf
name|git_cmd
decl_stmt|;
if|if
condition|(
name|cmd
operator|->
name|no_stdin
condition|)
block|{
name|s0
operator|=
name|dup
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|dup_devnull
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|need_in
condition|)
block|{
name|s0
operator|=
name|dup
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|fdin
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
block|{
name|s0
operator|=
name|dup
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|cmd
operator|->
name|in
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|no_stderr
condition|)
block|{
name|s2
operator|=
name|dup
argument_list|(
literal|2
argument_list|)
expr_stmt|;
name|dup_devnull
argument_list|(
literal|2
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|need_err
condition|)
block|{
name|s2
operator|=
name|dup
argument_list|(
literal|2
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|fderr
index|[
literal|1
index|]
argument_list|,
literal|2
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|no_stdout
condition|)
block|{
name|s1
operator|=
name|dup
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|dup_devnull
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|stdout_to_stderr
condition|)
block|{
name|s1
operator|=
name|dup
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
literal|2
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|need_out
condition|)
block|{
name|s1
operator|=
name|dup
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|fdout
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|out
operator|>
literal|1
condition|)
block|{
name|s1
operator|=
name|dup
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|cmd
operator|->
name|out
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|dir
condition|)
name|die
argument_list|(
literal|"chdir in start_command() not implemented"
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|env
condition|)
block|{
name|env
operator|=
name|copy_environ
argument_list|()
expr_stmt|;
for|for
control|(
init|;
operator|*
name|cmd
operator|->
name|env
condition|;
name|cmd
operator|->
name|env
operator|++
control|)
name|env
operator|=
name|env_setenv
argument_list|(
name|env
argument_list|,
operator|*
name|cmd
operator|->
name|env
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|git_cmd
condition|)
block|{
name|strbuf_init
argument_list|(
operator|&
name|git_cmd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|git_cmd
argument_list|,
literal|"git-%s"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
operator|=
name|git_cmd
operator|.
name|buf
expr_stmt|;
block|}
name|cmd
operator|->
name|pid
operator|=
name|spawnvpe
argument_list|(
name|_P_NOWAIT
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
name|cmd
operator|->
name|argv
argument_list|,
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|env
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|env
condition|)
name|free_environ
argument_list|(
name|env
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|git_cmd
condition|)
name|strbuf_release
argument_list|(
operator|&
name|git_cmd
argument_list|)
expr_stmt|;
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
operator|=
name|sargv0
expr_stmt|;
if|if
condition|(
name|s0
operator|>=
literal|0
condition|)
name|dup2
argument_list|(
name|s0
argument_list|,
literal|0
argument_list|)
operator|,
name|close
argument_list|(
name|s0
argument_list|)
expr_stmt|;
if|if
condition|(
name|s1
operator|>=
literal|0
condition|)
name|dup2
argument_list|(
name|s1
argument_list|,
literal|1
argument_list|)
operator|,
name|close
argument_list|(
name|s1
argument_list|)
expr_stmt|;
if|if
condition|(
name|s2
operator|>=
literal|0
condition|)
name|dup2
argument_list|(
name|s2
argument_list|,
literal|2
argument_list|)
operator|,
name|close
argument_list|(
name|s2
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|cmd
operator|->
name|pid
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|need_in
condition|)
name|close_pair
argument_list|(
name|fdin
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_out
condition|)
name|close_pair
argument_list|(
name|fdout
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|out
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_err
condition|)
name|close_pair
argument_list|(
name|fderr
argument_list|)
expr_stmt|;
return|return
operator|-
name|ERR_RUN_COMMAND_FORK
return|;
block|}
if|if
condition|(
name|need_in
condition|)
name|close
argument_list|(
name|fdin
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|in
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_out
condition|)
name|close
argument_list|(
name|fdout
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|out
condition|)
name|close
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|need_err
condition|)
name|close
argument_list|(
name|fderr
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|wait_or_whine
specifier|static
name|int
name|wait_or_whine
parameter_list|(
name|pid_t
name|pid
parameter_list|)
block|{
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
name|pid_t
name|waiting
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
name|waiting
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
name|errno
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
name|waiting
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
DECL|function|finish_command
name|int
name|finish_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
return|return
name|wait_or_whine
argument_list|(
name|cmd
operator|->
name|pid
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|run_command
name|int
name|run_command
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|)
block|{
name|int
name|code
init|=
name|start_command
argument_list|(
name|cmd
argument_list|)
decl_stmt|;
if|if
condition|(
name|code
condition|)
return|return
name|code
return|;
return|return
name|finish_command
argument_list|(
name|cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|prepare_run_command_v_opt
specifier|static
name|void
name|prepare_run_command_v_opt
parameter_list|(
name|struct
name|child_process
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|opt
parameter_list|)
block|{
name|memset
argument_list|(
name|cmd
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|cmd
argument_list|)
argument_list|)
expr_stmt|;
name|cmd
operator|->
name|argv
operator|=
name|argv
expr_stmt|;
name|cmd
operator|->
name|no_stdin
operator|=
name|opt
operator|&
name|RUN_COMMAND_NO_STDIN
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|cmd
operator|->
name|git_cmd
operator|=
name|opt
operator|&
name|RUN_GIT_CMD
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|cmd
operator|->
name|stdout_to_stderr
operator|=
name|opt
operator|&
name|RUN_COMMAND_STDOUT_TO_STDERR
condition|?
literal|1
else|:
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|run_command_v_opt
name|int
name|run_command_v_opt
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|opt
parameter_list|)
block|{
name|struct
name|child_process
name|cmd
decl_stmt|;
name|prepare_run_command_v_opt
argument_list|(
operator|&
name|cmd
argument_list|,
name|argv
argument_list|,
name|opt
argument_list|)
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|run_command_v_opt_cd
name|int
name|run_command_v_opt_cd
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
name|struct
name|child_process
name|cmd
decl_stmt|;
name|prepare_run_command_v_opt
argument_list|(
operator|&
name|cmd
argument_list|,
name|argv
argument_list|,
name|opt
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|dir
operator|=
name|dir
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|run_command_v_opt_cd_env
name|int
name|run_command_v_opt_cd_env
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|,
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|env
parameter_list|)
block|{
name|struct
name|child_process
name|cmd
decl_stmt|;
name|prepare_run_command_v_opt
argument_list|(
operator|&
name|cmd
argument_list|,
name|argv
argument_list|,
name|opt
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|dir
operator|=
name|dir
expr_stmt|;
name|cmd
operator|.
name|env
operator|=
name|env
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|start_async
name|int
name|start_async
parameter_list|(
name|struct
name|async
modifier|*
name|async
parameter_list|)
block|{
name|int
name|pipe_out
index|[
literal|2
index|]
decl_stmt|;
if|if
condition|(
name|pipe
argument_list|(
name|pipe_out
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"cannot create pipe: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
name|async
operator|->
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|async
operator|->
name|pid
operator|<
literal|0
condition|)
block|{
name|error
argument_list|(
literal|"fork (async) failed: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|close_pair
argument_list|(
name|pipe_out
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|async
operator|->
name|pid
condition|)
block|{
name|close
argument_list|(
name|pipe_out
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|exit
argument_list|(
operator|!
operator|!
name|async
operator|->
name|proc
argument_list|(
name|pipe_out
index|[
literal|1
index|]
argument_list|,
name|async
operator|->
name|data
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|async
operator|->
name|out
operator|=
name|pipe_out
index|[
literal|0
index|]
expr_stmt|;
name|close
argument_list|(
name|pipe_out
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|finish_async
name|int
name|finish_async
parameter_list|(
name|struct
name|async
modifier|*
name|async
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|wait_or_whine
argument_list|(
name|async
operator|->
name|pid
argument_list|)
condition|)
name|ret
operator|=
name|error
argument_list|(
literal|"waitpid (async) failed"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
