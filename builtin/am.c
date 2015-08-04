begin_unit
begin_comment
comment|/*  * Builtin "git am"  *  * Based on git-am.sh by Junio C Hamano.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_struct
DECL|struct|am_state
struct|struct
name|am_state
block|{
comment|/* state directory path */
DECL|member|dir
name|char
modifier|*
name|dir
decl_stmt|;
comment|/* current and last patch numbers, 1-indexed */
DECL|member|cur
name|int
name|cur
decl_stmt|;
DECL|member|last
name|int
name|last
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/**  * Initializes am_state with the default values. The state directory is set to  * dir.  */
end_comment
begin_function
DECL|function|am_state_init
specifier|static
name|void
name|am_state_init
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|)
block|{
name|memset
argument_list|(
name|state
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|state
argument_list|)
argument_list|)
expr_stmt|;
name|assert
argument_list|(
name|dir
argument_list|)
expr_stmt|;
name|state
operator|->
name|dir
operator|=
name|xstrdup
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Releases memory allocated by an am_state.  */
end_comment
begin_function
DECL|function|am_state_release
specifier|static
name|void
name|am_state_release
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
name|free
argument_list|(
name|state
operator|->
name|dir
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Returns path relative to the am_state directory.  */
end_comment
begin_function
DECL|function|am_path
specifier|static
specifier|inline
specifier|const
name|char
modifier|*
name|am_path
parameter_list|(
specifier|const
name|struct
name|am_state
modifier|*
name|state
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
return|return
name|mkpath
argument_list|(
literal|"%s/%s"
argument_list|,
name|state
operator|->
name|dir
argument_list|,
name|path
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/**  * Returns 1 if there is an am session in progress, 0 otherwise.  */
end_comment
begin_function
DECL|function|am_in_progress
specifier|static
name|int
name|am_in_progress
parameter_list|(
specifier|const
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|state
operator|->
name|dir
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
operator|||
operator|!
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|lstat
argument_list|(
name|am_path
argument_list|(
name|state
argument_list|,
literal|"last"
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|lstat
argument_list|(
name|am_path
argument_list|(
name|state
argument_list|,
literal|"next"
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/**  * Reads the contents of `file` in the `state` directory into `sb`. Returns the  * number of bytes read on success, -1 if the file does not exist. If `trim` is  * set, trailing whitespace will be removed.  */
end_comment
begin_function
DECL|function|read_state_file
specifier|static
name|int
name|read_state_file
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|struct
name|am_state
modifier|*
name|state
parameter_list|,
specifier|const
name|char
modifier|*
name|file
parameter_list|,
name|int
name|trim
parameter_list|)
block|{
name|strbuf_reset
argument_list|(
name|sb
argument_list|)
expr_stmt|;
if|if
condition|(
name|strbuf_read_file
argument_list|(
name|sb
argument_list|,
name|am_path
argument_list|(
name|state
argument_list|,
name|file
argument_list|)
argument_list|,
literal|0
argument_list|)
operator|>=
literal|0
condition|)
block|{
if|if
condition|(
name|trim
condition|)
name|strbuf_trim
argument_list|(
name|sb
argument_list|)
expr_stmt|;
return|return
name|sb
operator|->
name|len
return|;
block|}
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
return|return
operator|-
literal|1
return|;
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not read '%s'"
argument_list|)
argument_list|,
name|am_path
argument_list|(
name|state
argument_list|,
name|file
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Loads state from disk.  */
end_comment
begin_function
DECL|function|am_load
specifier|static
name|void
name|am_load
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|read_state_file
argument_list|(
operator|&
name|sb
argument_list|,
name|state
argument_list|,
literal|"next"
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"BUG: state file 'next' does not exist"
argument_list|)
expr_stmt|;
name|state
operator|->
name|cur
operator|=
name|strtol
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_state_file
argument_list|(
operator|&
name|sb
argument_list|,
name|state
argument_list|,
literal|"last"
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"BUG: state file 'last' does not exist"
argument_list|)
expr_stmt|;
name|state
operator|->
name|last
operator|=
name|strtol
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Removes the am_state directory, forcefully terminating the current am  * session.  */
end_comment
begin_function
DECL|function|am_destroy
specifier|static
name|void
name|am_destroy
parameter_list|(
specifier|const
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|state
operator|->
name|dir
argument_list|)
expr_stmt|;
name|remove_dir_recursively
argument_list|(
operator|&
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Setup a new am session for applying patches  */
end_comment
begin_function
DECL|function|am_setup
specifier|static
name|void
name|am_setup
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
if|if
condition|(
name|mkdir
argument_list|(
name|state
operator|->
name|dir
argument_list|,
literal|0777
argument_list|)
operator|<
literal|0
operator|&&
name|errno
operator|!=
name|EEXIST
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"failed to create directory '%s'"
argument_list|)
argument_list|,
name|state
operator|->
name|dir
argument_list|)
expr_stmt|;
comment|/* 	 * NOTE: Since the "next" and "last" files determine if an am_state 	 * session is in progress, they should be written last. 	 */
name|write_file
argument_list|(
name|am_path
argument_list|(
name|state
argument_list|,
literal|"next"
argument_list|)
argument_list|,
literal|1
argument_list|,
literal|"%d"
argument_list|,
name|state
operator|->
name|cur
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|am_path
argument_list|(
name|state
argument_list|,
literal|"last"
argument_list|)
argument_list|,
literal|1
argument_list|,
literal|"%d"
argument_list|,
name|state
operator|->
name|last
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Increments the patch pointer, and cleans am_state for the application of the  * next patch.  */
end_comment
begin_function
DECL|function|am_next
specifier|static
name|void
name|am_next
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
name|state
operator|->
name|cur
operator|++
expr_stmt|;
name|write_file
argument_list|(
name|am_path
argument_list|(
name|state
argument_list|,
literal|"next"
argument_list|)
argument_list|,
literal|1
argument_list|,
literal|"%d"
argument_list|,
name|state
operator|->
name|cur
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Applies all queued mail.  */
end_comment
begin_function
DECL|function|am_run
specifier|static
name|void
name|am_run
parameter_list|(
name|struct
name|am_state
modifier|*
name|state
parameter_list|)
block|{
while|while
condition|(
name|state
operator|->
name|cur
operator|<=
name|state
operator|->
name|last
condition|)
block|{
comment|/* NEEDSWORK: Patch application not implemented yet */
name|am_next
argument_list|(
name|state
argument_list|)
expr_stmt|;
block|}
name|am_destroy
argument_list|(
name|state
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_am
name|int
name|cmd_am
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
name|struct
name|am_state
name|state
decl_stmt|;
specifier|const
name|char
modifier|*
specifier|const
name|usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git am [options] [(<mbox>|<Maildir>)...]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_END
argument_list|()
block|}
decl_stmt|;
comment|/* 	 * NEEDSWORK: Once all the features of git-am.sh have been 	 * re-implemented in builtin/am.c, this preamble can be removed. 	 */
if|if
condition|(
operator|!
name|getenv
argument_list|(
literal|"_GIT_USE_BUILTIN_AM"
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
name|mkpath
argument_list|(
literal|"%s/git-am"
argument_list|,
name|git_exec_path
argument_list|()
argument_list|)
decl_stmt|;
if|if
condition|(
name|sane_execvp
argument_list|(
name|path
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
name|argv
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"could not exec %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|prefix
operator|=
name|setup_git_directory
argument_list|()
expr_stmt|;
name|trace_repo_setup
argument_list|(
name|prefix
argument_list|)
expr_stmt|;
name|setup_work_tree
argument_list|()
expr_stmt|;
block|}
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|am_state_init
argument_list|(
operator|&
name|state
argument_list|,
name|git_path
argument_list|(
literal|"rebase-apply"
argument_list|)
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|am_in_progress
argument_list|(
operator|&
name|state
argument_list|)
condition|)
name|am_load
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
else|else
name|am_setup
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
name|am_run
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
name|am_state_release
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
