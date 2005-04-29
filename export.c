begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_comment
comment|/*  * Show one commit  */
end_comment
begin_function
DECL|function|show_commit
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|char
name|cmdline
index|[
literal|400
index|]
decl_stmt|;
name|char
name|hex
index|[
literal|100
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"Id: %s\n"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|cmdline
argument_list|,
literal|"cat-file commit %s"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|system
argument_list|(
name|cmdline
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|parents
condition|)
block|{
name|char
modifier|*
name|against
init|=
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
decl_stmt|;
name|printf
argument_list|(
literal|"\n\n======== diff against %s ========\n"
argument_list|,
name|against
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|cmdline
argument_list|,
literal|"git-diff-tree -p %s %s"
argument_list|,
name|against
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|system
argument_list|(
name|cmdline
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"======== end ========\n\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Show all unseen commits, depth-first  */
end_comment
begin_function
DECL|function|show_unseen
name|void
name|show_unseen
parameter_list|(
name|struct
name|commit
modifier|*
name|top
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
if|if
condition|(
name|top
operator|->
name|object
operator|.
name|flags
operator|&
literal|2
condition|)
return|return;
name|top
operator|->
name|object
operator|.
name|flags
operator||=
literal|2
expr_stmt|;
name|parents
operator|=
name|top
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|show_unseen
argument_list|(
name|parents
operator|->
name|item
argument_list|)
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
name|show_commit
argument_list|(
name|top
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|export
name|void
name|export
parameter_list|(
name|struct
name|commit
modifier|*
name|top
parameter_list|,
name|struct
name|commit
modifier|*
name|base
parameter_list|)
block|{
name|mark_reachable
argument_list|(
operator|&
name|top
operator|->
name|object
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|base
condition|)
name|mark_reachable
argument_list|(
operator|&
name|base
operator|->
name|object
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|show_unseen
argument_list|(
name|top
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_commit
name|struct
name|commit
modifier|*
name|get_commit
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|object
operator|.
name|parsed
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to parse commit %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|parents
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|get_commit
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
return|return
name|commit
return|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
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
name|unsigned
name|char
name|base_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|top_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
operator|||
name|argc
operator|>
literal|4
operator|||
name|get_sha1_hex
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|top_sha1
argument_list|)
operator|||
operator|(
name|argc
operator|==
literal|3
operator|&&
name|get_sha1_hex
argument_list|(
name|argv
index|[
literal|2
index|]
argument_list|,
name|base_sha1
argument_list|)
operator|)
condition|)
name|usage
argument_list|(
literal|"git-export top [base]"
argument_list|)
expr_stmt|;
name|export
argument_list|(
name|get_commit
argument_list|(
name|top_sha1
argument_list|)
argument_list|,
name|argc
operator|==
literal|3
condition|?
name|get_commit
argument_list|(
name|base_sha1
argument_list|)
else|:
name|NULL
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
