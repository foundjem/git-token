begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_function
DECL|function|add_submodule_odb
specifier|static
name|int
name|add_submodule_odb
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|strbuf
name|objects_directory
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|alternate_object_database
modifier|*
name|alt_odb
decl_stmt|;
name|int
name|ret
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|git_dir
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|objects_directory
argument_list|,
literal|"%s/.git"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|git_dir
operator|=
name|read_gitfile_gently
argument_list|(
name|objects_directory
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|git_dir
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|objects_directory
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|objects_directory
argument_list|,
name|git_dir
argument_list|)
expr_stmt|;
block|}
name|strbuf_addstr
argument_list|(
operator|&
name|objects_directory
argument_list|,
literal|"/objects/"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|is_directory
argument_list|(
name|objects_directory
operator|.
name|buf
argument_list|)
condition|)
block|{
name|ret
operator|=
operator|-
literal|1
expr_stmt|;
goto|goto
name|done
goto|;
block|}
comment|/* avoid adding it twice */
for|for
control|(
name|alt_odb
operator|=
name|alt_odb_list
init|;
name|alt_odb
condition|;
name|alt_odb
operator|=
name|alt_odb
operator|->
name|next
control|)
if|if
condition|(
name|alt_odb
operator|->
name|name
operator|-
name|alt_odb
operator|->
name|base
operator|==
name|objects_directory
operator|.
name|len
operator|&&
operator|!
name|strncmp
argument_list|(
name|alt_odb
operator|->
name|base
argument_list|,
name|objects_directory
operator|.
name|buf
argument_list|,
name|objects_directory
operator|.
name|len
argument_list|)
condition|)
goto|goto
name|done
goto|;
name|alt_odb
operator|=
name|xmalloc
argument_list|(
name|objects_directory
operator|.
name|len
operator|+
literal|42
operator|+
sizeof|sizeof
argument_list|(
operator|*
name|alt_odb
argument_list|)
argument_list|)
expr_stmt|;
name|alt_odb
operator|->
name|next
operator|=
name|alt_odb_list
expr_stmt|;
name|strcpy
argument_list|(
name|alt_odb
operator|->
name|base
argument_list|,
name|objects_directory
operator|.
name|buf
argument_list|)
expr_stmt|;
name|alt_odb
operator|->
name|name
operator|=
name|alt_odb
operator|->
name|base
operator|+
name|objects_directory
operator|.
name|len
expr_stmt|;
name|alt_odb
operator|->
name|name
index|[
literal|2
index|]
operator|=
literal|'/'
expr_stmt|;
name|alt_odb
operator|->
name|name
index|[
literal|40
index|]
operator|=
literal|'\0'
expr_stmt|;
name|alt_odb
operator|->
name|name
index|[
literal|41
index|]
operator|=
literal|'\0'
expr_stmt|;
name|alt_odb_list
operator|=
name|alt_odb
expr_stmt|;
name|prepare_alt_odb
argument_list|()
expr_stmt|;
name|done
label|:
name|strbuf_release
argument_list|(
operator|&
name|objects_directory
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|handle_ignore_submodules_arg
name|void
name|handle_ignore_submodules_arg
parameter_list|(
name|struct
name|diff_options
modifier|*
name|diffopt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"all"
argument_list|)
condition|)
name|DIFF_OPT_SET
argument_list|(
name|diffopt
argument_list|,
name|IGNORE_SUBMODULES
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"untracked"
argument_list|)
condition|)
name|DIFF_OPT_SET
argument_list|(
name|diffopt
argument_list|,
name|IGNORE_UNTRACKED_IN_SUBMODULES
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"dirty"
argument_list|)
condition|)
name|DIFF_OPT_SET
argument_list|(
name|diffopt
argument_list|,
name|IGNORE_DIRTY_SUBMODULES
argument_list|)
expr_stmt|;
else|else
name|die
argument_list|(
literal|"bad --ignore-submodules argument: %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_submodule_summary
name|void
name|show_submodule_summary
parameter_list|(
name|FILE
modifier|*
name|f
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|char
name|one
index|[
literal|20
index|]
parameter_list|,
name|unsigned
name|char
name|two
index|[
literal|20
index|]
parameter_list|,
name|unsigned
name|dirty_submodule
parameter_list|,
specifier|const
name|char
modifier|*
name|del
parameter_list|,
specifier|const
name|char
modifier|*
name|add
parameter_list|,
specifier|const
name|char
modifier|*
name|reset
parameter_list|)
block|{
name|struct
name|rev_info
name|rev
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|,
modifier|*
name|left
init|=
name|left
decl_stmt|,
modifier|*
name|right
init|=
name|right
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|merge_bases
decl_stmt|,
modifier|*
name|list
decl_stmt|;
specifier|const
name|char
modifier|*
name|message
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|format
init|=
literal|"  %m %s"
decl_stmt|;
name|int
name|fast_forward
init|=
literal|0
decl_stmt|,
name|fast_backward
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|is_null_sha1
argument_list|(
name|two
argument_list|)
condition|)
name|message
operator|=
literal|"(submodule deleted)"
expr_stmt|;
elseif|else
if|if
condition|(
name|add_submodule_odb
argument_list|(
name|path
argument_list|)
condition|)
name|message
operator|=
literal|"(not checked out)"
expr_stmt|;
elseif|else
if|if
condition|(
name|is_null_sha1
argument_list|(
name|one
argument_list|)
condition|)
name|message
operator|=
literal|"(new submodule)"
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
operator|(
name|left
operator|=
name|lookup_commit_reference
argument_list|(
name|one
argument_list|)
operator|)
operator|||
operator|!
operator|(
name|right
operator|=
name|lookup_commit_reference
argument_list|(
name|two
argument_list|)
operator|)
condition|)
name|message
operator|=
literal|"(commits not present)"
expr_stmt|;
if|if
condition|(
operator|!
name|message
condition|)
block|{
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|setup_revisions
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|rev
operator|.
name|left_right
operator|=
literal|1
expr_stmt|;
name|rev
operator|.
name|first_parent_only
operator|=
literal|1
expr_stmt|;
name|left
operator|->
name|object
operator|.
name|flags
operator||=
name|SYMMETRIC_LEFT
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|left
operator|->
name|object
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|right
operator|->
name|object
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|merge_bases
operator|=
name|get_merge_bases
argument_list|(
name|left
argument_list|,
name|right
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|merge_bases
condition|)
block|{
if|if
condition|(
name|merge_bases
operator|->
name|item
operator|==
name|left
condition|)
name|fast_forward
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|merge_bases
operator|->
name|item
operator|==
name|right
condition|)
name|fast_backward
operator|=
literal|1
expr_stmt|;
block|}
for|for
control|(
name|list
operator|=
name|merge_bases
init|;
name|list
condition|;
name|list
operator|=
name|list
operator|->
name|next
control|)
block|{
name|list
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|list
operator|->
name|item
operator|->
name|object
argument_list|,
name|sha1_to_hex
argument_list|(
name|list
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|prepare_revision_walk
argument_list|(
operator|&
name|rev
argument_list|)
condition|)
name|message
operator|=
literal|"(revision walker failed)"
expr_stmt|;
block|}
if|if
condition|(
name|dirty_submodule
operator|&
name|DIRTY_SUBMODULE_UNTRACKED
condition|)
name|fprintf
argument_list|(
name|f
argument_list|,
literal|"Submodule %s contains untracked content\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirty_submodule
operator|&
name|DIRTY_SUBMODULE_MODIFIED
condition|)
name|fprintf
argument_list|(
name|f
argument_list|,
literal|"Submodule %s contains modified content\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|hashcmp
argument_list|(
name|one
argument_list|,
name|two
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return;
block|}
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"Submodule %s %s.."
argument_list|,
name|path
argument_list|,
name|find_unique_abbrev
argument_list|(
name|one
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fast_backward
operator|&&
operator|!
name|fast_forward
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|sb
argument_list|,
literal|'.'
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s"
argument_list|,
name|find_unique_abbrev
argument_list|(
name|two
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|message
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|" %s\n"
argument_list|,
name|message
argument_list|)
expr_stmt|;
else|else
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s:\n"
argument_list|,
name|fast_backward
condition|?
literal|" (rewind)"
else|:
literal|""
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|len
argument_list|,
literal|1
argument_list|,
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|message
condition|)
block|{
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
operator|&
name|rev
argument_list|)
operator|)
condition|)
block|{
name|struct
name|pretty_print_context
name|ctx
init|=
block|{
literal|0
block|}
decl_stmt|;
name|ctx
operator|.
name|date_mode
operator|=
name|rev
operator|.
name|date_mode
expr_stmt|;
name|strbuf_setlen
argument_list|(
operator|&
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|SYMMETRIC_LEFT
condition|)
block|{
if|if
condition|(
name|del
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|del
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|add
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|add
argument_list|)
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
name|format
argument_list|,
operator|&
name|sb
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
name|reset
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
name|reset
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|f
argument_list|,
literal|"%s"
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|clear_commit_marks
argument_list|(
name|left
argument_list|,
operator|~
literal|0
argument_list|)
expr_stmt|;
name|clear_commit_marks
argument_list|(
name|right
argument_list|,
operator|~
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
DECL|function|is_submodule_modified
name|unsigned
name|is_submodule_modified
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|ignore_untracked
parameter_list|)
block|{
name|ssize_t
name|len
decl_stmt|;
name|struct
name|child_process
name|cp
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
literal|"status"
block|,
literal|"--porcelain"
block|,
name|NULL
block|,
name|NULL
block|, 	}
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|unsigned
name|dirty_submodule
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|line
decl_stmt|,
modifier|*
name|next_line
decl_stmt|;
specifier|const
name|char
modifier|*
name|git_dir
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"%s/.git"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|git_dir
operator|=
name|read_gitfile_gently
argument_list|(
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_dir
condition|)
name|git_dir
operator|=
name|buf
operator|.
name|buf
expr_stmt|;
if|if
condition|(
operator|!
name|is_directory
argument_list|(
name|git_dir
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
comment|/* The submodule is not checked out, so it is not modified */
return|return
literal|0
return|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|ignore_untracked
condition|)
name|argv
index|[
literal|2
index|]
operator|=
literal|"-uno"
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
name|argv
operator|=
name|argv
expr_stmt|;
name|cp
operator|.
name|env
operator|=
name|local_repo_env
expr_stmt|;
name|cp
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|cp
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|cp
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|cp
operator|.
name|dir
operator|=
name|path
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|cp
argument_list|)
condition|)
name|die
argument_list|(
literal|"Could not run git status --porcelain"
argument_list|)
expr_stmt|;
name|len
operator|=
name|strbuf_read
argument_list|(
operator|&
name|buf
argument_list|,
name|cp
operator|.
name|out
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|line
operator|=
name|buf
operator|.
name|buf
expr_stmt|;
while|while
condition|(
name|len
operator|>
literal|2
condition|)
block|{
if|if
condition|(
operator|(
name|line
index|[
literal|0
index|]
operator|==
literal|'?'
operator|)
operator|&&
operator|(
name|line
index|[
literal|1
index|]
operator|==
literal|'?'
operator|)
condition|)
block|{
name|dirty_submodule
operator||=
name|DIRTY_SUBMODULE_UNTRACKED
expr_stmt|;
if|if
condition|(
name|dirty_submodule
operator|&
name|DIRTY_SUBMODULE_MODIFIED
condition|)
break|break;
block|}
else|else
block|{
name|dirty_submodule
operator||=
name|DIRTY_SUBMODULE_MODIFIED
expr_stmt|;
if|if
condition|(
name|ignore_untracked
operator|||
operator|(
name|dirty_submodule
operator|&
name|DIRTY_SUBMODULE_UNTRACKED
operator|)
condition|)
break|break;
block|}
name|next_line
operator|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|next_line
condition|)
break|break;
name|next_line
operator|++
expr_stmt|;
name|len
operator|-=
operator|(
name|next_line
operator|-
name|line
operator|)
expr_stmt|;
name|line
operator|=
name|next_line
expr_stmt|;
block|}
name|close
argument_list|(
name|cp
operator|.
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|cp
argument_list|)
condition|)
name|die
argument_list|(
literal|"git status --porcelain failed"
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
name|dirty_submodule
return|;
block|}
end_function
end_unit
