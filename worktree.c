begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"worktree.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_function
DECL|function|free_worktrees
name|void
name|free_worktrees
parameter_list|(
name|struct
name|worktree
modifier|*
modifier|*
name|worktrees
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|;
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
name|free
argument_list|(
name|worktrees
index|[
name|i
index|]
operator|->
name|path
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|worktrees
index|[
name|i
index|]
operator|->
name|id
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|worktrees
index|[
name|i
index|]
operator|->
name|head_ref
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|worktrees
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|worktrees
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * read 'path_to_ref' into 'ref'.  Also if is_detached is not NULL,  * set is_detached to 1 (0) if the ref is detatched (is not detached).  *  * $GIT_COMMON_DIR/$symref (e.g. HEAD) is practically outside $GIT_DIR so  * for linked worktrees, `resolve_ref_unsafe()` won't work (it uses  * git_path). Parse the ref ourselves.  *  * return -1 if the ref is not a proper ref, 0 otherwise (success)  */
end_comment
begin_function
DECL|function|parse_ref
specifier|static
name|int
name|parse_ref
parameter_list|(
name|char
modifier|*
name|path_to_ref
parameter_list|,
name|struct
name|strbuf
modifier|*
name|ref
parameter_list|,
name|int
modifier|*
name|is_detached
parameter_list|)
block|{
if|if
condition|(
name|is_detached
condition|)
operator|*
name|is_detached
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strbuf_readlink
argument_list|(
name|ref
argument_list|,
name|path_to_ref
argument_list|,
literal|0
argument_list|)
condition|)
block|{
comment|/* HEAD is symbolic link */
if|if
condition|(
operator|!
name|starts_with
argument_list|(
name|ref
operator|->
name|buf
argument_list|,
literal|"refs/"
argument_list|)
operator|||
name|check_refname_format
argument_list|(
name|ref
operator|->
name|buf
argument_list|,
literal|0
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
elseif|else
if|if
condition|(
name|strbuf_read_file
argument_list|(
name|ref
argument_list|,
name|path_to_ref
argument_list|,
literal|0
argument_list|)
operator|>=
literal|0
condition|)
block|{
comment|/* textual symref or detached */
if|if
condition|(
operator|!
name|starts_with
argument_list|(
name|ref
operator|->
name|buf
argument_list|,
literal|"ref:"
argument_list|)
condition|)
block|{
if|if
condition|(
name|is_detached
condition|)
operator|*
name|is_detached
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
name|strbuf_remove
argument_list|(
name|ref
argument_list|,
literal|0
argument_list|,
name|strlen
argument_list|(
literal|"ref:"
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_trim
argument_list|(
name|ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|check_refname_format
argument_list|(
name|ref
operator|->
name|buf
argument_list|,
literal|0
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
block|}
else|else
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/**  * Add the head_sha1 and head_ref (if not detached) to the given worktree  */
end_comment
begin_function
DECL|function|add_head_info
specifier|static
name|void
name|add_head_info
parameter_list|(
name|struct
name|strbuf
modifier|*
name|head_ref
parameter_list|,
name|struct
name|worktree
modifier|*
name|worktree
parameter_list|)
block|{
if|if
condition|(
name|head_ref
operator|->
name|len
condition|)
block|{
if|if
condition|(
name|worktree
operator|->
name|is_detached
condition|)
block|{
name|get_sha1_hex
argument_list|(
name|head_ref
operator|->
name|buf
argument_list|,
name|worktree
operator|->
name|head_sha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|resolve_ref_unsafe
argument_list|(
name|head_ref
operator|->
name|buf
argument_list|,
literal|0
argument_list|,
name|worktree
operator|->
name|head_sha1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|head_ref
operator|=
name|strbuf_detach
argument_list|(
name|head_ref
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/**  * get the main worktree  */
end_comment
begin_function
DECL|function|get_main_worktree
specifier|static
name|struct
name|worktree
modifier|*
name|get_main_worktree
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|worktree
modifier|*
name|worktree
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|worktree_path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|head_ref
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|is_bare
init|=
literal|0
decl_stmt|;
name|int
name|is_detached
init|=
literal|0
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|worktree_path
argument_list|,
name|absolute_path
argument_list|(
name|get_git_common_dir
argument_list|()
argument_list|)
argument_list|)
expr_stmt|;
name|is_bare
operator|=
operator|!
name|strbuf_strip_suffix
argument_list|(
operator|&
name|worktree_path
argument_list|,
literal|"/.git"
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_bare
condition|)
name|strbuf_strip_suffix
argument_list|(
operator|&
name|worktree_path
argument_list|,
literal|"/."
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s/HEAD"
argument_list|,
name|get_git_common_dir
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_ref
argument_list|(
name|path
operator|.
name|buf
argument_list|,
operator|&
name|head_ref
argument_list|,
operator|&
name|is_detached
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|done
goto|;
name|worktree
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|worktree
argument_list|)
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|path
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|worktree_path
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|id
operator|=
name|NULL
expr_stmt|;
name|worktree
operator|->
name|is_bare
operator|=
name|is_bare
expr_stmt|;
name|worktree
operator|->
name|head_ref
operator|=
name|NULL
expr_stmt|;
name|worktree
operator|->
name|is_detached
operator|=
name|is_detached
expr_stmt|;
name|worktree
operator|->
name|is_current
operator|=
literal|0
expr_stmt|;
name|add_head_info
argument_list|(
operator|&
name|head_ref
argument_list|,
name|worktree
argument_list|)
expr_stmt|;
name|done
label|:
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|worktree_path
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|head_ref
argument_list|)
expr_stmt|;
return|return
name|worktree
return|;
block|}
end_function
begin_function
DECL|function|get_linked_worktree
specifier|static
name|struct
name|worktree
modifier|*
name|get_linked_worktree
parameter_list|(
specifier|const
name|char
modifier|*
name|id
parameter_list|)
block|{
name|struct
name|worktree
modifier|*
name|worktree
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|worktree_path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|head_ref
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|is_detached
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|id
condition|)
name|die
argument_list|(
literal|"Missing linked worktree name"
argument_list|)
expr_stmt|;
name|strbuf_git_common_path
argument_list|(
operator|&
name|path
argument_list|,
literal|"worktrees/%s/gitdir"
argument_list|,
name|id
argument_list|)
expr_stmt|;
if|if
condition|(
name|strbuf_read_file
argument_list|(
operator|&
name|worktree_path
argument_list|,
name|path
operator|.
name|buf
argument_list|,
literal|0
argument_list|)
operator|<=
literal|0
condition|)
comment|/* invalid gitdir file */
goto|goto
name|done
goto|;
name|strbuf_rtrim
argument_list|(
operator|&
name|worktree_path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strbuf_strip_suffix
argument_list|(
operator|&
name|worktree_path
argument_list|,
literal|"/.git"
argument_list|)
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|worktree_path
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|worktree_path
argument_list|,
name|absolute_path
argument_list|(
literal|"."
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_strip_suffix
argument_list|(
operator|&
name|worktree_path
argument_list|,
literal|"/."
argument_list|)
expr_stmt|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s/worktrees/%s/HEAD"
argument_list|,
name|get_git_common_dir
argument_list|()
argument_list|,
name|id
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_ref
argument_list|(
name|path
operator|.
name|buf
argument_list|,
operator|&
name|head_ref
argument_list|,
operator|&
name|is_detached
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|done
goto|;
name|worktree
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|worktree
argument_list|)
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|path
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|worktree_path
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|id
operator|=
name|xstrdup
argument_list|(
name|id
argument_list|)
expr_stmt|;
name|worktree
operator|->
name|is_bare
operator|=
literal|0
expr_stmt|;
name|worktree
operator|->
name|head_ref
operator|=
name|NULL
expr_stmt|;
name|worktree
operator|->
name|is_detached
operator|=
name|is_detached
expr_stmt|;
name|worktree
operator|->
name|is_current
operator|=
literal|0
expr_stmt|;
name|add_head_info
argument_list|(
operator|&
name|head_ref
argument_list|,
name|worktree
argument_list|)
expr_stmt|;
name|done
label|:
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|worktree_path
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|head_ref
argument_list|)
expr_stmt|;
return|return
name|worktree
return|;
block|}
end_function
begin_function
DECL|function|mark_current_worktree
specifier|static
name|void
name|mark_current_worktree
parameter_list|(
name|struct
name|worktree
modifier|*
modifier|*
name|worktrees
parameter_list|)
block|{
name|struct
name|strbuf
name|git_dir
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|i
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|git_dir
argument_list|,
name|absolute_path
argument_list|(
name|get_git_dir
argument_list|()
argument_list|)
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
name|struct
name|worktree
modifier|*
name|wt
init|=
name|worktrees
index|[
name|i
index|]
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|path
argument_list|,
name|absolute_path
argument_list|(
name|get_worktree_git_dir
argument_list|(
name|wt
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
name|wt
operator|->
name|is_current
operator|=
operator|!
name|fspathcmp
argument_list|(
name|git_dir
operator|.
name|buf
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|wt
operator|->
name|is_current
condition|)
break|break;
block|}
name|strbuf_release
argument_list|(
operator|&
name|git_dir
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
DECL|function|get_worktrees
name|struct
name|worktree
modifier|*
modifier|*
name|get_worktrees
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|worktree
modifier|*
modifier|*
name|list
init|=
name|NULL
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
decl_stmt|;
name|struct
name|dirent
modifier|*
name|d
decl_stmt|;
name|int
name|counter
init|=
literal|0
decl_stmt|,
name|alloc
init|=
literal|2
decl_stmt|;
name|list
operator|=
name|xmalloc
argument_list|(
name|alloc
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|worktree
operator|*
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|list
index|[
name|counter
index|]
operator|=
name|get_main_worktree
argument_list|()
operator|)
condition|)
name|counter
operator|++
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s/worktrees"
argument_list|,
name|get_git_common_dir
argument_list|()
argument_list|)
expr_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|dir
condition|)
block|{
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
name|struct
name|worktree
modifier|*
name|linked
init|=
name|NULL
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|d
operator|->
name|d_name
argument_list|,
literal|"."
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|d
operator|->
name|d_name
argument_list|,
literal|".."
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|(
name|linked
operator|=
name|get_linked_worktree
argument_list|(
name|d
operator|->
name|d_name
argument_list|)
operator|)
condition|)
block|{
name|ALLOC_GROW
argument_list|(
name|list
argument_list|,
name|counter
operator|+
literal|1
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
name|list
index|[
name|counter
operator|++
index|]
operator|=
name|linked
expr_stmt|;
block|}
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
name|ALLOC_GROW
argument_list|(
name|list
argument_list|,
name|counter
operator|+
literal|1
argument_list|,
name|alloc
argument_list|)
expr_stmt|;
name|list
index|[
name|counter
index|]
operator|=
name|NULL
expr_stmt|;
name|mark_current_worktree
argument_list|(
name|list
argument_list|)
expr_stmt|;
return|return
name|list
return|;
block|}
end_function
begin_function
DECL|function|get_worktree_git_dir
specifier|const
name|char
modifier|*
name|get_worktree_git_dir
parameter_list|(
specifier|const
name|struct
name|worktree
modifier|*
name|wt
parameter_list|)
block|{
if|if
condition|(
operator|!
name|wt
condition|)
return|return
name|get_git_dir
argument_list|()
return|;
elseif|else
if|if
condition|(
operator|!
name|wt
operator|->
name|id
condition|)
return|return
name|get_git_common_dir
argument_list|()
return|;
else|else
return|return
name|git_common_path
argument_list|(
literal|"worktrees/%s"
argument_list|,
name|wt
operator|->
name|id
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|find_shared_symref
specifier|const
name|struct
name|worktree
modifier|*
name|find_shared_symref
parameter_list|(
specifier|const
name|char
modifier|*
name|symref
parameter_list|,
specifier|const
name|char
modifier|*
name|target
parameter_list|)
block|{
specifier|const
name|struct
name|worktree
modifier|*
name|existing
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|static
name|struct
name|worktree
modifier|*
modifier|*
name|worktrees
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|worktrees
condition|)
name|free_worktrees
argument_list|(
name|worktrees
argument_list|)
expr_stmt|;
name|worktrees
operator|=
name|get_worktrees
argument_list|()
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
name|struct
name|worktree
modifier|*
name|wt
init|=
name|worktrees
index|[
name|i
index|]
decl_stmt|;
name|strbuf_reset
argument_list|(
operator|&
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
name|path
argument_list|,
literal|"%s/%s"
argument_list|,
name|get_worktree_git_dir
argument_list|(
name|wt
argument_list|)
argument_list|,
name|symref
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_ref
argument_list|(
name|path
operator|.
name|buf
argument_list|,
operator|&
name|sb
argument_list|,
name|NULL
argument_list|)
condition|)
block|{
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|target
argument_list|)
condition|)
block|{
name|existing
operator|=
name|wt
expr_stmt|;
break|break;
block|}
block|}
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
name|existing
return|;
block|}
end_function
end_unit
