begin_unit
begin_comment
comment|/*  * builtin-help.c  *  * Builtin help-related commands (help, usage, version)  */
end_comment
begin_include
include|#
directive|include
file|<sys/ioctl.h>
end_include
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
file|"common-cmds.h"
end_include
begin_comment
comment|/* most GUI terminals set COLUMNS (although some don't export it) */
end_comment
begin_function
DECL|function|term_columns
specifier|static
name|int
name|term_columns
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|col_string
init|=
name|getenv
argument_list|(
literal|"COLUMNS"
argument_list|)
decl_stmt|;
name|int
name|n_cols
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|col_string
operator|&&
operator|(
name|n_cols
operator|=
name|atoi
argument_list|(
name|col_string
argument_list|)
operator|)
operator|>
literal|0
condition|)
return|return
name|n_cols
return|;
ifdef|#
directive|ifdef
name|TIOCGWINSZ
block|{
name|struct
name|winsize
name|ws
decl_stmt|;
if|if
condition|(
operator|!
name|ioctl
argument_list|(
literal|1
argument_list|,
name|TIOCGWINSZ
argument_list|,
operator|&
name|ws
argument_list|)
condition|)
block|{
if|if
condition|(
name|ws
operator|.
name|ws_col
condition|)
return|return
name|ws
operator|.
name|ws_col
return|;
block|}
block|}
endif|#
directive|endif
return|return
literal|80
return|;
block|}
end_function
begin_function
DECL|function|oom
specifier|static
name|void
name|oom
parameter_list|(
name|void
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: out of memory\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|mput_char
specifier|static
specifier|inline
name|void
name|mput_char
parameter_list|(
name|char
name|c
parameter_list|,
name|unsigned
name|int
name|num
parameter_list|)
block|{
while|while
condition|(
name|num
operator|--
condition|)
name|putchar
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|cmdname
specifier|static
struct|struct
name|cmdname
block|{
DECL|member|len
name|size_t
name|len
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
literal|1
index|]
decl_stmt|;
block|}
DECL|variable|cmdname
modifier|*
modifier|*
name|cmdname
struct|;
end_struct
begin_decl_stmt
DECL|variable|cmdname_alloc
DECL|variable|cmdname_cnt
specifier|static
name|int
name|cmdname_alloc
decl_stmt|,
name|cmdname_cnt
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_cmdname
specifier|static
name|void
name|add_cmdname
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|struct
name|cmdname
modifier|*
name|ent
decl_stmt|;
if|if
condition|(
name|cmdname_alloc
operator|<=
name|cmdname_cnt
condition|)
block|{
name|cmdname_alloc
operator|=
name|cmdname_alloc
operator|+
literal|200
expr_stmt|;
name|cmdname
operator|=
name|realloc
argument_list|(
name|cmdname
argument_list|,
name|cmdname_alloc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|cmdname
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmdname
condition|)
name|oom
argument_list|()
expr_stmt|;
block|}
name|ent
operator|=
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|ent
argument_list|)
operator|+
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ent
condition|)
name|oom
argument_list|()
expr_stmt|;
name|ent
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|memcpy
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ent
operator|->
name|name
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
name|cmdname
index|[
name|cmdname_cnt
operator|++
index|]
operator|=
name|ent
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmdname_compare
specifier|static
name|int
name|cmdname_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
name|struct
name|cmdname
modifier|*
name|a
init|=
operator|*
operator|(
expr|struct
name|cmdname
operator|*
operator|*
operator|)
name|a_
decl_stmt|;
name|struct
name|cmdname
modifier|*
name|b
init|=
operator|*
operator|(
expr|struct
name|cmdname
operator|*
operator|*
operator|)
name|b_
decl_stmt|;
return|return
name|strcmp
argument_list|(
name|a
operator|->
name|name
argument_list|,
name|b
operator|->
name|name
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|pretty_print_string_list
specifier|static
name|void
name|pretty_print_string_list
parameter_list|(
name|struct
name|cmdname
modifier|*
modifier|*
name|cmdname
parameter_list|,
name|int
name|longest
parameter_list|)
block|{
name|int
name|cols
init|=
literal|1
decl_stmt|,
name|rows
decl_stmt|;
name|int
name|space
init|=
name|longest
operator|+
literal|1
decl_stmt|;
comment|/* min 1 SP between words */
name|int
name|max_cols
init|=
name|term_columns
argument_list|()
operator|-
literal|1
decl_stmt|;
comment|/* don't print *on* the edge */
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
if|if
condition|(
name|space
operator|<
name|max_cols
condition|)
name|cols
operator|=
name|max_cols
operator|/
name|space
expr_stmt|;
name|rows
operator|=
operator|(
name|cmdname_cnt
operator|+
name|cols
operator|-
literal|1
operator|)
operator|/
name|cols
expr_stmt|;
name|qsort
argument_list|(
name|cmdname
argument_list|,
name|cmdname_cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|cmdname
argument_list|)
argument_list|,
name|cmdname_compare
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
name|rows
condition|;
name|i
operator|++
control|)
block|{
name|printf
argument_list|(
literal|"  "
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|cols
condition|;
name|j
operator|++
control|)
block|{
name|int
name|n
init|=
name|j
operator|*
name|rows
operator|+
name|i
decl_stmt|;
name|int
name|size
init|=
name|space
decl_stmt|;
if|if
condition|(
name|n
operator|>=
name|cmdname_cnt
condition|)
break|break;
if|if
condition|(
name|j
operator|==
name|cols
operator|-
literal|1
operator|||
name|n
operator|+
name|rows
operator|>=
name|cmdname_cnt
condition|)
name|size
operator|=
literal|1
expr_stmt|;
name|printf
argument_list|(
literal|"%-*s"
argument_list|,
name|size
argument_list|,
name|cmdname
index|[
name|n
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|list_commands
specifier|static
name|void
name|list_commands
parameter_list|(
specifier|const
name|char
modifier|*
name|exec_path
parameter_list|,
specifier|const
name|char
modifier|*
name|pattern
parameter_list|)
block|{
name|unsigned
name|int
name|longest
init|=
literal|0
decl_stmt|;
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|dirlen
decl_stmt|;
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|exec_path
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: '%s': %s\n"
argument_list|,
name|exec_path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|dirlen
operator|=
name|strlen
argument_list|(
name|exec_path
argument_list|)
expr_stmt|;
if|if
condition|(
name|PATH_MAX
operator|-
literal|20
operator|<
name|dirlen
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git: insanely long exec-path '%s'\n"
argument_list|,
name|exec_path
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|path
argument_list|,
name|exec_path
argument_list|,
name|dirlen
argument_list|)
expr_stmt|;
name|path
index|[
name|dirlen
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|entlen
decl_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
literal|"git-"
argument_list|,
literal|4
argument_list|)
condition|)
continue|continue;
name|strcpy
argument_list|(
name|path
operator|+
name|dirlen
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
operator|||
comment|/* stat, not lstat */
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
operator|!
operator|(
name|st
operator|.
name|st_mode
operator|&
name|S_IXUSR
operator|)
condition|)
continue|continue;
name|entlen
operator|=
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|has_extension
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
name|entlen
argument_list|,
literal|".exe"
argument_list|)
condition|)
name|entlen
operator|-=
literal|4
expr_stmt|;
if|if
condition|(
name|longest
operator|<
name|entlen
condition|)
name|longest
operator|=
name|entlen
expr_stmt|;
name|add_cmdname
argument_list|(
name|de
operator|->
name|d_name
operator|+
literal|4
argument_list|,
name|entlen
operator|-
literal|4
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"git commands available in '%s'\n"
argument_list|,
name|exec_path
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"----------------------------"
argument_list|)
expr_stmt|;
name|mput_char
argument_list|(
literal|'-'
argument_list|,
name|strlen
argument_list|(
name|exec_path
argument_list|)
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|pretty_print_string_list
argument_list|(
name|cmdname
argument_list|,
name|longest
operator|-
literal|4
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|list_common_cmds_help
specifier|static
name|void
name|list_common_cmds_help
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|longest
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|longest
operator|<
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
condition|)
name|longest
operator|=
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
literal|"The most commonly used git commands are:"
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
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|printf
argument_list|(
literal|"    %s"
argument_list|,
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|mput_char
argument_list|(
literal|' '
argument_list|,
name|longest
operator|-
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
operator|+
literal|4
argument_list|)
expr_stmt|;
name|puts
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|help
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
literal|"(use 'git help -a' to get a list of all installed git commands)"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_man_page
specifier|static
name|void
name|show_man_page
parameter_list|(
specifier|const
name|char
modifier|*
name|git_cmd
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|page
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|git_cmd
argument_list|,
literal|"git"
argument_list|,
literal|3
argument_list|)
condition|)
name|page
operator|=
name|git_cmd
expr_stmt|;
else|else
block|{
name|int
name|page_len
init|=
name|strlen
argument_list|(
name|git_cmd
argument_list|)
operator|+
literal|4
decl_stmt|;
name|char
modifier|*
name|p
init|=
name|malloc
argument_list|(
name|page_len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|strcpy
argument_list|(
name|p
argument_list|,
literal|"git-"
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|p
operator|+
literal|4
argument_list|,
name|git_cmd
argument_list|)
expr_stmt|;
name|p
index|[
name|page_len
index|]
operator|=
literal|0
expr_stmt|;
name|page
operator|=
name|p
expr_stmt|;
block|}
name|execlp
argument_list|(
literal|"man"
argument_list|,
literal|"man"
argument_list|,
name|page
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|help_unknown_cmd
name|void
name|help_unknown_cmd
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|)
block|{
name|printf
argument_list|(
literal|"git: '%s' is not a git-command\n\n"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|list_common_cmds_help
argument_list|()
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_version
name|int
name|cmd_version
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
name|printf
argument_list|(
literal|"git version %s\n"
argument_list|,
name|git_version_string
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_help
name|int
name|cmd_help
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
specifier|const
name|char
modifier|*
name|help_cmd
init|=
name|argc
operator|>
literal|1
condition|?
name|argv
index|[
literal|1
index|]
else|:
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec_path
init|=
name|git_exec_path
argument_list|()
decl_stmt|;
if|if
condition|(
operator|!
name|help_cmd
condition|)
block|{
name|printf
argument_list|(
literal|"usage: %s\n\n"
argument_list|,
name|git_usage_string
argument_list|)
expr_stmt|;
name|list_common_cmds_help
argument_list|()
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|help_cmd
argument_list|,
literal|"--all"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|help_cmd
argument_list|,
literal|"-a"
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"usage: %s\n\n"
argument_list|,
name|git_usage_string
argument_list|)
expr_stmt|;
if|if
condition|(
name|exec_path
condition|)
name|list_commands
argument_list|(
name|exec_path
argument_list|,
literal|"git-*"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
else|else
name|show_man_page
argument_list|(
name|help_cmd
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
