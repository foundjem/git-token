begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|advice_push_update_rejected
name|int
name|advice_push_update_rejected
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_non_ff_current
name|int
name|advice_push_non_ff_current
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_non_ff_default
name|int
name|advice_push_non_ff_default
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_non_ff_matching
name|int
name|advice_push_non_ff_matching
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_already_exists
name|int
name|advice_push_already_exists
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_fetch_first
name|int
name|advice_push_fetch_first
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_push_needs_force
name|int
name|advice_push_needs_force
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_status_hints
name|int
name|advice_status_hints
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_status_u_option
name|int
name|advice_status_u_option
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_commit_before_merge
name|int
name|advice_commit_before_merge
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_resolve_conflict
name|int
name|advice_resolve_conflict
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_implicit_identity
name|int
name|advice_implicit_identity
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_detached_head
name|int
name|advice_detached_head
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|advice_set_upstream_failure
name|int
name|advice_set_upstream_failure
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_struct
specifier|static
struct|struct
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|preference
name|int
modifier|*
name|preference
decl_stmt|;
block|}
DECL|variable|advice_config
name|advice_config
index|[]
init|=
block|{
block|{
literal|"pushupdaterejected"
block|,
operator|&
name|advice_push_update_rejected
block|}
block|,
block|{
literal|"pushnonffcurrent"
block|,
operator|&
name|advice_push_non_ff_current
block|}
block|,
block|{
literal|"pushnonffdefault"
block|,
operator|&
name|advice_push_non_ff_default
block|}
block|,
block|{
literal|"pushnonffmatching"
block|,
operator|&
name|advice_push_non_ff_matching
block|}
block|,
block|{
literal|"pushalreadyexists"
block|,
operator|&
name|advice_push_already_exists
block|}
block|,
block|{
literal|"pushfetchfirst"
block|,
operator|&
name|advice_push_fetch_first
block|}
block|,
block|{
literal|"pushneedsforce"
block|,
operator|&
name|advice_push_needs_force
block|}
block|,
block|{
literal|"statushints"
block|,
operator|&
name|advice_status_hints
block|}
block|,
block|{
literal|"statusuoption"
block|,
operator|&
name|advice_status_u_option
block|}
block|,
block|{
literal|"commitbeforemerge"
block|,
operator|&
name|advice_commit_before_merge
block|}
block|,
block|{
literal|"resolveconflict"
block|,
operator|&
name|advice_resolve_conflict
block|}
block|,
block|{
literal|"implicitidentity"
block|,
operator|&
name|advice_implicit_identity
block|}
block|,
block|{
literal|"detachedhead"
block|,
operator|&
name|advice_detached_head
block|}
block|,
block|{
literal|"setupstreamfailure"
block|,
operator|&
name|advice_set_upstream_failure
block|}
block|,
comment|/* make this an alias for backward compatibility */
block|{
literal|"pushnonfastforward"
block|,
operator|&
name|advice_push_update_rejected
block|}
block|}
struct|;
end_struct
begin_function
DECL|function|advise
name|void
name|advise
parameter_list|(
specifier|const
name|char
modifier|*
name|advice
parameter_list|,
modifier|...
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|va_list
name|params
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|,
modifier|*
name|np
decl_stmt|;
name|va_start
argument_list|(
name|params
argument_list|,
name|advice
argument_list|)
expr_stmt|;
name|strbuf_vaddf
argument_list|(
operator|&
name|buf
argument_list|,
name|advice
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|buf
operator|.
name|buf
init|;
operator|*
name|cp
condition|;
name|cp
operator|=
name|np
control|)
block|{
name|np
operator|=
name|strchrnul
argument_list|(
name|cp
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"hint: %.*s\n"
argument_list|)
argument_list|,
call|(
name|int
call|)
argument_list|(
name|np
operator|-
name|cp
argument_list|)
argument_list|,
name|cp
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|np
condition|)
name|np
operator|++
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_default_advice_config
name|int
name|git_default_advice_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|k
init|=
name|skip_prefix
argument_list|(
name|var
argument_list|,
literal|"advice."
argument_list|)
decl_stmt|;
name|int
name|i
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
name|advice_config
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|strcmp
argument_list|(
name|k
argument_list|,
name|advice_config
index|[
name|i
index|]
operator|.
name|name
argument_list|)
condition|)
continue|continue;
operator|*
name|advice_config
index|[
name|i
index|]
operator|.
name|preference
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|error_resolve_conflict
name|int
name|error_resolve_conflict
parameter_list|(
specifier|const
name|char
modifier|*
name|me
parameter_list|)
block|{
name|error
argument_list|(
literal|"'%s' is not possible because you have unmerged files."
argument_list|,
name|me
argument_list|)
expr_stmt|;
if|if
condition|(
name|advice_resolve_conflict
condition|)
comment|/* 		 * Message used both when 'git commit' fails and when 		 * other commands doing a merge do. 		 */
name|advise
argument_list|(
name|_
argument_list|(
literal|"Fix them up in the work tree,\n"
literal|"and then use 'git add/rm<file>' as\n"
literal|"appropriate to mark resolution and make a commit,\n"
literal|"or use 'git commit -a'."
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|die_resolve_conflict
name|void
name|NORETURN
name|die_resolve_conflict
parameter_list|(
specifier|const
name|char
modifier|*
name|me
parameter_list|)
block|{
name|error_resolve_conflict
argument_list|(
name|me
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"Exiting because of an unresolved conflict."
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|detach_advice
name|void
name|detach_advice
parameter_list|(
specifier|const
name|char
modifier|*
name|new_name
parameter_list|)
block|{
specifier|const
name|char
name|fmt
index|[]
init|=
literal|"Note: checking out '%s'.\n\n"
literal|"You are in 'detached HEAD' state. You can look around, make experimental\n"
literal|"changes and commit them, and you can discard any commits you make in this\n"
literal|"state without impacting any branches by performing another checkout.\n\n"
literal|"If you want to create a new branch to retain commits you create, you may\n"
literal|"do so (now or later) by using -b with the checkout command again. Example:\n\n"
literal|"  git checkout -b new_branch_name\n\n"
decl_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|fmt
argument_list|,
name|new_name
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
