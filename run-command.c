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
begin_ifndef
ifndef|#
directive|ifndef
name|WIN32
end_ifndef
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
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|prepare_shell_cmd
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|prepare_shell_cmd
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|argc
decl_stmt|,
name|nargc
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|nargv
decl_stmt|;
for|for
control|(
name|argc
operator|=
literal|0
init|;
name|argv
index|[
name|argc
index|]
condition|;
name|argc
operator|++
control|)
empty_stmt|;
comment|/* just counting */
comment|/* +1 for NULL, +3 for "sh -c" plus extra $0 */
name|nargv
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|nargv
argument_list|)
operator|*
operator|(
name|argc
operator|+
literal|1
operator|+
literal|3
operator|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|1
condition|)
name|die
argument_list|(
literal|"BUG: shell command is empty"
argument_list|)
expr_stmt|;
if|if
condition|(
name|strcspn
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"|&;<>()$`\\\"' \t\n*?[#~=%"
argument_list|)
operator|!=
name|strlen
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
condition|)
block|{
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
literal|"sh"
expr_stmt|;
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
literal|"-c"
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
else|else
block|{
name|struct
name|strbuf
name|arg0
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|arg0
argument_list|,
literal|"%s \"$@\""
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|arg0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
for|for
control|(
name|argc
operator|=
literal|0
init|;
name|argv
index|[
name|argc
index|]
condition|;
name|argc
operator|++
control|)
name|nargv
index|[
name|nargc
operator|++
index|]
operator|=
name|argv
index|[
name|argc
index|]
expr_stmt|;
name|nargv
index|[
name|nargc
index|]
operator|=
name|NULL
expr_stmt|;
return|return
name|nargv
return|;
block|}
end_function
begin_ifndef
ifndef|#
directive|ifndef
name|WIN32
end_ifndef
begin_function
DECL|function|execv_shell_cmd
specifier|static
name|int
name|execv_shell_cmd
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|nargv
init|=
name|prepare_shell_cmd
argument_list|(
name|argv
argument_list|)
decl_stmt|;
name|trace_argv_printf
argument_list|(
name|nargv
argument_list|,
literal|"trace: exec:"
argument_list|)
expr_stmt|;
name|execvp
argument_list|(
name|nargv
index|[
literal|0
index|]
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
name|nargv
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|nargv
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
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
name|int
name|failed_errno
init|=
name|failed_errno
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
name|failed_errno
operator|=
name|errno
expr_stmt|;
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
goto|goto
name|fail_pipe
goto|;
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
name|failed_errno
operator|=
name|errno
expr_stmt|;
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
goto|goto
name|fail_pipe
goto|;
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
name|failed_errno
operator|=
name|errno
expr_stmt|;
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
name|fail_pipe
label|:
name|error
argument_list|(
literal|"cannot create pipe for %s: %s"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
name|strerror
argument_list|(
name|failed_errno
argument_list|)
argument_list|)
expr_stmt|;
name|errno
operator|=
name|failed_errno
expr_stmt|;
return|return
operator|-
literal|1
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
name|trace_argv_printf
argument_list|(
name|cmd
operator|->
name|argv
argument_list|,
literal|"trace: run_command:"
argument_list|)
expr_stmt|;
ifndef|#
directive|ifndef
name|WIN32
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
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
name|die_errno
argument_list|(
literal|"exec '%s': cd to '%s' failed"
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
name|preexec_cb
condition|)
name|cmd
operator|->
name|preexec_cb
argument_list|()
expr_stmt|;
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
elseif|else
if|if
condition|(
name|cmd
operator|->
name|use_shell
condition|)
block|{
name|execv_shell_cmd
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
name|trace_printf
argument_list|(
literal|"trace: exec '%s' failed: %s\n"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|127
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|cmd
operator|->
name|pid
operator|<
literal|0
condition|)
name|error
argument_list|(
literal|"cannot fork() for %s: %s"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
argument_list|,
name|strerror
argument_list|(
name|failed_errno
operator|=
name|errno
argument_list|)
argument_list|)
expr_stmt|;
else|#
directive|else
block|{
name|int
name|fhin
init|=
literal|0
decl_stmt|,
name|fhout
init|=
literal|1
decl_stmt|,
name|fherr
init|=
literal|2
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|sargv
init|=
name|cmd
operator|->
name|argv
decl_stmt|;
name|char
modifier|*
modifier|*
name|env
init|=
name|environ
decl_stmt|;
if|if
condition|(
name|cmd
operator|->
name|no_stdin
condition|)
name|fhin
operator|=
name|open
argument_list|(
literal|"/dev/null"
argument_list|,
name|O_RDWR
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_in
condition|)
name|fhin
operator|=
name|dup
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
name|fhin
operator|=
name|dup
argument_list|(
name|cmd
operator|->
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|no_stderr
condition|)
name|fherr
operator|=
name|open
argument_list|(
literal|"/dev/null"
argument_list|,
name|O_RDWR
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_err
condition|)
name|fherr
operator|=
name|dup
argument_list|(
name|fderr
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|no_stdout
condition|)
name|fhout
operator|=
name|open
argument_list|(
literal|"/dev/null"
argument_list|,
name|O_RDWR
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|cmd
operator|->
name|stdout_to_stderr
condition|)
name|fhout
operator|=
name|dup
argument_list|(
name|fherr
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|need_out
condition|)
name|fhout
operator|=
name|dup
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
operator|>
literal|1
condition|)
name|fhout
operator|=
name|dup
argument_list|(
name|cmd
operator|->
name|out
argument_list|)
expr_stmt|;
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
name|env
operator|=
name|make_augmented_environ
argument_list|(
name|cmd
operator|->
name|env
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|git_cmd
condition|)
block|{
name|cmd
operator|->
name|argv
operator|=
name|prepare_git_cmd
argument_list|(
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmd
operator|->
name|use_shell
condition|)
block|{
name|cmd
operator|->
name|argv
operator|=
name|prepare_shell_cmd
argument_list|(
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
block|}
name|cmd
operator|->
name|pid
operator|=
name|mingw_spawnvpe
argument_list|(
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
name|env
argument_list|,
name|fhin
argument_list|,
name|fhout
argument_list|,
name|fherr
argument_list|)
expr_stmt|;
name|failed_errno
operator|=
name|errno
expr_stmt|;
if|if
condition|(
name|cmd
operator|->
name|pid
operator|<
literal|0
operator|&&
operator|(
operator|!
name|cmd
operator|->
name|silent_exec_failure
operator|||
name|errno
operator|!=
name|ENOENT
operator|)
condition|)
name|error
argument_list|(
literal|"cannot spawn %s: %s"
argument_list|,
name|cmd
operator|->
name|argv
index|[
literal|0
index|]
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
name|free
argument_list|(
name|cmd
operator|->
name|argv
argument_list|)
expr_stmt|;
name|cmd
operator|->
name|argv
operator|=
name|sargv
expr_stmt|;
if|if
condition|(
name|fhin
operator|!=
literal|0
condition|)
name|close
argument_list|(
name|fhin
argument_list|)
expr_stmt|;
if|if
condition|(
name|fhout
operator|!=
literal|1
condition|)
name|close
argument_list|(
name|fhout
argument_list|)
expr_stmt|;
if|if
condition|(
name|fherr
operator|!=
literal|2
condition|)
name|close
argument_list|(
name|fherr
argument_list|)
expr_stmt|;
block|}
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
name|errno
operator|=
name|failed_errno
expr_stmt|;
return|return
operator|-
literal|1
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
parameter_list|,
specifier|const
name|char
modifier|*
name|argv0
parameter_list|,
name|int
name|silent_exec_failure
parameter_list|)
block|{
name|int
name|status
decl_stmt|,
name|code
init|=
operator|-
literal|1
decl_stmt|;
name|pid_t
name|waiting
decl_stmt|;
name|int
name|failed_errno
init|=
literal|0
decl_stmt|;
while|while
condition|(
operator|(
name|waiting
operator|=
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
operator|)
operator|<
literal|0
operator|&&
name|errno
operator|==
name|EINTR
condition|)
empty_stmt|;
comment|/* nothing */
if|if
condition|(
name|waiting
operator|<
literal|0
condition|)
block|{
name|failed_errno
operator|=
name|errno
expr_stmt|;
name|error
argument_list|(
literal|"waitpid for %s failed: %s"
argument_list|,
name|argv0
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|waiting
operator|!=
name|pid
condition|)
block|{
name|error
argument_list|(
literal|"waitpid is confused (%s)"
argument_list|,
name|argv0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|WIFSIGNALED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|code
operator|=
name|WTERMSIG
argument_list|(
name|status
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s died of signal %d"
argument_list|,
name|argv0
argument_list|,
name|code
argument_list|)
expr_stmt|;
comment|/* 		 * This return value is chosen so that code& 0xff 		 * mimics the exit code that a POSIX shell would report for 		 * a program that died from this signal. 		 */
name|code
operator|-=
literal|128
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|code
operator|=
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
expr_stmt|;
comment|/* 		 * Convert special exit code when execvp failed. 		 */
if|if
condition|(
name|code
operator|==
literal|127
condition|)
block|{
name|code
operator|=
operator|-
literal|1
expr_stmt|;
name|failed_errno
operator|=
name|ENOENT
expr_stmt|;
if|if
condition|(
operator|!
name|silent_exec_failure
condition|)
name|error
argument_list|(
literal|"cannot run %s: %s"
argument_list|,
name|argv0
argument_list|,
name|strerror
argument_list|(
name|ENOENT
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|error
argument_list|(
literal|"waitpid is confused (%s)"
argument_list|,
name|argv0
argument_list|)
expr_stmt|;
block|}
name|errno
operator|=
name|failed_errno
expr_stmt|;
return|return
name|code
return|;
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
name|silent_exec_failure
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
name|cmd
operator|->
name|silent_exec_failure
operator|=
name|opt
operator|&
name|RUN_SILENT_EXEC_FAILURE
condition|?
literal|1
else|:
literal|0
expr_stmt|;
name|cmd
operator|->
name|use_shell
operator|=
name|opt
operator|&
name|RUN_USING_SHELL
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
begin_ifdef
ifdef|#
directive|ifdef
name|WIN32
end_ifdef
begin_function
DECL|function|run_thread
specifier|static
name|unsigned
name|__stdcall
name|run_thread
parameter_list|(
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|async
modifier|*
name|async
init|=
name|data
decl_stmt|;
return|return
name|async
operator|->
name|proc
argument_list|(
name|async
operator|->
name|fd_for_proc
argument_list|,
name|async
operator|->
name|data
argument_list|)
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
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
name|out
operator|=
name|pipe_out
index|[
literal|0
index|]
expr_stmt|;
ifndef|#
directive|ifndef
name|WIN32
comment|/* Flush stdio before fork() to avoid cloning buffers */
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
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
name|close
argument_list|(
name|pipe_out
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
else|#
directive|else
name|async
operator|->
name|fd_for_proc
operator|=
name|pipe_out
index|[
literal|1
index|]
expr_stmt|;
name|async
operator|->
name|tid
operator|=
operator|(
name|HANDLE
operator|)
name|_beginthreadex
argument_list|(
name|NULL
argument_list|,
literal|0
argument_list|,
name|run_thread
argument_list|,
name|async
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|async
operator|->
name|tid
condition|)
block|{
name|error
argument_list|(
literal|"cannot create thread: %s"
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
endif|#
directive|endif
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
ifndef|#
directive|ifndef
name|WIN32
name|int
name|ret
init|=
name|wait_or_whine
argument_list|(
name|async
operator|->
name|pid
argument_list|,
literal|"child process"
argument_list|,
literal|0
argument_list|)
decl_stmt|;
else|#
directive|else
name|DWORD
name|ret
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|WaitForSingleObject
argument_list|(
name|async
operator|->
name|tid
argument_list|,
name|INFINITE
argument_list|)
operator|!=
name|WAIT_OBJECT_0
condition|)
name|ret
operator|=
name|error
argument_list|(
literal|"waiting for thread failed: %lu"
argument_list|,
name|GetLastError
argument_list|()
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|GetExitCodeThread
argument_list|(
name|async
operator|->
name|tid
argument_list|,
operator|&
name|ret
argument_list|)
condition|)
name|ret
operator|=
name|error
argument_list|(
literal|"cannot get thread exit code: %lu"
argument_list|,
name|GetLastError
argument_list|()
argument_list|)
expr_stmt|;
name|CloseHandle
argument_list|(
name|async
operator|->
name|tid
argument_list|)
expr_stmt|;
endif|#
directive|endif
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|run_hook
name|int
name|run_hook
parameter_list|(
specifier|const
name|char
modifier|*
name|index_file
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
modifier|...
parameter_list|)
block|{
name|struct
name|child_process
name|hook
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|argv
init|=
name|NULL
decl_stmt|,
modifier|*
name|env
index|[
literal|2
index|]
decl_stmt|;
name|char
name|index
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|size_t
name|i
init|=
literal|0
decl_stmt|,
name|alloc
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|access
argument_list|(
name|git_path
argument_list|(
literal|"hooks/%s"
argument_list|,
name|name
argument_list|)
argument_list|,
name|X_OK
argument_list|)
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|va_start
argument_list|(
name|args
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|argv
argument_list|,
name|i
operator|+
literal|1
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
name|argv
index|[
name|i
operator|++
index|]
operator|=
name|git_path
argument_list|(
literal|"hooks/%s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
while|while
condition|(
name|argv
index|[
name|i
operator|-
literal|1
index|]
condition|)
block|{
name|ALLOC_GROW
argument_list|(
name|argv
argument_list|,
name|i
operator|+
literal|1
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
name|argv
index|[
name|i
operator|++
index|]
operator|=
name|va_arg
argument_list|(
name|args
argument_list|,
specifier|const
name|char
operator|*
argument_list|)
expr_stmt|;
block|}
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|hook
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|hook
argument_list|)
argument_list|)
expr_stmt|;
name|hook
operator|.
name|argv
operator|=
name|argv
expr_stmt|;
name|hook
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|hook
operator|.
name|stdout_to_stderr
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|index_file
condition|)
block|{
name|snprintf
argument_list|(
name|index
argument_list|,
sizeof|sizeof
argument_list|(
name|index
argument_list|)
argument_list|,
literal|"GIT_INDEX_FILE=%s"
argument_list|,
name|index_file
argument_list|)
expr_stmt|;
name|env
index|[
literal|0
index|]
operator|=
name|index
expr_stmt|;
name|env
index|[
literal|1
index|]
operator|=
name|NULL
expr_stmt|;
name|hook
operator|.
name|env
operator|=
name|env
expr_stmt|;
block|}
name|ret
operator|=
name|run_command
argument_list|(
operator|&
name|hook
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|argv
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
