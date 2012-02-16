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
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"prompt.h"
end_include
begin_include
include|#
directive|include
file|"compat/terminal.h"
end_include
begin_function
DECL|function|do_askpass
specifier|static
name|char
modifier|*
name|do_askpass
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
name|prompt
parameter_list|)
block|{
name|struct
name|child_process
name|pass
decl_stmt|;
specifier|const
name|char
modifier|*
name|args
index|[
literal|3
index|]
decl_stmt|;
specifier|static
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|err
init|=
literal|0
decl_stmt|;
name|args
index|[
literal|0
index|]
operator|=
name|cmd
expr_stmt|;
name|args
index|[
literal|1
index|]
operator|=
name|prompt
expr_stmt|;
name|args
index|[
literal|2
index|]
operator|=
name|NULL
expr_stmt|;
name|memset
argument_list|(
operator|&
name|pass
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|pass
argument_list|)
argument_list|)
expr_stmt|;
name|pass
operator|.
name|argv
operator|=
name|args
expr_stmt|;
name|pass
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|pass
argument_list|)
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buffer
argument_list|,
name|pass
operator|.
name|out
argument_list|,
literal|20
argument_list|)
operator|<
literal|0
condition|)
name|err
operator|=
literal|1
expr_stmt|;
name|close
argument_list|(
name|pass
operator|.
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|pass
argument_list|)
condition|)
name|err
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
name|error
argument_list|(
literal|"unable to read askpass response from '%s'"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|strbuf_setlen
argument_list|(
operator|&
name|buffer
argument_list|,
name|strcspn
argument_list|(
name|buffer
operator|.
name|buf
argument_list|,
literal|"\r\n"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|strbuf_detach
argument_list|(
operator|&
name|buffer
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_prompt
name|char
modifier|*
name|git_prompt
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|char
modifier|*
name|r
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|flags
operator|&
name|PROMPT_ASKPASS
condition|)
block|{
specifier|const
name|char
modifier|*
name|askpass
decl_stmt|;
name|askpass
operator|=
name|getenv
argument_list|(
literal|"GIT_ASKPASS"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|askpass
condition|)
name|askpass
operator|=
name|askpass_program
expr_stmt|;
if|if
condition|(
operator|!
name|askpass
condition|)
name|askpass
operator|=
name|getenv
argument_list|(
literal|"SSH_ASKPASS"
argument_list|)
expr_stmt|;
if|if
condition|(
name|askpass
operator|&&
operator|*
name|askpass
condition|)
name|r
operator|=
name|do_askpass
argument_list|(
name|askpass
argument_list|,
name|prompt
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|r
condition|)
name|r
operator|=
name|git_terminal_prompt
argument_list|(
name|prompt
argument_list|,
name|flags
operator|&
name|PROMPT_ECHO
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|r
condition|)
block|{
comment|/* prompts already contain ": " at the end */
name|die
argument_list|(
literal|"could not read %s%s"
argument_list|,
name|prompt
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
name|r
return|;
block|}
end_function
begin_function
DECL|function|git_getpass
name|char
modifier|*
name|git_getpass
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|)
block|{
return|return
name|git_prompt
argument_list|(
name|prompt
argument_list|,
name|PROMPT_ASKPASS
argument_list|)
return|;
block|}
end_function
end_unit
