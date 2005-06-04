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
begin_define
DECL|macro|SEEN
define|#
directive|define
name|SEEN
value|(1u<< 0)
end_define
begin_define
DECL|macro|INTERESTING
define|#
directive|define
name|INTERESTING
value|(1u<< 1)
end_define
begin_define
DECL|macro|UNINTERESTING
define|#
directive|define
name|UNINTERESTING
value|(1u<< 2)
end_define
begin_decl_stmt
DECL|variable|rev_list_usage
specifier|static
specifier|const
name|char
name|rev_list_usage
index|[]
init|=
literal|"usage: git-rev-list [OPTION] commit-id<commit-id>\n"
literal|"  --max-count=nr\n"
literal|"  --max-age=epoch\n"
literal|"  --min-age=epoch\n"
literal|"  --header\n"
literal|"  --pretty"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose_header
specifier|static
name|int
name|verbose_header
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_parents
specifier|static
name|int
name|show_parents
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pretty_print
specifier|static
name|int
name|pretty_print
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|hdr_termination
specifier|static
name|int
name|hdr_termination
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|prefix
specifier|static
specifier|const
name|char
modifier|*
name|prefix
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|max_age
specifier|static
name|unsigned
name|long
name|max_age
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|min_age
specifier|static
name|unsigned
name|long
name|min_age
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|max_count
specifier|static
name|int
name|max_count
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_commit
specifier|static
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s%s"
argument_list|,
name|prefix
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
if|if
condition|(
name|show_parents
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
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
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose_header
condition|)
block|{
specifier|const
name|char
modifier|*
name|buf
init|=
name|commit
operator|->
name|buffer
decl_stmt|;
if|if
condition|(
name|pretty_print
condition|)
block|{
specifier|static
name|char
name|pretty_header
index|[
literal|16384
index|]
decl_stmt|;
name|pretty_print_commit
argument_list|(
name|commit
operator|->
name|buffer
argument_list|,
operator|~
literal|0
argument_list|,
name|pretty_header
argument_list|,
sizeof|sizeof
argument_list|(
name|pretty_header
argument_list|)
argument_list|)
expr_stmt|;
name|buf
operator|=
name|pretty_header
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|buf
argument_list|,
name|hdr_termination
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|show_commit_list
specifier|static
name|void
name|show_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_most_recent_commit
argument_list|(
operator|&
name|list
argument_list|,
name|SEEN
argument_list|)
decl_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
if|if
condition|(
name|min_age
operator|!=
operator|-
literal|1
operator|&&
operator|(
name|commit
operator|->
name|date
operator|>
name|min_age
operator|)
condition|)
continue|continue;
if|if
condition|(
name|max_age
operator|!=
operator|-
literal|1
operator|&&
operator|(
name|commit
operator|->
name|date
operator|<
name|max_age
operator|)
condition|)
break|break;
if|if
condition|(
name|max_count
operator|!=
operator|-
literal|1
operator|&&
operator|!
name|max_count
operator|--
condition|)
break|break;
name|show_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|mark_parents_uninteresting
specifier|static
name|void
name|mark_parents_uninteresting
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|everybody_uninteresting
specifier|static
name|int
name|everybody_uninteresting
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|limit_list
name|struct
name|commit_list
modifier|*
name|limit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|newlist
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
modifier|*
name|p
init|=
operator|&
name|newlist
decl_stmt|;
do|do
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_most_recent_commit
argument_list|(
operator|&
name|list
argument_list|,
name|SEEN
argument_list|)
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|commit
operator|->
name|object
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
name|mark_parents_uninteresting
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|everybody_uninteresting
argument_list|(
name|list
argument_list|)
condition|)
break|break;
continue|continue;
block|}
name|p
operator|=
operator|&
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
name|p
argument_list|)
operator|->
name|next
expr_stmt|;
block|}
do|while
condition|(
name|list
condition|)
do|;
return|return
name|newlist
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
name|struct
name|commit_list
modifier|*
name|list
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|,
name|limited
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|int
name|flags
decl_stmt|;
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--max-count="
argument_list|,
literal|12
argument_list|)
condition|)
block|{
name|max_count
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|12
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--max-age="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|max_age
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|10
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--min-age="
argument_list|,
literal|10
argument_list|)
condition|)
block|{
name|min_age
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|10
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--header"
argument_list|)
condition|)
block|{
name|verbose_header
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--pretty"
argument_list|)
condition|)
block|{
name|verbose_header
operator|=
literal|1
expr_stmt|;
name|pretty_print
operator|=
literal|1
expr_stmt|;
name|hdr_termination
operator|=
literal|'\n'
expr_stmt|;
name|prefix
operator|=
literal|"commit "
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--parents"
argument_list|)
condition|)
block|{
name|show_parents
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|flags
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'^'
condition|)
block|{
name|flags
operator|=
name|UNINTERESTING
expr_stmt|;
name|arg
operator|++
expr_stmt|;
name|limited
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|parse_commit
argument_list|(
name|commit
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"bad commit object %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|flags
expr_stmt|;
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|list
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|limited
condition|)
name|list
operator|=
name|limit_list
argument_list|(
name|list
argument_list|)
expr_stmt|;
name|show_commit_list
argument_list|(
name|list
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
