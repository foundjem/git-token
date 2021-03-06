begin_unit
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
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_include
include|#
directive|include
file|"branch.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"worktree.h"
end_include
begin_decl_stmt
DECL|variable|worktree_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|worktree_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git worktree add [<options>]<path> [<branch>]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git worktree list [<options>]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git worktree prune [<options>]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|add_opts
struct|struct
name|add_opts
block|{
DECL|member|force
name|int
name|force
decl_stmt|;
DECL|member|detach
name|int
name|detach
decl_stmt|;
DECL|member|checkout
name|int
name|checkout
decl_stmt|;
DECL|member|new_branch
specifier|const
name|char
modifier|*
name|new_branch
decl_stmt|;
DECL|member|force_new_branch
name|int
name|force_new_branch
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|show_only
specifier|static
name|int
name|show_only
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
specifier|static
name|int
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|expire
specifier|static
name|unsigned
name|long
name|expire
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|prune_worktree
specifier|static
name|int
name|prune_worktree
parameter_list|(
specifier|const
name|char
modifier|*
name|id
parameter_list|,
name|struct
name|strbuf
modifier|*
name|reason
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|char
modifier|*
name|path
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|is_directory
argument_list|(
name|git_path
argument_list|(
literal|"worktrees/%s"
argument_list|,
name|id
argument_list|)
argument_list|)
condition|)
block|{
name|strbuf_addf
argument_list|(
name|reason
argument_list|,
name|_
argument_list|(
literal|"Removing worktrees/%s: not a valid directory"
argument_list|)
argument_list|,
name|id
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|file_exists
argument_list|(
name|git_path
argument_list|(
literal|"worktrees/%s/locked"
argument_list|,
name|id
argument_list|)
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|stat
argument_list|(
name|git_path
argument_list|(
literal|"worktrees/%s/gitdir"
argument_list|,
name|id
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|strbuf_addf
argument_list|(
name|reason
argument_list|,
name|_
argument_list|(
literal|"Removing worktrees/%s: gitdir file does not exist"
argument_list|)
argument_list|,
name|id
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|fd
operator|=
name|open
argument_list|(
name|git_path
argument_list|(
literal|"worktrees/%s/gitdir"
argument_list|,
name|id
argument_list|)
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|strbuf_addf
argument_list|(
name|reason
argument_list|,
name|_
argument_list|(
literal|"Removing worktrees/%s: unable to read gitdir file (%s)"
argument_list|)
argument_list|,
name|id
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|len
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|path
operator|=
name|xmallocz
argument_list|(
name|len
argument_list|)
expr_stmt|;
name|read_in_full
argument_list|(
name|fd
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
while|while
condition|(
name|len
operator|&&
operator|(
name|path
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
operator|||
name|path
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\r'
operator|)
condition|)
name|len
operator|--
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
block|{
name|strbuf_addf
argument_list|(
name|reason
argument_list|,
name|_
argument_list|(
literal|"Removing worktrees/%s: invalid gitdir file"
argument_list|)
argument_list|,
name|id
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|path
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
operator|!
name|file_exists
argument_list|(
name|path
argument_list|)
condition|)
block|{
name|struct
name|stat
name|st_link
decl_stmt|;
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
comment|/* 		 * the repo is moved manually and has not been 		 * accessed since? 		 */
if|if
condition|(
operator|!
name|stat
argument_list|(
name|git_path
argument_list|(
literal|"worktrees/%s/link"
argument_list|,
name|id
argument_list|)
argument_list|,
operator|&
name|st_link
argument_list|)
operator|&&
name|st_link
operator|.
name|st_nlink
operator|>
literal|1
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|st
operator|.
name|st_mtime
operator|<=
name|expire
condition|)
block|{
name|strbuf_addf
argument_list|(
name|reason
argument_list|,
name|_
argument_list|(
literal|"Removing worktrees/%s: gitdir file points to non-existent location"
argument_list|)
argument_list|,
name|id
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
else|else
block|{
return|return
literal|0
return|;
block|}
block|}
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|prune_worktrees
specifier|static
name|void
name|prune_worktrees
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|reason
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|git_path
argument_list|(
literal|"worktrees"
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|d
decl_stmt|;
name|int
name|ret
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return;
while|while
condition|(
operator|(
name|d
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
if|if
condition|(
name|is_dot_or_dotdot
argument_list|(
name|d
operator|->
name|d_name
argument_list|)
condition|)
continue|continue;
name|strbuf_reset
argument_list|(
operator|&
name|reason
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|prune_worktree
argument_list|(
name|d
operator|->
name|d_name
argument_list|,
operator|&
name|reason
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|show_only
operator|||
name|verbose
condition|)
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|reason
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_only
condition|)
continue|continue;
name|strbuf_reset
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|path
argument_list|,
name|git_path
argument_list|(
literal|"worktrees/%s"
argument_list|,
name|d
operator|->
name|d_name
argument_list|)
argument_list|)
expr_stmt|;
name|ret
operator|=
name|remove_dir_recursively
argument_list|(
operator|&
name|path
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
operator|&&
name|errno
operator|==
name|ENOTDIR
condition|)
name|ret
operator|=
name|unlink
argument_list|(
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
name|error_errno
argument_list|(
name|_
argument_list|(
literal|"failed to remove '%s'"
argument_list|)
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
condition|)
name|rmdir
argument_list|(
name|git_path
argument_list|(
literal|"worktrees"
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|reason
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|prune
specifier|static
name|int
name|prune
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|,
name|N_
argument_list|(
literal|"do not remove, show only"
argument_list|)
argument_list|)
block|,
name|OPT__VERBOSE
argument_list|(
operator|&
name|verbose
argument_list|,
name|N_
argument_list|(
literal|"report pruned objects"
argument_list|)
argument_list|)
block|,
name|OPT_EXPIRY_DATE
argument_list|(
literal|0
argument_list|,
literal|"expire"
argument_list|,
operator|&
name|expire
argument_list|,
name|N_
argument_list|(
literal|"expire objects older than<time>"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|expire
operator|=
name|ULONG_MAX
expr_stmt|;
name|ac
operator|=
name|parse_options
argument_list|(
name|ac
argument_list|,
name|av
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|worktree_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ac
condition|)
name|usage_with_options
argument_list|(
name|worktree_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|prune_worktrees
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|junk_work_tree
specifier|static
name|char
modifier|*
name|junk_work_tree
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|junk_git_dir
specifier|static
name|char
modifier|*
name|junk_git_dir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|is_junk
specifier|static
name|int
name|is_junk
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|junk_pid
specifier|static
name|pid_t
name|junk_pid
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|remove_junk
specifier|static
name|void
name|remove_junk
parameter_list|(
name|void
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
operator|!
name|is_junk
operator|||
name|getpid
argument_list|()
operator|!=
name|junk_pid
condition|)
return|return;
if|if
condition|(
name|junk_git_dir
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|junk_git_dir
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
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|junk_work_tree
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|junk_work_tree
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
block|}
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_junk_on_signal
specifier|static
name|void
name|remove_junk_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_junk
argument_list|()
expr_stmt|;
name|sigchain_pop
argument_list|(
name|signo
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|signo
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|worktree_basename
specifier|static
specifier|const
name|char
modifier|*
name|worktree_basename
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
modifier|*
name|olen
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|int
name|len
decl_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
while|while
condition|(
name|len
operator|&&
name|is_dir_sep
argument_list|(
name|path
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
for|for
control|(
name|name
operator|=
name|path
operator|+
name|len
operator|-
literal|1
init|;
name|name
operator|>
name|path
condition|;
name|name
operator|--
control|)
if|if
condition|(
name|is_dir_sep
argument_list|(
operator|*
name|name
argument_list|)
condition|)
block|{
name|name
operator|++
expr_stmt|;
break|break;
block|}
operator|*
name|olen
operator|=
name|len
expr_stmt|;
return|return
name|name
return|;
block|}
end_function
begin_function
DECL|function|add_worktree
specifier|static
name|int
name|add_worktree
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|struct
name|add_opts
modifier|*
name|opts
parameter_list|)
block|{
name|struct
name|strbuf
name|sb_git
init|=
name|STRBUF_INIT
decl_stmt|,
name|sb_repo
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|struct
name|child_process
name|cp
decl_stmt|;
name|struct
name|argv_array
name|child_env
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
name|int
name|counter
init|=
literal|0
decl_stmt|,
name|len
decl_stmt|,
name|ret
decl_stmt|;
name|struct
name|strbuf
name|symref
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|file_exists
argument_list|(
name|path
argument_list|)
operator|&&
operator|!
name|is_empty_dir
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"'%s' already exists"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
comment|/* is 'refname' a branch or commit? */
if|if
condition|(
operator|!
name|opts
operator|->
name|detach
operator|&&
operator|!
name|strbuf_check_branch_ref
argument_list|(
operator|&
name|symref
argument_list|,
name|refname
argument_list|)
operator|&&
name|ref_exists
argument_list|(
name|symref
operator|.
name|buf
argument_list|)
condition|)
block|{
comment|/* it's a branch */
if|if
condition|(
operator|!
name|opts
operator|->
name|force
condition|)
name|die_if_checked_out
argument_list|(
name|symref
operator|.
name|buf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* must be a commit */
name|commit
operator|=
name|lookup_commit_reference_by_name
argument_list|(
name|refname
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"invalid reference: %s"
argument_list|)
argument_list|,
name|refname
argument_list|)
expr_stmt|;
block|}
name|name
operator|=
name|worktree_basename
argument_list|(
name|path
argument_list|,
operator|&
name|len
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|sb_repo
argument_list|,
name|git_path
argument_list|(
literal|"worktrees/%.*s"
argument_list|,
call|(
name|int
call|)
argument_list|(
name|path
operator|+
name|len
operator|-
name|name
argument_list|)
argument_list|,
name|name
argument_list|)
argument_list|)
expr_stmt|;
name|len
operator|=
name|sb_repo
operator|.
name|len
expr_stmt|;
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|sb_repo
operator|.
name|buf
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not create leading directories of '%s'"
argument_list|)
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
while|while
condition|(
operator|!
name|stat
argument_list|(
name|sb_repo
operator|.
name|buf
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
name|counter
operator|++
expr_stmt|;
name|strbuf_setlen
argument_list|(
operator|&
name|sb_repo
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb_repo
argument_list|,
literal|"%d"
argument_list|,
name|counter
argument_list|)
expr_stmt|;
block|}
name|name
operator|=
name|strrchr
argument_list|(
name|sb_repo
operator|.
name|buf
argument_list|,
literal|'/'
argument_list|)
operator|+
literal|1
expr_stmt|;
name|junk_pid
operator|=
name|getpid
argument_list|()
expr_stmt|;
name|atexit
argument_list|(
name|remove_junk
argument_list|)
expr_stmt|;
name|sigchain_push_common
argument_list|(
name|remove_junk_on_signal
argument_list|)
expr_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
name|sb_repo
operator|.
name|buf
argument_list|,
literal|0777
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not create directory of '%s'"
argument_list|)
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|junk_git_dir
operator|=
name|xstrdup
argument_list|(
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|is_junk
operator|=
literal|1
expr_stmt|;
comment|/* 	 * lock the incomplete repo so prune won't delete it, unlock 	 * after the preparation is over. 	 */
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/locked"
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
literal|"initializing"
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb_git
argument_list|,
literal|"%s/.git"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|sb_git
operator|.
name|buf
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"could not create leading directories of '%s'"
argument_list|)
argument_list|,
name|sb_git
operator|.
name|buf
argument_list|)
expr_stmt|;
name|junk_work_tree
operator|=
name|xstrdup
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/gitdir"
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
literal|"%s"
argument_list|,
name|real_path
argument_list|(
name|sb_git
operator|.
name|buf
argument_list|)
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|sb_git
operator|.
name|buf
argument_list|,
literal|"gitdir: %s/worktrees/%s"
argument_list|,
name|real_path
argument_list|(
name|get_git_common_dir
argument_list|()
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
comment|/* 	 * This is to keep resolve_ref() happy. We need a valid HEAD 	 * or is_git_directory() will reject the directory. Any value which 	 * looks like an object ID will do since it will be immediately 	 * replaced by the symbolic-ref or update-ref invocation in the new 	 * worktree. 	 */
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/HEAD"
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|sha1_to_hex
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/commondir"
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|write_file
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
literal|"../.."
argument_list|)
expr_stmt|;
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"Preparing %s (identifier %s)"
argument_list|)
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|argv_array_pushf
argument_list|(
operator|&
name|child_env
argument_list|,
literal|"%s=%s"
argument_list|,
name|GIT_DIR_ENVIRONMENT
argument_list|,
name|sb_git
operator|.
name|buf
argument_list|)
expr_stmt|;
name|argv_array_pushf
argument_list|(
operator|&
name|child_env
argument_list|,
literal|"%s=%s"
argument_list|,
name|GIT_WORK_TREE_ENVIRONMENT
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|cp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cp
argument_list|)
argument_list|)
expr_stmt|;
name|cp
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|commit
condition|)
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"update-ref"
argument_list|,
literal|"HEAD"
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|commit
operator|->
name|object
operator|.
name|oid
argument_list|)
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
else|else
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"symbolic-ref"
argument_list|,
literal|"HEAD"
argument_list|,
name|symref
operator|.
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|cp
operator|.
name|env
operator|=
name|child_env
operator|.
name|argv
expr_stmt|;
name|ret
operator|=
name|run_command
argument_list|(
operator|&
name|cp
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
goto|goto
name|done
goto|;
if|if
condition|(
name|opts
operator|->
name|checkout
condition|)
block|{
name|cp
operator|.
name|argv
operator|=
name|NULL
expr_stmt|;
name|argv_array_clear
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"reset"
argument_list|,
literal|"--hard"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|cp
operator|.
name|env
operator|=
name|child_env
operator|.
name|argv
expr_stmt|;
name|ret
operator|=
name|run_command
argument_list|(
operator|&
name|cp
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
goto|goto
name|done
goto|;
block|}
name|is_junk
operator|=
literal|0
expr_stmt|;
name|free
argument_list|(
name|junk_work_tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|junk_git_dir
argument_list|)
expr_stmt|;
name|junk_work_tree
operator|=
name|NULL
expr_stmt|;
name|junk_git_dir
operator|=
name|NULL
expr_stmt|;
name|done
label|:
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/locked"
argument_list|,
name|sb_repo
operator|.
name|buf
argument_list|)
expr_stmt|;
name|unlink_or_warn
argument_list|(
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|argv_array_clear
argument_list|(
operator|&
name|child_env
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|symref
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb_repo
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb_git
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|add
specifier|static
name|int
name|add
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|struct
name|add_opts
name|opts
decl_stmt|;
specifier|const
name|char
modifier|*
name|new_branch_force
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|,
modifier|*
name|branch
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__FORCE
argument_list|(
operator|&
name|opts
operator|.
name|force
argument_list|,
name|N_
argument_list|(
literal|"checkout<branch> even if already checked out in other worktree"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'b'
argument_list|,
name|NULL
argument_list|,
operator|&
name|opts
operator|.
name|new_branch
argument_list|,
name|N_
argument_list|(
literal|"branch"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"create a new branch"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|'B'
argument_list|,
name|NULL
argument_list|,
operator|&
name|new_branch_force
argument_list|,
name|N_
argument_list|(
literal|"branch"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"create or reset a branch"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"detach"
argument_list|,
operator|&
name|opts
operator|.
name|detach
argument_list|,
name|N_
argument_list|(
literal|"detach HEAD at named commit"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"checkout"
argument_list|,
operator|&
name|opts
operator|.
name|checkout
argument_list|,
name|N_
argument_list|(
literal|"populate the new working tree"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|opts
operator|.
name|checkout
operator|=
literal|1
expr_stmt|;
name|ac
operator|=
name|parse_options
argument_list|(
name|ac
argument_list|,
name|av
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|worktree_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|!
name|opts
operator|.
name|detach
operator|+
operator|!
operator|!
name|opts
operator|.
name|new_branch
operator|+
operator|!
operator|!
name|new_branch_force
operator|>
literal|1
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"-b, -B, and --detach are mutually exclusive"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ac
operator|<
literal|1
operator|||
name|ac
operator|>
literal|2
condition|)
name|usage_with_options
argument_list|(
name|worktree_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|path
operator|=
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|strlen
argument_list|(
name|prefix
argument_list|)
argument_list|,
name|av
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|branch
operator|=
name|ac
operator|<
literal|2
condition|?
literal|"HEAD"
else|:
name|av
index|[
literal|1
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|branch
argument_list|,
literal|"-"
argument_list|)
condition|)
name|branch
operator|=
literal|"@{-1}"
expr_stmt|;
name|opts
operator|.
name|force_new_branch
operator|=
operator|!
operator|!
name|new_branch_force
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|force_new_branch
condition|)
block|{
name|struct
name|strbuf
name|symref
init|=
name|STRBUF_INIT
decl_stmt|;
name|opts
operator|.
name|new_branch
operator|=
name|new_branch_force
expr_stmt|;
if|if
condition|(
operator|!
name|opts
operator|.
name|force
operator|&&
operator|!
name|strbuf_check_branch_ref
argument_list|(
operator|&
name|symref
argument_list|,
name|opts
operator|.
name|new_branch
argument_list|)
operator|&&
name|ref_exists
argument_list|(
name|symref
operator|.
name|buf
argument_list|)
condition|)
name|die_if_checked_out
argument_list|(
name|symref
operator|.
name|buf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|symref
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|ac
operator|<
literal|2
operator|&&
operator|!
name|opts
operator|.
name|new_branch
operator|&&
operator|!
name|opts
operator|.
name|detach
condition|)
block|{
name|int
name|n
decl_stmt|;
specifier|const
name|char
modifier|*
name|s
init|=
name|worktree_basename
argument_list|(
name|path
argument_list|,
operator|&
name|n
argument_list|)
decl_stmt|;
name|opts
operator|.
name|new_branch
operator|=
name|xstrndup
argument_list|(
name|s
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|.
name|new_branch
condition|)
block|{
name|struct
name|child_process
name|cp
decl_stmt|;
name|memset
argument_list|(
operator|&
name|cp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cp
argument_list|)
argument_list|)
expr_stmt|;
name|cp
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"branch"
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|force_new_branch
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--force"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
name|opts
operator|.
name|new_branch
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
name|branch
argument_list|)
expr_stmt|;
if|if
condition|(
name|run_command
argument_list|(
operator|&
name|cp
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|branch
operator|=
name|opts
operator|.
name|new_branch
expr_stmt|;
block|}
return|return
name|add_worktree
argument_list|(
name|path
argument_list|,
name|branch
argument_list|,
operator|&
name|opts
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|show_worktree_porcelain
specifier|static
name|void
name|show_worktree_porcelain
parameter_list|(
name|struct
name|worktree
modifier|*
name|wt
parameter_list|)
block|{
name|printf
argument_list|(
literal|"worktree %s\n"
argument_list|,
name|wt
operator|->
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|wt
operator|->
name|is_bare
condition|)
name|printf
argument_list|(
literal|"bare\n"
argument_list|)
expr_stmt|;
else|else
block|{
name|printf
argument_list|(
literal|"HEAD %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|wt
operator|->
name|head_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|wt
operator|->
name|is_detached
condition|)
name|printf
argument_list|(
literal|"detached\n"
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"branch %s\n"
argument_list|,
name|wt
operator|->
name|head_ref
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_worktree
specifier|static
name|void
name|show_worktree
parameter_list|(
name|struct
name|worktree
modifier|*
name|wt
parameter_list|,
name|int
name|path_maxlen
parameter_list|,
name|int
name|abbrev_len
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|cur_path_len
init|=
name|strlen
argument_list|(
name|wt
operator|->
name|path
argument_list|)
decl_stmt|;
name|int
name|path_adj
init|=
name|cur_path_len
operator|-
name|utf8_strwidth
argument_list|(
name|wt
operator|->
name|path
argument_list|)
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%-*s "
argument_list|,
literal|1
operator|+
name|path_maxlen
operator|+
name|path_adj
argument_list|,
name|wt
operator|->
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|wt
operator|->
name|is_bare
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
literal|"(bare)"
argument_list|)
expr_stmt|;
else|else
block|{
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%-*s "
argument_list|,
name|abbrev_len
argument_list|,
name|find_unique_abbrev
argument_list|(
name|wt
operator|->
name|head_sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|wt
operator|->
name|is_detached
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"[%s]"
argument_list|,
name|shorten_unambiguous_ref
argument_list|(
name|wt
operator|->
name|head_ref
argument_list|,
literal|0
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
literal|"(detached HEAD)"
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sb
operator|.
name|buf
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
begin_function
DECL|function|measure_widths
specifier|static
name|void
name|measure_widths
parameter_list|(
name|struct
name|worktree
modifier|*
modifier|*
name|wt
parameter_list|,
name|int
modifier|*
name|abbrev
parameter_list|,
name|int
modifier|*
name|maxlen
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|wt
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
block|{
name|int
name|sha1_len
decl_stmt|;
name|int
name|path_len
init|=
name|strlen
argument_list|(
name|wt
index|[
name|i
index|]
operator|->
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
name|path_len
operator|>
operator|*
name|maxlen
condition|)
operator|*
name|maxlen
operator|=
name|path_len
expr_stmt|;
name|sha1_len
operator|=
name|strlen
argument_list|(
name|find_unique_abbrev
argument_list|(
name|wt
index|[
name|i
index|]
operator|->
name|head_sha1
argument_list|,
operator|*
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|sha1_len
operator|>
operator|*
name|abbrev
condition|)
operator|*
name|abbrev
operator|=
name|sha1_len
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|list
specifier|static
name|int
name|list
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|porcelain
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"porcelain"
argument_list|,
operator|&
name|porcelain
argument_list|,
name|N_
argument_list|(
literal|"machine-readable output"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|ac
operator|=
name|parse_options
argument_list|(
name|ac
argument_list|,
name|av
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|worktree_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ac
condition|)
name|usage_with_options
argument_list|(
name|worktree_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
else|else
block|{
name|struct
name|worktree
modifier|*
modifier|*
name|worktrees
init|=
name|get_worktrees
argument_list|()
decl_stmt|;
name|int
name|path_maxlen
init|=
literal|0
decl_stmt|,
name|abbrev
init|=
name|DEFAULT_ABBREV
decl_stmt|,
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|porcelain
condition|)
name|measure_widths
argument_list|(
name|worktrees
argument_list|,
operator|&
name|abbrev
argument_list|,
operator|&
name|path_maxlen
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|worktrees
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|porcelain
condition|)
name|show_worktree_porcelain
argument_list|(
name|worktrees
index|[
name|i
index|]
argument_list|)
expr_stmt|;
else|else
name|show_worktree
argument_list|(
name|worktrees
index|[
name|i
index|]
argument_list|,
name|path_maxlen
argument_list|,
name|abbrev
argument_list|)
expr_stmt|;
block|}
name|free_worktrees
argument_list|(
name|worktrees
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_worktree
name|int
name|cmd_worktree
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
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
if|if
condition|(
name|ac
operator|<
literal|2
condition|)
name|usage_with_options
argument_list|(
name|worktree_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|prefix
condition|)
name|prefix
operator|=
literal|""
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|,
literal|"add"
argument_list|)
condition|)
return|return
name|add
argument_list|(
name|ac
operator|-
literal|1
argument_list|,
name|av
operator|+
literal|1
argument_list|,
name|prefix
argument_list|)
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|,
literal|"prune"
argument_list|)
condition|)
return|return
name|prune
argument_list|(
name|ac
operator|-
literal|1
argument_list|,
name|av
operator|+
literal|1
argument_list|,
name|prefix
argument_list|)
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|av
index|[
literal|1
index|]
argument_list|,
literal|"list"
argument_list|)
condition|)
return|return
name|list
argument_list|(
name|ac
operator|-
literal|1
argument_list|,
name|av
operator|+
literal|1
argument_list|,
name|prefix
argument_list|)
return|;
name|usage_with_options
argument_list|(
name|worktree_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
